#include "cPreviewPanel.h"

BEGIN_EVENT_TABLE(cPreviewPanel, wxPanel)
	EVT_PAINT(cPreviewPanel::PaintEvent)
	EVT_SIZE(cPreviewPanel::OnSize)
	EVT_MOTION(cPreviewPanel::OnMouseMoved)
	EVT_MOUSEWHEEL(cPreviewPanel::OnMouseWheelMoved)
	EVT_LEFT_DOWN(cPreviewPanel::OnPreviewMouseLeftPressed)
	EVT_LEFT_UP(cPreviewPanel::OnPreviewMouseLeftReleased)
END_EVENT_TABLE()

cPreviewPanel::cPreviewPanel
(
	wxWindow* parent, 
	wxSizer* sizer, 
	std::unique_ptr<PreviewPanelVariables::InputPreviewPanelArgs> inputArgs
) 
	: wxPanel(parent)
{
	m_ParentArguments.reset(inputArgs.release());

	SetDoubleBuffered(true);

	sizer->Add(this, 1, wxEXPAND);

	InitDefaultComponents();
}

auto cPreviewPanel::SetKETEKData
(
	unsigned long* const mcaData, 
	const unsigned long dataSize, 
	const unsigned long long sum
) -> void
{
	if (!mcaData) return;

	auto prevImageSize = m_ImageSize;

	if (dataSize != m_ImageSize.GetWidth() || !m_ImageData)
	{
		m_ImageSize = wxSize(dataSize, m_RBFinish.y - m_LUStart.y);
		m_ImageData = std::make_unique<unsigned long[]>(m_ImageSize.GetWidth());
	}

	memcpy(m_ImageData.get(), mcaData, m_ImageSize.GetWidth() * sizeof(unsigned long));
	// Sending signal to wxThread, that we are done copying the data 
	mcaData[0] = ULONG_MAX - mcaData[0];

	double multiplicator{};
	auto maxValue = std::max_element(&m_ImageData[0], &m_ImageData[m_ImageSize.GetWidth()]);

	if (*maxValue)
	{
		m_MaxPosValueInData.first = std::distance(&m_ImageData[0], maxValue);
		m_MaxPosValueInData.second = *maxValue;
		multiplicator = abs((double)(m_ImageSize.GetHeight() - 1) / *maxValue);
		m_SumData = sum;
		m_MaxEventsCountOnGraph = *maxValue > m_MaxEventsCountOnGraph ? ((*maxValue + 9) / 10) * 10 : m_MaxEventsCountOnGraph;
		if (!m_ReferenceData)
			m_MaxEventsCountOnGraph = ((*maxValue + 9) / 10) * 10;
	}

	//LOGF("Multiplicator: ", multiplicator);

	/*
	Saving previous values for correct displaying of the image in the same place,
	where it was before capturing.
	*/
	{
		auto temp_zoom = m_Zoom;
		auto temp_pan_offset = m_PanOffset;
		auto temp_start_draw_pos = m_StartDrawPos;
		m_Zoom = 1.0;
		m_PanOffset = {};
		ChangeSizeOfImageInDependenceOnCanvasSize();

		if (m_IsImageSet)
		{
			m_Zoom = temp_zoom;
			m_PanOffset = temp_pan_offset;
			m_StartDrawPos = temp_start_draw_pos;
			if (prevImageSize != m_ImageSize)
			{
				m_Zoom = 1.0;
				m_PanOffset = {};
				m_StartDrawPos = m_NotZoomedGraphicsBitmapOffset;
			}
		}
	}

	m_IsImageSet = true;
	m_IsGraphicsBitmapSet = false;

	if (!m_ViewInitialized)
	{
		InitializeView();
	}
	else
	{
		const double xCenter = 0.5 * (m_View.xMin + m_View.xMax);
		const double xRange = m_View.xMax - m_View.xMin;
		const double yCenter = 0.5 * (m_View.yMin + m_View.yMax);
		const double yRange = m_View.yMax - m_View.yMin;

		m_View.xMin = xCenter - xRange / 2.0;
		m_View.xMax = xCenter + xRange / 2.0;
		m_View.yMin = yCenter - yRange / 2.0;
		m_View.yMax = yCenter + yRange / 2.0;

		ClampView();
	}

	Refresh();
}

auto cPreviewPanel::SetKETEKReferenceData
(
	unsigned long* const mcaData, 
	const unsigned long dataSize, 
	const unsigned long long sum
) -> void
{
	if (!mcaData) return;

	auto prevImageSize = m_ImageSize;

	if (dataSize != m_ImageSize.GetWidth() || !m_ReferenceData)
	{
		m_ImageSize = wxSize(dataSize, m_RBFinish.y - m_LUStart.y);
		m_ReferenceData = std::make_unique<unsigned long[]>(m_ImageSize.GetWidth());
	}

	memcpy(m_ReferenceData.get(), mcaData, m_ImageSize.GetWidth() * sizeof(unsigned long));
	// Sending signal to wxThread, that we are done copying the data 
	mcaData[0] = ULONG_MAX - mcaData[0];

	auto maxValue = std::max_element(&m_ReferenceData[0], &m_ReferenceData[m_ImageSize.GetWidth()]);

	if (*maxValue)
	{
		m_MaxEventsCountOnGraph = *maxValue > m_MaxEventsCountOnGraph ? ((*maxValue + 9) / 10) * 10 : m_MaxEventsCountOnGraph;
	}

	/*
	Saving previous values for correct displaying of the image in the same place,
	where it was before capturing.
	*/
	{
		auto temp_zoom = m_Zoom;
		auto temp_pan_offset = m_PanOffset;
		auto temp_start_draw_pos = m_StartDrawPos;
		m_Zoom = 1.0;
		m_PanOffset = {};
		ChangeSizeOfImageInDependenceOnCanvasSize();

		if (m_IsImageSet)
		{
			m_Zoom = temp_zoom;
			m_PanOffset = temp_pan_offset;
			m_StartDrawPos = temp_start_draw_pos;
			if (prevImageSize != m_ImageSize)
			{
				m_Zoom = 1.0;
				m_PanOffset = {};
				m_StartDrawPos = m_NotZoomedGraphicsBitmapOffset;
			}
		}
	}

	m_IsImageSet = true;
	m_IsGraphicsBitmapSet = false;

	if (!m_ViewInitialized)
	{
		InitializeView();
	}
	else
	{
		const double xCenter = 0.5 * (m_View.xMin + m_View.xMax);
		const double xRange = m_View.xMax - m_View.xMin;
		const double yCenter = 0.5 * (m_View.yMin + m_View.yMax);
		const double yRange = m_View.yMax - m_View.yMin;

		m_View.xMin = xCenter - xRange / 2.0;
		m_View.xMax = xCenter + xRange / 2.0;
		m_View.yMin = yCenter - yRange / 2.0;
		m_View.yMax = yCenter + yRange / 2.0;

		ClampView();
	}

	Refresh();

}

bool cPreviewPanel::SavePNG(const wxString& filePath)
{
	const wxSize sz = GetClientSize();
	if (sz.x <= 0 || sz.y <= 0) return false;

	wxBitmap bmp(sz.x, sz.y, 32);
	wxMemoryDC mdc(bmp);
	mdc.SetBackground(wxBrush(wxBRUSHSTYLE_TRANSPARENT));
	mdc.Clear();

	if (auto* gc = wxGraphicsContext::Create(mdc))
	{
		if (m_ViewInitialized)
		{
			DrawReferenceDataViewport(gc);
			DrawCapturedDataViewport(gc);
			DrawHorizontalRulerViewport(gc);
			DrawVerticalRulerViewport(gc);
		}

		if (m_IsImageSet)
		{
			DrawMaxValue(gc);
			DrawSumEvents(gc);
		}

		delete gc;
	}

	mdc.SelectObject(wxNullBitmap);
	return bmp.IsOk() && bmp.ConvertToImage().SaveFile(filePath, wxBITMAP_TYPE_PNG);
}

auto cPreviewPanel::SetBackgroundColor(wxColour bckg_colour) -> void
{
	SetBackgroundColour(bckg_colour);
}

auto cPreviewPanel::SetCrossHairButtonActive(bool activate) -> void
{
	//m_CrossHairTool->ActivateToolButton(activate, activate);
	//m_CrossHairTool->SetCursorPosOnCanvas(m_CursorPosOnCanvas);
	ChangeCursorInDependenceOfCurrentParameters();
	Refresh();
}

auto cPreviewPanel::SetValueDisplayingActive(bool activate) -> void
{
	m_DisplayPixelValues = activate;
}

void cPreviewPanel::SetXCrossHairPosFromParentWindow(const int& x_pos)
{
	int corrected_x_pos = x_pos - 1;
	if (corrected_x_pos >= 0 && corrected_x_pos < m_ImageSize.GetWidth())
	{
		m_CrossHairPos.x = corrected_x_pos;
		//m_CrossHairTool->SetXPosFromParent(corrected_x_pos);
		Refresh();
	}
}

void cPreviewPanel::SetYCrossHairPosFromParentWindow(const int& y_pos)
{
	int corrected_y_pos = y_pos - 1;
	if (corrected_y_pos >= 0 && corrected_y_pos < m_ImageSize.GetHeight())
	{
		m_CrossHairPos.y = corrected_y_pos;
		//m_CrossHairTool->SetYPosFromParent(corrected_y_pos);
		Refresh();
	}
}

auto cPreviewPanel::SettingCrossHairPosFromParentWindow(bool set) -> void
{
	//m_CrossHairTool->ActivateSetPositionFromParentWindow(set);
	//m_SettingCrossHairPos = set;
}

auto cPreviewPanel::InitializeSelectedCamera(const std::string& camera_sn) -> void
{
	//if (m_XimeaCameraControl->CloseCamera())
	//{
	//	m_SelectedCameraSN = camera_sn;
	//	m_XimeaCameraControl->InitializeCameraBySN(m_SelectedCameraSN);
	//}
}

void cPreviewPanel::SetCameraCapturedImage()
{
	/* 
	Saving previous values for correct displaying of the image in the same place, 
	where it was before capturing.
	*/
	{
		auto temp_zoom = m_Zoom;
		auto temp_pan_offset = m_PanOffset;
		auto temp_start_draw_pos = m_StartDrawPos;
		m_Zoom = 1.0;
		m_PanOffset = {};
		ChangeSizeOfImageInDependenceOnCanvasSize();
		if (m_IsImageSet)
		{
			m_Zoom = temp_zoom;
			m_PanOffset = temp_pan_offset;
			m_StartDrawPos = temp_start_draw_pos;
		}
	}

	/* CrossHair*/
	{
		//m_CrossHairTool->SetImageDataType(ToolsVariables::DATA_U12);
		//m_CrossHairTool->SetImageDimensions(m_ImageSize);
		//m_CrossHairTool->SetZoomOfOriginalSizeImage(m_ZoomOnOriginalSizeImage);
		//m_CrossHairTool->UpdateZoomValue(m_Zoom);
		//m_CrossHairTool->SetImageStartDrawPos(wxRealPoint
		//(
		//	m_StartDrawPos.x * m_Zoom / m_ZoomOnOriginalSizeImage,
		//	m_StartDrawPos.y * m_Zoom / m_ZoomOnOriginalSizeImage
		//));
		//m_CrossHairTool->SetXPosFromParent(m_ImageSize.GetWidth() / 2);
		//m_CrossHairTool->SetYPosFromParent(m_ImageSize.GetHeight() / 2);
	}

	m_IsImageSet = true;
	m_IsGraphicsBitmapSet = false;
	Refresh();
}

void cPreviewPanel::CaptureAndSaveDataFromCamera
(
	const unsigned long& exposure_time_us, 
	const wxString& path, 
	const std::string& start_hours,
	const std::string& start_minutes,
	const std::string& start_seconds,
	const int& frame_number, 
	const float& first_axis_position, 
	const float& second_axis_position
)
{
	//auto image_ptr = m_XimeaCameraControl->GetImage();
	//if (!image_ptr) return;

	/* Save Captured Image */
	{
		std::string first_axis_position_str = std::format("{:.3f}", first_axis_position);
		std::replace(first_axis_position_str.begin(), first_axis_position_str.end(), '.', '_');

		std::string second_axis_position_str = std::format("{:.3f}", second_axis_position);
		std::replace(second_axis_position_str.begin(), second_axis_position_str.end(), '.', '_');
		
		const std::string file_name = std::string(path.mb_str()) + std::string("\\") +
			std::string("img_") + 
			std::to_string(frame_number) + std::string("_") + 
			start_hours + std::string("H_") + 
			start_minutes + std::string("M_") + 
			start_seconds + std::string("S_") + 
			std::to_string(exposure_time_us) + std::string("us") 
			+ std::string("_1A_") + first_axis_position_str 
			+ std::string("_2A_") + second_axis_position_str 
			+ std::string(".tif");

		//cv::Mat cv_img
		//(
		//	cv::Size(m_XimeaCameraControl->GetImageWidth(), m_XimeaCameraControl->GetImageHeight()),
		//	CV_8UC1, 
		//	image_ptr, 
		//	cv::Mat::AUTO_STEP
		//);
		//cv::imwrite(file_name, cv_img);
	}

	//SetCameraCapturedImage(image_ptr);
}

void cPreviewPanel::CalculateMatlabJetColormapPixelRGB8bit
(
	const unsigned char& value, 
	unsigned char& r, 
	unsigned char& g, 
	unsigned char& b
)
{
	unsigned char x0_8bit{ 31 }, x1_8bit{ 95 }, x2_8bit{ 159 }, x3_8bit{ 223 }, x4_8bit{ 255 };
	if (value < x0_8bit)
	{
		r = 0;
		g = 0;
		b = 255 * 0.51563f + (float)value * (255.0f - 255 * 0.51563f) / (float)x0_8bit;
	}
	else if (value >= x0_8bit && value <= x1_8bit)
	{
		r = 0;
		g = (float)(value - x0_8bit) * 255.0f / (float)(x1_8bit - x0_8bit);
		b = 255;
	}
	else if (value > x1_8bit && value < x2_8bit)
	{
		r = (float)(value - x1_8bit) * 255.0f / (float)(x2_8bit - x1_8bit);
		g = 255;
		b = (float)(x2_8bit - value) * 255.0f / (float)(x2_8bit - x1_8bit);
	}
	else if (value >= x2_8bit && value <= x3_8bit)
	{
		r = 255;
		g = (float)(x3_8bit - value) * 255.0f / (float)(x3_8bit - x2_8bit);
		b = 0;
	}
	else if (value > x3_8bit)
	{
		r = 255.0f * 0.5f + (float)(x4_8bit - value) * (255.0f - 255.0f * 0.5f) / (float)(x4_8bit - x3_8bit);
		g = 0;
		b = 0;
	}
}

void cPreviewPanel::CalculateMatlabJetColormapPixelRGB12bit(const unsigned short& value, unsigned char& r, unsigned char& g, unsigned char& b)
{
	unsigned short x0_12bit{ 498 }, x1_12bit{ 1526 }, x2_12bit{ 2553 }, x3_12bit{ 3581 }, x4_12bit{ 4095 };
	if (value < x0_12bit)
	{
		r = 0;
		g = 0;
		b = 255 * 0.51563f + (float)value * (255.0f - 255 * 0.51563f) / (float)x0_12bit;
	}
	else if (value >= x0_12bit && value <= x1_12bit)
	{
		r = 0;
		g = (float)(value - x0_12bit) * 255.0f / (float)(x1_12bit - x0_12bit);
		b = 255;
	}
	else if (value > x1_12bit && value < x2_12bit)
	{
		r = (float)(value - x1_12bit) * 255.0f / (float)(x2_12bit - x1_12bit);
		g = 255;
		b = (float)(x2_12bit - value) * 255.0f / (float)(x2_12bit - x1_12bit);
	}
	else if (value >= x2_12bit && value <= x3_12bit)
	{
		r = 255;
		g = (float)(x3_12bit - value) * 255.0f / (float)(x3_12bit - x2_12bit);
		b = 0;
	}
	else if (value > x3_12bit && value < x4_12bit)
	{
		r = 255.0f * 0.5f + (float)(x4_12bit - value) * (255.0f - 255.0f * 0.5f) / (float)(x4_12bit - x3_12bit);
		g = 0;
		b = 0;
	}
	else if (value == x4_12bit)
	{
		// Saturation (white)
		r = 255.f;
		g = 255.f;
		b = 255.f;
	}
}

void cPreviewPanel::CalculateMatlabJetColormapPixelRGB16bit
(
	const uint16_t& value, 
	unsigned char& r, 
	unsigned char& g, 
	unsigned char& b
)
{
	uint16_t x0{ 7967 }, x1{ 24415 }, x2{ 40863 }, x3{ 57311 }, x4{ 65535 };
	if (value < x0)
	{
		r = 0;
		g = 0;
		b = 255 * 0.51563f + (float)value * (255.0f - 255 * 0.51563f) / (float)x0;
	}
	else if (value >= x0 && value <= x1)
	{
		r = 0;
		g = (float)(value - x0) * 255.0f / (float)(x1 - x0);
		b = 255;
	}
	else if (value > x1 && value < x2)
	{
		r = (float)(value - x1) * 255.0f / (float)(x2 - x1);
		g = 255;
		b = (float)(x2 - value) * 255.0f / (float)(x2 - x1);
	}
	else if (value >= x2 && value <= x3)
	{
		r = 255;
		g = (float)(x3 - value) * 255.0f / (float)(x3 - x2);
		b = 0;
	}
	else if (value > x3)
	{
		r = 255.0f * 0.5f + (float)(x4 - value) * (255.0f - 255.0f * 0.5f) / (float)(x4 - x3);
		g = 0;
		b = 0;
	}
}

void cPreviewPanel::OnMouseMoved(wxMouseEvent& evt)
{
	if (!m_ImageData && !m_ReferenceData) return;

	m_CursorPosOnCanvas = evt.GetPosition();

	if (m_IsDragging)
	{
		wxPoint pos = evt.GetPosition();
		PanPixels(pos.x - m_LastMousePos.x, pos.y - m_LastMousePos.y);
		m_LastMousePos = pos;
	}

	Refresh();

	UpdateStatusBarWithCursorPosition();
}

void cPreviewPanel::OnMouseWheelMoved(wxMouseEvent& evt)
{
	if (!m_ViewInitialized) return;

	const int rot = evt.GetWheelRotation();
	if (rot == 0) return;

	const double factor = (rot > 0) ? 1.25 : (1.0 / 1.25);

	m_LastMousePos = evt.GetPosition();

	if (evt.ShiftDown())
		ZoomY(factor, m_LastMousePos.y);
	else
		ZoomX(factor, m_LastMousePos.x);

	Refresh();
}

void cPreviewPanel::AddZoom(const double& zoom, bool zoom_in_center_of_window)
{
	wxRealPoint center = zoom_in_center_of_window ?
		wxRealPoint(GetSize().x / 2.0, GetSize().y / 2.0) :
		wxRealPoint(
			m_CursorPosOnCanvas.x,
			m_CursorPosOnCanvas.y);

	SetZoom(m_Zoom * zoom, center);
}

void cPreviewPanel::SetZoom(const double& zoom, const wxRealPoint& center_)
{
	wxRealPoint position_on_image{};
	position_on_image.x = (center_.x - m_PanOffset.x) / m_Zoom;
	position_on_image.y = (center_.y - m_PanOffset.y) / m_Zoom;

	m_Zoom = zoom;

	m_PanOffset.x = m_Zoom > 1.0 ? center_.x - position_on_image.x * m_Zoom : 0.0;
	m_PanOffset.y = m_Zoom > 1.0 ? center_.y - position_on_image.y * m_Zoom : 0.0;

	m_StartDrawPos.x = m_PanOffset.x / m_Zoom + m_NotZoomedGraphicsBitmapOffset.x;
	m_StartDrawPos.y = m_PanOffset.y / m_Zoom + m_NotZoomedGraphicsBitmapOffset.y;
	Refresh();
}

void cPreviewPanel::ProcessPan(const wxRealPoint& point_, bool refresh_)
{
	m_PanDeltaPoints = m_PanStartPoint - point_;
	//LOG2F("PanStartPoint x: ", m_PanStartPoint.x, " y: ", m_PanStartPoint.y);
	m_StartDrawPos.x = m_PanOffset.x / m_Zoom - m_PanDeltaPoints.x / m_Zoom + m_NotZoomedGraphicsBitmapOffset.x;
	m_StartDrawPos.y = m_PanOffset.y / m_Zoom - m_PanDeltaPoints.y / m_Zoom + m_NotZoomedGraphicsBitmapOffset.y;
	if (refresh_) Refresh();
}

void cPreviewPanel::FinishPan(bool refresh)
{
	if (m_Panning)
	{
		m_PanOffset.x -= m_PanDeltaPoints.x;
		m_PanOffset.y -= m_PanDeltaPoints.y;

		m_PanDeltaPoints = { 0.0, 0.0 };

		m_Panning = false;

		if (refresh) Refresh();
	}
}

void cPreviewPanel::CheckIfMouseAboveImage()
{
	m_IsCursorInsideImage = false;
	wxRealPoint cursor_pos_on_image =
	{
		m_CursorPosOnCanvas.x / m_Zoom - m_StartDrawPos.x,
		m_CursorPosOnCanvas.y / m_Zoom - m_StartDrawPos.y
	};
	if ((cursor_pos_on_image.x >= 0.0 &&
		(int)cursor_pos_on_image.x < m_ImageSize.GetWidth()) &&
		(cursor_pos_on_image.y >= 0.0 &&
			(int)cursor_pos_on_image.y < m_ImageSize.GetHeight()))
		m_IsCursorInsideImage = true;
}

void cPreviewPanel::CalculatePositionOnImage()
{
	m_NotCheckedCursorPosOnImage.x = m_CursorPosOnCanvas.x / m_Zoom - m_StartDrawPos.x;
	m_NotCheckedCursorPosOnImage.y = m_CursorPosOnCanvas.y / m_Zoom - m_StartDrawPos.y;

	/* Checking X */
	m_CheckedCursorPosOnImage.x = m_NotCheckedCursorPosOnImage.x >= 0.0 ? m_NotCheckedCursorPosOnImage.x : 0.0;
	m_CheckedCursorPosOnImage.x = m_NotCheckedCursorPosOnImage.x < (double)m_ImageSize.GetWidth() ? m_CheckedCursorPosOnImage.x : (double)m_ImageSize.GetWidth() - 1.0;
	/* Checking Y */
	m_CheckedCursorPosOnImage.y = m_NotCheckedCursorPosOnImage.y >= 0.0 ? m_NotCheckedCursorPosOnImage.y : 0.0;
	m_CheckedCursorPosOnImage.y = m_NotCheckedCursorPosOnImage.y < (double)m_ImageSize.GetHeight() ? m_CheckedCursorPosOnImage.y : (double)m_ImageSize.GetHeight() - 1.0;
}

void cPreviewPanel::OnPreviewMouseLeftPressed(wxMouseEvent& evt)
{
	if (!m_ViewInitialized) return;

	const wxPoint pos = evt.GetPosition();
	if (pos.x < m_LUStart.x || pos.x > m_RBFinish.x ||
		pos.y < m_LUStart.y || pos.y > m_RBFinish.y)
		return;

	m_IsDragging = true;
	m_LastMousePos = pos;
	CaptureMouse();
}

void cPreviewPanel::OnPreviewMouseLeftReleased(wxMouseEvent& evt)
{
	if (m_IsDragging)
	{
		m_IsDragging = false;
		if (HasCapture())
			ReleaseMouse();
	}
}

void cPreviewPanel::ChangeCursorInDependenceOfCurrentParameters()
{
	auto current_cursor = wxCURSOR_DEFAULT;

	/* CrossHair Tool */
	//current_cursor = m_SettingCrossHairPos ? wxCURSOR_QUESTION_ARROW : current_cursor;
	//current_cursor = m_CrossHairTool->UpdateCursor(current_cursor);

	SetCursor(current_cursor);
}

auto cPreviewPanel::UpdateStatusBarWithCursorPosition() -> void
{
	if ((!m_ImageData && !m_ReferenceData) || !m_ViewInitialized)
		return;

	int positionInData = static_cast<int>(std::lround(ScreenToDataX(static_cast<int>(m_CursorPosOnCanvas.x))));
	positionInData = std::clamp(positionInData, 0, m_ImageSize.GetWidth() - 1);

	wxString strTxt;

	if (m_ImageData)
	{
		strTxt += "E: ";
		strTxt += wxString::Format(wxT("%.2f"), static_cast<double>(positionInData) * m_BinSize);
		strTxt += " [keV] C: ";
		strTxt += wxString::Format(wxT("%lu"), m_ImageData[positionInData]);
	}

	if (m_ReferenceData)
	{
		if (m_ImageData)
			strTxt += " | ";

		strTxt += "E: ";
		strTxt += wxString::Format(wxT("%.2f"), static_cast<double>(positionInData) * m_ReferenceBinSize);
		strTxt += " [keV] C: ";
		strTxt += wxString::Format(wxT("%lu"), m_ReferenceData[positionInData]);
	}

	m_ParentArguments->status_bar->SetStatusText(strTxt);
}

void cPreviewPanel::DrawCrossHair(wxGraphicsContext* graphics_context)
{
	//graphics_context->SetPen(*wxRED_PEN);
	//m_CrossHairTool->DrawCrossHair(graphics_context, m_ImageData.get());
	//if (m_DisplayPixelValues)
	//	m_CrossHairTool->DrawPixelValues(graphics_context, m_ImageData.get());
}

void cPreviewPanel::InitializeView()
{
	if (!m_ImageData && !m_ReferenceData) return;

	m_View.xMin = 0.0;
	m_View.xMax = std::max(1.0, GetDataWidth() - 1.0);
	m_View.yMin = 0.0;
	m_View.yMax = std::max(1.0, static_cast<double>(m_MaxEventsCountOnGraph));
	m_ViewInitialized = true;
}

void cPreviewPanel::ClampView()
{
	const double fullXMin = 0.0;
	const double fullXMax = std::max(1.0, GetDataWidth() - 1.0);
	const double fullYMin = 0.0;
	const double fullYMax = std::max(1.0, static_cast<double>(m_MaxEventsCountOnGraph));

	double xRange = std::max(1.0, m_View.xMax - m_View.xMin);
	double yRange = std::max(1.0, m_View.yMax - m_View.yMin);

	xRange = std::min(xRange, fullXMax - fullXMin);
	yRange = std::min(yRange, fullYMax - fullYMin);

	if (m_View.xMin < fullXMin)
	{
		m_View.xMin = fullXMin;
		m_View.xMax = fullXMin + xRange;
	}
	if (m_View.xMax > fullXMax)
	{
		m_View.xMax = fullXMax;
		m_View.xMin = fullXMax - xRange;
	}

	if (m_View.yMin < fullYMin)
	{
		m_View.yMin = fullYMin;
		m_View.yMax = fullYMin + yRange;
	}
	if (m_View.yMax > fullYMax)
	{
		m_View.yMax = fullYMax;
		m_View.yMin = fullYMax - yRange;
	}

	m_View.xMin = std::max(fullXMin, m_View.xMin);
	m_View.xMax = std::min(fullXMax, m_View.xMax);
	m_View.yMin = std::max(fullYMin, m_View.yMin);
	m_View.yMax = std::min(fullYMax, m_View.yMax);
}

double cPreviewPanel::ScreenToDataX(int screenX) const
{
	const double plotWidth = m_RBFinish.x - m_LUStart.x;
	if (plotWidth <= 0.0) return m_View.xMin;

	const double t = (screenX - m_LUStart.x) / plotWidth;
	return m_View.xMin + t * (m_View.xMax - m_View.xMin);
}

double cPreviewPanel::ScreenToDataY(int screenY) const
{
	const double plotHeight = m_RBFinish.y - m_LUStart.y;
	if (plotHeight <= 0.0) return m_View.yMin;

	const double t = (m_RBFinish.y - screenY) / plotHeight;
	return m_View.yMin + t * (m_View.yMax - m_View.yMin);
}

double cPreviewPanel::DataToScreenX(double dataX) const
{
	const double xRange = m_View.xMax - m_View.xMin;
	if (xRange <= 0.0) return m_LUStart.x;

	const double t = (dataX - m_View.xMin) / xRange;
	return m_LUStart.x + t * (m_RBFinish.x - m_LUStart.x);
}

double cPreviewPanel::DataToScreenY(double dataY) const
{
	const double yRange = m_View.yMax - m_View.yMin;
	if (yRange <= 0.0) return m_RBFinish.y;

	const double t = (dataY - m_View.yMin) / yRange;
	return m_RBFinish.y - t * (m_RBFinish.y - m_LUStart.y);
}

void cPreviewPanel::ZoomX(double factor, int anchorScreenX)
{
	if (!m_ViewInitialized) return;

	const double anchorDataX = ScreenToDataX(anchorScreenX);
	const double oldRange = m_View.xMax - m_View.xMin;
	const double newRange = std::max(10.0, oldRange / factor);

	const double ratio = (anchorDataX - m_View.xMin) / oldRange;

	m_View.xMin = anchorDataX - ratio * newRange;
	m_View.xMax = m_View.xMin + newRange;

	ClampView();
}

void cPreviewPanel::ZoomY(double factor, int anchorScreenY)
{
	if (!m_ViewInitialized) return;

	const double anchorDataY = ScreenToDataY(anchorScreenY);
	const double oldRange = m_View.yMax - m_View.yMin;
	const double newRange = std::max(10.0, oldRange / factor);

	const double ratio = (anchorDataY - m_View.yMin) / oldRange;

	m_View.yMin = anchorDataY - ratio * newRange;
	m_View.yMax = m_View.yMin + newRange;

	ClampView();
}

void cPreviewPanel::PanPixels(int dx, int dy)
{
	if (!m_ViewInitialized) return;

	const double plotWidth = m_RBFinish.x - m_LUStart.x;
	const double plotHeight = m_RBFinish.y - m_LUStart.y;
	if (plotWidth <= 0.0 || plotHeight <= 0.0) return;

	const double dataDx = dx * (m_View.xMax - m_View.xMin) / plotWidth;
	const double dataDy = dy * (m_View.yMax - m_View.yMin) / plotHeight;

	m_View.xMin -= dataDx;
	m_View.xMax -= dataDx;

	m_View.yMin += dataDy;
	m_View.yMax += dataDy;

	ClampView();
}

void cPreviewPanel::DrawCapturedDataViewport(wxGraphicsContext* gc)
{
	if (!m_ImageData || !m_ViewInitialized) return;

	const int startIdx = std::max(0, static_cast<int>(std::floor(m_View.xMin)));
	const int endIdx = std::min(m_ImageSize.GetWidth() - 1, static_cast<int>(std::ceil(m_View.xMax)));

	if (endIdx <= startIdx) return;

	wxGraphicsPath path = gc->CreatePath();
	bool first = true;

	for (int i = startIdx; i <= endIdx; ++i)
	{
		const double x = DataToScreenX(static_cast<double>(i));
		const double y = DataToScreenY(static_cast<double>(m_ImageData[i]));

		if (first)
		{
			path.MoveToPoint(x, y);
			first = false;
		}
		else
		{
			path.AddLineToPoint(x, y);
		}
	}

	gc->SetPen(*wxGREEN_PEN);
	gc->StrokePath(path);
}

void cPreviewPanel::DrawReferenceDataViewport(wxGraphicsContext* gc)
{
	if (!m_ReferenceData || !m_ViewInitialized) return;

	const int startIdx = std::max(0, static_cast<int>(std::floor(m_View.xMin)));
	const int endIdx = std::min(m_ImageSize.GetWidth() - 1, static_cast<int>(std::ceil(m_View.xMax)));

	if (endIdx <= startIdx) return;

	wxGraphicsPath path = gc->CreatePath();
	bool first = true;

	for (int i = startIdx; i <= endIdx; ++i)
	{
		const double x = DataToScreenX(static_cast<double>(i));
		const double y = DataToScreenY(static_cast<double>(m_ReferenceData[i]));

		if (first)
		{
			path.MoveToPoint(x, y);
			first = false;
		}
		else
		{
			path.AddLineToPoint(x, y);
		}
	}

	gc->SetPen(*wxRED_PEN);
	gc->StrokePath(path);
}

void cPreviewPanel::DrawHorizontalRulerViewport(wxGraphicsContext* gc)
{
	if (!m_ViewInitialized) return;

	wxColour fontColour(0, 162, 232, 200);
	gc->SetPen(wxPen(fontColour));
	gc->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD), fontColour);

	const double axisY = m_RBFinish.y + (GetSize().GetHeight() - m_RBFinish.y) / 2.0;
	gc->StrokeLine(m_LUStart.x, axisY, m_RBFinish.x, axisY);

	constexpr int tickCount = 10;
	for (int i = 0; i <= tickCount; ++i)
	{
		const double t = static_cast<double>(i) / tickCount;
		const double screenX = m_LUStart.x + t * (m_RBFinish.x - m_LUStart.x);
		const double dataX = m_View.xMin + t * (m_View.xMax - m_View.xMin);

		const double energy =
			(m_BinSize != 0.0 ? dataX * m_BinSize : dataX * m_ReferenceBinSize);

		const wxString label = wxString::Format(wxT("%.2f"), energy);

		wxDouble tw{}, th{};
		gc->GetTextExtent(label, &tw, &th);
		gc->DrawText(label, screenX - tw / 2.0, m_RBFinish.y + (GetSize().GetHeight() - m_RBFinish.y) / 8.0);
		gc->StrokeLine(screenX, axisY - 6.0, screenX, axisY);
	}

	const wxString unit = "[keV]";
	wxDouble tw{}, th{};
	gc->GetTextExtent(unit, &tw, &th);
	gc->DrawText(unit, m_LUStart.x + (m_RBFinish.x - m_LUStart.x) / 2.0 - tw / 2.0,
		m_RBFinish.y + (GetSize().GetHeight() - m_RBFinish.y) * 3.0 / 4.0 - th / 2.0);
}

void cPreviewPanel::DrawVerticalRulerViewport(wxGraphicsContext* gc)
{
	if (!m_ViewInitialized) return;

	wxColour fontColour(0, 162, 232, 200);
	gc->SetPen(wxPen(fontColour));
	gc->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD), fontColour);

	const double axisX = m_LUStart.x / 2.0;
	gc->StrokeLine(axisX, m_LUStart.y, axisX, m_RBFinish.y);

	constexpr int tickCount = 10;
	for (int i = 0; i <= tickCount; ++i)
	{
		const double t = static_cast<double>(i) / tickCount;
		const double screenY = m_RBFinish.y - t * (m_RBFinish.y - m_LUStart.y);
		const double dataY = m_View.yMin + t * (m_View.yMax - m_View.yMin);

		const wxString label = wxString::Format(wxT("%.0f"), dataY);

		wxDouble tw{}, th{};
		gc->GetTextExtent(label, &tw, &th);
		gc->DrawText(label, axisX + m_LUStart.x / 4.0, screenY - th / 2.0);
		gc->StrokeLine(axisX, screenY, axisX + m_LUStart.x / 12.0, screenY);
	}
}

void cPreviewPanel::InitDefaultComponents()
{
	//m_GraphicsBitmapImage = std::make_unique<wxGraphicsBitmap>();
	/* Tools */
	//m_CrossHairTool = std::make_unique<CrossHairTool>
	//	(
	//		m_ParentArguments->x_pos_crosshair, 
	//		m_ParentArguments->y_pos_crosshair
	//	);
	//m_XimeaCameraControl = std::make_unique<XimeaControl>();
}

void cPreviewPanel::PaintEvent(wxPaintEvent& evt)
{
	wxBufferedPaintDC dc(this);
	Render(dc);
}

void cPreviewPanel::Render(wxBufferedPaintDC& dc)
{
	dc.Clear();

	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
	if (!gc)
		return;

	if (m_ViewInitialized)
	{
		DrawReferenceDataViewport(gc);
		DrawCapturedDataViewport(gc);

		if (m_CursorPosOnCanvas.x >= m_LUStart.x && m_CursorPosOnCanvas.x <= m_RBFinish.x &&
			m_CursorPosOnCanvas.y >= m_LUStart.y && m_CursorPosOnCanvas.y <= m_RBFinish.y)
		{
			DrawVerticalLineBelowCursor(gc, m_LUStart, m_RBFinish);
			DrawReferenceValueBelowCursor(gc, m_LUStart, m_RBFinish);
			DrawCapturedValueBelowCursor(gc, m_LUStart, m_RBFinish);
		}

		DrawHorizontalRulerViewport(gc);
		DrawVerticalRulerViewport(gc);
	}

	if (m_IsImageSet)
	{
		DrawMaxValue(gc);
		DrawSumEvents(gc);
	}

	delete gc;
}

auto cPreviewPanel::AdjustKETEKImageMultithread
(
	const unsigned long* const data, 
	const double multiplicationValue,
	const int startX, 
	const int startY, 
	const int finishX, 
	const int finishY
) -> void
{
	if (!m_Image.IsOk()) return;
	int currentValue = 0U;
	unsigned char red{}, green{ 255 }, blue{};
	unsigned long long position_in_data_pointer{};
	auto imgHeight = m_ImageSize.GetHeight();
	int y{};

	for (auto x{ startX }; x < finishX; ++x)
	{
		currentValue = (int)(multiplicationValue * data[position_in_data_pointer]);
#ifdef _DEBUG
		//if (currentValue)
			//LOGI("Current Value: ", currentValue);

#endif // _DEBUG

		y = imgHeight - abs(currentValue) - 1;
		m_Image.SetRGB(x, y, red, green, blue);
		++position_in_data_pointer;
	}
}

void cPreviewPanel::CreateGraphicsBitmapImage(wxGraphicsContext* gc_)
{
	if (!m_IsGraphicsBitmapSet && m_IsImageSet)
	{
		if (m_Image.IsOk())
		{
			m_GraphicsBitmapImage = gc_->CreateBitmapFromImage(m_Image);
			m_IsGraphicsBitmapSet = true;
		}
	}
}

auto cPreviewPanel::DrawVerticalLineBelowCursor(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void
{
	if (!m_ImageData && !m_ReferenceData) return;

	gc->SetPen(*wxWHITE_PEN);
	// Draw vertical line
	{
		wxGraphicsPath path = gc->CreatePath();

		path.MoveToPoint
		(
			m_CursorPosOnCanvas.x,
			luStart.y
		);
		path.AddLineToPoint
		(
			m_CursorPosOnCanvas.x,
			rbFinish.y
		);
		gc->StrokePath(path);
	}


}

auto cPreviewPanel::DrawCapturedValueBelowCursor(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void
{
	if (!m_ImageData) return;

	wxFont font = wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxColour fontColor = wxColour(0, 255, 0, 100);
	gc->SetFont(font, fontColor);

	int positionInData = static_cast<int>(std::lround(ScreenToDataX(static_cast<int>(m_CursorPosOnCanvas.x))));
	positionInData = std::clamp(positionInData, 0, m_ImageSize.GetWidth() - 1);

	// Draw value
	{
		wxString curr_value{};
		curr_value += "Energy: ";
		curr_value += wxString::Format(wxT("%.2f"), (double)positionInData * m_BinSize);
		curr_value += " Count: ";
		curr_value += wxString::Format(wxT("%lu"), m_ImageData[positionInData]);
		wxDouble widthText{}, heightText{};
		gc->GetTextExtent(curr_value, &widthText, &heightText);
		gc->DrawText
		(
			curr_value,
			m_CursorPosOnCanvas.x - widthText / 2.0,
			luStart.y - luStart.y / 4.0 - heightText
		);
	}

	auto penWidth = 4;
	auto alpha = 120;
	gc->SetPen(wxPen(wxColour(200, 140, 255, alpha), penWidth));
	// Draw Cross
	{
		auto crossWidth = 10.0;
		auto graphHeight = rbFinish.y - luStart.y;
		auto start_draw_y_position = rbFinish.y;
		auto current_y = graphHeight * (double)m_ImageData[positionInData] / (double)m_MaxEventsCountOnGraph;
		const auto drawY = DataToScreenY(static_cast<double>(m_ImageData[positionInData]));

		gc->StrokeLine
		(
			m_CursorPosOnCanvas.x - crossWidth / 2.0,
			drawY - crossWidth / 2.0,
			m_CursorPosOnCanvas.x + crossWidth / 2.0,
			drawY + crossWidth / 2.0
		);

		gc->StrokeLine
		(
			m_CursorPosOnCanvas.x - crossWidth / 2.0,
			drawY + crossWidth / 2.0,
			m_CursorPosOnCanvas.x + crossWidth / 2.0,
			drawY - crossWidth / 2.0
		);
	}

}

auto cPreviewPanel::DrawReferenceValueBelowCursor(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void
{
	if (!m_ReferenceData) return;
	if (m_ReferenceBinSize < m_BinSize) return;

	wxFont font = wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxColour fontColor = wxColour(255, 0, 0, 100);
	gc->SetFont(font, fontColor);

	int positionInData = static_cast<int>(std::lround(ScreenToDataX(static_cast<int>(m_CursorPosOnCanvas.x))));
	positionInData = std::clamp(positionInData, 0, m_ImageSize.GetWidth() - 1);

	// Draw value
	{
		wxString curr_value{};
		curr_value += "Energy: ";
		curr_value += wxString::Format(wxT("%.2f"), (double)positionInData * m_ReferenceBinSize);
		curr_value += " Count: ";
		curr_value += wxString::Format(wxT("%lu"), m_ReferenceData[positionInData]);
		wxDouble widthText{}, heightText{};
		gc->GetTextExtent(curr_value, &widthText, &heightText);
		gc->DrawText
		(
			curr_value,
			m_CursorPosOnCanvas.x - widthText / 2.0,
			luStart.y - luStart.y / 2.0 - heightText
		);
	}

	auto penWidth = 4;
	auto alpha = 120;
	gc->SetPen(wxPen(wxColour(0, 255, 255, alpha), penWidth));
	// Draw Cross
	{
		auto crossWidth = 10.0;
		auto graphHeight = rbFinish.y - luStart.y;
		auto start_draw_y_position = rbFinish.y;
		auto current_y = graphHeight * (double)m_ReferenceData[positionInData] / (double)m_MaxEventsCountOnGraph;
		const auto drawY = DataToScreenY(static_cast<double>(m_ReferenceData[positionInData]));

		gc->StrokeLine
		(
			m_CursorPosOnCanvas.x - crossWidth / 2.0,
			drawY - crossWidth / 2.0,
			m_CursorPosOnCanvas.x + crossWidth / 2.0,
			drawY + crossWidth / 2.0
		);

		gc->StrokeLine
		(
			m_CursorPosOnCanvas.x - crossWidth / 2.0,
			drawY + crossWidth / 2.0,
			m_CursorPosOnCanvas.x + crossWidth / 2.0,
			drawY - crossWidth / 2.0
		);
	}
}

void cPreviewPanel::DrawCapturedData(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish)
{
	if (!m_ImageData) return;

	wxGraphicsPath path = gc->CreatePath();
	int start_draw_y_position{};
	double current_x{}, delta_x{}, current_y{};

	//auto offsetX = 50.0;
	//auto offsetY = 50.0;
	//auto graphHeight = GetSize().GetHeight() - 2 * offsetY;
	auto graphHeight = rbFinish.y - luStart.y;

	start_draw_y_position = rbFinish.y;

	delta_x = (rbFinish.x - luStart.x) / m_ImageSize.GetWidth();
	current_x = luStart.x;

	auto position_in_data = 0UL;;
	for (auto x{ 0 }; x < m_ImageSize.GetWidth() - 1; ++x)
	{
		current_y = graphHeight * (double)m_ImageData[position_in_data] / (double)m_MaxEventsCountOnGraph;
		path.MoveToPoint(current_x, (double)start_draw_y_position - current_y);
		current_x += delta_x;
		current_y = graphHeight * (double)m_ImageData[position_in_data + 1] / (double)m_MaxEventsCountOnGraph;
		path.AddLineToPoint(current_x, (double)start_draw_y_position - current_y);
		++position_in_data;
	}

	gc->SetPen(*wxGREEN_PEN);
	gc->DrawPath(path);
}

void cPreviewPanel::DrawReferenceData(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish)
{
	if (!m_ReferenceData) return;

	wxGraphicsPath path = gc->CreatePath();
	int start_draw_y_position{};
	double current_x{}, delta_x{}, current_y{};

	//auto offsetX = 50.0;
	//auto offsetY = 50.0;
	//auto graphHeight = GetSize().GetHeight() - 2 * offsetY;
	auto graphHeight = rbFinish.y - luStart.y;

	start_draw_y_position = rbFinish.y;

	if (m_ReferenceBinSize >= m_BinSize)
		delta_x = (rbFinish.x - luStart.x) / m_ImageSize.GetWidth();
	else
		return;

	current_x = luStart.x;

	auto position_in_data = 0UL;;
	for (auto x{ 0 }; x < m_ImageSize.GetWidth() - 1; ++x)
	{
		current_y = graphHeight * (double)m_ReferenceData[position_in_data] / (double)m_MaxEventsCountOnGraph;
		path.MoveToPoint(current_x, (double)start_draw_y_position - current_y);
		current_x += delta_x;
		current_y = graphHeight * (double)m_ReferenceData[position_in_data + 1] / (double)m_MaxEventsCountOnGraph;
		path.AddLineToPoint(current_x, (double)start_draw_y_position - current_y);
		++position_in_data;
	}

	gc->SetPen(*wxRED_PEN);
	gc->DrawPath(path);

}

auto cPreviewPanel::DrawMaxValue(wxGraphicsContext* gc) -> void
{
	if (!m_ImageData) return;

	auto screenWidth = GetSize().GetWidth();
	auto screenHeight = GetSize().GetHeight();
	auto incrementX = 10.0;
	auto startX = 20.0;
	auto startY = 70.0;

	wxFont font = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxColour fontColor = wxColour(160, 240, 180, 100);
	gc->SetFont(font, fontColor);

	// Draw value
	{
		wxString curr_value{};
		curr_value += "Energy: ";
		curr_value += wxString::Format(wxT("%.4f"), (double)m_MaxPosValueInData.first * m_BinSize);
		curr_value += " Count: ";
		curr_value += wxString::Format(wxT("%lu"), m_MaxPosValueInData.second);
		wxDouble widthText{}, heightText{};
		gc->GetTextExtent(curr_value, &widthText, &heightText);
		gc->DrawText
		(
			curr_value,
			startX,
			startY - heightText
		);
	}
}

auto cPreviewPanel::DrawSumEvents(wxGraphicsContext* gc) -> void
{
	if (!m_ImageData) return;

	auto screenWidth = GetSize().GetWidth();
	auto screenHeight = GetSize().GetHeight();
	auto incrementX = 10.0;
	auto startX = 20.0;
	auto startY = 20.0;

	wxFont font = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxColour fontColor = wxColour(255, 128, 0, 100);
	gc->SetFont(font, fontColor);

	// Draw value
	{
		wxString curr_value{};
		curr_value += "Events: ";
		curr_value += wxString::Format(wxT("%llu"), m_SumData);
		wxDouble widthText{}, heightText{};
		gc->GetTextExtent(curr_value, &widthText, &heightText);
		gc->DrawText
		(
			curr_value,
			startX,
			startY
		);
	}

}

auto cPreviewPanel::DrawHorizontalRuller(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void
{
	if (!m_ImageData && !m_ReferenceData) return;

	wxColour fontColour = wxColour(0, 162, 232, 200);
	gc->SetPen(wxPen(fontColour));
	// Draw horizontal line
	{
		wxGraphicsPath path = gc->CreatePath();

		path.MoveToPoint
		(
			luStart.x,
			rbFinish.y + (GetSize().GetHeight() - rbFinish.y) / 2
		);
		path.AddLineToPoint
		(
			rbFinish.x,
			rbFinish.y + (GetSize().GetHeight() - rbFinish.y) / 2
		);
		gc->StrokePath(path);
	}

	wxFont font = wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, fontColour);

	// Draw scale values
	{
		wxString curr_value{};
		wxDouble widthText{}, heightText{};
		auto scaleValuesNumber = 10;
		auto scaleFactor = m_BinSize == 0.0 ? m_ImageSize.GetWidth() * m_ReferenceBinSize / scaleValuesNumber : m_ImageSize.GetWidth() * m_BinSize / scaleValuesNumber;
		for (auto i{ 0 }; i <= scaleValuesNumber; ++i)
		{
			curr_value = wxString::Format(wxT("%.2f"), (double)(i * scaleFactor));
			gc->GetTextExtent(curr_value, &widthText, &heightText);
			gc->DrawText
			(
				curr_value,
				luStart.x + i * (rbFinish.x - luStart.x) / scaleValuesNumber - widthText / 2.0,
				rbFinish.y + (GetSize().GetHeight() - rbFinish.y) / 8
			);

			gc->StrokeLine
			(
				luStart.x + i * (rbFinish.x - luStart.x) / scaleValuesNumber,
				rbFinish.y + (GetSize().GetHeight() - rbFinish.y) / 2 - (GetSize().GetHeight() - rbFinish.y) / 12.0,
				luStart.x + i * (rbFinish.x - luStart.x) / scaleValuesNumber,
				rbFinish.y + (GetSize().GetHeight() - rbFinish.y) / 2
			);
		}
	}

	// Draw [keV]
	{
		wxString curr_value{};
		curr_value = "[keV]";
		wxDouble widthText{}, heightText{};
		gc->GetTextExtent(curr_value, &widthText, &heightText);
		gc->DrawText
		(
			curr_value,
			luStart.x + (rbFinish.x - luStart.x) / 2.0 - widthText / 2.0,
			rbFinish.y + (GetSize().GetHeight() - rbFinish.y) * 3 / 4.0 - heightText / 2.0
		);
	}
}

auto cPreviewPanel::DrawVerticalRuller(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void
{
	if (!m_ImageData && !m_ReferenceData) return;

	wxColour fontColour = wxColour(0, 162, 232, 200);
	gc->SetPen(wxPen(fontColour));

	// Draw vertical line
	{
		wxGraphicsPath path = gc->CreatePath();

		path.MoveToPoint
		(
			luStart.x / 2.0,
			luStart.y
		);
		path.AddLineToPoint
		(
			luStart.x / 2.0,
			rbFinish.y
		);
		gc->StrokePath(path);
	}

	wxFont font = wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, fontColour);

	// Draw scale values
	wxString curr_value{};
	wxDouble widthText{}, heightText{};
	{
		auto scaleValuesNumber = 10;
		auto scaleFactor = m_MaxEventsCountOnGraph / scaleValuesNumber;
		for (auto i{ 0 }; i <= scaleValuesNumber; ++i)
		{
			curr_value = wxString::Format(wxT("%i"), (int)(i * scaleFactor));
			gc->GetTextExtent(curr_value, &widthText, &heightText);
			gc->DrawText
			(
				curr_value,
				luStart.x / 2.0 + luStart.x / 4.0,
				rbFinish.y - heightText / 2.0 - i * (m_RBFinish.y - m_LUStart.y) / scaleValuesNumber
			);

			// Stroking horizontal line
			gc->StrokeLine
			(
				luStart.x / 2.0,
				rbFinish.y - i * (m_RBFinish.y - m_LUStart.y) / scaleValuesNumber,
				luStart.x / 2.0 + luStart.x / 12.0,
				rbFinish.y - i * (m_RBFinish.y - m_LUStart.y) / scaleValuesNumber
			);
		}
	}

	// Draw Value of the Left side of the image
	{
		curr_value = "[Events]";
		gc->GetTextExtent(curr_value, &widthText, &heightText);
		wxRealPoint draw_point =
		{
			luStart.x / 4 - heightText / 2,
			luStart.y + (rbFinish.y - luStart.y) / 2 + widthText / 2.0
		};

		// Set up the transformation matrix for a 90-degree counterclockwise rotation
		gc->Translate(draw_point.x, draw_point.y);
		gc->Rotate(-M_PI / 2.0); // Rotate 90 degrees counterclockwise (pi/2 radians)
		gc->Translate(-draw_point.x, -draw_point.y);
		gc->DrawText(curr_value, draw_point.x, draw_point.y);
	}

}

void cPreviewPanel::OnSize(wxSizeEvent& evt)
{
	m_CanvasSize = evt.GetSize();
	m_LUStart = { GetSize().GetWidth() / 10.0, GetSize().GetHeight() / 10.0 };
	m_RBFinish = { GetSize().GetWidth() * 9.0 / 10.0, GetSize().GetHeight() * 9.0 / 10.0 };

	ClampView();
	Refresh();

	evt.Skip();
}

void cPreviewPanel::ChangeSizeOfImageInDependenceOnCanvasSize()
{
	wxSize current_image_size{ m_ImageSize };
	wxSize canvas_size{ GetSize().GetWidth(), GetSize().GetHeight() };
	m_ZoomOnOriginalSizeImage = 1.0;
	while (canvas_size.GetWidth() < current_image_size.GetWidth() || canvas_size.GetHeight() < current_image_size.GetHeight())
	{
		current_image_size.SetWidth(current_image_size.GetWidth() / 2);
		current_image_size.SetHeight(current_image_size.GetHeight() / 2);
		m_ZoomOnOriginalSizeImage *= 2.0;
	}
	m_ImageOnCanvasSize.Set(current_image_size.GetWidth(), current_image_size.GetHeight());
	//m_CrossHairTool->SetImageOnCanvasSize(current_image_size);
	m_NotZoomedGraphicsBitmapOffset.x = (canvas_size.GetWidth() - m_ImageOnCanvasSize.GetWidth()) / 2;
	m_NotZoomedGraphicsBitmapOffset.y = (canvas_size.GetHeight() - m_ImageOnCanvasSize.GetHeight()) / 2;
	m_NotZoomedGraphicsBitmapOffset.x *= m_ZoomOnOriginalSizeImage / m_Zoom;
	m_NotZoomedGraphicsBitmapOffset.y *= m_ZoomOnOriginalSizeImage / m_Zoom;
	m_StartDrawPos = m_NotZoomedGraphicsBitmapOffset;

}

auto cPreviewPanel::UpdateCrossHairOnSize() -> void
{
	//m_CrossHairTool->SetImageDimensions(m_ImageSize);
	//m_CrossHairTool->SetZoomOfOriginalSizeImage(m_ZoomOnOriginalSizeImage);
	//m_CrossHairTool->UpdateZoomValue(m_Zoom);
	//m_CrossHairTool->SetImageStartDrawPos(wxRealPoint
	//(
	//	m_StartDrawPos.x * m_Zoom / m_ZoomOnOriginalSizeImage,
	//	m_StartDrawPos.y * m_Zoom / m_ZoomOnOriginalSizeImage
	//));
	//m_CrossHairTool->SetXPosFromParent(m_CrossHairPos.x);
	//m_CrossHairTool->SetYPosFromParent(m_CrossHairPos.y);
}

