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

		m_View.xMin = xCenter - xRange / 2.0;
		m_View.xMax = xCenter + xRange / 2.0;

		RefreshYView(false);
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

		m_View.xMin = xCenter - xRange / 2.0;
		m_View.xMax = xCenter + xRange / 2.0;

		RefreshYView(false);
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
	mdc.SetBackground(*wxWHITE_BRUSH);
	mdc.Clear();

	if (auto* gc = wxGraphicsContext::Create(mdc))
	{
		if (m_ViewInitialized)
		{
			DrawReferenceDataViewport(gc);
			DrawCapturedDataViewport(gc);
			DrawHorizontalRulerViewport(gc, false);
			DrawVerticalRulerViewport(gc, false);
		}

		DrawOverviewOverlay(gc);

		if (ShouldDrawSummaryOverlay())
		{
			DrawMaxValue(gc);
			DrawSumEvents(gc);
		}

		DrawTemperatureOverlay(gc);

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

void cPreviewPanel::SetHardEnergyRange(double minKeV, double maxKeV)
{
	const double binSize = (m_BinSize > 0.0) ? m_BinSize : m_ReferenceBinSize;
	if (binSize <= 0.0 || m_ImageSize.GetWidth() <= 0)
		return;

	if (minKeV > maxKeV)
		std::swap(minKeV, maxKeV);

	const double fullXMin = 0.0;
	const double fullXMax = std::max(1.0, GetDataWidth() - 1.0);

	double minData = minKeV / binSize;
	double maxData = maxKeV / binSize;

	minData = std::clamp(minData, fullXMin, fullXMax);
	maxData = std::clamp(maxData, fullXMin, fullXMax);

	if (maxData <= minData)
		return;

	m_HardXRangeEnabled = true;
	m_HardXMinData = minData;
	m_HardXMaxData = maxData;

	m_View.xMin = m_HardXMinData;
	m_View.xMax = m_HardXMaxData;

	ClampView();
	Refresh();
}

void cPreviewPanel::ResetHardEnergyRangeToFullData()
{
	const double fullXMin = 0.0;
	const double fullXMax = std::max(1.0, GetDataWidth() - 1.0);

	m_HardXRangeEnabled = true;
	m_HardXMinData = fullXMin;
	m_HardXMaxData = fullXMax;

	m_View.xMin = m_HardXMinData;
	m_View.xMax = m_HardXMaxData;

	ClampView();
	Refresh();
}

double cPreviewPanel::GetMaxEnergyKeV() const
{
	if (m_ImageSize.GetWidth() <= 0)
		return 0.0;

	const double binSize = (m_BinSize > 0.0) ? m_BinSize : m_ReferenceBinSize;
	if (binSize <= 0.0)
		return 0.0;

	return static_cast<double>(m_ImageSize.GetWidth() - 1) * binSize;
}

void cPreviewPanel::SetPerformanceOverlayEnabled(bool enabled, double exposureSeconds)
{
	m_ShowPerformanceOverlay = enabled;
	m_ExposureSecondsForFPS = std::max(0.0, exposureSeconds);

	if (!enabled)
		ResetFrameStats();

	Refresh();
}

void cPreviewPanel::NotifyNewFrame(unsigned long long frameNumber)
{
	if (!m_ShowPerformanceOverlay)
		return;

	m_CurrentFrameNumber = frameNumber;

	const auto now = std::chrono::steady_clock::now();

	double effectiveFrameSeconds = m_ExposureSecondsForFPS;

	if (m_HasLastFrameTime)
	{
		const double measuredSeconds =
			std::chrono::duration_cast<std::chrono::microseconds>(now - m_LastFrameTime).count() / 1'000'000.0;

		if (effectiveFrameSeconds > 0.0)
			effectiveFrameSeconds = std::max(measuredSeconds, effectiveFrameSeconds);
		else
			effectiveFrameSeconds = measuredSeconds;
	}

	if (effectiveFrameSeconds > 0.0)
	{
		const double instantaneousFps = 1.0 / effectiveFrameSeconds;

		if (m_DisplayedFPS <= 0.0)
			m_DisplayedFPS = instantaneousFps;
		else
			m_DisplayedFPS = 0.85 * m_DisplayedFPS + 0.15 * instantaneousFps;
	}

	m_LastFrameTime = now;
	m_HasLastFrameTime = true;
}

void cPreviewPanel::ResetFrameStats()
{
	m_CurrentFrameNumber = 0;
	m_DisplayedFPS = 0.0;
	m_LastFrameTime = {};
	m_HasLastFrameTime = false;
}

void cPreviewPanel::SetBoardTemperature(double temperatureC)
{
	m_CurrentBoardTemperatureC = temperatureC;
	m_HasBoardTemperature = true;
	Refresh();
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

	const double fullXMin = 0.0;
	const double fullXMax = std::max(1.0, GetDataWidth() - 1.0);

	m_View.xMin = m_HardXRangeEnabled ? m_HardXMinData : fullXMin;
	m_View.xMax = m_HardXRangeEnabled ? m_HardXMaxData : fullXMax;
	m_ViewInitialized = true;

	RefreshYView(false);
	ClampView();
}

void cPreviewPanel::ClampView()
{
	const double fullXMin = 0.0;
	const double fullXMax = std::max(1.0, GetDataWidth() - 1.0);

	const double allowedXMin = m_HardXRangeEnabled ? std::clamp(m_HardXMinData, fullXMin, fullXMax) : fullXMin;
	const double allowedXMax = m_HardXRangeEnabled ? std::clamp(m_HardXMaxData, fullXMin, fullXMax) : fullXMax;

	const double fullYMin = 0.0;
	const double fullYMax = std::max(1.0, static_cast<double>(m_MaxEventsCountOnGraph));

	double xRange = std::max(1.0, m_View.xMax - m_View.xMin);
	double yRange = std::max(1.0, m_View.yMax - m_View.yMin);

	xRange = std::min(xRange, allowedXMax - allowedXMin);
	yRange = std::min(yRange, fullYMax - fullYMin);

	if (m_View.xMin < allowedXMin)
	{
		m_View.xMin = allowedXMin;
		m_View.xMax = allowedXMin + xRange;
	}
	if (m_View.xMax > allowedXMax)
	{
		m_View.xMax = allowedXMax;
		m_View.xMin = allowedXMax - xRange;
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

	m_View.xMin = std::max(allowedXMin, m_View.xMin);
	m_View.xMax = std::min(allowedXMax, m_View.xMax);
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

void cPreviewPanel::DrawHorizontalRulerViewport(wxGraphicsContext* gc, const bool isDarkBackground)
{
	if (!gc || !m_ViewInitialized)
		return;

	const wxSize panelSize = GetSize();
	const double plotLeft = m_LUStart.x;
	const double plotRight = m_RBFinish.x;
	const double plotBottom = m_RBFinish.y;

	const double rulerTop = plotBottom + 6.0;
	const double rulerHeight = std::max(42.0, panelSize.GetHeight() - plotBottom - 12.0);
	const double rulerBottom = rulerTop + rulerHeight;
	const double axisY = rulerTop + 16.0;

	const wxColour borderColour = isDarkBackground
		? wxColour(120, 135, 155, 170)
		: wxColour(185, 190, 198, 140);

	const wxColour fillColour = isDarkBackground
		? wxColour(24, 30, 38, 220)
		: wxColour(245, 247, 250, 210);

	const wxColour axisColour = isDarkBackground
		? wxColour(210, 220, 232, 210)
		: wxColour(110, 118, 128, 190);

	const wxColour tickColour = isDarkBackground
		? wxColour(170, 186, 204, 195)
		: wxColour(135, 144, 154, 180);

	const wxColour guideColour = isDarkBackground
		? wxColour(165, 180, 198, 42)
		: wxColour(170, 176, 184, 65);

	const wxColour textColour = isDarkBackground
		? wxColour(232, 238, 245, 220)
		: wxColour(50, 50, 50, 195);

	const wxColour unitColour = isDarkBackground
		? wxColour(200, 210, 220, 80)
		: wxColour(60, 60, 60, 80);

	gc->SetPen(wxPen(borderColour, 1));
	gc->SetBrush(wxBrush(fillColour));
	gc->DrawRoundedRectangle(plotLeft, rulerTop, plotRight - plotLeft, rulerHeight, 8.0);

	gc->SetPen(wxPen(axisColour, 1.5));
	gc->StrokeLine(plotLeft + 8.0, axisY, plotRight - 8.0, axisY);

	const int tickCount = std::clamp(static_cast<int>((plotRight - plotLeft) / 95.0), 6, 11);
	const double xRange = m_View.xMax - m_View.xMin;
	if (xRange <= 0.0)
		return;

	const double activeBinSize = (m_BinSize > 0.0) ? m_BinSize : m_ReferenceBinSize;

	wxFont tickFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont unitFont(24, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	for (int i = 0; i <= tickCount; ++i)
	{
		const double t = static_cast<double>(i) / static_cast<double>(tickCount);
		const double screenX = plotLeft + t * (plotRight - plotLeft);
		const double dataX = m_View.xMin + t * xRange;
		const double energy = dataX * activeBinSize;

		int decimals = 2;
		if (activeBinSize >= 1.0)
			decimals = 0;
		else if (xRange * activeBinSize > 200.0)
			decimals = 1;

		const wxString label = PreviewPanelVariables::CreateStringWithPrecision(energy, decimals);

		gc->SetPen(wxPen(tickColour, 1.5));
		gc->StrokeLine(screenX, axisY - 6.0, screenX, axisY + 6.0);

		gc->SetPen(wxPen(guideColour, 1, wxPENSTYLE_DOT));
		gc->StrokeLine(screenX, m_LUStart.y, screenX, m_RBFinish.y);

		gc->SetFont(tickFont, textColour);

		wxDouble tw{}, th{};
		gc->GetTextExtent(label, &tw, &th);

		double labelX = screenX - tw / 2.0;
		const double labelY = axisY + 9.0;

		if (i == 0)
			labelX = screenX + 4.0;
		else if (i == tickCount)
			labelX = screenX - tw - 4.0;

		labelX = std::clamp(labelX, plotLeft + 4.0, plotRight - tw - 4.0);

		gc->DrawText(label, labelX, labelY);
	}

	const wxString unit = "Energy [keV]";
	gc->SetFont(unitFont, unitColour);

	wxDouble tw{}, th{};
	gc->GetTextExtent(unit, &tw, &th);
	gc->DrawText
	(
		unit,
		plotLeft + (plotRight - plotLeft) / 2.0 - tw / 2.0,
		rulerBottom - th - 5.0
	);
}

void cPreviewPanel::DrawVerticalRulerViewport(wxGraphicsContext* gc, const bool isDarkBackground)
{
	if (!gc || !m_ViewInitialized)
		return;

	const double plotLeft = m_LUStart.x;
	const double plotTop = m_LUStart.y;
	const double plotBottom = m_RBFinish.y;

	const double rulerLeft = 8.0;
	const double rulerWidth = std::max(52.0, plotLeft - 16.0);
	const double rulerRight = rulerLeft + rulerWidth;
	const double axisX = rulerRight - 16.0;

	const wxColour borderColour = isDarkBackground
		? wxColour(120, 135, 155, 170)
		: wxColour(185, 190, 198, 140);

	const wxColour fillColour = isDarkBackground
		? wxColour(24, 30, 38, 220)
		: wxColour(245, 247, 250, 210);

	const wxColour axisColour = isDarkBackground
		? wxColour(210, 220, 232, 210)
		: wxColour(110, 118, 128, 190);

	const wxColour tickColour = isDarkBackground
		? wxColour(170, 186, 204, 195)
		: wxColour(135, 144, 154, 180);

	const wxColour guideColour = isDarkBackground
		? wxColour(165, 180, 198, 42)
		: wxColour(170, 176, 184, 65);

	const wxColour textColour = isDarkBackground
		? wxColour(232, 238, 245, 220)
		: wxColour(50, 50, 50, 195);

	const wxColour unitColour = isDarkBackground
		? wxColour(200, 210, 220, 80)
		: wxColour(60, 60, 60, 80);

	gc->SetPen(wxPen(borderColour, 1));
	gc->SetBrush(wxBrush(fillColour));
	gc->DrawRoundedRectangle(rulerLeft, plotTop, rulerWidth, plotBottom - plotTop, 8.0);

	gc->SetPen(wxPen(axisColour, 1.5));
	gc->StrokeLine(axisX, plotTop + 8.0, axisX, plotBottom - 8.0);

	const int tickCount = std::clamp(static_cast<int>((plotBottom - plotTop) / 58.0), 6, 10);
	const double yRange = m_View.yMax - m_View.yMin;
	if (yRange <= 0.0)
		return;

	wxFont tickFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont unitFont(24, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	for (int i = 0; i <= tickCount; ++i)
	{
		const double t = static_cast<double>(i) / static_cast<double>(tickCount);
		const double screenY = plotBottom - t * (plotBottom - plotTop);
		const double dataY = m_View.yMin + t * yRange;

		const wxString label = PreviewPanelVariables::CreateStringWithPrecision(dataY);

		gc->SetPen(wxPen(tickColour, 1.5));
		gc->StrokeLine(axisX - 6.0, screenY, axisX + 6.0, screenY);

		gc->SetPen(wxPen(guideColour, 1, wxPENSTYLE_DOT));
		gc->StrokeLine(m_LUStart.x, screenY, m_RBFinish.x, screenY);

		gc->SetFont(tickFont, textColour);

		wxDouble tw{}, th{};
		gc->GetTextExtent(label, &tw, &th);

		const double labelX = rulerLeft + 8.0;
		double labelY = screenY - th / 2.0;

		if (i == 0)
			labelY = screenY - th - 2.0;
		else if (i == tickCount)
			labelY = screenY + 2.0;

		labelY = std::clamp(labelY, plotTop + 2.0, plotBottom - th - 2.0);

		gc->DrawText(label, labelX, labelY);
	}

	const wxString unit = "Events";
	gc->SetFont(unitFont, unitColour);

	wxDouble tw{}, th{};
	gc->GetTextExtent(unit, &tw, &th);

	const double cx = rulerLeft + 74.0;
	const double cy = plotTop + (plotBottom - plotTop) / 2.0;

	gc->PushState();
	gc->Translate(cx, cy);
	gc->Rotate(-M_PI / 2.0);
	gc->DrawText(unit, -tw / 2.0, -th / 2.0);
	gc->PopState();
}

auto cPreviewPanel::IsDarkBackground() const -> bool
{
	const wxColour bg = GetBackgroundColour();

	// Standard perceived luminance.
	const double luminance =
		0.2126 * static_cast<double>(bg.Red()) +
		0.7152 * static_cast<double>(bg.Green()) +
		0.0722 * static_cast<double>(bg.Blue());

	return luminance < 140.0;
}

bool cPreviewPanel::ShouldDrawOverviewOverlay() const
{
	if ((!m_ImageData && !m_ReferenceData) || !m_ViewInitialized || m_ImageSize.GetWidth() <= 1)
		return false;

	return HasEffectiveHardRange() || HasEffectiveZoomedViewport();
}

bool cPreviewPanel::HasEffectiveHardRange() const
{
	if (!m_HardXRangeEnabled || m_ImageSize.GetWidth() <= 1)
		return false;

	const double fullXMin = 0.0;
	const double fullXMax = std::max(1.0, GetDataWidth() - 1.0);
	const double eps = 1e-6;

	return std::abs(m_HardXMinData - fullXMin) > eps ||
		std::abs(m_HardXMaxData - fullXMax) > eps;
}

bool cPreviewPanel::HasEffectiveZoomedViewport() const
{
	if (!m_ViewInitialized)
		return false;

	const double allowedXMin = HasEffectiveHardRange() ? m_HardXMinData : 0.0;
	const double allowedXMax = HasEffectiveHardRange()
		? m_HardXMaxData
		: std::max(1.0, GetDataWidth() - 1.0);

	const double eps = 1e-6;

	return std::abs(m_View.xMin - allowedXMin) > eps ||
		std::abs(m_View.xMax - allowedXMax) > eps;
}

void cPreviewPanel::DrawOverviewOverlay(wxGraphicsContext* gc)
{
	if (!ShouldDrawOverviewOverlay())
		return;

	const double margin = 12.0;
	const double overlayWidth = 220.0;
	const double overlayHeight = 90.0;

	const double x = GetSize().GetWidth() - overlayWidth - margin;
	const double y = margin;

	const double innerPad = 8.0;
	const wxRect2DDouble outerRect(x, y, overlayWidth, overlayHeight);
	const wxRect2DDouble plotRect(
		x + innerPad,
		y + innerPad + 14.0,
		overlayWidth - 2.0 * innerPad,
		overlayHeight - 2.0 * innerPad - 18.0
	);

	gc->SetPen(wxPen(wxColour(210, 210, 210, 180), 1));
	gc->SetBrush(wxBrush(wxColour(20, 20, 20, 170)));
	gc->DrawRoundedRectangle(outerRect.m_x, outerRect.m_y, outerRect.m_width, outerRect.m_height, 8.0);

	wxFont titleFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(titleFont, wxColour(235, 235, 235, 220));
	gc->DrawText("Overview", x + innerPad, y + 2.0);

	const double fullXMin = 0.0;
	const double fullXMax = std::max(1.0, GetDataWidth() - 1.0);
	const double fullRange = std::max(1.0, fullXMax - fullXMin);

	auto dataToOverlayX = [&](double dataX) -> double
		{
			const double t = (dataX - fullXMin) / fullRange;
			return plotRect.m_x + t * plotRect.m_width;
		};

	const double overviewYMax = GetOverviewYMax();

	auto valueToOverlayY = [&](double valueY) -> double
		{
			const double t = std::clamp(valueY / overviewYMax, 0.0, 1.0);
			return plotRect.m_y + plotRect.m_height - t * plotRect.m_height;
		};

	gc->SetPen(wxPen(wxColour(130, 130, 130, 200), 1));
	gc->SetBrush(*wxTRANSPARENT_BRUSH);
	gc->DrawRectangle(plotRect.m_x, plotRect.m_y, plotRect.m_width, plotRect.m_height);

	const int samples = std::min<int>(m_ImageSize.GetWidth(), static_cast<int>(plotRect.m_width));
	const int step = std::max(1, m_ImageSize.GetWidth() / std::max(1, samples));

	auto drawOverviewSeries =
		[&](const unsigned long* data, const wxColour& colour, int penWidth = 1)
		{
			if (!data) return;

			wxGraphicsPath path = gc->CreatePath();
			bool first = true;

			for (int i = 0; i < m_ImageSize.GetWidth(); i += step)
			{
				const double px = dataToOverlayX(static_cast<double>(i));
				const double py = valueToOverlayY(static_cast<double>(data[i]));

				if (first)
				{
					path.MoveToPoint(px, py);
					first = false;
				}
				else
				{
					path.AddLineToPoint(px, py);
				}
			}

			const int lastIdx = m_ImageSize.GetWidth() - 1;
			if (lastIdx >= 0 && (lastIdx % step) != 0)
			{
				path.AddLineToPoint(
					dataToOverlayX(static_cast<double>(lastIdx)),
					valueToOverlayY(static_cast<double>(data[lastIdx]))
				);
			}

			gc->SetPen(wxPen(colour, penWidth));
			gc->StrokePath(path);
		};

	// reference first, captured second, so captured stays visually on top
	drawOverviewSeries(m_ReferenceData.get(), wxColour(255, 90, 90, 190), 1);
	drawOverviewSeries(m_ImageData.get(), wxColour(120, 255, 120, 190), 1);

	if (HasEffectiveHardRange())
	{
		const double hardX1 = dataToOverlayX(m_HardXMinData);
		const double hardX2 = dataToOverlayX(m_HardXMaxData);

		gc->SetPen(wxPen(wxColour(255, 170, 0, 230), 2));
		gc->SetBrush(wxBrush(wxColour(255, 170, 0, 40)));
		gc->DrawRectangle(hardX1, plotRect.m_y, std::max(1.0, hardX2 - hardX1), plotRect.m_height);
	}

	if (HasEffectiveZoomedViewport())
	{
		const double viewX1 = dataToOverlayX(m_View.xMin);
		const double viewX2 = dataToOverlayX(m_View.xMax);

		gc->SetPen(wxPen(wxColour(0, 220, 255, 255), 2));
		gc->SetBrush(wxBrush(wxColour(0, 220, 255, 50)));
		gc->DrawRectangle(viewX1, plotRect.m_y, std::max(1.0, viewX2 - viewX1), plotRect.m_height);
	}

	wxFont legendFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	gc->SetFont(legendFont, wxColour(240, 240, 240, 220));

	double legendY = y + overlayHeight - 14.0;
	double cursorX = x + innerPad;

	if (HasEffectiveHardRange())
	{
		gc->SetPen(wxPen(wxColour(255, 170, 0, 255), 2));
		gc->StrokeLine(cursorX, legendY + 5.0, cursorX + 12.0, legendY + 5.0);
		gc->DrawText("Hard range", cursorX + 16.0, legendY);
		cursorX += 78.0;
	}

	if (HasEffectiveZoomedViewport())
	{
		gc->SetPen(wxPen(wxColour(0, 220, 255, 255), 2));
		gc->StrokeLine(cursorX, legendY + 5.0, cursorX + 12.0, legendY + 5.0);
		gc->DrawText("Viewport", cursorX + 16.0, legendY);
	}
}

double cPreviewPanel::GetOverviewYMax() const
{
	if (m_ImageSize.GetWidth() <= 0)
		return 1.0;

	int startIdx = 0;
	int endIdx = m_ImageSize.GetWidth() - 1;

	if (HasEffectiveHardRange())
	{
		startIdx = static_cast<int>(std::floor(m_HardXMinData));
		endIdx = static_cast<int>(std::ceil(m_HardXMaxData));
	}

	double maxY = 1.0;

	if (m_ImageData)
		maxY = std::max(maxY, GetMaxValueInRange(m_ImageData.get(), startIdx, endIdx));

	if (m_ReferenceData)
		maxY = std::max(maxY, GetMaxValueInRange(m_ReferenceData.get(), startIdx, endIdx));

	return maxY;
}

double cPreviewPanel::GetMaxValueInRange(const unsigned long* data, int startIdx, int endIdx) const
{
	if (!data || m_ImageSize.GetWidth() <= 0)
		return 1.0;

	startIdx = std::clamp(startIdx, 0, m_ImageSize.GetWidth() - 1);
	endIdx = std::clamp(endIdx, 0, m_ImageSize.GetWidth() - 1);

	if (endIdx < startIdx)
		std::swap(startIdx, endIdx);

	unsigned long maxValue = 0;
	for (int i = startIdx; i <= endIdx; ++i)
		maxValue = std::max(maxValue, data[i]);

	return std::max(1.0, static_cast<double>(maxValue));
}

void cPreviewPanel::DrawPerformanceOverlay(wxGraphicsContext* gc)
{
	if (!gc || !m_IsImageSet || !m_ShowPerformanceOverlay)
		return;

	const wxString fpsText = wxString::Format(wxT("FPS %.1f"), m_DisplayedFPS);
	const wxString frameText = wxString::Format(wxT("Frame: %llu"), m_CurrentFrameNumber);
	const wxString text = fpsText + "  |  " + frameText;

	wxFont font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, wxColour(245, 245, 245));

	wxDouble tw{}, th{};
	gc->GetTextExtent(text, &tw, &th);

	const double padX = 14.0;
	const double padY = 8.0;
	const double badgeW = tw + 2.0 * padX;
	const double badgeH = th + 2.0 * padY;

	const double x = GetSize().GetWidth() / 2 - badgeW / 2;
	const double y = 18.0;

	// Soft shadow
	gc->SetPen(*wxTRANSPARENT_PEN);
	gc->SetBrush(wxBrush(wxColour(0, 0, 0, 70)));
	gc->DrawRoundedRectangle(x + 3.0, y + 3.0, badgeW, badgeH, 10.0);

	// Main glass background
	gc->SetPen(wxPen(wxColour(255, 255, 255, 60), 1));
	gc->SetBrush(wxBrush(wxColour(18, 24, 32, 190)));
	gc->DrawRoundedRectangle(x, y, badgeW, badgeH, 10.0);

	// Accent strip
	const double accentW = 6.0;
	gc->SetPen(*wxTRANSPARENT_PEN);
	gc->SetBrush(wxBrush(wxColour(0, 200, 255, 220)));
	gc->DrawRoundedRectangle(x, y, accentW, badgeH, 10.0);

	// Thin highlight at top
	gc->SetPen(*wxTRANSPARENT_PEN);
	gc->SetBrush(wxBrush(wxColour(255, 255, 255, 28)));
	gc->DrawRoundedRectangle(x + 1.0, y + 1.0, badgeW - 2.0, badgeH * 0.42, 9.0);

	// Text
	gc->SetFont(font, wxColour(245, 245, 245, 235));
	gc->DrawText(text, x + padX, y + padY);
}

wxRect2DDouble cPreviewPanel::GetPerformanceOverlayRect(wxGraphicsContext* gc) const
{
	if (!gc || !m_IsImageSet || !m_ShowPerformanceOverlay)
		return wxRect2DDouble();

	const wxString fpsText = wxString::Format(wxT("FPS %.1f"), m_DisplayedFPS);
	const wxString frameText = wxString::Format(wxT("Frame: %llu"), m_CurrentFrameNumber);
	const wxString text = fpsText + "  |  " + frameText;

	wxFont font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, wxColour(245, 245, 245));

	wxDouble tw{}, th{};
	gc->GetTextExtent(text, &tw, &th);

	const double padX = 14.0;
	const double padY = 8.0;
	const double badgeW = tw + 2.0 * padX;
	const double badgeH = th + 2.0 * padY;

	const double x = GetSize().GetWidth() - badgeW - 18.0;
	const double y = 18.0;

	return wxRect2DDouble(x, y, badgeW, badgeH);
}

wxRect2DDouble cPreviewPanel::GetOverviewOverlayRect() const
{
	if (!ShouldDrawOverviewOverlay())
		return wxRect2DDouble();

	const double margin = 12.0;
	const double overlayWidth = 220.0;
	const double overlayHeight = 90.0;

	const double x = GetSize().GetWidth() - overlayWidth - margin;
	const double y = margin + GetSize().GetHeight() / 10.0;

	return wxRect2DDouble(x, y, overlayWidth, overlayHeight);
}

double cPreviewPanel::GetCursorInfoTopY(wxGraphicsContext* gc) const
{
	const auto perfRect = GetPerformanceOverlayRect(gc);
	const auto overviewRect = GetOverviewOverlayRect();

	double top = m_LUStart.y + 8.0;

	if (perfRect.m_width > 0.0)
		top = std::max(top, perfRect.m_y + perfRect.m_height + 10.0);

	if (overviewRect.m_width > 0.0)
	{
		const double availableTop = top;
		const double availableBottom = overviewRect.m_y - 10.0;

		if (availableBottom > availableTop)
			top = availableTop + (availableBottom - availableTop) * 0.5 - 22.0;
	}

	return top;
}

void cPreviewPanel::DrawOverlayBadge(wxGraphicsContext* gc, const wxString& text, double x, double y, const wxColour& accent, const wxColour& textColour, double opacityScale) const
{
	if (!gc || text.IsEmpty())
		return;

	wxFont font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, textColour);

	wxDouble tw{}, th{};
	gc->GetTextExtent(text, &tw, &th);

	const double padX = 10.0;
	const double padY = 6.0;
	const double badgeW = tw + 2.0 * padX;
	const double badgeH = th + 2.0 * padY;

	x = std::clamp(x, 8.0, GetSize().GetWidth() - badgeW - 8.0);
	y = std::clamp(y, 8.0, GetSize().GetHeight() - badgeH - 8.0);

	gc->SetPen(*wxTRANSPARENT_PEN);
	gc->SetBrush(wxBrush(wxColour(0, 0, 0, static_cast<unsigned char>(70 * opacityScale))));
	gc->DrawRoundedRectangle(x + 2.0, y + 2.0, badgeW, badgeH, 8.0);

	gc->SetPen(wxPen(wxColour(255, 255, 255, static_cast<unsigned char>(55 * opacityScale)), 1));
	gc->SetBrush(wxBrush(wxColour(18, 24, 32, static_cast<unsigned char>(185 * opacityScale))));
	gc->DrawRoundedRectangle(x, y, badgeW, badgeH, 8.0);

	gc->SetBrush(wxBrush(wxColour(accent.Red(), accent.Green(), accent.Blue(), static_cast<unsigned char>(215 * opacityScale))));
	gc->DrawRoundedRectangle(x, y, 5.0, badgeH, 8.0);

	gc->SetFont(font, textColour);
	gc->DrawText(text, x + padX, y + padY);
}

bool cPreviewPanel::ShouldDrawCursorOverlay() const
{
	if (!m_DisplayPixelValues)
		return false;

	if (!m_ViewInitialized)
		return false;

	if (!m_ImageData && !m_ReferenceData)
		return false;

	return m_CursorPosOnCanvas.x >= m_LUStart.x && m_CursorPosOnCanvas.x <= m_RBFinish.x &&
		m_CursorPosOnCanvas.y >= m_LUStart.y && m_CursorPosOnCanvas.y <= m_RBFinish.y;
}

bool cPreviewPanel::ShouldDrawSummaryOverlay() const
{
	if (!m_IsImageSet || !m_ImageData)
		return false;

	return true;
}

double cPreviewPanel::GetCurrentGraphYMax() const
{
	double maxY = 10.0;

	if (m_ImageData && m_ImageSize.GetWidth() > 0)
		maxY = std::max(maxY, GetMaxValueInRange(m_ImageData.get(), 0, m_ImageSize.GetWidth() - 1));

	if (m_ReferenceData && m_ImageSize.GetWidth() > 0)
		maxY = std::max(maxY, GetMaxValueInRange(m_ReferenceData.get(), 0, m_ImageSize.GetWidth() - 1));

	return std::max(1.0, maxY);
}

void cPreviewPanel::RefreshYView(bool preserveUserZoom)
{
	const double fullYMin = 0.0;
	const double fullYMax = GetCurrentGraphYMax();

	m_MaxEventsCountOnGraph = static_cast<unsigned long>(std::ceil(fullYMax));

	if (!m_ViewInitialized)
	{
		m_View.yMin = fullYMin;
		m_View.yMax = fullYMax;
		return;
	}

	if (!preserveUserZoom)
	{
		m_View.yMin = fullYMin;
		m_View.yMax = fullYMax;
		return;
	}

	const double oldRange = std::max(1.0, m_View.yMax - m_View.yMin);
	const double yCenter = 0.5 * (m_View.yMin + m_View.yMax);

	m_View.yMin = std::max(fullYMin, yCenter - oldRange / 2.0);
	m_View.yMax = std::min(fullYMax, yCenter + oldRange / 2.0);

	if (m_View.yMax - m_View.yMin < 1.0)
	{
		m_View.yMin = fullYMin;
		m_View.yMax = fullYMax;
	}
}

void cPreviewPanel::DrawTemperatureOverlay(wxGraphicsContext* gc)
{
	if (!gc || !m_HasBoardTemperature || !m_IsImageSet)
		return;

	const wxString text = wxString::Format(wxT("Board: %.1f [degC]"), m_CurrentBoardTemperatureC);

	wxFont font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, wxColour(245, 245, 245, 235));

	wxDouble tw{}, th{};
	gc->GetTextExtent(text, &tw, &th);

	const double padX = 10.0;
	const double padY = 6.0;
	const double badgeW = tw + 2.0 * padX;
	const double badgeH = th + 2.0 * padY;

	const auto perfRect = GetPerformanceOverlayRect(gc);
	const auto overviewRect = GetOverviewOverlayRect();

	double x = GetSize().GetWidth() - badgeW - 18.0;
	double y = 18.0;

	if (perfRect.m_width > 0.0)
		y = perfRect.m_y + perfRect.m_height + 10.0;

	if (overviewRect.m_width > 0.0)
	{
		const double overviewBottom = overviewRect.m_y + overviewRect.m_height;
		y = std::max(y, overviewBottom + 10.0);
	}

	DrawOverlayBadge
	(
		gc,
		text,
		x,
		y,
		wxColour(235, 51, 36),
		wxColour(250, 248, 245, 235),
		1.0
	);
}

wxRect2DDouble cPreviewPanel::GetTemperatureOverlayRect(wxGraphicsContext* gc) const
{
	if (!gc || !m_HasBoardTemperature || !m_IsImageSet)
		return wxRect2DDouble();

	const wxString text = wxString::Format(wxT("Board Temp: %.2f C"), m_CurrentBoardTemperatureC);

	wxFont font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, wxColour(245, 245, 245, 235));

	wxDouble tw{}, th{};
	gc->GetTextExtent(text, &tw, &th);

	const double padX = 10.0;
	const double padY = 6.0;
	const double badgeW = tw + 2.0 * padX;
	const double badgeH = th + 2.0 * padY;

	const auto perfRect = GetPerformanceOverlayRect(gc);
	const auto overviewRect = GetOverviewOverlayRect();

	double x = GetSize().GetWidth() - badgeW - 18.0;
	double y = 18.0;

	if (perfRect.m_width > 0.0)
		y = perfRect.m_y + perfRect.m_height + 10.0;

	if (overviewRect.m_width > 0.0)
	{
		const double overviewBottom = overviewRect.m_y + overviewRect.m_height;
		y = std::max(y, overviewBottom + 10.0);
	}

	return wxRect2DDouble(x, y, badgeW, badgeH);
}

void cPreviewPanel::InitDefaultComponents()
{
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

		if (ShouldDrawCursorOverlay())
		{
			DrawVerticalLineBelowCursor(gc, m_LUStart, m_RBFinish);
			DrawReferenceValueBelowCursor(gc, m_LUStart, m_RBFinish);
			DrawCapturedValueBelowCursor(gc, m_LUStart, m_RBFinish);
		}

		auto isDarkBackground = IsDarkBackground();

		DrawHorizontalRulerViewport(gc, isDarkBackground);
		DrawVerticalRulerViewport(gc, isDarkBackground);
	}

	DrawOverviewOverlay(gc);

	if (ShouldDrawSummaryOverlay())
	{
		DrawMaxValue(gc);
		DrawSumEvents(gc);
	}

	DrawTemperatureOverlay(gc);

	DrawPerformanceOverlay(gc);

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
	if (!gc || (!m_ImageData && !m_ReferenceData))
		return;

	const double x = m_CursorPosOnCanvas.x;

	gc->SetPen(wxPen(wxColour(255, 255, 255, 40), 1, wxPENSTYLE_DOT));
	gc->StrokeLine(x, luStart.y, x, rbFinish.y);

	gc->SetPen(wxPen(wxColour(0, 220, 255, 120), 2));
	gc->StrokeLine(x, rbFinish.y - 16.0, x, rbFinish.y);

	gc->SetBrush(wxBrush(wxColour(0, 220, 255, 140)));
	gc->SetPen(*wxTRANSPARENT_PEN);
	gc->DrawEllipse(x - 3.0, rbFinish.y - 3.0, 6.0, 6.0);
}

auto cPreviewPanel::DrawCapturedValueBelowCursor(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void
{
	if (!gc || !m_ImageData)
		return;

	int positionInData = static_cast<int>(std::lround(ScreenToDataX(static_cast<int>(m_CursorPosOnCanvas.x))));
	positionInData = std::clamp(positionInData, 0, m_ImageSize.GetWidth() - 1);

	const wxString text = wxString::Format
	(
		wxT("Captured  %.2f keV  |  %lu"),
		static_cast<double>(positionInData) * m_BinSize,
		m_ImageData[positionInData]
	);

	wxFont font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, wxColour(245, 245, 245));

	wxDouble tw{}, th{};
	gc->GetTextExtent(text, &tw, &th);

	const double badgeX = m_CursorPosOnCanvas.x - tw / 2.0 - 10.0;
	const double badgeY = GetCursorInfoTopY(gc) + 34.0;

	DrawOverlayBadge(gc, text, badgeX, badgeY, wxColour(60, 220, 110));

	const double drawY = DataToScreenY(static_cast<double>(m_ImageData[positionInData]));
	gc->SetPen(wxPen(wxColour(60, 220, 110, 210), 2));
	gc->SetBrush(wxBrush(wxColour(60, 220, 110, 170)));
	gc->DrawEllipse(m_CursorPosOnCanvas.x - 4.0, drawY - 4.0, 8.0, 8.0);
}

auto cPreviewPanel::DrawReferenceValueBelowCursor(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void
{
	if (!gc || !m_ReferenceData)
		return;

	int positionInData = static_cast<int>(std::lround(ScreenToDataX(static_cast<int>(m_CursorPosOnCanvas.x))));
	positionInData = std::clamp(positionInData, 0, m_ImageSize.GetWidth() - 1);

	const wxString text = wxString::Format
	(
		wxT("Reference  %.2f keV  |  %lu"),
		static_cast<double>(positionInData) * m_ReferenceBinSize,
		m_ReferenceData[positionInData]
	);

	wxFont font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, wxColour(245, 245, 245));

	wxDouble tw{}, th{};
	gc->GetTextExtent(text, &tw, &th);

	const double badgeX = m_CursorPosOnCanvas.x - tw / 2.0 - 10.0;
	const double badgeY = GetCursorInfoTopY(gc);

	DrawOverlayBadge(gc, text, badgeX, badgeY, wxColour(255, 90, 90));

	const double drawY = DataToScreenY(static_cast<double>(m_ReferenceData[positionInData]));
	gc->SetPen(wxPen(wxColour(255, 90, 90, 210), 2));
	gc->SetBrush(wxBrush(wxColour(255, 90, 90, 170)));
	gc->DrawEllipse(m_CursorPosOnCanvas.x - 4.0, drawY - 4.0, 8.0, 8.0);
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
	if (!gc || !m_ImageData)
		return;

	const wxString text = wxString::Format
	(
		wxT("Peak  %.4f keV  |  %lu"),
		static_cast<double>(m_MaxPosValueInData.first) * m_BinSize,
		m_MaxPosValueInData.second
	);

	DrawOverlayBadge(gc, text, 18.0, 42.0, wxColour(90, 235, 150));
}

auto cPreviewPanel::DrawSumEvents(wxGraphicsContext* gc) -> void
{
	if (!gc || !m_ImageData)
		return;

	const wxString text = wxString::Format(wxT("Events  %llu"), m_SumData);
	DrawOverlayBadge(gc, text, 18.0, 8.0, wxColour(255, 150, 40));
}

auto cPreviewPanel::DrawHorizontalRuler(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void
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

auto cPreviewPanel::DrawVerticalRuler(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void
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

	const double leftMargin = std::max(68.0, GetSize().GetWidth() * 0.10);
	const double topMargin = std::max(18.0, GetSize().GetHeight() * 0.08);
	const double rightMargin = 8.0;
	const double bottomMargin = std::max(62.0, GetSize().GetHeight() * 0.10);

	m_LUStart = { leftMargin, topMargin };
	m_RBFinish =
	{
		std::max(leftMargin + 20.0, GetSize().GetWidth() - rightMargin),
		std::max(topMargin + 20.0, GetSize().GetHeight() - bottomMargin)
	};

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

