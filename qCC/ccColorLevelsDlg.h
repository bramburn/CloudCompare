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

#ifndef CC_COLOR_LEVELS_DLG_HEADER
#define CC_COLOR_LEVELS_DLG_HEADER

/**
 * @file ccColorLevelsDlg.h
 *
 * @brief Color levels dialog
 *
 * Dialog for adjusting color levels.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QColor>
#include <ui_colorLevelsDlg.h>

class ccHistogramWindow;
class ccGenericPointCloud;

/**
 * @brief Color levels dialog
 *
 * Adjust color levels with histogram display.
 */
class ccColorLevelsDlg : public QDialog
    , public Ui::ColorLevelsDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 * @param[in] pointCloud Point cloud
	 */
	ccColorLevelsDlg(QWidget* parent, ccGenericPointCloud* pointCloud);

	/**
	 * @brief Scale color fields
	 * @param[in] cloud Point cloud
	 * @param[in] inputLevelMin Input minimum
	 * @param[in] inputLevelMax Input maximum
	 * @param[in] outputLevelMin Output minimum
	 * @param[in] outputLevelMax Output maximum
	 * @param[in] applyRGB Apply flags
	 * @return true on success
	 */
	static bool ScaleColorFields(ccGenericPointCloud* cloud, int inputLevelMin, int inputLevelMax, int outputLevelMin, int outputLevelMax, const bool applyRGB[3]);

  protected:
	void onChannelChanged(int);
	void onApply();

	/// Channel
	enum CHANNELS
	{
		RGB = 0,
		RED = 1,
		GREEN = 2,
		BLUE = 3
	};

	/// Update histogram
	void updateHistogram();

	/// Histogram view
	ccHistogramWindow* m_histogram;

	/// Point cloud
	ccGenericPointCloud* m_cloud;
};

#endif // CC_COLOR_LEVELS_DLG_HEADER
