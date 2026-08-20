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
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

#ifndef CC_FROM_SCALAR_DLG_HEADER
#define CC_FROM_SCALAR_DLG_HEADER

/**
 * @file ccColorFromScalarDlg.h
 *
 * @brief Color from scalar dialog for mapping scalar fields to colors.
 *
 * @details Dialog for mapping scalar field values to RGBA color channels.
 * Each color channel (R, G, B, A) can be independently mapped to
 * any scalar field in the point cloud, with customizable:
 * - Value range (saturation limits)
 * - Color scale (gradient)
 * - Reverse mapping option
 *
 * This enables sophisticated colorization of point clouds based
 * on their scalar field values.
 *
 * @author CloudCompare project
 *
 * @see ccScalarField
 * @see ccColorScale
 * @see ccHistogramWindow
 */

// Qt
#include <QColor>

// CC
#include <ccColorScale.h>
#include <ccHistogramWindow.h>
#include <ccScalarField.h>

class ccPointCloud;

namespace Ui
{
	class ColorFromScalarDialog;
}

/**
 * @brief Dialog for mapping scalar fields to colors.
 *
 * @details Provides a UI for mapping scalar field values to RGBA
 * color channels. Each channel can be independently configured:
 *
 * - **Channel Selection**: Choose which scalar field maps to R, G, B, A
 * - **Range**: Set the min/max values that map to 0 and 255
 * - **Color Scale**: Select the gradient used for the mapping
 * - **Reverse**: Invert the color mapping
 *
 * The dialog displays histograms for each channel to help the user
 * understand the distribution of values.
 *
 * This is useful for:
 * - Creating false-color visualizations
 * - Mapping multiple properties to color channels
 * - Highlighting specific value ranges
 *
 * @extends QDialog
 */
class ccColorFromScalarDlg : public QDialog
{
	Q_OBJECT

  public:
	//! Number of color channels
	static constexpr int c_channelCount = 4;

	/**
	 * @brief Construct the color-from-scalar dialog.
	 *
	 * @param[in] parent Parent widget.
	 * @param[in] pointCloud Point cloud with scalar fields to map.
	 */
	ccColorFromScalarDlg(QWidget* parent, ccPointCloud* pointCloud);

	/**
	 * @brief Destructor.
	 */
	~ccColorFromScalarDlg();

	/**
	 * @brief Refresh the display.
	 */
	void refreshDisplay();

	/**
	 * @brief Update histogram for a channel.
	 *
	 * @param[in] channel Channel index (0=R, 1=G, 2=B, 3=A).
	 */
	void updateHistogram(int channel);

  protected slots:
	/**
	 * @brief Handle R channel scalar field change.
	 * @param[in] index New scalar field index.
	 */
	void onChannelChangedR(int index)
	{
		updateChannel(0);
	}

	/**
	 * @brief Handle G channel scalar field change.
	 * @param[in] index New scalar field index.
	 */
	void onChannelChangedG(int index)
	{
		updateChannel(1);
	}

	/**
	 * @brief Handle B channel scalar field change.
	 * @param[in] index New scalar field index.
	 */
	void onChannelChangedB(int index)
	{
		updateChannel(2);
	}

	/**
	 * @brief Handle A channel scalar field change.
	 * @param[in] index New scalar field index.
	 */
	void onChannelChangedA(int index)
	{
		updateChannel(3);
	}

	/**
	 * @brief Update a specific channel.
	 * @param[in] channel Channel index.
	 */
	void updateChannel(int channel);

	/**
	 * @brief Update spin box limits.
	 * @param[in] channel Channel index.
	 */
	void updateSpinBoxLimits(int channel);

	/**
	 * @brief Set default saturation for a channel.
	 * @param[in] channel Channel index.
	 */
	void setDefaultSatValuePerChannel(int channel);

	/**
	 * @brief Update color map displays.
	 */
	void updateColormaps();

	/**
	 * @brief Handle minimum value change.
	 * @param[in] n Channel index.
	 * @param[in] val New minimum value.
	 * @param[in] slider Whether change came from slider.
	 */
	void minChanged(int n, double val, bool slider);

	/**
	 * @brief Handle maximum value change.
	 * @param[in] n Channel index.
	 * @param[in] val New maximum value.
	 * @param[in] slider Whether change came from slider.
	 */
	void maxChanged(int n, double val, bool slider);

	/**
	 * @brief Handle R min spin box change.
	 */
	void minSpinChangedR(double val) { minChanged(0, val, false); }

	/**
	 * @brief Handle R max spin box change.
	 */
	void maxSpinChangedR(double val) { maxChanged(0, val, false); }

	/**
	 * @brief Handle G min spin box change.
	 */
	void minSpinChangedG(double val) { minChanged(1, val, false); }

	/**
	 * @brief Handle G max spin box change.
	 */
	void maxSpinChangedG(double val) { maxChanged(1, val, false); }

	/**
	 * @brief Handle B min spin box change.
	 */
	void minSpinChangedB(double val) { minChanged(2, val, false); }

	/**
	 * @brief Handle B max spin box change.
	 */
	void maxSpinChangedB(double val) { maxChanged(2, val, false); }

	/**
	 * @brief Handle A min spin box change.
	 */
	void minSpinChangedA(double val) { minChanged(3, val, false); }

	/**
	 * @brief Handle A max spin box change.
	 */
	void maxSpinChangedA(double val) { maxChanged(3, val, false); }

	/**
	 * @brief Handle R toggle.
	 */
	void toggleFixedR() { updateChannel(0); }

	/**
	 * @brief Handle G toggle.
	 */
	void toggleFixedG() { updateChannel(1); }

	/**
	 * @brief Handle B toggle.
	 */
	void toggleFixedB() { updateChannel(2); }

	/**
	 * @brief Handle A toggle.
	 */
	void toggleFixedA() { updateChannel(3); }

	/**
	 * @brief Toggle colors.
	 * @param[in] val Toggle state.
	 */
	void toggleColors(int val);

	/**
	 * @brief Toggle color mode.
	 * @param[in] state Mode state.
	 */
	void toggleColorMode(bool state);

	/**
	 * @brief Apply changes and close.
	 */
	void onApply();

	/**
	 * @brief Disable all except cancel button.
	 */
	void disableAllButCancel();

  protected:
	/**
	 * @brief Handle resize events.
	 * @param[in] event Resize event.
	 */
	void resizeEvent(QResizeEvent* event) override;

	//! Previous fixed state for channels
	bool m_prevFixed[c_channelCount];

	//! Histogram displays for each channel
	/** Index: 0=red, 1=green, 2=blue, 3=alpha */
	ccHistogramWindow* m_histograms[c_channelCount];

	//! Scalar fields for each channel
	ccScalarField* m_scalars[c_channelCount];

	//! Scalar field combo boxes
	QComboBox* m_combos[c_channelCount];

	//! Minimum spin boxes
	QDoubleSpinBox* m_boxes_min[c_channelCount];

	//! Maximum spin boxes
	QDoubleSpinBox* m_boxes_max[c_channelCount];

	//! Minimum labels
	QLabel* m_labels_min[c_channelCount];

	//! Maximum labels
	QLabel* m_labels_max[c_channelCount];

	//! Reverse checkboxes
	QCheckBox* m_reverse[c_channelCount];

	//! Minimum saturation values
	double m_minSat[c_channelCount];

	//! Maximum saturation values
	double m_maxSat[c_channelCount];

	//! Color scales for each channel
	ccColorScale::Shared m_colors[c_channelCount];

	//! Associated point cloud
	ccPointCloud* m_cloud;

	//! Original color scale (for restoration)
	ccColorScale::Shared m_storedOrigColorScale;

	//! Original saturation range
	ccScalarField::Range m_storedOrigSatRange;

	//! Original display range
	ccScalarField::Range m_storedOrigDisplayRange;

	//! System invalid state
	bool m_systemInvalid;

  private:
	//! UI definition
	Ui::ColorFromScalarDialog* m_ui;
};

#endif // CC_FROM_SCALAR_DLG_HEADER
