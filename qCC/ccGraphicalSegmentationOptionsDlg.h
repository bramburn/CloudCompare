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
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

#ifndef CC_GRAPHICAL_SEGMENTATION_OPTIONS_DLG_HEADER
#define CC_GRAPHICAL_SEGMENTATION_OPTIONS_DLG_HEADER

/**
 * @file ccGraphicalSegmentationOptionsDlg.h
 *
 * @brief Segmentation options dialog for point cloud segmentation.
 *
 * @details Dialog for configuring options used by the graphical
 * segmentation tool.
 *
 * Segmentation separates a point cloud into:
 * - **Remaining points**: Outside the segmented region
 * - **Segmented points**: Inside the segmented region
 *
 * Options include:
 * - Segmented/Remaining suffix naming
 * - Segmentation mode (include/exclude)
 * - Display preferences
 *
 * @author CloudCompare project
 *
 * @see ccGraphicalSegmentationTool
 */

// Qt
#include <QString>

// GUI
#include <ui_graphicalSegmentationOptionsDlg.h>

/**
 * @brief Dialog for segmentation tool configuration.
 *
 * @details Provides options for the graphical segmentation process
 * including naming conventions and display settings.
 *
 * Settings are persisted to application settings.
 *
 * @extends QDialog
 */
class ccGraphicalSegmentationOptionsDlg : public QDialog
    , public Ui::GraphicalSegmentationOptionsDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the options dialog.
	 *
	 * @param[in] windowTitle Window title.
	 * @param[in] parent Parent widget.
	 */
	ccGraphicalSegmentationOptionsDlg(const QString windowTitle = QString(), QWidget* parent = nullptr);

	/**
	 * @brief Accept and save settings.
	 */
	void accept() override;

	/**
	 * @brief Get settings key for segmentation tool options.
	 *
	 * @return Settings key.
	 */
	static QString SegmentationToolOptionsKey()
	{
		return "SegmentationToolOptions";
	}

	/**
	 * @brief Get settings key for remaining suffix.
	 *
	 * @return Settings key.
	 */
	static QString RemainingSuffixKey()
	{
		return "Remaining";
	}

	/**
	 * @brief Get settings key for segmented suffix.
	 *
	 * @return Settings key.
	 */
	static QString SegmentedSuffixKey()
	{
		return "Segmented";
	}
};

#endif // CC_GRAPHICAL_SEGMENTATION_OPTIONS_DLG_HEADER
