#pragma once
#ifndef CPREVIEWPANEL_H
#define CPREVIEWPANEL_H

#include "wx/wx.h"
#include "wx/dcbuffer.h"
#include "wx/graphics.h"
#include "wx/tglbtn.h"

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <format>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "cPreviewTools.h"
//#include "XimeaControl.h"

// Macros
#ifdef _DEBUG
#define LOG(message) wxLogDebug(message);
#define LOGST(message, number) wxLogDebug(message + wxString::Format(wxT("%i"), number))
#define LOGF(message, number) wxLogDebug(message + wxString::Format(wxT("%.2f"), number))
#define LOGI(message, number) wxLogDebug(message + wxString::Format(wxT("%i"), number))
#define LOG2I(first_message, first_number, second_message, second_number) wxLogDebug(first_message + wxString::Format(wxT("%i"), first_number) + second_message + wxString::Format(wxT("%i"), second_number))
#define LOG2F(first_message, first_number, second_message, second_number) wxLogDebug(first_message + wxString::Format(wxT("%.2f"), first_number) + second_message + wxString::Format(wxT("%.2f"), second_number))
#define LOG6F(first_message, first_number, second_message, second_number, third_message, third_number, fourth_message, fourth_number, fifth_message, fifth_number, sixth_message, sixth_number) wxLogDebug(first_message + wxString::Format(wxT("%.2f"), first_number) + second_message + wxString::Format(wxT("%.2f"), second_number) + third_message + wxString::Format(wxT("%.2f"), third_number) + fourth_message + wxString::Format(wxT("%.2f"), fourth_number) + fifth_message + wxString::Format(wxT("%.2f"), fifth_number) + sixth_message + wxString::Format(wxT("%.2f"), sixth_number))
#else
#define LOG(message)
#define LOGST(message, number)
#define LOGF(message, number)
#define LOGI(message, number)
#define LOG2I(first_message, first_number, second_message, second_number) 
#define LOG2F(first_message, first_number, second_message, second_number) 
#define LOG6F(first_message, first_number, second_message, second_number, third_message, third_number, fourth_message, fourth_number, fifth_message_fifth_number, sixth_message, sixth_number)
#endif // _DEBUG

namespace PreviewPanelVariables
{
	enum Device
	{
		XPIN,
		KETEK,
		TIMEPIX_ADVACAM,
		FLI_CAMERA
	};

	struct InputPreviewPanelArgs
	{
		wxTextCtrl* desired_left_border_range_txt_ctrl{}, * desired_right_border_range_txt_ctrl{};
		wxStatusBar* status_bar{};

		InputPreviewPanelArgs() {};

		InputPreviewPanelArgs
		(
			wxStatusBar* status_bar,
			wxTextCtrl* desired_left_border_range_txt_ctrl,
			wxTextCtrl* desired_right_border_range_txt_ctrl
		) : status_bar(status_bar),
		desired_left_border_range_txt_ctrl(desired_left_border_range_txt_ctrl),
		desired_right_border_range_txt_ctrl(desired_right_border_range_txt_ctrl)
		{};
	};

	static auto CreateStringWithPrecision(double value, int decimalPlaces = 0) -> wxString
	{
		std::ostringstream stream;
		if (decimalPlaces > 0)
		{
			stream.precision(decimalPlaces);
			stream << std::fixed;
		}
		stream << value;
		return wxString::FromUTF8(stream.str());
	};
}

class cPreviewPanel final : public wxPanel
{
public:
	cPreviewPanel
	(
		wxWindow* parent, 
		wxSizer* sizer, 
		std::unique_ptr<PreviewPanelVariables::InputPreviewPanelArgs> inputArgs
	);
	auto SetCurrentDevice(const int device) -> void { m_CurrentDevice = device; };
	auto SetKETEKData
	(
		unsigned long* const mcaData,
		const unsigned long dataSize,
		const unsigned long long sum
	) -> void;

	auto SetKETEKReferenceData
	(
		unsigned long* const mcaData,
		const unsigned long dataSize,
		const unsigned long long sum
	) -> void;

	bool SavePNG(const wxString& filePath);

	auto SetBackgroundColor(wxColour bckg_colour) -> void;
	auto SetCrossHairButtonActive(bool activate = false) -> void;
	auto SetValueDisplayingActive(bool activate = false) -> void;
	void SetXCrossHairPosFromParentWindow(const int& x_pos);
	void SetYCrossHairPosFromParentWindow(const int& y_pos);
	auto SettingCrossHairPosFromParentWindow(bool set = false) -> void;
	auto SetImageSize() -> wxSize { return m_Image.GetSize(); };
	auto InitializeSelectedCamera(const std::string& camera_sn) -> void;
	auto SetCameraCapturedImage() -> void;
	void CaptureAndSaveDataFromCamera
	(
		const unsigned long& exposure_time_us, 
		const wxString& path, 
		const std::string& start_hours,
		const std::string& start_minutes,
		const std::string& start_seconds,
		const int& frame_number, 
		const float& first_axis_position, 
		const float& second_axis_position = 0.f
	);
	void CalculateMatlabJetColormapPixelRGB8bit
	(
		const unsigned char& value, 
		unsigned char& r, 
		unsigned char& g, 
		unsigned char& b
	);
	void CalculateMatlabJetColormapPixelRGB12bit
	(
		const unsigned short& value, 
		unsigned char& r, 
		unsigned char& g, 
		unsigned char& b
	);

	auto SetBinSize(const double binSize) -> void { m_BinSize = binSize; };
	auto SetReferenceBinSize(const double binSize) -> void { m_ReferenceBinSize = binSize; };

	void SetHardEnergyRange(double minKeV, double maxKeV);
	void ResetHardEnergyRangeToFullData();
	double GetMaxEnergyKeV() const;

	bool HasCustomizedXDomain() const { return HasEffectiveHardRange() || HasEffectiveZoomedViewport(); }

	void SetPerformanceOverlayEnabled(bool enabled, double exposureSeconds = 0.0);
	void NotifyNewFrame(unsigned long long frameNumber);
	void ResetFrameStats();

private:
	void InitDefaultComponents();
	void PaintEvent(wxPaintEvent& evt);
	void Render(wxBufferedPaintDC& dc);
	void DrawImage(wxGraphicsContext* gc);
	auto AdjustKETEKImageMultithread
	(
		const unsigned long* const data, 
		const double multiplicationValue,
		const int startX, 
		const int startY, 
		const int finishX, 
		const int finishY
	) -> void;
	void CreateGraphicsBitmapImage(wxGraphicsContext* gc_);

	auto DrawVerticalLineBelowCursor(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void;
	auto DrawCapturedValueBelowCursor(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void;
	auto DrawReferenceValueBelowCursor(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void;
	void DrawCapturedData(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish);
	void DrawReferenceData(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish);
	auto DrawMaxValue(wxGraphicsContext* gc) -> void;
	auto DrawSumEvents(wxGraphicsContext* gc) -> void;
	auto DrawHorizontalRuler(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void;
	auto DrawVerticalRuller(wxGraphicsContext* gc, const wxRealPoint luStart, const wxRealPoint rbFinish) -> void;
	void OnSize(wxSizeEvent& evt);
	void ChangeSizeOfImageInDependenceOnCanvasSize();
	auto UpdateCrossHairOnSize() -> void;
	void CalculateMatlabJetColormapPixelRGB16bit(const uint16_t& value, unsigned char& r, unsigned char& g, unsigned char& b);
	void OnMouseMoved(wxMouseEvent& evt);
	/* Zooming */
	void OnMouseWheelMoved(wxMouseEvent& evt);
	void AddZoom(const double& zoom, bool zoom_in_center_of_window = false);
	void SetZoom(const double& zoom, const wxRealPoint& center_);
	/* Panning */
	void ProcessPan(const wxRealPoint& point_, bool refresh_);
	void FinishPan(bool refresh);

	void CheckIfMouseAboveImage();
	void CalculatePositionOnImage();

	void OnPreviewMouseLeftPressed(wxMouseEvent& evt);
	void OnPreviewMouseLeftReleased(wxMouseEvent& evt);

	void ChangeCursorInDependenceOfCurrentParameters();

	auto UpdateStatusBarWithCursorPosition() -> void;

	/* CrossHair */
	void DrawCrossHair(wxGraphicsContext* graphics_context);

private:
	void InitializeView();
	void ClampView();
	double GetDataWidth() const { return static_cast<double>(m_ImageSize.GetWidth()); }
	double GetVisibleXRange() const { return m_View.xMax - m_View.xMin; }
	double GetVisibleYRange() const { return m_View.yMax - m_View.yMin; }

	double ScreenToDataX(int screenX) const;
	double ScreenToDataY(int screenY) const;
	double DataToScreenX(double dataX) const;
	double DataToScreenY(double dataY) const;

	void ZoomX(double factor, int anchorScreenX);
	void ZoomY(double factor, int anchorScreenY);
	void PanPixels(int dx, int dy);

	void DrawCapturedDataViewport(wxGraphicsContext* gc);
	void DrawReferenceDataViewport(wxGraphicsContext* gc);
	void DrawHorizontalRulerViewport(wxGraphicsContext* gc);
	void DrawVerticalRulerViewport(wxGraphicsContext* gc);

	bool ShouldDrawOverviewOverlay() const;
	bool HasEffectiveHardRange() const;
	bool HasEffectiveZoomedViewport() const;
	void DrawOverviewOverlay(wxGraphicsContext* gc);

	double GetOverviewYMax() const;
	double GetMaxValueInRange(const unsigned long* data, int startIdx, int endIdx) const;

	void DrawPerformanceOverlay(wxGraphicsContext* gc);
	double GetDisplayedFPS() const { return m_DisplayedFPS; }

	wxRect2DDouble GetPerformanceOverlayRect(wxGraphicsContext* gc) const;
	wxRect2DDouble GetOverviewOverlayRect() const;
	double GetCursorInfoTopY(wxGraphicsContext* gc) const;

	void DrawOverlayBadge
	(
		wxGraphicsContext* gc,
		const wxString& text,
		double x,
		double y,
		const wxColour& accent,
		const wxColour& textColour = wxColour(245, 245, 245, 235),
		double opacityScale = 1.0
	) const;

	bool ShouldDrawCursorOverlay() const;
	bool ShouldDrawSummaryOverlay() const;

	double GetCurrentGraphYMax() const;
	void RefreshYView(bool preserveUserZoom = false);

private:
	int m_Width{}, m_Height{};
	bool m_IsGraphicsBitmapSet{}, m_IsImageSet{};

	wxRealPoint m_LUStart{ GetSize().GetWidth() / 10.0, GetSize().GetHeight() / 10.0 };
	wxRealPoint m_RBFinish{ GetSize().GetWidth() * 9 / 10.0, GetSize().GetHeight() * 9 / 10.0 };

	wxImage m_Image{};

	wxGraphicsBitmap m_GraphicsBitmapImage{};
	std::unique_ptr<unsigned long[]> m_ImageData{}, m_ReferenceData{};

	unsigned long m_MaxEventsCountOnGraph{ 10 };
	std::pair<unsigned short, unsigned long> m_MaxPosValueInData{};
	unsigned long long m_SumData{};
	double m_BinSize{}, m_ReferenceBinSize{};
	
	wxSize m_ImageSize{}, m_ImageOnCanvasSize{}, m_CanvasSize{};
	wxRealPoint m_NotCheckedCursorPosOnImage{}, m_CheckedCursorPosOnImage{}, m_CursorPosOnCanvas{};
	wxRealPoint m_NotZoomedGraphicsBitmapOffset{}, m_StartDrawPos{};
	bool m_IsCursorInsideImage{};
	/* Panning */
	bool m_Panning{};
	wxRealPoint m_PanOffset{}, m_PanStartPoint{}, m_PanDeltaPoints{};

	/* Zoom */
	double m_Zoom{}, m_ZoomOnOriginalSizeImage{};

	//std::unique_ptr<XimeaControl> m_XimeaCameraControl{};
	std::string m_SelectedCameraSN{};

	/* CrossHair Tool */
	//std::unique_ptr<CrossHairTool> m_CrossHairTool{};
	wxPoint m_CrossHairPos{};
	bool m_ChangingCrossHairPosition{};
	//bool m_SettingCrossHairPos{};

	bool m_DisplayPixelValues{};

	std::unique_ptr<PreviewPanelVariables::InputPreviewPanelArgs> m_ParentArguments{};

	int m_CurrentDevice{};

	struct Viewport
	{
		double xMin{ 0.0 };
		double xMax{ 1.0 };
		double yMin{ 0.0 };
		double yMax{ 1.0 };
	};

	Viewport m_View{};
	bool m_ViewInitialized{ false };

	wxPoint m_LastMousePos{};
	bool m_IsDragging{ false };

	bool m_HardXRangeEnabled{ false };
	double m_HardXMinData{ 0.0 };
	double m_HardXMaxData{ 1.0 };

	unsigned long long m_CurrentFrameNumber{ 0 };
	double m_DisplayedFPS{ 0.0 };
	std::chrono::steady_clock::time_point m_LastFrameTime{};
	bool m_HasLastFrameTime{ false };

	bool m_ShowPerformanceOverlay{ false };
	double m_ExposureSecondsForFPS{ 0.0 };

	DECLARE_EVENT_TABLE();
};

#endif // !CPREVIEWPANEL_H

