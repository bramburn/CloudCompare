#pragma once

// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

// Always first
#include <ccIncludeGL.h>

// Qt
#include <QDialog>

/**
 * @file ccHistogramWindow.h
 *
 * @brief Histogram widget for displaying scalar field distributions.
 *
 * @details Widget for visualizing scalar field value distributions
 * as histograms.
 *
 * Features:
 * - Histogram display from scalar fields
 * - Configurable number of bins/classes
 * - Multiple color schemes (solid, gradient, SF scale)
 * - Interactive display/saturation range selection
 * - Overlay curve support
 * - Export to CSV/image
 *
 * Used throughout CloudCompare for visualizing:
 * - Scalar field value distributions
 * - Color/height statistics
 * - Distance comparisons
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccScalarField
 * @see ccColorLevelsDlg
 */

// qCC_db
#include <ccScalarField.h>

// QCustomPlot
#include "ccQCustomPlot.h"

class QCPArrow;
class QCPBarsWithText;
class QCPColoredBars;
class QCPHiddenArea;
class QCPTextElement;

class Ui_HistogramDialog;

/**
 * @brief Widget for displaying scalar field histograms.
 *
 * @details Extends QCustomPlot to provide histogram visualization
 * capabilities for CloudCompare scalar fields.
 *
 * Features:
 * - Build histograms from scalar fields
 * - Interactive range selection
 * - Multiple color schemes
 * - Overlay curves (e.g., cumulative distribution)
 * - Zoom and pan
 *
 * @extends QCustomPlot
 */
class ccHistogramWindow : public QCustomPlot
{
	Q_OBJECT

  public:
	/**
	 * @brief Color scheme options.
	 */
	enum HISTOGRAM_COLOR_SCHEME
	{
		USE_SOLID_COLOR,     //!< Single solid color
		USE_CUSTOM_COLOR_SCALE, //!< Gradient color scale
		USE_SF_SCALE         //!< Scalar field color scale
	};

	/**
	 * @brief Axis display options.
	 */
	enum class AxisDisplayOption
	{
		None  = 0x0,
		XAxis = 0x01,
		YAxis = 0x02,
		All   = XAxis | YAxis
	};
	Q_DECLARE_FLAGS(AxisDisplayOptions, AxisDisplayOption)

	/**
	 * @brief Scalar field interaction modes.
	 */
	enum class SFInteractionMode
	{
		None            = 0x0,
		DisplayRange    = 0x01, //!< Interactive display range
		SaturationRange = 0x02, //!< Interactive saturation range
		All             = DisplayRange | SaturationRange
	};
	Q_DECLARE_FLAGS(SFInteractionModes, SFInteractionMode)

	/**
	 * @brief Construct the histogram window.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccHistogramWindow(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccHistogramWindow();

	/**
	 * @brief Set the histogram title.
	 *
	 * @param[in] str Title string.
	 */
	void setTitle(const QString& str);

	/**
	 * @brief Set axis labels.
	 *
	 * @param[in] xLabel X-axis label.
	 * @param[in] yLabel Y-axis label.
	 */
	void setAxisLabels(const QString& xLabel, const QString& yLabel);

	/**
	 * @brief Compute histogram from a scalar field.
	 *
	 * @param[in] sf Scalar field to visualize.
	 * @param[in] initialNumberOfClasses Initial bin count.
	 * @param[in] numberOfClassesCanBeChanged Allow user to change bin count.
	 * @param[in] showNaNValuesInGrey Show NaN values in gray.
	 */
	void fromSF(ccScalarField* sf,
	            unsigned       initialNumberOfClasses      = 0,
	            bool           numberOfClassesCanBeChanged = true,
	            bool           showNaNValuesInGrey         = true);

	/**
	 * @brief Create histogram from a bin array.
	 *
	 * @param[in] histoValues Bin counts.
	 * @param[in] minVal Minimum value.
	 * @param[in] maxVal Maximum value.
	 */
	void fromBinArray(const std::vector<unsigned>& histoValues,
	                  double                       minVal,
	                  double                       maxVal);

	/**
	 * @brief Create histogram from a bin array.
	 *
	 * @param[in] histoValues Bin counts.
	 * @param[in] sf Scalar field for reference.
	 */
	void fromBinArray(const std::vector<unsigned>& histoValues,
	                  ccScalarField*               sf);

	/**
	 * @brief Set overlay curve values.
	 *
	 * @param[in] curveValues Y-values for overlay curve.
	 *
	 * @details Used to show cumulative distribution or similar.
	 */
	void setCurveValues(const std::vector<double>& curveValues);

	/**
	 * @brief Set the color scheme.
	 *
	 * @param[in] scheme Color scheme to use.
	 */
	void setColorScheme(HISTOGRAM_COLOR_SCHEME scheme)
	{
		m_colorScheme = scheme;
	}

	/**
	 * @brief Set solid color.
	 *
	 * @param[in] color Color to use.
	 *
	 * @note Only used with USE_SOLID_COLOR scheme.
	 */
	void setSolidColor(QColor color)
	{
		m_solidColor = color;
	}

	/**
	 * @brief Set gradient color scale.
	 *
	 * @param[in] scale Color scale to use.
	 *
	 * @note Only used with USE_CUSTOM_COLOR_SCALE scheme.
	 */
	void setColorScale(ccColorScale::Shared scale)
	{
		m_colorScale = scale;
	}

	/**
	 * @brief Clear the histogram.
	 */
	void clear();

	/**
	 * @brief Refresh the display.
	 */
	void refresh();

	/**
	 * @brief Refresh histogram bars only.
	 */
	void refreshBars();

	/**
	 * @brief Get histogram bins.
	 * @return Bin counts.
	 */
	inline const std::vector<unsigned>& histoValues() const
	{
		return m_histoValues;
	}

	/**
	 * @brief Get minimum value.
	 * @return Minimum histogram value.
	 */
	inline double minVal() const
	{
		return m_minVal;
	}

	/**
	 * @brief Get maximum value.
	 * @return Maximum histogram value.
	 */
	inline double maxVal() const
	{
		return m_maxVal;
	}

	/**
	 * @brief Enable SF interaction mode.
	 *
	 * @param[in] modes Interaction modes to enable.
	 */
	void setSFInteractionMode(SFInteractionModes modes);

	/**
	 * @brief Set axis display options.
	 *
	 * @param[in] axisOptions Which axes to show.
	 */
	void setAxisDisplayOption(AxisDisplayOptions axisOptions);

	/**
	 * @brief Set refresh behavior after resize.
	 *
	 * @param[in] refreshAfterResize Auto-refresh after resize.
	 */
	void setRefreshAfterResize(bool refreshAfterResize);

	/**
	 * @brief Set minimum display value.
	 * @param[in] value Minimum value.
	 */
	void setMinDispValue(double value);

	/**
	 * @brief Set maximum display value.
	 * @param[in] value Maximum value.
	 */
	void setMaxDispValue(double value);

	/**
	 * @brief Set minimum saturation value.
	 * @param[in] value Minimum saturation.
	 */
	void setMinSatValue(double value);

	/**
	 * @brief Set maximum saturation value.
	 * @param[in] value Maximum saturation.
	 */
	void setMaxSatValue(double value);

  signals:
	/**
	 * @brief Display min value changed.
	 * @param[in] value New minimum.
	 */
	void sfMinDispValChanged(double value);

	/**
	 * @brief Display max value changed.
	 * @param[in] value New maximum.
	 */
	void sfMaxDispValChanged(double value);

	/**
	 * @brief Saturation min value changed.
	 * @param[in] value New minimum.
	 */
	void sfMinSatValChanged(double value);

	/**
	 * @brief Saturation max value changed.
	 * @param[in] value New maximum.
	 */
	void sfMaxSatValChanged(double value);

  protected slots:
	/**
	 * @brief Set number of bins.
	 *
	 * @param[in] n Number of bins (should be multiple of 4).
	 */
	void setNumberOfClasses(size_t n);

  protected: // events
	/**
	 * @brief Mouse press handler.
	 */
	void mousePressEvent(QMouseEvent* event) override;

	/**
	 * @brief Mouse move handler.
	 */
	void mouseMoveEvent(QMouseEvent* event) override;

	/**
	 * @brief Mouse wheel handler.
	 */
	void wheelEvent(QWheelEvent* event) override;

	/**
	 * @brief Resize handler.
	 */
	void resizeEvent(QResizeEvent* event) override;

	/**
	 * @brief Get maximum bin count.
	 * @return Maximum value in bins.
	 */
	unsigned getMaxHistoVal();

	/**
	 * @brief Clear internal structures.
	 */
	void clearInternal();

	/**
	 * @brief Compute bins from scalar field.
	 *
	 * @param[in] binCount Number of bins.
	 * @return true on success.
	 */
	bool computeBinArrayFromSF(size_t binCount);

	/**
	 * @brief Update overlay curve width.
	 *
	 * @param[in] w Widget width.
	 * @param[in] h Widget height.
	 */
	void updateOverlayCurveWidth(int w, int h);

  protected:
	//! Title string
	QString m_titleStr;

	//! Title element
	QCPTextElement* m_titlePlot;

	//! Color scheme
	HISTOGRAM_COLOR_SCHEME m_colorScheme;

	//! Solid color
	QColor m_solidColor;

	//! Color scale
	ccColorScale::Shared m_colorScale;

	//! Associated scalar field
	ccScalarField* m_associatedSF;

	//! Whether bin count can be changed
	bool m_numberOfClassesCanBeChanged;

	//! Auto-refresh after resize
	bool m_refreshAfterResize;

	//! Histogram bars
	QCPColoredBars* m_histogram;

	//! Bin values
	std::vector<unsigned> m_histoValues;

	//! Min/max values
	double m_minVal;
	double m_maxVal;
	unsigned m_maxHistoVal;

	//! Overlay curve
	QCPGraph* m_overlayCurve;
	std::vector<double> m_curveValues;

	//! Vertical indicator
	QCPBarsWithText* m_vertBar;
	bool m_drawVerticalIndicator;
	double m_verticalIndicatorPositionPercent;

	//! Rendering font
	QFont m_renderingFont;

	//! Axis display options
	AxisDisplayOptions m_axisDisplayOptions;

	//! SF interaction modes
	SFInteractionModes m_sfInteractionModes;

	//! Selectable items in SF mode
	enum SELECTABLE_ITEMS
	{
		NONE,
		LEFT_AREA,
		RIGHT_AREA,
		BOTH_AREAS,
		LEFT_ARROW,
		RIGHT_ARROW,
		BOTH_ARROWS
	};

	//! Current selection
	SELECTABLE_ITEMS m_selectedItem;

	//! Greyed areas
	QCPHiddenArea* m_areaLeft;
	double m_areaLeftlastValue;
	QCPHiddenArea* m_areaRight;
	double m_areaRightlastValue;

	//! Arrows
	QCPArrow* m_arrowLeft;
	double m_arrowLeftlastValue;
	QCPArrow* m_arrowRight;
	double m_arrowRightlastValue;

	//! Last mouse click
	QPoint m_lastMouseClick;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ccHistogramWindow::AxisDisplayOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(ccHistogramWindow::SFInteractionModes)

/**
 * @brief Dialog wrapper for ccHistogramWindow.
 *
 * @details Provides a standalone dialog containing a histogram
 * with export functionality.
 *
 * @extends QDialog
 */
class ccHistogramWindowDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the histogram dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccHistogramWindowDlg(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccHistogramWindowDlg();

	/**
	 * @brief Get the histogram window.
	 * @return Pointer to histogram window.
	 */
	inline ccHistogramWindow* window()
	{
		return m_win;
	}

	/**
	 * @brief Export histogram to CSV.
	 *
	 * @param[in] filename Output file path.
	 * @return true on success.
	 */
	bool exportToCSV(QString filename) const;

  protected slots:
	/**
	 * @brief Export to CSV button handler.
	 */
	void onExportToCSV();

	/**
	 * @brief Export to image button handler.
	 */
	void onExportToImage();

  protected:
	//! Histogram window
	ccHistogramWindow* m_win;

	//! UI definition
	Ui_HistogramDialog* m_gui;
};
