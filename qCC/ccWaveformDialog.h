// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                       COPYRIGHT: CNRS / OSUR                           #
// #                                                                        //
// ##########################################################################

#ifndef CC_WAVEFORM_DIALOG_HEADER
#define CC_WAVEFORM_DIALOG_HEADER

/**
 * @file ccWaveformDialog.h
 *
 * @brief Waveform visualization dialog for waveform-capable sensors.
 *
 * @details Dialog and widget for displaying waveform data from
 * terrestrial laser scanners (e.g., Riegl, FARO).
 *
 * Waveforms contain:
 * - Time-resolved return intensity
 * - Multiple echo information
 * - Peak detection
 *
 * Features:
 * - Waveform curve display
 * - Peak markers
 * - Vertical indicators
 * - 2D label visualization
 * - CSV export
 *
 * @author CNRS / OSUR
 */

#include "cc2DLabel.h"
#include "ccPickingListener.h"

#include <QDialog>

// QCustomPlot
#include "ccQCustomPlot.h"

class QCPArrow;
class QCPBarsWithText;
class QCPColoredBars;
class QCPHiddenArea;
class QCPTextElement;

class Ui_WaveDialog;
class ccPointCloud;
class ccPickingHub;

/**
 * @brief Widget for displaying waveform curves.
 *
 * @extends QCustomPlot
 */
class ccWaveWidget : public QCustomPlot
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the waveform widget.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccWaveWidget(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccWaveWidget() override;

	/**
	 * @brief Set the title.
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
	 * @brief Initialize waveform display.
	 *
	 * @param[in] cloud Point cloud.
	 * @param[in] pointIndex Index of the point.
	 * @param[in] logScale Use logarithmic scale.
	 * @param[in] maxValue Maximum amplitude.
	 */
	void init(ccPointCloud* cloud, unsigned pointIndex, bool logScale, double maxValue = 0.0);

	/**
	 * @brief Clear the display.
	 */
	void clear();

	/**
	 * @brief Refresh the display.
	 */
	void refresh();

  protected:
	//! Handle mouse press
	void mousePressEvent(QMouseEvent* event) override;

	//! Handle mouse move
	void mouseMoveEvent(QMouseEvent* event) override;

	//! Handle resize
	void resizeEvent(QResizeEvent* event) override;

	/**
	 * @brief Clear internal structures.
	 */
	void clearInternal();

	/**
	 * @brief Update curve width.
	 *
	 * @param[in] w Widget width.
	 * @param[in] h Widget height.
	 */
	void updateCurveWidth(int w, int h);

  protected:
	//! Title string
	QString m_titleStr;

	//! Title element
	QCPTextElement* m_titlePlot;

	//! Wave curve
	QCPGraph* m_curve;

	//! Curve values
	std::vector<double> m_curveValues;

	//! Time step
	double m_dt;

	//! Min amplitude
	double m_minA;

	//! Max amplitude
	double m_maxA;

	//! Echo position
	double m_echoPos;

	//! Vertical indicator bar
	QCPBarsWithText* m_vertBar;

	//! Draw vertical indicator
	bool m_drawVerticalIndicator;

	//! Vertical indicator position (%)
	double m_verticalIndicatorPositionPercent;

	//! Peak marker
	QCPBarsWithText* m_peakBar;

	//! Rendering font
	QFont m_renderingFont;

	//! Last mouse click position
	QPoint m_lastMouseClick;
};

/**
 * @brief Waveform dialog for point cloud waveform visualization.
 *
 * @extends QDialog
 * @extends ccPickingListener
 */
class ccWaveDialog : public QDialog
    , public ccPickingListener
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the waveform dialog.
	 *
	 * @param[in] cloud Point cloud with waveforms.
	 * @param[in] pickingHub Picking hub for point selection.
	 * @param[in] parent Parent widget.
	 */
	explicit ccWaveDialog(ccPointCloud* cloud, ccPickingHub* pickingHub, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccWaveDialog() override;

	/**
	 * @brief Get the waveform widget.
	 *
	 * @return Waveform widget.
	 */
	inline ccWaveWidget* waveWidget()
	{
		return m_widget;
	}

	/**
	 * @brief Handle item picked.
	 *
	 * @param[in] pi Picked item.
	 */
	virtual void onItemPicked(const PickedItem& pi) override;

  protected slots:
	/**
	 * @brief Handle point index change.
	 *
	 * @param[in] index New index.
	 */
	void onPointIndexChanged(int index);

	/**
	 * @brief Add 2D label for point.
	 *
	 * @param[in] cloud Point cloud.
	 * @param[in] pointIndex Point index.
	 */
	void add2DLabel(ccPointCloud* cloud, unsigned int pointIndex);

	/**
	 * @brief Update current waveform display.
	 */
	void updateCurrentWaveform();

	/**
	 * @brief Handle picking button toggle.
	 *
	 * @param[in] checked Toggle state.
	 */
	void onPointPickingButtonToggled(bool checked);

	/**
	 * @brief Export waveform as CSV.
	 */
	void onExportWaveAsCSV();

  protected:
	//! Point cloud
	ccPointCloud* m_cloud;

	//! Wave widget
	ccWaveWidget* m_widget;

	//! Picking hub
	ccPickingHub* m_pickingHub;

	//! GUI definition
	Ui_WaveDialog* m_gui;

	//! Max wave amplitude
	double m_waveMax;

	//! 2D label for visualization
	std::shared_ptr<cc2DLabel> m_label;

	//! Associated display
	ccGenericGLDisplay* m_display;
};

#endif // CC_WAVEFORM_DIALOG_HEADER
