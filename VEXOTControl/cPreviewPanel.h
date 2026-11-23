#pragma once
#ifndef CPREVIEWPANEL_H
#define CPREVIEWPANEL_H

#include "wx/wx.h"
#include "wx/dcbuffer.h"
#include "wx/graphics.h"
#include "wx/tglbtn.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

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

	static auto GetRequiredDecimalsFromStep(double step) -> int
	{
		step = std::abs(step);

		if (step <= 0.0)
			return 0;

		if (step >= 1.0)
			return 0;

		return std::clamp(static_cast<int>(std::ceil(-std::log10(step) - 1e-9)), 0, 8);
	}

	static auto FormatEnergyValue(double value, double step) -> wxString
	{
		const int decimals = GetRequiredDecimalsFromStep(step);
		wxString text = wxString::Format(wxT("%.*f"), decimals, value);

		if (text.Find('.') != wxNOT_FOUND)
		{
			while (text.EndsWith("0"))
				text.RemoveLast();

			if (text.EndsWith("."))
				text.RemoveLast();
		}

		return text;
	}

	// Fixed export resolution for SavePNG, independent of whatever size
	// the panel happens to be on screen - so a narrow/resized window never
	// produces a narrow saved graph.
	constexpr int EXPORT_WIDTH = 1920;
	constexpr int EXPORT_HEIGHT = 1080;
}

// Spectrum plot: captured spectrum and an optional reference spectrum,
// drawn against energy (keV). Each spectrum keeps its own length and bin
// size, so a reference recorded with different settings lines up by energy.
//
// Mouse: wheel = zoom energy, Shift+wheel = zoom counts, drag = pan,
// Ctrl+drag = box zoom, click = add marker, double-click = reset view,
// right-click = menu. Keys: Home = reset, L = log scale, P = peak labels,
// F = fill, arrows = pan, +/- = zoom, Delete = clear markers.
class cPreviewPanel final : public wxPanel
{
public:
	cPreviewPanel
	(
		wxWindow* parent,
		wxSizer* sizer,
		std::unique_ptr<PreviewPanelVariables::InputPreviewPanelArgs> inputArgs
	);
	~cPreviewPanel() override;

	auto SetCurrentDevice(const int device) -> void { m_CurrentDevice = device; };

	auto SetKETEKData
	(
		const unsigned long* const mcaData,
		const unsigned long dataSize,
		const unsigned long long sum
	) -> void;

	auto SetKETEKReferenceData
	(
		const unsigned long* const mcaData,
		const unsigned long dataSize,
		const unsigned long long sum
	) -> void;

	bool SavePNG(const wxString& filePath);

	auto SetBackgroundColor(wxColour bckg_colour) -> void;
	auto SetCursorOverlayActive(bool activate = false) -> void;

	// Kept for source compatibility with the old camera preview; no-ops.
	void SetXCrossHairPosFromParentWindow(const int&) {}
	void SetYCrossHairPosFromParentWindow(const int&) {}

	auto SetBinSize(const double binSize) -> void;
	auto SetReferenceBinSize(const double binSize) -> void;

	void SetHardEnergyRange(double minKeV, double maxKeV);
	void ResetHardEnergyRangeToFullData();
	double GetMaxEnergyKeV() const;

	bool HasCustomizedXDomain() const { return HasEffectiveHardRange() || HasEffectiveZoomedViewport(); }

	void SetPerformanceOverlayEnabled(bool enabled, double exposureSeconds = 0.0);
	void NotifyNewFrame(unsigned long long frameNumber);
	void ResetFrameStats();

	void SetBoardTemperature(double temperatureC);
	void SetSDDTemperature(double temperatureC);

public:
	// One spectrum (captured or reference).
	struct Series
	{
		std::vector<unsigned long> counts{};
		double binSizeKeV{ 0.0 };
		unsigned long long sum{ 0 };

		bool HasData() const { return !counts.empty(); }
		int Size() const { return static_cast<int>(counts.size()); }
	};

	struct Viewport
	{
		double xMin{ 0.0 }; // keV
		double xMax{ 1.0 }; // keV
		double yMin{ 0.0 }; // axis units: counts (linear) or log10(1 + counts)
		double yMax{ 1.0 };
	};

	struct Layout
	{
		double scale{ 1.0 };
		wxRect2DDouble header{};
		wxRect2DDouble navigator{}; // empty when not shown
		wxRect2DDouble plot{};
		wxSize size{};
	};

	struct DetectedPeak
	{
		double energyKeV{};
		unsigned long counts{};
		wxString label{}; // e.g. "Fe Ka"; empty if no line matched
	};

	struct Palette
	{
		wxColour plotBackground, plotBorder, grid, gridMinor;
		wxColour text, textMuted;
		wxColour captured, capturedFill, reference;
		wxColour marker, markerText, regionFill;
		wxColour cursor, chipBackground, chipBorder, navigatorShade;
	};

	// Maps energy/axis values into a screen rectangle; used for the main
	// plot and for the overview strip.
	struct Mapping
	{
		wxRect2DDouble rect{};
		double xMin{}, xMax{}, yMin{}, yMax{};
	};

private:
	/* Data and domain */
	auto SetSeriesData(Series& series, const unsigned long* data, unsigned long size, unsigned long long sum) -> void;
	auto EffectiveBinSize(const Series& series) const -> double;
	auto PrimarySeries() const -> const Series*;
	auto DomainMaxKeV() const -> double;
	auto MinimumXRange() const -> double;
	auto ValueAtEnergy(const Series& series, double energyKeV, int* indexOut = nullptr) const -> long long;
	auto MaxInEnergyRange(const Series& series, double minKeV, double maxKeV) const -> unsigned long;
	auto SnapToBin(double energyKeV) const -> double;
	auto OnDataChanged(bool firstData) -> void;

	/* View */
	auto InitializeView() -> void;
	auto ClampView() -> void;
	auto AllowedXMin() const -> double;
	auto AllowedXMax() const -> double;
	auto HardRangeUsable() const -> bool;
	auto NavigatorInner() const -> wxRect2DDouble;
	auto AutoFitY() -> void;
	auto ResetView() -> void;
	bool HasEffectiveHardRange() const;
	bool HasEffectiveZoomedViewport() const;
	auto ZoomX(double factor, double anchorScreenX) -> void;
	auto ZoomY(double factor, double anchorScreenY) -> void;
	auto PanPixels(double dx, double dy) -> bool;

	/* Y axis transform */
	auto ToAxisY(double counts) const -> double;
	auto FromAxisY(double axis) const -> double;

	/* Screen <-> data */
	auto ComputeLayout(const wxSize& size, double scale, bool forExport) const -> Layout;
	auto MainMapping() const -> Mapping;
	auto ScreenToEnergy(double screenX) const -> double;
	auto EnergyToScreen(double energyKeV) const -> double;
	auto ScreenToAxisY(double screenY) const -> double;
	auto AxisYToScreen(double axisY) const -> double;
	auto CountsToScreen(double counts) const -> double;
	auto IsInsidePlot(const wxPoint& point) const -> bool;
	auto IsInsideNavigator(const wxPoint& point) const -> bool;

	/* Rendering */
	void OnPaint(wxPaintEvent& evt);
	void OnSize(wxSizeEvent& evt);
	auto RenderAll(wxGraphicsContext* gc, bool exporting) -> void;
	auto DrawHeader(wxGraphicsContext* gc, bool exporting) -> void;
	auto DrawNavigator(wxGraphicsContext* gc) -> void;
	auto DrawPlotFrameAndGrid(wxGraphicsContext* gc) -> void;
	auto DrawAxes(wxGraphicsContext* gc) -> void;
	auto DrawSeries(wxGraphicsContext* gc, const Series& series, const Mapping& mapping, const wxColour& line, const wxColour& fill, bool drawFill, double lineWidth) -> void;
	auto DrawRegionsOfInterest(wxGraphicsContext* gc) -> void;
	auto DrawMarkers(wxGraphicsContext* gc) -> void;
	auto DrawPeakLabels(wxGraphicsContext* gc) -> void;
	auto DrawCursor(wxGraphicsContext* gc) -> void;
	auto DrawBoxZoom(wxGraphicsContext* gc) -> void;
	auto DrawEmptyState(wxGraphicsContext* gc) -> void;
	auto DrawChip(wxGraphicsContext* gc, double x, double y, const wxString& text, const wxColour& accent, bool alignRight, double maxRight) -> double;
	auto ScheduleRefresh() -> void;
	void OnRefreshTimer(wxTimerEvent& evt);

	auto Font(double pixelSize, bool bold = false) const -> wxFont;
	auto Px(double value) const -> double { return value * m_Layout.scale; }
	auto IsDarkBackground() const -> bool;

	/* Interaction */
	void OnMouseMoved(wxMouseEvent& evt);
	void OnMouseWheel(wxMouseEvent& evt);
	void OnLeftDown(wxMouseEvent& evt);
	void OnLeftUp(wxMouseEvent& evt);
	void OnLeftDoubleClick(wxMouseEvent& evt);
	void OnRightDown(wxMouseEvent& evt);
	void OnMouseLeave(wxMouseEvent& evt);
	void OnCaptureLost(wxMouseCaptureLostEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
	auto EndDrag() -> void;
	auto ShowContextMenu(const wxPoint& position) -> void;
	auto UpdateStatusBar() -> void;

	/* Markers and regions */
	auto AddMarkerAtScreenX(double screenX) -> bool;
	auto FindMarkerNearScreenX(double screenX, double tolerancePixels) const -> int;
	struct RegionStatistics { double fromKeV{}, toKeV{}; unsigned long long gross{}; double net{}; bool valid{ false }; };
	auto ComputeRegion(double fromKeV, double toKeV) const -> RegionStatistics;

	/* Peaks and element lines */
	auto DetectPeaks() -> void;
	auto PeakNear(double energyKeV) const -> const DetectedPeak*;
	auto NearestLineLabel(double energyKeV, double toleranceKeV, double* lineEnergyKeV = nullptr) const -> wxString;

	/* Export */
	auto ExportVisibleCsv() -> void;

private:
	std::unique_ptr<PreviewPanelVariables::InputPreviewPanelArgs> m_ParentArguments{};
	int m_CurrentDevice{};

	Series m_Captured{};
	Series m_Reference{};
	double m_CapturedBinSizeSetting{ 0.0 };
	double m_ReferenceBinSizeSetting{ 0.0 };

	Viewport m_View{};
	bool m_ViewInitialized{ false };
	bool m_AutoY{ true };
	bool m_LogScale{ false };
	bool m_FillCaptured{ true };
	bool m_ShowPeakLabels{ false };

	bool m_HardXRangeEnabled{ false };
	// "Full data" is remembered as such, not as the energy it had at the
	// time, so it follows the data when the bin size or length changes.
	bool m_HardXRangeIsFull{ false };
	double m_HardXMinKeV{ 0.0 };
	double m_HardXMaxKeV{ 0.0 };
	// Allowed range at the last ClampView(), to tell whether the view was
	// showing everything before the data changed.
	double m_LastAllowedMinKeV{ 0.0 };
	double m_LastAllowedMaxKeV{ 0.0 };

	/* Double-click handling: the click before a double-click adds a marker */
	bool m_LastClickAddedMarker{ false };
	double m_LastClickMarkerKeV{ 0.0 };

	/* Status bar message that mouse moves must not overwrite right away */
	std::chrono::steady_clock::time_point m_StatusMessageUntil{};

	Layout m_Layout{};
	Palette m_Pal{};

	/* Mouse state */
	wxPoint m_CursorPos{ -1, -1 };
	bool m_CursorInside{ false };
	bool m_RenderCursorOverlay{ false };
	enum class DragMode { None, Pending, Pan, BoxZoom, Navigator };
	DragMode m_DragMode{ DragMode::None };
	wxPoint m_DragStart{};
	wxPoint m_LastMousePos{};
	static constexpr int ClickDragThresholdPixels = 4;

	/* Markers (energies in keV) */
	static constexpr std::size_t MaxMarkerCount = 10;
	std::vector<double> m_Markers{};

	std::vector<DetectedPeak> m_Peaks{};

	/* Header information */
	unsigned long long m_CurrentFrameNumber{ 0 };
	double m_DisplayedFPS{ 0.0 };
	std::chrono::steady_clock::time_point m_LastFrameTime{};
	bool m_HasLastFrameTime{ false };
	bool m_ShowPerformanceOverlay{ false };
	double m_ExposureSecondsForFPS{ 0.0 };

	double m_CurrentBoardTemperatureC{ 0.0 };
	bool m_HasBoardTemperature{ false };
	double m_CurrentSDDTemperatureC{ 0.0 };
	bool m_HasSDDTemperature{ false };

	/* Refresh throttling */
	wxTimer m_RefreshTimer{};
	std::chrono::steady_clock::time_point m_LastPaint{};
	static constexpr int MinRefreshIntervalMs = 33;

	wxString m_LastStatusText{};
};

#endif // !CPREVIEWPANEL_H
