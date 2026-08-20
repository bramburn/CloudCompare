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
// #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_COLOR_LEVELS_DLG_HEADER
#define CC_COLOR_LEVELS_DLG_HEADER

/**
 * @file ccColorLevelsDlg.h
 *
 * @brief Color levels dialog for adjusting color distribution.
 *
 * @details Dialog for adjusting color levels (brightness, contrast)
 * of point cloud colors. Provides:
 * - Histogram display of color distribution
 * - Input/output level sliders
 * - Per-channel or combined RGB adjustment
 *
 * This is similar to the "Levels" adjustment in photo editing
 * software like Photoshop or GIMP.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccHistogramWindow
 * @see ccEntityAction::adjustColorLevels()
 */

// Qt
#include <QColor>
#include <ui_colorLevelsDlg.h>

class ccHistogramWindow;
class ccGenericPointCloud;

/**
 * @brief Dialog for adjusting color levels.
 *
 * @details Provides an interface similar to image editing software
 * for adjusting the color levels of point cloud colors.
 *
 * Features:
 * - Histogram display showing color distribution
 * - Input level sliders (black point / white point)
 * - Output level sliders (brightness adjustment)
 * - Per-channel adjustment (R, G, B separately)
 * - Combined RGB adjustment
 *
 * The dialog shows a histogram of the selected channel(s)
 * and allows the user to adjust the tonal range.
 *
 * @extends QDialog
 * @extends Ui::ColorLevelsDialog
 */
class ccColorLevelsDlg : public QDialog
    , public Ui::ColorLevelsDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Color channels for adjustment.
	 */
	enum CHANNELS
	{
		RGB   = 0, //!< All RGB channels combined
		RED   = 1, //!< Red channel only
		GREEN = 2, //!< Green channel only
		BLUE  = 3  //!< Blue channel only
	};

	/**
	 * @brief Construct the color levels dialog.
	 *
	 * @param[in] parent Parent widget.
	 * @param[in] pointCloud Point cloud to adjust colors for.
	 */
	ccColorLevelsDlg(QWidget* parent, ccGenericPointCloud* pointCloud);

	/**
	 * @brief Apply color scaling to a cloud.
	 *
	 * @param[in] cloud Point cloud to modify.
	 * @param[in] inputLevelMin Input black point (0-255).
	 * @param[in] inputLevelMax Input white point (0-255).
	 * @param[in] outputLevelMin Output minimum (0-255).
	 * @param[in] outputLevelMax Output maximum (0-255).
	 * @param[in] applyRGB Which channels to apply (R, G, B).
	 *
	 * @return true on success.
	 *
	 * @details Static method to apply level adjustment to a cloud's
	 * colors. Values are mapped linearly from input range to
	 * output range.
	 *
	 * @note The cloud's colors are modified in place.
	 */
	static bool ScaleColorFields(ccGenericPointCloud* cloud, int inputLevelMin, int inputLevelMax, int outputLevelMin, int outputLevelMax, const bool applyRGB[3]);

  protected slots:
	/**
	 * @brief Handle channel selection change.
	 *
	 * @param[in] index Selected channel index.
	 */
	void onChannelChanged(int index);

	/**
	 * @brief Apply the color adjustments.
	 */
	void onApply();

  protected:
	/**
	 * @brief Update the histogram display.
	 */
	void updateHistogram();

	//! Histogram display widget
	ccHistogramWindow* m_histogram;

	//! Point cloud being edited
	ccGenericPointCloud* m_cloud;
};

#endif // CC_COLOR_LEVELS_DLG_HEADER
