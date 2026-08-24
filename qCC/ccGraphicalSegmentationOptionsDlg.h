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
 * @brief Segmentation options dialog
 *
 * Dialog for segmentation tool options.
 *
 * @author CloudCompare project
 */

// Qt
#include <QString>

// GUI
#include <ui_graphicalSegmentationOptionsDlg.h>

/**
 * @brief Segmentation options dialog
 *
 * Configure segmentation tool options.
 */
class ccGraphicalSegmentationOptionsDlg : public QDialog
    , public Ui::GraphicalSegmentationOptionsDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] windowTitle Window title
	 * @param[in] parent Parent widget
	 */
	ccGraphicalSegmentationOptionsDlg(const QString windowTitle = QString(), QWidget* parent = nullptr);

	/// Accept dialog
	void accept();

	/// Segmentation tool options key
	static QString SegmentationToolOptionsKey()
	{
		return "SegmentationToolOptions";
	}

	/// Remaining suffix key
	static QString RemainingSuffixKey()
	{
		return "Remaining";
	}

	/// Segmented suffix key
	static QString SegmentedSuffixKey()
	{
		return "Segmented";
	}
};

#endif // CC_GRAPHICAL_SEGMENTATION_OPTIONS_DLG_HEADER
