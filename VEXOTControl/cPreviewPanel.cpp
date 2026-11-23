#include "cPreviewPanel.h"
#include "XRayLines.h"
#include "AppLog.h"

#include "wx/filedlg.h"
#include "wx/menu.h"

#include <iomanip>
#include <limits>
#include <tuple>

namespace
{
	// Picks a "nice" round number close to the given range (Heckbert, "Nice
	// Numbers for Graph Labels"): 1, 2, 5 or 10 x 10^n.
	double NiceNum(double range, bool round)
	{
		if (range <= 0.0) return 0.0;

		const double exponent = std::floor(std::log10(range));
		const double fraction = range / std::pow(10.0, exponent);
		double niceFraction;

		if (round)
		{
			if (fraction < 1.5)      niceFraction = 1.0;
			else if (fraction < 3.0) niceFraction = 2.0;
			else if (fraction < 7.0) niceFraction = 5.0;
			else                     niceFraction = 10.0;
		}
		else
		{
			if (fraction <= 1.0)      niceFraction = 1.0;
			else if (fraction <= 2.0) niceFraction = 2.0;
			else if (fraction <= 5.0) niceFraction = 5.0;
			else                      niceFraction = 10.0;
		}

		return niceFraction * std::pow(10.0, exponent);
	}

	// std::clamp with lo > hi is undefined (and asserts in Debug); that
	// happens for layout values when the panel is very small.
	template <class T>
	T ClampSafe(T value, T lo, T hi)
	{
		return hi < lo ? lo : std::clamp(value, lo, hi);
	}

	// Round-number ticks inside [dataMin, dataMax].
	std::vector<double> BuildTicks(double dataMin, double dataMax, int maxTickCount)
	{
		std::vector<double> ticks;

		if (!(dataMax > dataMin) || maxTickCount < 2)
			return ticks;

		const double step = NiceNum((dataMax - dataMin) / static_cast<double>(maxTickCount - 1), true);
		if (step <= 0.0 || !std::isfinite(step))
			return ticks;

		const double first = std::ceil(dataMin / step - 1e-9) * step;
		for (double t = first; t <= dataMax + step * 1e-9 && ticks.size() < 64; t += step)
			ticks.push_back(std::abs(t) < step * 1e-9 ? 0.0 : t);

		return ticks;
	}

	// Count-axis labels: "1k", "2.5k" for round thousands, otherwise digits
	// with apostrophe separators ("5'290").
	wxString FormatEventsTickLabel(double value)
	{
		const long long rounded = static_cast<long long>(std::llround(value));
		const bool negative = rounded < 0;
		const unsigned long long absVal = static_cast<unsigned long long>(negative ? -rounded : rounded);

		wxString result;

		if (absVal >= 1'000'000ULL && absVal % 100'000ULL == 0ULL)
		{
			wxString mText = wxString::Format(wxT("%.1f"), static_cast<double>(absVal) / 1'000'000.0);
			if (mText.EndsWith(".0"))
				mText.RemoveLast(2);
			result = mText + "M";
		}
		else if (absVal >= 1000ULL && absVal % 100ULL == 0ULL)
		{
			wxString kText = wxString::Format(wxT("%.2f"), static_cast<double>(absVal) / 1000.0);

			while (kText.EndsWith("0"))
				kText.RemoveLast();

			if (kText.EndsWith("."))
				kText.RemoveLast();

			result = kText + "k";
		}
		else
		{
			const wxString digits = wxString::Format(wxT("%llu"), absVal);
			int sinceSeparator = 0;

			for (int i = static_cast<int>(digits.length()) - 1; i >= 0; --i)
			{
				result = wxString(digits[i]) + result;
				if (++sinceSeparator == 3 && i != 0)
				{
					result = "'" + result;
					sinceSeparator = 0;
				}
			}
		}

		return negative ? ("-" + result) : result;
	}

	// Full count with apostrophe separators: 1'234'567.
	wxString FormatThousands(unsigned long long value)
	{
		const wxString digits = wxString::Format(wxT("%llu"), value);
		wxString grouped;
		int sinceSeparator = 0;

		for (int i = static_cast<int>(digits.length()) - 1; i >= 0; --i)
		{
			grouped = wxString(digits[i]) + grouped;
			if (++sinceSeparator == 3 && i != 0)
			{
				grouped = "'" + grouped;
				sinceSeparator = 0;
			}
		}

		return grouped;
	}

	// Compact count: 1.2k, 3.4M.
	wxString FormatCompact(unsigned long long value)
	{
		const auto withSuffix = [](double scaled, const wxString& suffix)
			{
				wxString text = wxString::Format(wxT("%.1f"), scaled);
				if (text.EndsWith(".0"))
					text.RemoveLast(2);
				return text + suffix;
			};

		const double v = static_cast<double>(value);
		if (v >= 1e12) return withSuffix(v / 1e12, "T");
		if (v >= 1e9)  return withSuffix(v / 1e9, "B");
		if (v >= 1e6)  return withSuffix(v / 1e6, "M");
		if (v >= 1e3)  return withSuffix(v / 1e3, "k");
		return wxString::Format(wxT("%llu"), value);
	}

	const wxString GreekAlpha = wxString(L"\u03B1");
	const wxString GreekBeta = wxString(L"\u03B2");
	const wxString SigmaSign = wxString(L"\u03A3");
	const wxString DegreeC = wxString(L"\u00B0C");
	const wxString Approx = wxString(L"\u2248");
	const wxString MiddleDot = wxString(L"\u00B7");

	cPreviewPanel::Palette MakePalette(bool dark)
	{
		cPreviewPanel::Palette p{};

		if (dark)
		{
			p.plotBackground = wxColour(28, 32, 38);
			p.plotBorder = wxColour(255, 255, 255, 46);
			p.grid = wxColour(255, 255, 255, 22);
			p.gridMinor = wxColour(255, 255, 255, 10);
			p.text = wxColour(226, 230, 236);
			p.textMuted = wxColour(156, 165, 176);
			p.captured = wxColour(74, 210, 134);
			p.capturedFill = wxColour(74, 210, 134, 46);
			p.reference = wxColour(242, 102, 102);
			p.marker = wxColour(255, 193, 70);
			p.markerText = wxColour(32, 26, 10);
			p.regionFill = wxColour(255, 193, 70, 36);
			p.cursor = wxColour(72, 196, 255);
			p.chipBackground = wxColour(28, 32, 38, 235);
			p.chipBorder = wxColour(255, 255, 255, 34);
			p.navigatorShade = wxColour(0, 0, 0, 120);
		}
		else
		{
			p.plotBackground = wxColour(255, 255, 255);
			p.plotBorder = wxColour(0, 0, 0, 50);
			p.grid = wxColour(0, 0, 0, 20);
			p.gridMinor = wxColour(0, 0, 0, 9);
			p.text = wxColour(34, 38, 46);
			p.textMuted = wxColour(96, 104, 116);
			p.captured = wxColour(20, 150, 82);
			p.capturedFill = wxColour(20, 150, 82, 40);
			p.reference = wxColour(206, 52, 52);
			p.marker = wxColour(214, 136, 0);
			p.markerText = wxColour(255, 255, 255);
			p.regionFill = wxColour(214, 136, 0, 30);
			p.cursor = wxColour(0, 120, 210);
			p.chipBackground = wxColour(255, 255, 255, 240);
			p.chipBorder = wxColour(0, 0, 0, 34);
			p.navigatorShade = wxColour(110, 118, 130, 70);
		}

		return p;
	}

	wxGraphicsPen MakePen(wxGraphicsContext* gc, const wxColour& colour, double width, wxPenStyle style = wxPENSTYLE_SOLID)
	{
		return gc->CreatePen(wxGraphicsPenInfo(colour, width, style).Join(wxJOIN_ROUND).Cap(wxCAP_ROUND));
	}

	enum ContextMenuId
	{
		MenuRemoveMarker = wxID_HIGHEST + 4100,
		MenuClearMarkers,
		MenuLogScale,
		MenuFill,
		MenuPeaks,
		MenuResetView,
		MenuExportCsv,
		MenuSavePng
	};
}

/* ======================= Construction ======================= */

cPreviewPanel::cPreviewPanel
(
	wxWindow* parent,
	wxSizer* sizer,
	std::unique_ptr<PreviewPanelVariables::InputPreviewPanelArgs> inputArgs
)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxWANTS_CHARS)
{
	m_ParentArguments = std::move(inputArgs);

	// Both are needed to repaint without flicker on every mouse move (the
	// cursor readout): no background erase, and Windows-composited double
	// buffering on top of the buffered paint DC, as the old panel had.
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetDoubleBuffered(true);

	sizer->Add(this, 1, wxEXPAND);

	Bind(wxEVT_PAINT, &cPreviewPanel::OnPaint, this);
	Bind(wxEVT_SIZE, &cPreviewPanel::OnSize, this);
	Bind(wxEVT_MOTION, &cPreviewPanel::OnMouseMoved, this);
	Bind(wxEVT_MOUSEWHEEL, &cPreviewPanel::OnMouseWheel, this);
	Bind(wxEVT_LEFT_DOWN, &cPreviewPanel::OnLeftDown, this);
	Bind(wxEVT_LEFT_UP, &cPreviewPanel::OnLeftUp, this);
	Bind(wxEVT_LEFT_DCLICK, &cPreviewPanel::OnLeftDoubleClick, this);
	Bind(wxEVT_RIGHT_DOWN, &cPreviewPanel::OnRightDown, this);
	Bind(wxEVT_LEAVE_WINDOW, &cPreviewPanel::OnMouseLeave, this);
	Bind(wxEVT_MOUSE_CAPTURE_LOST, &cPreviewPanel::OnCaptureLost, this);
	Bind(wxEVT_KEY_DOWN, &cPreviewPanel::OnKeyDown, this);

	m_RefreshTimer.SetOwner(this);
	Bind(wxEVT_TIMER, &cPreviewPanel::OnRefreshTimer, this, m_RefreshTimer.GetId());

	m_Layout = ComputeLayout(GetClientSize(), GetDPIScaleFactor(), false);
}

cPreviewPanel::~cPreviewPanel()
{
	m_RefreshTimer.Stop();
}

/* ======================= Data ======================= */

auto cPreviewPanel::SetSeriesData(Series& series, const unsigned long* data, unsigned long size, unsigned long long sum) -> void
{
	if (!data || size == 0)
	{
		series.counts.clear();
		series.sum = 0;
		return;
	}

	series.counts.assign(data, data + size);
	series.sum = sum;
}

auto cPreviewPanel::SetKETEKData
(
	const unsigned long* const mcaData,
	const unsigned long dataSize,
	const unsigned long long sum
) -> void
{
	if (!mcaData)
		return;

	const bool firstData = !m_Captured.HasData() && !m_Reference.HasData();
	SetSeriesData(m_Captured, mcaData, dataSize, sum);
	m_Captured.binSizeKeV = m_CapturedBinSizeSetting;
	OnDataChanged(firstData);
}

auto cPreviewPanel::SetKETEKReferenceData
(
	const unsigned long* const mcaData,
	const unsigned long dataSize,
	const unsigned long long sum
) -> void
{
	if (!mcaData)
		return;

	const bool firstData = !m_Captured.HasData() && !m_Reference.HasData();
	SetSeriesData(m_Reference, mcaData, dataSize, sum);
	m_Reference.binSizeKeV = m_ReferenceBinSizeSetting;
	OnDataChanged(firstData);
}

auto cPreviewPanel::SetBinSize(const double binSize) -> void
{
	m_CapturedBinSizeSetting = binSize;
	m_Captured.binSizeKeV = binSize;

	if (m_ViewInitialized)
		OnDataChanged(false);
}

auto cPreviewPanel::SetReferenceBinSize(const double binSize) -> void
{
	m_ReferenceBinSizeSetting = binSize;
	m_Reference.binSizeKeV = binSize;

	if (m_ViewInitialized)
		OnDataChanged(false);
}

auto cPreviewPanel::OnDataChanged(bool firstData) -> void
{
	if (!PrimarySeries())
	{
		ScheduleRefresh();
		return;
	}

	// If the view showed the whole allowed range before, keep showing the
	// whole range after the data changed (e.g. a different bin size).
	const double eps = 1e-9 * std::max(1.0, m_LastAllowedMaxKeV);
	const bool viewWasFull = m_ViewInitialized &&
		std::abs(m_View.xMin - m_LastAllowedMinKeV) <= eps &&
		std::abs(m_View.xMax - m_LastAllowedMaxKeV) <= eps;

	if (!m_ViewInitialized || firstData)
	{
		InitializeView();
	}
	else if (viewWasFull)
	{
		m_View.xMin = AllowedXMin();
		m_View.xMax = AllowedXMax();
		ClampView();
	}
	else
	{
		ClampView();
	}

	// Peaks are kept up to date even when labels are off: the cursor
	// readout uses them to suggest element lines.
	DetectPeaks();

	m_LastStatusText.clear();
	UpdateStatusBar();
	ScheduleRefresh();
}

auto cPreviewPanel::EffectiveBinSize(const Series& series) const -> double
{
	if (series.binSizeKeV > 0.0)
		return series.binSizeKeV;

	// A spectrum without its own bin size is drawn with the other one's.
	const Series& other = (&series == &m_Captured) ? m_Reference : m_Captured;
	return other.binSizeKeV > 0.0 ? other.binSizeKeV : 0.0;
}

auto cPreviewPanel::PrimarySeries() const -> const Series*
{
	if (m_Captured.HasData() && EffectiveBinSize(m_Captured) > 0.0)
		return &m_Captured;

	if (m_Reference.HasData() && EffectiveBinSize(m_Reference) > 0.0)
		return &m_Reference;

	return nullptr;
}

auto cPreviewPanel::DomainMaxKeV() const -> double
{
	double maxKeV = 0.0;

	for (const Series* series : { &m_Captured, &m_Reference })
	{
		const double bin = EffectiveBinSize(*series);
		if (series->Size() > 1 && bin > 0.0)
			maxKeV = std::max(maxKeV, static_cast<double>(series->Size() - 1) * bin);
	}

	return maxKeV;
}

auto cPreviewPanel::MinimumXRange() const -> double
{
	const Series* primary = PrimarySeries();
	return primary ? 10.0 * EffectiveBinSize(*primary) : 0.01;
}

auto cPreviewPanel::ValueAtEnergy(const Series& series, double energyKeV, int* indexOut) const -> long long
{
	const double bin = EffectiveBinSize(series);
	if (!series.HasData() || bin <= 0.0 || !std::isfinite(energyKeV))
		return -1;

	const long long index = std::llround(energyKeV / bin);
	if (index < 0 || index >= series.Size())
		return -1;

	if (indexOut)
		*indexOut = static_cast<int>(index);

	return static_cast<long long>(series.counts[static_cast<std::size_t>(index)]);
}

auto cPreviewPanel::MaxInEnergyRange(const Series& series, double minKeV, double maxKeV) const -> unsigned long
{
	const double bin = EffectiveBinSize(series);
	if (!series.HasData() || bin <= 0.0)
		return 0;

	const int first = std::max(0, static_cast<int>(std::floor(minKeV / bin)));
	const int last = std::min(series.Size() - 1, static_cast<int>(std::ceil(maxKeV / bin)));

	unsigned long maxValue = 0;
	for (int i = first; i <= last; ++i)
		maxValue = std::max(maxValue, series.counts[static_cast<std::size_t>(i)]);

	return maxValue;
}

auto cPreviewPanel::SnapToBin(double energyKeV) const -> double
{
	const Series* primary = PrimarySeries();
	if (!primary)
		return energyKeV;

	const double bin = EffectiveBinSize(*primary);
	return std::round(energyKeV / bin) * bin;
}

/* ======================= View ======================= */

auto cPreviewPanel::HardRangeUsable() const -> bool
{
	if (!m_HardXRangeEnabled || m_HardXRangeIsFull)
		return false;

	// A range that no longer overlaps the data (e.g. after a shorter
	// spectrum arrived) is ignored rather than collapsing the view.
	const double domain = DomainMaxKeV();
	const double minKeV = ClampSafe(m_HardXMinKeV, 0.0, domain);
	const double maxKeV = ClampSafe(m_HardXMaxKeV, 0.0, domain);
	return maxKeV - minKeV >= MinimumXRange() * 0.1;
}

auto cPreviewPanel::AllowedXMin() const -> double
{
	return HardRangeUsable() ? ClampSafe(m_HardXMinKeV, 0.0, DomainMaxKeV()) : 0.0;
}

auto cPreviewPanel::AllowedXMax() const -> double
{
	const double domain = DomainMaxKeV();
	const double maxKeV = HardRangeUsable() ? ClampSafe(m_HardXMaxKeV, 0.0, domain) : domain;
	return std::max(maxKeV, AllowedXMin() + 1e-6);
}

auto cPreviewPanel::InitializeView() -> void
{
	if (!PrimarySeries())
		return;

	m_View.xMin = AllowedXMin();
	m_View.xMax = AllowedXMax();
	m_ViewInitialized = true;
	m_AutoY = true;

	ClampView();
}

auto cPreviewPanel::ResetView() -> void
{
	if (!PrimarySeries())
		return;

	m_AutoY = true;
	m_View.xMin = AllowedXMin();
	m_View.xMax = AllowedXMax();
	ClampView();
	UpdateStatusBar();
	ScheduleRefresh();
}

auto cPreviewPanel::ClampView() -> void
{
	const double allowedMin = AllowedXMin();
	const double allowedMax = AllowedXMax();
	const double allowedRange = std::max(1e-6, allowedMax - allowedMin);

	double range = m_View.xMax - m_View.xMin;
	if (!std::isfinite(range) || !std::isfinite(m_View.xMin) || range <= 0.0)
	{
		m_View.xMin = allowedMin;
		range = allowedRange;
	}

	range = ClampSafe(range, std::min(MinimumXRange(), allowedRange), allowedRange);

	m_View.xMin = ClampSafe(m_View.xMin, allowedMin, std::max(allowedMin, allowedMax - range));
	m_View.xMax = m_View.xMin + range;

	m_LastAllowedMinKeV = allowedMin;
	m_LastAllowedMaxKeV = allowedMax;

	if (m_AutoY)
	{
		AutoFitY();
	}
	else
	{
		const double minYRange = m_LogScale ? 0.3 : 5.0;

		if (!std::isfinite(m_View.yMin) || !std::isfinite(m_View.yMax) || m_View.yMax - m_View.yMin < 1e-9)
		{
			m_AutoY = true;
			AutoFitY();
			return;
		}

		double yRange = std::max(minYRange, m_View.yMax - m_View.yMin);
		m_View.yMin = std::max(0.0, m_View.yMin);
		m_View.yMax = m_View.yMin + yRange;
	}
}

auto cPreviewPanel::AutoFitY() -> void
{
	unsigned long maxVisible = 0;

	for (const Series* series : { &m_Captured, &m_Reference })
		maxVisible = std::max(maxVisible, MaxInEnergyRange(*series, m_View.xMin, m_View.xMax));

	m_View.yMin = 0.0;

	if (m_LogScale)
		m_View.yMax = std::max(ToAxisY(10.0), ToAxisY(static_cast<double>(maxVisible)) + 0.2);
	else
		m_View.yMax = std::max(10.0, static_cast<double>(maxVisible) * 1.08);
}

bool cPreviewPanel::HasEffectiveHardRange() const
{
	const double domain = DomainMaxKeV();
	if (!HardRangeUsable() || domain <= 0.0)
		return false;

	const Series* primary = PrimarySeries();
	const double eps = primary ? 0.5 * EffectiveBinSize(*primary) : 1e-6;

	return AllowedXMin() > eps || AllowedXMax() < domain - eps;
}

bool cPreviewPanel::HasEffectiveZoomedViewport() const
{
	if (!m_ViewInitialized || !PrimarySeries())
		return false;

	const double eps = 0.5 * EffectiveBinSize(*PrimarySeries());

	return std::abs(m_View.xMin - AllowedXMin()) > eps ||
		std::abs(m_View.xMax - AllowedXMax()) > eps;
}

void cPreviewPanel::SetHardEnergyRange(double minKeV, double maxKeV)
{
	const double domain = DomainMaxKeV();
	if (domain <= 0.0)
		return;

	if (minKeV > maxKeV)
		std::swap(minKeV, maxKeV);

	minKeV = ClampSafe(minKeV, 0.0, domain);
	maxKeV = ClampSafe(maxKeV, 0.0, domain);

	if (maxKeV - minKeV < MinimumXRange() * 0.1)
		return;

	m_HardXRangeEnabled = true;
	m_HardXRangeIsFull = false;
	m_HardXMinKeV = minKeV;
	m_HardXMaxKeV = maxKeV;

	m_View.xMin = minKeV;
	m_View.xMax = maxKeV;

	ClampView();
	ScheduleRefresh();
}

void cPreviewPanel::ResetHardEnergyRangeToFullData()
{
	m_HardXRangeEnabled = true;
	m_HardXRangeIsFull = true;
	m_HardXMinKeV = 0.0;
	m_HardXMaxKeV = DomainMaxKeV();

	m_View.xMin = AllowedXMin();
	m_View.xMax = AllowedXMax();

	ClampView();
	ScheduleRefresh();
}

double cPreviewPanel::GetMaxEnergyKeV() const
{
	return DomainMaxKeV();
}

auto cPreviewPanel::ZoomX(double factor, double anchorScreenX) -> void
{
	if (!m_ViewInitialized || factor <= 0.0)
		return;

	const double range = m_View.xMax - m_View.xMin;
	if (range <= 0.0)
		return;

	const double anchor = ClampSafe(ScreenToEnergy(anchorScreenX), m_View.xMin, m_View.xMax);
	const double allowedRange = AllowedXMax() - AllowedXMin();
	const double newRange = ClampSafe(range / factor, std::min(MinimumXRange(), allowedRange), allowedRange);
	const double ratio = ClampSafe((anchor - m_View.xMin) / range, 0.0, 1.0);

	m_View.xMin = anchor - ratio * newRange;
	m_View.xMax = m_View.xMin + newRange;

	ClampView();
}

auto cPreviewPanel::ZoomY(double factor, double anchorScreenY) -> void
{
	if (!m_ViewInitialized || factor <= 0.0)
		return;

	m_AutoY = false;

	const double range = m_View.yMax - m_View.yMin;
	if (range <= 0.0)
		return;

	const double anchor = ClampSafe(ScreenToAxisY(anchorScreenY), m_View.yMin, m_View.yMax);
	const double minRange = m_LogScale ? 0.3 : 5.0;
	const double newRange = std::max(minRange, range / factor);
	const double ratio = ClampSafe((anchor - m_View.yMin) / range, 0.0, 1.0);

	m_View.yMin = std::max(0.0, anchor - ratio * newRange);
	m_View.yMax = m_View.yMin + newRange;

	ClampView();
}

auto cPreviewPanel::PanPixels(double dx, double dy) -> bool
{
	const auto& plot = m_Layout.plot;
	if (!m_ViewInitialized || plot.m_width <= 0.0 || plot.m_height <= 0.0)
		return false;

	const Viewport before = m_View;

	const double dataDx = dx * (m_View.xMax - m_View.xMin) / plot.m_width;
	m_View.xMin -= dataDx;
	m_View.xMax -= dataDx;

	if (!m_AutoY)
	{
		const double dataDy = dy * (m_View.yMax - m_View.yMin) / plot.m_height;
		m_View.yMin += dataDy;
		m_View.yMax += dataDy;

		if (m_View.yMin < 0.0)
		{
			m_View.yMax -= m_View.yMin;
			m_View.yMin = 0.0;
		}
	}

	ClampView();

	return before.xMin != m_View.xMin || before.xMax != m_View.xMax ||
		before.yMin != m_View.yMin || before.yMax != m_View.yMax;
}

/* ======================= Transforms ======================= */

auto cPreviewPanel::ToAxisY(double counts) const -> double
{
	return m_LogScale ? std::log10(1.0 + std::max(0.0, counts)) : counts;
}

auto cPreviewPanel::FromAxisY(double axis) const -> double
{
	return m_LogScale ? std::pow(10.0, axis) - 1.0 : axis;
}

auto cPreviewPanel::ComputeLayout(const wxSize& size, double scale, bool forExport) const -> Layout
{
	Layout layout{};
	layout.scale = std::max(0.5, scale);
	layout.size = size;

	const double s = layout.scale;
	const double w = static_cast<double>(size.GetWidth());
	const double h = static_cast<double>(size.GetHeight());

	const double margin = 10.0 * s;
	const double gap = 8.0 * s;
	const double headerHeight = 26.0 * s;
	const double leftAxis = 66.0 * s;
	const double bottomAxis = 48.0 * s;
	const double right = 16.0 * s;

	const bool showNavigator = !forExport && PrimarySeries() && HasCustomizedXDomain();
	const double navigatorHeight = showNavigator ? 36.0 * s : 0.0;

	layout.header = wxRect2DDouble(margin, margin, std::max(0.0, w - 2.0 * margin), headerHeight);

	double y = margin + headerHeight + gap;
	const double plotLeft = margin + leftAxis;
	const double plotWidth = std::max(20.0, w - plotLeft - right - margin);

	if (showNavigator)
	{
		layout.navigator = wxRect2DDouble(plotLeft, y, plotWidth, navigatorHeight);
		y += navigatorHeight + gap;
	}

	layout.plot = wxRect2DDouble(plotLeft, y, plotWidth, std::max(20.0, h - y - bottomAxis - margin));

	return layout;
}

auto cPreviewPanel::MainMapping() const -> Mapping
{
	return Mapping{ m_Layout.plot, m_View.xMin, m_View.xMax, m_View.yMin, m_View.yMax };
}

auto cPreviewPanel::ScreenToEnergy(double screenX) const -> double
{
	const auto& plot = m_Layout.plot;
	if (plot.m_width <= 0.0)
		return m_View.xMin;

	const double t = (screenX - plot.m_x) / plot.m_width;
	return m_View.xMin + t * (m_View.xMax - m_View.xMin);
}

auto cPreviewPanel::EnergyToScreen(double energyKeV) const -> double
{
	const double range = m_View.xMax - m_View.xMin;
	if (range <= 0.0)
		return m_Layout.plot.m_x;

	return m_Layout.plot.m_x + (energyKeV - m_View.xMin) / range * m_Layout.plot.m_width;
}

auto cPreviewPanel::ScreenToAxisY(double screenY) const -> double
{
	const auto& plot = m_Layout.plot;
	if (plot.m_height <= 0.0)
		return m_View.yMin;

	const double t = (plot.m_y + plot.m_height - screenY) / plot.m_height;
	return m_View.yMin + t * (m_View.yMax - m_View.yMin);
}

auto cPreviewPanel::AxisYToScreen(double axisY) const -> double
{
	const auto& plot = m_Layout.plot;
	const double range = m_View.yMax - m_View.yMin;
	if (range <= 0.0)
		return plot.m_y + plot.m_height;

	return plot.m_y + plot.m_height - (axisY - m_View.yMin) / range * plot.m_height;
}

auto cPreviewPanel::CountsToScreen(double counts) const -> double
{
	return AxisYToScreen(ToAxisY(counts));
}

auto cPreviewPanel::IsInsidePlot(const wxPoint& point) const -> bool
{
	return m_ViewInitialized && m_Layout.plot.Contains(wxPoint2DDouble(point.x, point.y));
}

auto cPreviewPanel::IsInsideNavigator(const wxPoint& point) const -> bool
{
	return m_ViewInitialized && m_Layout.navigator.m_width > 0.0 &&
		m_Layout.navigator.Contains(wxPoint2DDouble(point.x, point.y));
}

/* ======================= Rendering ======================= */

auto cPreviewPanel::Font(double pixelSize, bool bold) const -> wxFont
{
	wxFontInfo info(wxSize(0, std::max(6, static_cast<int>(std::lround(pixelSize)))));
	info.FaceName("Segoe UI");
	if (bold)
		info.Bold();
	return wxFont(info);
}

auto cPreviewPanel::IsDarkBackground() const -> bool
{
	const wxColour bg = GetBackgroundColour();

	const double luminance =
		0.2126 * static_cast<double>(bg.Red()) +
		0.7152 * static_cast<double>(bg.Green()) +
		0.0722 * static_cast<double>(bg.Blue());

	return luminance < 140.0;
}

auto cPreviewPanel::ScheduleRefresh() -> void
{
	const auto now = std::chrono::steady_clock::now();
	const auto sinceLastPaint =
		std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastPaint).count();

	if (sinceLastPaint >= MinRefreshIntervalMs)
	{
		Refresh(false);
		return;
	}

	if (!m_RefreshTimer.IsRunning())
		m_RefreshTimer.StartOnce(static_cast<int>(MinRefreshIntervalMs - sinceLastPaint) + 1);
}

void cPreviewPanel::OnRefreshTimer(wxTimerEvent& WXUNUSED(evt))
{
	Refresh(false);
}

void cPreviewPanel::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
	m_LastPaint = std::chrono::steady_clock::now();

	wxBufferedPaintDC dc(this);
	dc.SetBackground(wxBrush(GetBackgroundColour()));
	dc.Clear();

	m_Layout = ComputeLayout(GetClientSize(), GetDPIScaleFactor(), false);

	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
	if (!gc)
		return;

	RenderAll(gc.get(), false);
}

void cPreviewPanel::OnSize(wxSizeEvent& evt)
{
	m_Layout = ComputeLayout(GetClientSize(), GetDPIScaleFactor(), false);
	ScheduleRefresh();
	evt.Skip();
}

auto cPreviewPanel::RenderAll(wxGraphicsContext* gc, bool exporting) -> void
{
	m_Pal = MakePalette(!exporting && IsDarkBackground());
	gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);
	gc->SetInterpolationQuality(wxINTERPOLATION_GOOD);

	const auto& plot = m_Layout.plot;

	// Plot frame
	gc->SetPen(MakePen(gc, m_Pal.plotBorder, Px(1.0)));
	gc->SetBrush(wxBrush(m_Pal.plotBackground));
	gc->DrawRoundedRectangle(plot.m_x, plot.m_y, plot.m_width, plot.m_height, Px(6.0));

	if (!m_ViewInitialized || !PrimarySeries())
	{
		DrawEmptyState(gc);
		return;
	}

	DrawHeader(gc, exporting);

	if (!exporting && m_Layout.navigator.m_width > 0.0)
		DrawNavigator(gc);

	DrawPlotFrameAndGrid(gc);

	gc->PushState();
	gc->Clip(plot.m_x, plot.m_y, plot.m_width, plot.m_height);

	DrawRegionsOfInterest(gc);

	const Mapping mapping = MainMapping();
	DrawSeries(gc, m_Reference, mapping, m_Pal.reference, wxColour(), false, Px(1.4));
	DrawSeries(gc, m_Captured, mapping, m_Pal.captured, m_Pal.capturedFill, m_FillCaptured, Px(1.6));

	if (m_ShowPeakLabels)
		DrawPeakLabels(gc);

	DrawMarkers(gc);

	if (!exporting)
	{
		DrawCursor(gc);
		DrawBoxZoom(gc);
	}

	gc->PopState();
	gc->ResetClip();

	DrawAxes(gc);
}

auto cPreviewPanel::DrawEmptyState(wxGraphicsContext* gc) -> void
{
	const auto& plot = m_Layout.plot;
	const wxString text =
		"No spectrum yet - take a single shot, start live capturing,\n"
		"or open an .mca file to show it as a reference.\n\n"
		"Wheel: zoom energy   Shift+wheel: zoom counts   Drag: pan   Ctrl+drag: box zoom\n"
		"Click: marker (two markers sum the region between them)   Double-click: reset   Right-click: menu";

	gc->SetFont(Font(Px(14.0)), m_Pal.textMuted);

	wxDouble tw{}, th{};
	gc->GetTextExtent(text, &tw, &th);
	gc->DrawText(text, plot.m_x + (plot.m_width - tw) / 2.0, plot.m_y + (plot.m_height - th) / 2.0);
}

auto cPreviewPanel::DrawChip(wxGraphicsContext* gc, double x, double y, const wxString& text, const wxColour& accent, bool alignRight, double maxRight) -> double
{
	gc->SetFont(Font(Px(12.0)), m_Pal.text);

	wxDouble tw{}, th{};
	gc->GetTextExtent(text, &tw, &th);

	const double height = Px(24.0);
	const double padX = Px(9.0);
	const double dot = Px(7.0);
	const double width = padX + dot + Px(6.0) + tw + padX;
	const double left = alignRight ? x - width : x;

	if (!alignRight && left + width > maxRight)
		return 0.0;

	gc->SetPen(MakePen(gc, m_Pal.chipBorder, Px(1.0)));
	gc->SetBrush(wxBrush(m_Pal.chipBackground));
	gc->DrawRoundedRectangle(left, y, width, height, height / 2.0);

	gc->SetPen(*wxTRANSPARENT_PEN);
	gc->SetBrush(wxBrush(accent));
	gc->DrawEllipse(left + padX, y + (height - dot) / 2.0, dot, dot);

	gc->DrawText(text, left + padX + dot + Px(6.0), y + (height - th) / 2.0);

	return width;
}

auto cPreviewPanel::DrawHeader(wxGraphicsContext* gc, bool exporting) -> void
{
	const auto& header = m_Layout.header;
	const double y = header.m_y + (header.m_height - Px(24.0)) / 2.0;
	const double spacing = Px(8.0);

	// Right side: status chips, laid out from the right edge.
	double right = header.m_x + header.m_width;

	const auto addRight = [&](const wxString& text, const wxColour& accent)
		{
			const double width = DrawChip(gc, right, y, text, accent, true, 0.0);
			right -= width + spacing;
		};

	if (m_HasBoardTemperature)
		addRight(wxString::Format("Board %.1f ", m_CurrentBoardTemperatureC) + DegreeC, wxColour(235, 70, 50));

	if (m_HasSDDTemperature)
		addRight(wxString::Format("SDD %.1f ", m_CurrentSDDTemperatureC) + DegreeC, wxColour(0, 162, 232));

	if (!exporting && m_ShowPerformanceOverlay)
	{
		wxString text = wxString::Format("Frame %llu", m_CurrentFrameNumber);
		if (m_DisplayedFPS > 0.0)
			text += wxString::Format("  %s  %.2f fps", MiddleDot, m_DisplayedFPS);
		addRight(text, m_Pal.cursor);
	}

	if (m_LogScale)
		addRight("Log scale", m_Pal.textMuted);

	// Left side: one legend chip per spectrum, with its summary.
	double left = header.m_x;
	const double maxRight = right;

	if (m_Captured.HasData())
	{
		const auto peak = std::max_element(m_Captured.counts.begin(), m_Captured.counts.end());
		const double peakKeV = static_cast<double>(std::distance(m_Captured.counts.begin(), peak)) * EffectiveBinSize(m_Captured);

		const wxString text = wxString::Format
		(
			"Captured   %s %s   %s   peak %s keV (%s)",
			SigmaSign, FormatCompact(m_Captured.sum), MiddleDot,
			PreviewPanelVariables::FormatEnergyValue(peakKeV, 0.001),
			FormatCompact(*peak)
		);

		const double width = DrawChip(gc, left, y, text, m_Pal.captured, false, maxRight);
		if (width > 0.0)
			left += width + spacing;
	}

	if (m_Reference.HasData())
	{
		unsigned long long referenceSum = m_Reference.sum;
		if (referenceSum == 0)
			for (const auto value : m_Reference.counts)
				referenceSum += value;

		const wxString text = wxString::Format("Reference   %s %s", SigmaSign, FormatCompact(referenceSum));
		DrawChip(gc, left, y, text, m_Pal.reference, false, maxRight);
	}
}

auto cPreviewPanel::DrawSeries(wxGraphicsContext* gc, const Series& series, const Mapping& mapping, const wxColour& line, const wxColour& fill, bool drawFill, double lineWidth) -> void
{
	const double bin = EffectiveBinSize(series);
	const int count = series.Size();
	const auto& rect = mapping.rect;
	const double xRange = mapping.xMax - mapping.xMin;
	const double yRange = mapping.yMax - mapping.yMin;

	if (count < 2 || bin <= 0.0 || rect.m_width <= 0.0 || xRange <= 0.0 || yRange <= 0.0)
		return;

	const auto toX = [&](int index)
		{
			return rect.m_x + (static_cast<double>(index) * bin - mapping.xMin) / xRange * rect.m_width;
		};
	const auto toY = [&](unsigned long counts)
		{
			return rect.m_y + rect.m_height - (ToAxisY(static_cast<double>(counts)) - mapping.yMin) / yRange * rect.m_height;
		};

	const int first = std::max(0, static_cast<int>(std::floor(mapping.xMin / bin)) - 1);
	const int last = std::min(count - 1, static_cast<int>(std::ceil(mapping.xMax / bin)) + 1);
	if (last <= first)
		return;

	std::vector<wxPoint2DDouble> points;
	const double binsPerPixel = static_cast<double>(last - first) / rect.m_width;

	if (binsPerPixel <= 2.0)
	{
		points.reserve(static_cast<std::size_t>(last - first + 1));
		for (int i = first; i <= last; ++i)
			points.emplace_back(toX(i), toY(series.counts[static_cast<std::size_t>(i)]));
	}
	else
	{
		// More bins than pixels: keep each pixel column's first, minimum,
		// maximum and last value, so narrow peaks are never dropped.
		points.reserve(static_cast<std::size_t>(rect.m_width * 4.0) + 8);

		int column = std::numeric_limits<int>::min();
		int firstIdx = 0, lastIdx = 0, minIdx = 0, maxIdx = 0;

		const auto flush = [&]()
			{
				if (column == std::numeric_limits<int>::min())
					return;

				const double x = toX(firstIdx);
				points.emplace_back(x, toY(series.counts[static_cast<std::size_t>(firstIdx)]));

				const int a = std::min(minIdx, maxIdx);
				const int b = std::max(minIdx, maxIdx);
				points.emplace_back(x, toY(series.counts[static_cast<std::size_t>(a)]));
				points.emplace_back(x, toY(series.counts[static_cast<std::size_t>(b)]));
				points.emplace_back(x, toY(series.counts[static_cast<std::size_t>(lastIdx)]));
			};

		for (int i = first; i <= last; ++i)
		{
			const int c = static_cast<int>(std::floor(toX(i)));
			const unsigned long value = series.counts[static_cast<std::size_t>(i)];

			if (c != column)
			{
				flush();
				column = c;
				firstIdx = lastIdx = minIdx = maxIdx = i;
				continue;
			}

			lastIdx = i;
			if (value < series.counts[static_cast<std::size_t>(minIdx)]) minIdx = i;
			if (value > series.counts[static_cast<std::size_t>(maxIdx)]) maxIdx = i;
		}

		flush();
	}

	if (points.size() < 2)
		return;

	wxGraphicsPath path = gc->CreatePath();
	path.MoveToPoint(points.front());
	for (std::size_t i = 1; i < points.size(); ++i)
		path.AddLineToPoint(points[i]);

	if (drawFill && fill.IsOk())
	{
		wxGraphicsPath area = gc->CreatePath();
		area.AddPath(path);
		const double bottom = rect.m_y + rect.m_height;
		area.AddLineToPoint(points.back().m_x, bottom);
		area.AddLineToPoint(points.front().m_x, bottom);
		area.CloseSubpath();

		gc->SetPen(*wxTRANSPARENT_PEN);
		gc->SetBrush(wxBrush(fill));
		gc->FillPath(area);
	}

	gc->SetPen(MakePen(gc, line, lineWidth));
	gc->SetBrush(*wxTRANSPARENT_BRUSH);
	gc->StrokePath(path);
}

auto cPreviewPanel::DrawPlotFrameAndGrid(wxGraphicsContext* gc) -> void
{
	const auto& plot = m_Layout.plot;

	gc->PushState();
	gc->Clip(plot.m_x, plot.m_y, plot.m_width, plot.m_height);

	// Vertical grid (energy)
	const int xTickCount = ClampSafe(static_cast<int>(plot.m_width / Px(90.0)), 3, 12);
	gc->SetPen(MakePen(gc, m_Pal.grid, Px(1.0)));
	for (const double tick : BuildTicks(m_View.xMin, m_View.xMax, xTickCount))
	{
		const double x = std::round(EnergyToScreen(tick)) + 0.5;
		gc->StrokeLine(x, plot.m_y, x, plot.m_y + plot.m_height);
	}

	// Horizontal grid (counts)
	if (m_LogScale)
	{
		const double maxCounts = FromAxisY(m_View.yMax);
		for (double decade = 1.0; decade <= maxCounts * 10.0 && decade < 1e15; decade *= 10.0)
		{
			for (int m = 1; m <= 9; ++m)
			{
				const double value = decade * m;
				const double axis = ToAxisY(value);
				if (axis < m_View.yMin || axis > m_View.yMax)
					continue;

				gc->SetPen(MakePen(gc, m == 1 ? m_Pal.grid : m_Pal.gridMinor, Px(1.0)));
				const double y = std::round(AxisYToScreen(axis)) + 0.5;
				gc->StrokeLine(plot.m_x, y, plot.m_x + plot.m_width, y);
			}
		}
	}
	else
	{
		const int yTickCount = ClampSafe(static_cast<int>(plot.m_height / Px(46.0)), 3, 10);
		for (const double tick : BuildTicks(m_View.yMin, m_View.yMax, yTickCount))
		{
			const double y = std::round(AxisYToScreen(tick)) + 0.5;
			gc->StrokeLine(plot.m_x, y, plot.m_x + plot.m_width, y);
		}
	}

	gc->PopState();
	gc->ResetClip();
}

auto cPreviewPanel::DrawAxes(wxGraphicsContext* gc) -> void
{
	const auto& plot = m_Layout.plot;
	const double bottom = plot.m_y + plot.m_height;

	gc->SetPen(MakePen(gc, m_Pal.plotBorder, Px(1.0)));
	gc->SetBrush(*wxTRANSPARENT_BRUSH);
	gc->DrawRoundedRectangle(plot.m_x, plot.m_y, plot.m_width, plot.m_height, Px(6.0));

	const wxFont tickFont = Font(Px(12.0));
	const wxFont titleFont = Font(Px(13.0), true);

	// Energy axis
	{
		const int xTickCount = ClampSafe(static_cast<int>(plot.m_width / Px(90.0)), 3, 12);
		const auto ticks = BuildTicks(m_View.xMin, m_View.xMax, xTickCount);
		const double step = ticks.size() >= 2 ? ticks[1] - ticks[0] : (m_View.xMax - m_View.xMin);

		gc->SetFont(tickFont, m_Pal.textMuted);
		gc->SetPen(MakePen(gc, m_Pal.plotBorder, Px(1.0)));

		double lastRight = -1e9;
		for (const double tick : ticks)
		{
			const double x = EnergyToScreen(tick);
			gc->StrokeLine(x, bottom, x, bottom + Px(4.0));

			const wxString label = PreviewPanelVariables::FormatEnergyValue(tick, step);
			wxDouble tw{}, th{};
			gc->GetTextExtent(label, &tw, &th);

			const double labelX = ClampSafe(x - tw / 2.0, plot.m_x - Px(20.0), plot.m_x + plot.m_width - tw + Px(10.0));
			if (labelX < lastRight + Px(6.0))
				continue;

			gc->DrawText(label, labelX, bottom + Px(6.0));
			lastRight = labelX + tw;
		}

		gc->SetFont(titleFont, m_Pal.text);
		const wxString title = "Energy [keV]";
		wxDouble tw{}, th{};
		gc->GetTextExtent(title, &tw, &th);
		gc->DrawText(title, plot.m_x + (plot.m_width - tw) / 2.0, bottom + Px(24.0));
	}

	// Counts axis
	{
		std::vector<std::pair<double, wxString>> ticks; // axis value, label

		if (m_LogScale)
		{
			if (m_View.yMin <= 0.0)
				ticks.emplace_back(0.0, "0");

			const double maxCounts = FromAxisY(m_View.yMax);
			for (double decade = 1.0; decade <= maxCounts && decade < 1e15; decade *= 10.0)
			{
				const double axis = ToAxisY(decade);
				if (axis >= m_View.yMin && axis <= m_View.yMax)
					ticks.emplace_back(axis, FormatEventsTickLabel(decade));
			}
		}
		else
		{
			const int yTickCount = ClampSafe(static_cast<int>(plot.m_height / Px(46.0)), 3, 10);
			for (const double tick : BuildTicks(m_View.yMin, m_View.yMax, yTickCount))
				ticks.emplace_back(tick, FormatEventsTickLabel(tick));
		}

		gc->SetFont(tickFont, m_Pal.textMuted);
		gc->SetPen(MakePen(gc, m_Pal.plotBorder, Px(1.0)));

		double lastTop = 1e9;
		for (const auto& [axis, label] : ticks)
		{
			const double y = AxisYToScreen(axis);
			gc->StrokeLine(plot.m_x - Px(4.0), y, plot.m_x, y);

			wxDouble tw{}, th{};
			gc->GetTextExtent(label, &tw, &th);

			const double labelY = ClampSafe(y - th / 2.0, plot.m_y - th / 2.0, bottom - th / 2.0);
			if (labelY + th > lastTop - Px(2.0))
				continue;

			gc->DrawText(label, plot.m_x - Px(7.0) - tw, labelY);
			lastTop = labelY;
		}

		gc->SetFont(titleFont, m_Pal.text);
		const wxString title = "Events";
		wxDouble tw{}, th{};
		gc->GetTextExtent(title, &tw, &th);
		gc->DrawText(title, m_Layout.header.m_x, plot.m_y + (plot.m_height + tw) / 2.0, M_PI / 2.0);
	}
}

auto cPreviewPanel::NavigatorInner() const -> wxRect2DDouble
{
	const auto& nav = m_Layout.navigator;
	return wxRect2DDouble(nav.m_x + Px(2.0), nav.m_y + Px(3.0), nav.m_width - Px(4.0), nav.m_height - Px(5.0));
}

auto cPreviewPanel::DrawNavigator(wxGraphicsContext* gc) -> void
{
	const auto& nav = m_Layout.navigator;
	const double domain = DomainMaxKeV();
	if (domain <= 0.0)
		return;

	gc->SetPen(MakePen(gc, m_Pal.plotBorder, Px(1.0)));
	gc->SetBrush(wxBrush(m_Pal.plotBackground));
	gc->DrawRoundedRectangle(nav.m_x, nav.m_y, nav.m_width, nav.m_height, Px(4.0));

	unsigned long maxCounts = 1;
	for (const Series* series : { &m_Captured, &m_Reference })
		maxCounts = std::max(maxCounts, MaxInEnergyRange(*series, 0.0, domain));

	const wxRect2DDouble inner = NavigatorInner();
	const Mapping mapping{ inner, 0.0, domain, 0.0, ToAxisY(static_cast<double>(maxCounts)) * 1.05 };

	gc->PushState();
	gc->Clip(nav.m_x, nav.m_y, nav.m_width, nav.m_height);

	DrawSeries(gc, m_Reference, mapping, m_Pal.reference, wxColour(), false, Px(1.0));
	DrawSeries(gc, m_Captured, mapping, m_Pal.captured, m_Pal.capturedFill, true, Px(1.0));

	const auto toNavX = [&](double energy) { return inner.m_x + energy / domain * inner.m_width; };

	// Dim everything outside the visible range
	const double viewLeft = toNavX(m_View.xMin);
	const double viewRight = toNavX(m_View.xMax);

	gc->SetPen(*wxTRANSPARENT_PEN);
	gc->SetBrush(wxBrush(m_Pal.navigatorShade));
	gc->DrawRectangle(nav.m_x, nav.m_y, viewLeft - nav.m_x, nav.m_height);
	gc->DrawRectangle(viewRight, nav.m_y, nav.m_x + nav.m_width - viewRight, nav.m_height);

	gc->SetPen(MakePen(gc, m_Pal.cursor, Px(1.5)));
	gc->SetBrush(*wxTRANSPARENT_BRUSH);
	gc->DrawRectangle(viewLeft, nav.m_y + Px(1.0), std::max(Px(2.0), viewRight - viewLeft), nav.m_height - Px(2.0));

	// Limits of the energy range set in the controls
	if (HasEffectiveHardRange())
	{
		gc->SetPen(MakePen(gc, m_Pal.marker, Px(2.0)));
		for (const double energy : { AllowedXMin(), AllowedXMax() })
		{
			const double x = toNavX(energy);
			gc->StrokeLine(x, nav.m_y, x, nav.m_y + nav.m_height);
		}
	}

	gc->PopState();
	gc->ResetClip();
}

auto cPreviewPanel::DrawRegionsOfInterest(wxGraphicsContext* gc) -> void
{
	const auto& plot = m_Layout.plot;

	for (std::size_t i = 0; i + 1 < m_Markers.size(); i += 2)
	{
		const auto region = ComputeRegion(m_Markers[i], m_Markers[i + 1]);
		if (!region.valid)
			continue;

		const double x1 = EnergyToScreen(region.fromKeV);
		const double x2 = EnergyToScreen(region.toKeV);
		if (x2 < plot.m_x || x1 > plot.m_x + plot.m_width)
			continue;

		gc->SetPen(*wxTRANSPARENT_PEN);
		gc->SetBrush(wxBrush(m_Pal.regionFill));
		gc->DrawRectangle(x1, plot.m_y, x2 - x1, plot.m_height);

		const wxString text = wxString::Format
		(
			"M%zu-M%zu   %s %s   net %s",
			i + 1, i + 2, SigmaSign,
			FormatThousands(region.gross),
			FormatThousands(static_cast<unsigned long long>(std::max(0.0, std::round(region.net))))
		);

		gc->SetFont(Font(Px(11.5)), m_Pal.text);
		wxDouble tw{}, th{};
		gc->GetTextExtent(text, &tw, &th);

		const double padX = Px(6.0);
		const double boxW = tw + 2.0 * padX;
		const double boxH = th + Px(6.0);
		const double cx = ClampSafe((x1 + x2) / 2.0, plot.m_x + boxW / 2.0 + Px(4.0), plot.m_x + plot.m_width - boxW / 2.0 - Px(4.0));
		const double boxY = plot.m_y + Px(4.0) + 3.0 * Px(22.0);

		gc->SetPen(MakePen(gc, m_Pal.marker, Px(1.0)));
		gc->SetBrush(wxBrush(m_Pal.chipBackground));
		gc->DrawRoundedRectangle(cx - boxW / 2.0, boxY, boxW, boxH, Px(4.0));
		gc->DrawText(text, cx - tw / 2.0, boxY + Px(3.0));
	}
}

auto cPreviewPanel::DrawMarkers(wxGraphicsContext* gc) -> void
{
	const auto& plot = m_Layout.plot;
	const double bin = PrimarySeries() ? EffectiveBinSize(*PrimarySeries()) : 0.001;

	for (std::size_t i = 0; i < m_Markers.size(); ++i)
	{
		const double energy = m_Markers[i];
		if (energy < m_View.xMin || energy > m_View.xMax)
			continue;

		const double x = EnergyToScreen(energy);

		gc->SetPen(MakePen(gc, m_Pal.marker, Px(1.3), wxPENSTYLE_SHORT_DASH));
		gc->StrokeLine(x, plot.m_y, x, plot.m_y + plot.m_height);

		// Points where the marker crosses each spectrum
		for (const auto& [series, colour] : { std::pair{ &m_Captured, m_Pal.captured }, std::pair{ &m_Reference, m_Pal.reference } })
		{
			const long long value = ValueAtEnergy(*series, energy);
			if (value < 0)
				continue;

			const double y = CountsToScreen(static_cast<double>(value));
			gc->SetPen(MakePen(gc, m_Pal.plotBackground, Px(1.5)));
			gc->SetBrush(wxBrush(colour));
			gc->DrawEllipse(x - Px(4.0), y - Px(4.0), Px(8.0), Px(8.0));
		}

		// Tag: number, energy and the values at the marker
		wxString text = wxString::Format("M%zu  %s keV", i + 1, PreviewPanelVariables::FormatEnergyValue(energy, bin));
		const long long captured = ValueAtEnergy(m_Captured, energy);
		const long long reference = ValueAtEnergy(m_Reference, energy);
		if (captured >= 0)
			text += "  C " + FormatCompact(static_cast<unsigned long long>(captured));
		if (reference >= 0)
			text += "  R " + FormatCompact(static_cast<unsigned long long>(reference));

		gc->SetFont(Font(Px(11.5), true), m_Pal.markerText);
		wxDouble tw{}, th{};
		gc->GetTextExtent(text, &tw, &th);

		const double padX = Px(6.0);
		const double boxW = tw + 2.0 * padX;
		const double boxH = th + Px(4.0);
		const double boxX = ClampSafe(x - boxW / 2.0, plot.m_x + Px(3.0), plot.m_x + plot.m_width - boxW - Px(3.0));
		const double boxY = plot.m_y + Px(4.0) + static_cast<double>(i % 3) * Px(22.0);

		gc->SetPen(*wxTRANSPARENT_PEN);
		gc->SetBrush(wxBrush(m_Pal.marker));
		gc->DrawRoundedRectangle(boxX, boxY, boxW, boxH, Px(4.0));
		gc->DrawText(text, boxX + padX, boxY + Px(2.0));
	}
}

auto cPreviewPanel::DrawPeakLabels(wxGraphicsContext* gc) -> void
{
	const auto& plot = m_Layout.plot;
	std::vector<wxRect2DDouble> placed;

	gc->SetFont(Font(Px(11.5), true), m_Pal.text);

	// m_Peaks is sorted by prominence, so the strongest peaks win overlaps.
	for (const auto& peak : m_Peaks)
	{
		if (peak.energyKeV < m_View.xMin || peak.energyKeV > m_View.xMax)
			continue;

		const wxString text = peak.label.IsEmpty()
			? PreviewPanelVariables::FormatEnergyValue(peak.energyKeV, 0.01)
			: peak.label;

		wxDouble tw{}, th{};
		gc->GetTextExtent(text, &tw, &th);

		const double x = EnergyToScreen(peak.energyKeV);

		// Place the label above whichever spectrum is taller here.
		double top = CountsToScreen(static_cast<double>(peak.counts));
		for (const Series* series : { &m_Captured, &m_Reference })
		{
			const long long value = ValueAtEnergy(*series, peak.energyKeV);
			if (value >= 0)
				top = std::min(top, CountsToScreen(static_cast<double>(value)));
		}
		double labelY = std::max(plot.m_y + Px(2.0), top - th - Px(10.0));
		const wxRect2DDouble rect(x - tw / 2.0 - Px(3.0), labelY - Px(1.0), tw + Px(6.0), th + Px(2.0));

		bool overlaps = false;
		for (const auto& other : placed)
			if (rect.Intersects(other)) { overlaps = true; break; }
		if (overlaps)
			continue;

		placed.push_back(rect);

		gc->SetPen(MakePen(gc, m_Pal.textMuted, Px(1.0)));
		gc->StrokeLine(x, top - Px(3.0), x, labelY + th + Px(1.0));

		gc->SetPen(*wxTRANSPARENT_PEN);
		gc->SetBrush(wxBrush(m_Pal.chipBackground));
		gc->DrawRoundedRectangle(rect.m_x, rect.m_y, rect.m_width, rect.m_height, Px(3.0));
		gc->DrawText(text, x - tw / 2.0, labelY);
	}
}

auto cPreviewPanel::DrawCursor(wxGraphicsContext* gc) -> void
{
	if (!m_RenderCursorOverlay || !m_CursorInside || m_DragMode == DragMode::Pan || m_DragMode == DragMode::BoxZoom)
		return;

	const auto& plot = m_Layout.plot;
	const Series* primary = PrimarySeries();
	if (!primary)
		return;

	const double energy = SnapToBin(ScreenToEnergy(m_CursorPos.x));
	const double x = EnergyToScreen(energy);

	gc->SetPen(MakePen(gc, wxColour(m_Pal.cursor.Red(), m_Pal.cursor.Green(), m_Pal.cursor.Blue(), 150), Px(1.0)));
	gc->StrokeLine(x, plot.m_y, x, plot.m_y + plot.m_height);

	std::vector<std::pair<wxString, wxColour>> lines;
	lines.emplace_back(PreviewPanelVariables::FormatEnergyValue(energy, EffectiveBinSize(*primary)) + " keV", wxColour());

	for (const auto& [series, name, colour] : {
		std::tuple{ &m_Captured, wxString("Captured"), m_Pal.captured },
		std::tuple{ &m_Reference, wxString("Reference"), m_Pal.reference } })
	{
		const long long value = ValueAtEnergy(*series, energy);
		if (value < 0)
			continue;

		const double y = CountsToScreen(static_cast<double>(value));
		gc->SetPen(MakePen(gc, m_Pal.plotBackground, Px(1.5)));
		gc->SetBrush(wxBrush(colour));
		gc->DrawEllipse(x - Px(4.0), y - Px(4.0), Px(8.0), Px(8.0));

		lines.emplace_back(name + "  " + FormatThousands(static_cast<unsigned long long>(value)), colour);
	}

	// Only suggest an element when the cursor is on a detected peak: almost
	// any energy is within a few tens of eV of some line in the table.
	if (const DetectedPeak* peak = PeakNear(energy); peak && !peak->label.IsEmpty())
	{
		lines.emplace_back
		(
			"Peak " + PreviewPanelVariables::FormatEnergyValue(peak->energyKeV, 0.001) + " keV  " + Approx + " " + peak->label,
			wxColour()
		);
	}

	// Readout box next to the cursor, flipped to the left near the edge
	const wxFont bold = Font(Px(12.0), true);
	const wxFont normal = Font(Px(12.0));
	const double padX = Px(8.0);
	const double lineHeight = Px(17.0);
	const double dot = Px(7.0);

	double boxW = 0.0;
	for (std::size_t i = 0; i < lines.size(); ++i)
	{
		gc->SetFont(i == 0 ? bold : normal, m_Pal.text);
		wxDouble tw{}, th{};
		gc->GetTextExtent(lines[i].first, &tw, &th);
		boxW = std::max(boxW, tw + (lines[i].second.IsOk() ? dot + Px(6.0) : 0.0));
	}
	boxW += 2.0 * padX;
	const double boxH = static_cast<double>(lines.size()) * lineHeight + Px(8.0);

	double boxX = x + Px(12.0);
	if (boxX + boxW > plot.m_x + plot.m_width - Px(4.0))
		boxX = x - Px(12.0) - boxW;
	const double boxY = ClampSafe(static_cast<double>(m_CursorPos.y) - boxH - Px(12.0), plot.m_y + Px(4.0), plot.m_y + plot.m_height - boxH - Px(4.0));

	gc->SetPen(MakePen(gc, m_Pal.chipBorder, Px(1.0)));
	gc->SetBrush(wxBrush(m_Pal.chipBackground));
	gc->DrawRoundedRectangle(boxX, boxY, boxW, boxH, Px(5.0));

	double textY = boxY + Px(4.0);
	for (std::size_t i = 0; i < lines.size(); ++i)
	{
		double textX = boxX + padX;
		if (lines[i].second.IsOk())
		{
			gc->SetPen(*wxTRANSPARENT_PEN);
			gc->SetBrush(wxBrush(lines[i].second));
			gc->DrawEllipse(textX, textY + (lineHeight - dot) / 2.0, dot, dot);
			textX += dot + Px(6.0);
		}

		gc->SetFont(i == 0 ? bold : normal, i == 0 ? m_Pal.text : m_Pal.textMuted);
		gc->DrawText(lines[i].first, textX, textY);
		textY += lineHeight;
	}
}

auto cPreviewPanel::DrawBoxZoom(wxGraphicsContext* gc) -> void
{
	if (m_DragMode != DragMode::BoxZoom)
		return;

	const double x1 = std::min(m_DragStart.x, m_CursorPos.x);
	const double y1 = std::min(m_DragStart.y, m_CursorPos.y);
	const double x2 = std::max(m_DragStart.x, m_CursorPos.x);
	const double y2 = std::max(m_DragStart.y, m_CursorPos.y);

	gc->SetPen(MakePen(gc, m_Pal.cursor, Px(1.0), wxPENSTYLE_SHORT_DASH));
	gc->SetBrush(wxBrush(wxColour(m_Pal.cursor.Red(), m_Pal.cursor.Green(), m_Pal.cursor.Blue(), 30)));
	gc->DrawRectangle(x1, y1, x2 - x1, y2 - y1);
}

/* ======================= Interaction ======================= */

void cPreviewPanel::OnMouseMoved(wxMouseEvent& evt)
{
	const wxPoint position = evt.GetPosition();
	const wxPoint previous = m_CursorPos;
	m_CursorPos = position;

	const bool wasInside = m_CursorInside;
	m_CursorInside = IsInsidePlot(position);

	bool needsRefresh = false;

	switch (m_DragMode)
	{
	case DragMode::Pending:
	{
		const int dx = position.x - m_DragStart.x;
		const int dy = position.y - m_DragStart.y;
		if (dx * dx + dy * dy > ClickDragThresholdPixels * ClickDragThresholdPixels)
		{
			m_DragMode = DragMode::Pan;
			needsRefresh = PanPixels(position.x - m_LastMousePos.x, position.y - m_LastMousePos.y) || true;
		}
		break;
	}
	case DragMode::Pan:
		needsRefresh = PanPixels(position.x - m_LastMousePos.x, position.y - m_LastMousePos.y);
		break;

	case DragMode::Navigator:
	{
		const auto inner = NavigatorInner();
		if (m_Layout.navigator.m_width > 0.0 && inner.m_width > 0.0)
		{
			const double energy = (position.x - inner.m_x) / inner.m_width * DomainMaxKeV();
			const double range = m_View.xMax - m_View.xMin;
			m_View.xMin = energy - range / 2.0;
			m_View.xMax = m_View.xMin + range;
			ClampView();
			needsRefresh = true;
		}
		break;
	}
	case DragMode::BoxZoom:
		needsRefresh = true;
		break;

	default:
		needsRefresh = m_RenderCursorOverlay && (m_CursorInside || wasInside) &&
			(wasInside != m_CursorInside || previous.x != position.x || previous.y != position.y);
		break;
	}

	m_LastMousePos = position;

	if (needsRefresh)
		ScheduleRefresh();

	UpdateStatusBar();
}

void cPreviewPanel::OnMouseWheel(wxMouseEvent& evt)
{
	if (!m_ViewInitialized)
		return;

	const int rotation = evt.GetWheelRotation();
	if (rotation == 0)
		return;

	const double factor = (rotation > 0) ? 1.25 : (1.0 / 1.25);

	if (evt.ShiftDown())
		ZoomY(factor, evt.GetPosition().y);
	else
		ZoomX(factor, evt.GetPosition().x);

	UpdateStatusBar();
	ScheduleRefresh();
}

void cPreviewPanel::OnLeftDown(wxMouseEvent& evt)
{
	SetFocus();

	const wxPoint position = evt.GetPosition();
	m_DragStart = position;
	m_LastMousePos = position;

	if (IsInsideNavigator(position))
	{
		m_DragMode = DragMode::Navigator;
		wxMouseEvent move(evt);
		OnMouseMoved(move);
	}
	else if (IsInsidePlot(position))
	{
		m_DragMode = evt.ControlDown() ? DragMode::BoxZoom : DragMode::Pending;
	}
	else
	{
		return;
	}

	if (!HasCapture())
		CaptureMouse();
}

void cPreviewPanel::OnLeftUp(wxMouseEvent& evt)
{
	const wxPoint position = evt.GetPosition();

	m_LastClickAddedMarker = false;

	if (m_DragMode == DragMode::Pending && IsInsidePlot(position))
	{
		m_LastClickAddedMarker = AddMarkerAtScreenX(position.x);
	}
	else if (m_DragMode == DragMode::BoxZoom)
	{
		const double x1 = std::min(m_DragStart.x, position.x);
		const double x2 = std::max(m_DragStart.x, position.x);
		const double y1 = std::min(m_DragStart.y, position.y);
		const double y2 = std::max(m_DragStart.y, position.y);

		if (x2 - x1 >= Px(5.0))
		{
			const double e1 = ScreenToEnergy(x1);
			const double e2 = ScreenToEnergy(x2);
			m_View.xMin = e1;
			m_View.xMax = e2;

			// Also zoom the counts axis if the box was drawn tall enough to
			// mean it.
			if (y2 - y1 >= Px(5.0) && y2 - y1 < m_Layout.plot.m_height * 0.95)
			{
				const double top = ScreenToAxisY(y1);
				const double bottom = ScreenToAxisY(y2);
				m_AutoY = false;
				m_View.yMin = std::max(0.0, bottom);
				m_View.yMax = top;
			}

			ClampView();
		}
	}

	EndDrag();
	UpdateStatusBar();
	ScheduleRefresh();
}

void cPreviewPanel::OnLeftDoubleClick(wxMouseEvent& evt)
{
	// A double-click arrives after the first click's down/up, which has
	// already added a marker: take it back, the user meant "reset".
	if (m_LastClickAddedMarker)
	{
		const auto it = std::find(m_Markers.begin(), m_Markers.end(), m_LastClickMarkerKeV);
		if (it != m_Markers.end())
			m_Markers.erase(it);
		m_LastClickAddedMarker = false;
	}

	if (IsInsidePlot(evt.GetPosition()) || IsInsideNavigator(evt.GetPosition()))
		ResetView();
	else
		ScheduleRefresh();
}

void cPreviewPanel::OnRightDown(wxMouseEvent& evt)
{
	SetFocus();
	ShowContextMenu(evt.GetPosition());
}

void cPreviewPanel::OnMouseLeave(wxMouseEvent& WXUNUSED(evt))
{
	if (m_CursorInside)
	{
		m_CursorInside = false;
		ScheduleRefresh();
	}
}

void cPreviewPanel::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(evt))
{
	// Mouse capture can be taken away (Alt+Tab, a dialog): end any drag
	// without applying it.
	m_DragMode = DragMode::None;
	ScheduleRefresh();
}

auto cPreviewPanel::EndDrag() -> void
{
	m_DragMode = DragMode::None;

	if (HasCapture())
		ReleaseMouse();
}

void cPreviewPanel::OnKeyDown(wxKeyEvent& evt)
{
	const auto& plot = m_Layout.plot;
	const double centerX = plot.m_x + plot.m_width / 2.0;

	switch (evt.GetKeyCode())
	{
	case WXK_HOME:
		ResetView();
		break;
	case 'L':
		m_LogScale = !m_LogScale;
		m_AutoY = true;
		ClampView();
		break;
	case 'P':
		m_ShowPeakLabels = !m_ShowPeakLabels;
		if (m_ShowPeakLabels)
			DetectPeaks();
		break;
	case 'F':
		m_FillCaptured = !m_FillCaptured;
		break;
	case WXK_DELETE:
	case WXK_BACK:
		m_Markers.clear();
		break;
	case WXK_LEFT:
		PanPixels(plot.m_width * 0.1, 0.0);
		break;
	case WXK_RIGHT:
		PanPixels(-plot.m_width * 0.1, 0.0);
		break;
	case '+':
	case '=':
	case WXK_ADD:
	case WXK_NUMPAD_ADD:
		ZoomX(1.25, centerX);
		break;
	case '-':
	case WXK_SUBTRACT:
	case WXK_NUMPAD_SUBTRACT:
		ZoomX(1.0 / 1.25, centerX);
		break;
	case WXK_ESCAPE:
		EndDrag();
		break;
	default:
		evt.Skip();
		return;
	}

	UpdateStatusBar();
	ScheduleRefresh();
}

auto cPreviewPanel::ShowContextMenu(const wxPoint& position) -> void
{
	wxMenu menu;

	const int markerIndex = IsInsidePlot(position) ? FindMarkerNearScreenX(position.x, Px(7.0)) : -1;
	if (markerIndex >= 0)
	{
		menu.Append(MenuRemoveMarker, wxString::Format("Remove marker M%d", markerIndex + 1));
	}
	menu.Append(MenuClearMarkers, "Clear all markers\tDelete")->Enable(!m_Markers.empty());
	menu.AppendSeparator();
	menu.AppendCheckItem(MenuLogScale, "Logarithmic counts scale\tL")->Check(m_LogScale);
	menu.AppendCheckItem(MenuFill, "Fill under captured spectrum\tF")->Check(m_FillCaptured);
	menu.AppendCheckItem(MenuPeaks, "Label peaks with element lines\tP")->Check(m_ShowPeakLabels);
	menu.AppendSeparator();
	menu.Append(MenuResetView, "Reset view\tHome")->Enable(m_ViewInitialized);
	menu.Append(MenuExportCsv, "Export visible data as CSV...")->Enable(PrimarySeries() != nullptr);
	menu.Append(MenuSavePng, "Save plot as PNG...")->Enable(PrimarySeries() != nullptr);

	switch (GetPopupMenuSelectionFromUser(menu, position))
	{
	case MenuRemoveMarker:
		if (markerIndex >= 0 && markerIndex < static_cast<int>(m_Markers.size()))
			m_Markers.erase(m_Markers.begin() + markerIndex);
		break;
	case MenuClearMarkers:
		m_Markers.clear();
		break;
	case MenuLogScale:
		m_LogScale = !m_LogScale;
		m_AutoY = true;
		ClampView();
		break;
	case MenuFill:
		m_FillCaptured = !m_FillCaptured;
		break;
	case MenuPeaks:
		m_ShowPeakLabels = !m_ShowPeakLabels;
		if (m_ShowPeakLabels)
			DetectPeaks();
		break;
	case MenuResetView:
		ResetView();
		break;
	case MenuExportCsv:
		ExportVisibleCsv();
		break;
	case MenuSavePng:
	{
		wxFileDialog dialog(this, "Save plot as PNG", "", "spectrum.png", "PNG images (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (dialog.ShowModal() == wxID_OK && !SavePNG(dialog.GetPath()))
			wxLogError("The plot could not be saved to %s.", dialog.GetPath());
		break;
	}
	default:
		break;
	}

	ScheduleRefresh();
}

auto cPreviewPanel::UpdateStatusBar() -> void
{
	if (!m_ParentArguments || !m_ParentArguments->status_bar || !PrimarySeries() || !m_CursorInside)
		return;

	if (std::chrono::steady_clock::now() < m_StatusMessageUntil)
		return;

	const Series* primary = PrimarySeries();
	const double energy = SnapToBin(ScreenToEnergy(m_CursorPos.x));

	wxString text = "E " + PreviewPanelVariables::FormatEnergyValue(energy, EffectiveBinSize(*primary)) + " keV";

	const long long captured = ValueAtEnergy(m_Captured, energy);
	if (captured >= 0)
		text += "   |   Captured " + FormatThousands(static_cast<unsigned long long>(captured));

	const long long reference = ValueAtEnergy(m_Reference, energy);
	if (reference >= 0)
		text += "   |   Reference " + FormatThousands(static_cast<unsigned long long>(reference));

	if (const DetectedPeak* peak = PeakNear(energy); peak && !peak->label.IsEmpty())
		text += "   |   peak " + PreviewPanelVariables::FormatEnergyValue(peak->energyKeV, 0.001) + " keV " + Approx + " " + peak->label;

	if (text == m_LastStatusText)
		return;

	m_LastStatusText = text;
	m_ParentArguments->status_bar->SetStatusText(text);
}

/* ======================= Markers and regions ======================= */

auto cPreviewPanel::AddMarkerAtScreenX(double screenX) -> bool
{
	const Series* primary = PrimarySeries();
	if (!primary)
		return false;

	const double bin = EffectiveBinSize(*primary);
	const double energy = ClampSafe(SnapToBin(ScreenToEnergy(screenX)), 0.0, DomainMaxKeV());

	for (const double existing : m_Markers)
		if (std::abs(existing - energy) < bin * 0.5)
			return false; // one marker per channel

	if (m_Markers.size() >= MaxMarkerCount)
	{
		wxBell();
		if (m_ParentArguments && m_ParentArguments->status_bar)
		{
			m_ParentArguments->status_bar->SetStatusText
			(
				wxString::Format("Maximum number of markers reached (%zu). Right-click a marker to remove it.", MaxMarkerCount)
			);
			m_StatusMessageUntil = std::chrono::steady_clock::now() + std::chrono::seconds(3);
			m_LastStatusText.clear();
		}
		return false;
	}

	m_Markers.push_back(energy);
	std::sort(m_Markers.begin(), m_Markers.end());
	m_LastClickMarkerKeV = energy;
	ScheduleRefresh();
	return true;
}

auto cPreviewPanel::FindMarkerNearScreenX(double screenX, double tolerancePixels) const -> int
{
	int best = -1;
	double bestDistance = tolerancePixels;

	for (std::size_t i = 0; i < m_Markers.size(); ++i)
	{
		if (m_Markers[i] < m_View.xMin || m_Markers[i] > m_View.xMax)
			continue;

		const double distance = std::abs(EnergyToScreen(m_Markers[i]) - screenX);
		if (distance <= bestDistance)
		{
			bestDistance = distance;
			best = static_cast<int>(i);
		}
	}

	return best;
}

auto cPreviewPanel::ComputeRegion(double fromKeV, double toKeV) const -> RegionStatistics
{
	RegionStatistics result{};

	const Series& series = m_Captured.HasData() ? m_Captured : m_Reference;
	const double bin = EffectiveBinSize(series);
	if (!series.HasData() || bin <= 0.0)
		return result;

	if (fromKeV > toKeV)
		std::swap(fromKeV, toKeV);

	const int first = ClampSafe(static_cast<int>(std::lround(fromKeV / bin)), 0, series.Size() - 1);
	const int last = ClampSafe(static_cast<int>(std::lround(toKeV / bin)), 0, series.Size() - 1);
	if (last <= first)
		return result;

	for (int i = first; i <= last; ++i)
		result.gross += series.counts[static_cast<std::size_t>(i)];

	// Linear background between the region edges, each averaged over 3
	// channels, subtracted from the gross sum.
	const auto edgeMean = [&](int center)
		{
			double sum = 0.0;
			int n = 0;
			for (int i = center - 1; i <= center + 1; ++i)
			{
				if (i < 0 || i >= series.Size())
					continue;
				sum += static_cast<double>(series.counts[static_cast<std::size_t>(i)]);
				++n;
			}
			return n ? sum / n : 0.0;
		};

	const double channels = static_cast<double>(last - first + 1);
	result.net = static_cast<double>(result.gross) - channels * (edgeMean(first) + edgeMean(last)) / 2.0;
	result.fromKeV = static_cast<double>(first) * bin;
	result.toKeV = static_cast<double>(last) * bin;
	result.valid = true;
	return result;
}

/* ======================= Peaks and element lines ======================= */

auto cPreviewPanel::NearestLineLabel(double energyKeV, double toleranceKeV, double* lineEnergyKeV) const -> wxString
{
	struct Candidate { double score; double energy; wxString label; };
	std::vector<Candidate> candidates;

	for (const auto& element : XRayLines::Table)
	{
		const std::pair<double, wxString> lines[] =
		{
			{ element.kAlpha1, "K" + GreekAlpha + "1" },
			{ element.kBeta1,  "K" + GreekBeta + "1" },
			{ element.lAlpha1, "L" + GreekAlpha + "1" },
			{ element.lBeta1,  "L" + GreekBeta + "1" },
		};

		for (std::size_t k = 0; k < 4; ++k)
		{
			if (lines[k].first <= 0.0)
				continue;

			const double lineKeV = lines[k].first / 1000.0;
			const double distance = std::abs(lineKeV - energyKeV);
			if (distance > toleranceKeV)
				continue;

			// Prefer the stronger alpha lines over beta lines at similar distance.
			const bool beta = (k == 1 || k == 3);
			candidates.push_back({ distance * (beta ? 1.6 : 1.0), lineKeV, wxString::FromUTF8(element.symbol) + " " + lines[k].second });
		}
	}

	if (candidates.empty())
		return {};

	std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) { return a.score < b.score; });

	if (lineEnergyKeV)
		*lineEnergyKeV = candidates.front().energy;

	// Two lines this close can't be told apart by energy alone: show both.
	wxString label = candidates.front().label;
	if (candidates.size() > 1 && candidates[1].score < candidates.front().score * 2.0 + 0.01)
		label += " / " + candidates[1].label;

	return label;
}

auto cPreviewPanel::PeakNear(double energyKeV) const -> const DetectedPeak*
{
	const DetectedPeak* best = nullptr;

	for (const auto& peak : m_Peaks)
	{
		// Roughly the half width of a peak at this energy for an SDD.
		const double window = std::max(0.05, 0.01 * peak.energyKeV);
		const double distance = std::abs(peak.energyKeV - energyKeV);

		if (distance <= window && (!best || distance < std::abs(best->energyKeV - energyKeV)))
			best = &peak;
	}

	return best;
}

auto cPreviewPanel::DetectPeaks() -> void
{
	m_Peaks.clear();

	const Series& series = m_Captured.HasData() ? m_Captured : m_Reference;
	const double bin = EffectiveBinSize(series);
	const int n = series.Size();
	if (n < 16 || bin <= 0.0)
		return;

	// 5-channel moving average to suppress counting noise
	std::vector<double> smooth(static_cast<std::size_t>(n), 0.0);
	for (int i = 0; i < n; ++i)
	{
		double sum = 0.0;
		int count = 0;
		for (int j = std::max(0, i - 2); j <= std::min(n - 1, i + 2); ++j)
		{
			sum += static_cast<double>(series.counts[static_cast<std::size_t>(j)]);
			++count;
		}
		smooth[static_cast<std::size_t>(i)] = sum / count;
	}

	const double globalMax = *std::max_element(smooth.begin(), smooth.end());
	if (globalMax < 5.0)
		return;

	const int halfWidth = std::max(3, static_cast<int>(std::lround(0.08 / bin)));   // ~80 eV
	const int baseWidth = std::max(9, static_cast<int>(std::lround(0.25 / bin)));   // ~250 eV
	const int firstChannel = static_cast<int>(std::ceil(0.6 / bin));                 // skip the noise peak near 0

	struct Candidate { int index; double prominence; };
	std::vector<Candidate> candidates;

	for (int i = std::max(1, firstChannel); i < n - 1; ++i)
	{
		const double value = smooth[static_cast<std::size_t>(i)];
		if (value < 5.0 || !(value > smooth[static_cast<std::size_t>(i - 1)] && value >= smooth[static_cast<std::size_t>(i + 1)]))
			continue;

		bool isMax = true;
		for (int j = std::max(0, i - halfWidth); j <= std::min(n - 1, i + halfWidth) && isMax; ++j)
			if (smooth[static_cast<std::size_t>(j)] > value)
				isMax = false;
		if (!isMax)
			continue;

		double leftMin = value, rightMin = value;
		for (int j = std::max(0, i - baseWidth); j < i; ++j)
			leftMin = std::min(leftMin, smooth[static_cast<std::size_t>(j)]);
		for (int j = i + 1; j <= std::min(n - 1, i + baseWidth); ++j)
			rightMin = std::min(rightMin, smooth[static_cast<std::size_t>(j)]);

		const double base = std::max(leftMin, rightMin);
		const double prominence = value - base;

		// Significant against Poisson noise of the background, and not tiny
		// compared with the strongest peak.
		if (prominence >= 3.0 * std::sqrt(base + 1.0) && prominence >= 0.01 * globalMax)
			candidates.push_back({ i, prominence });
	}

	std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) { return a.prominence > b.prominence; });
	if (candidates.size() > 15)
		candidates.resize(15);

	for (const auto& candidate : candidates)
	{
		// Report the raw maximum near the smoothed peak.
		int top = candidate.index;
		for (int j = std::max(0, candidate.index - 2); j <= std::min(n - 1, candidate.index + 2); ++j)
			if (series.counts[static_cast<std::size_t>(j)] > series.counts[static_cast<std::size_t>(top)])
				top = j;

		const double energy = static_cast<double>(top) * bin;

		DetectedPeak peak{};
		peak.energyKeV = energy;
		peak.counts = series.counts[static_cast<std::size_t>(top)];
		peak.label = NearestLineLabel(energy, std::max(0.06, 0.012 * energy));
		m_Peaks.push_back(peak);
	}
}

/* ======================= Export ======================= */

bool cPreviewPanel::SavePNG(const wxString& filePath)
{
	// Always exported at a fixed resolution and in the light theme, so a
	// small window never produces a small graph and prints stay readable.
	const wxSize exportSize(PreviewPanelVariables::EXPORT_WIDTH, PreviewPanelVariables::EXPORT_HEIGHT);

	const Layout savedLayout = m_Layout;
	m_Layout = ComputeLayout(exportSize, 1.6, true);

	wxBitmap bitmap(exportSize.GetWidth(), exportSize.GetHeight(), 32);
	{
		wxMemoryDC dc(bitmap);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
		if (gc)
			RenderAll(gc.get(), true);

		dc.SelectObject(wxNullBitmap);
	}

	m_Layout = savedLayout;

	return bitmap.IsOk() && bitmap.ConvertToImage().SaveFile(filePath, wxBITMAP_TYPE_PNG);
}

auto cPreviewPanel::ExportVisibleCsv() -> void
{
	const Series* primary = PrimarySeries();
	if (!primary)
		return;

	wxFileDialog dialog(this, "Export visible data as CSV", "", "spectrum.csv", "CSV files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() != wxID_OK)
		return;

	// Energy grid of the primary spectrum over the whole visible range; a
	// longer reference is not cut at the primary's last channel.
	const double bin = EffectiveBinSize(*primary);
	const int first = std::max(0, static_cast<int>(std::ceil(m_View.xMin / bin - 1e-9)));
	const int last = static_cast<int>(std::floor(std::min(m_View.xMax, DomainMaxKeV()) / bin + 1e-9));

	std::ostringstream out;
	out << "# VEXOTControl spectrum export\n";
	out << "# Energy range: " << std::fixed << std::setprecision(4) << m_View.xMin << " - " << m_View.xMax << " keV\n";

	const bool hasCaptured = m_Captured.HasData() && EffectiveBinSize(m_Captured) > 0.0;
	const bool hasReference = m_Reference.HasData() && EffectiveBinSize(m_Reference) > 0.0;

	if (hasReference && hasCaptured && std::abs(EffectiveBinSize(m_Reference) - EffectiveBinSize(m_Captured)) > 1e-12)
		out << "# Reference values are taken from the reference channel nearest to each energy.\n";

	out << "Energy_keV";
	if (hasCaptured) out << ",Captured";
	if (hasReference) out << ",Reference";
	out << "\n";

	for (int i = first; i <= last; ++i)
	{
		const double energy = static_cast<double>(i) * bin;
		out << std::setprecision(5) << energy;

		if (hasCaptured)
		{
			const long long value = ValueAtEnergy(m_Captured, energy);
			out << ',';
			if (value >= 0) out << value;
		}

		if (hasReference)
		{
			const long long value = ValueAtEnergy(m_Reference, energy);
			out << ',';
			if (value >= 0) out << value;
		}

		out << "\n";
	}

	std::string error{};
	if (!FileUtils::WriteFileAtomically(std::filesystem::path(dialog.GetPath().ToStdWstring()), out.str(), &error))
	{
		wxLogError("The data could not be exported to %s:\n%s", dialog.GetPath(), wxString::FromUTF8(error));
		return;
	}

	AppLog::Info("Data", "Exported visible spectrum to " + dialog.GetPath().utf8_string());
}

/* ======================= Header information ======================= */

auto cPreviewPanel::SetBackgroundColor(wxColour bckg_colour) -> void
{
	SetBackgroundColour(bckg_colour);
	ScheduleRefresh();
}

auto cPreviewPanel::SetCursorOverlayActive(bool activate) -> void
{
	if (m_RenderCursorOverlay == activate)
		return;

	m_RenderCursorOverlay = activate;
	ScheduleRefresh();
}

void cPreviewPanel::SetPerformanceOverlayEnabled(bool enabled, double exposureSeconds)
{
	m_ShowPerformanceOverlay = enabled;
	m_ExposureSecondsForFPS = std::max(0.0, exposureSeconds);

	if (!enabled)
		ResetFrameStats();

	ScheduleRefresh();
}

void cPreviewPanel::NotifyNewFrame(unsigned long long frameNumber)
{
	if (!m_ShowPerformanceOverlay)
		return;

	m_CurrentFrameNumber = frameNumber;

	const auto now = std::chrono::steady_clock::now();
	double frameSeconds = m_ExposureSecondsForFPS;

	if (m_HasLastFrameTime)
	{
		const double measured =
			std::chrono::duration_cast<std::chrono::microseconds>(now - m_LastFrameTime).count() / 1'000'000.0;

		frameSeconds = frameSeconds > 0.0 ? std::max(measured, frameSeconds) : measured;
	}

	if (frameSeconds > 0.0)
	{
		const double fps = 1.0 / frameSeconds;
		m_DisplayedFPS = m_DisplayedFPS <= 0.0 ? fps : 0.85 * m_DisplayedFPS + 0.15 * fps;
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
	ScheduleRefresh();
}

void cPreviewPanel::SetSDDTemperature(double temperatureC)
{
	m_CurrentSDDTemperatureC = temperatureC;
	m_HasSDDTemperature = true;
	ScheduleRefresh();
}
