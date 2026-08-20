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
// #                  COPYRIGHT: Daniel Girardeau-Montaut                   #
// #                                                                        #
// ##########################################################################

#ifndef CC_FILTER_BY_VALUE_DIALOG_HEADER
#define CC_FILTER_BY_VALUE_DIALOG_HEADER

/**
 * @file ccFilterByValueDlg.h
 *
 * @brief Filter by value dialog for scalar field range filtering.
 *
 * @details Dialog for filtering points based on scalar field values.
 * Allows the user to select a range of values and choose what
 * to do with filtered/unfiltered points.
 *
 * Modes:
 * - **Export**: Keep only points within the selected range
 * - **Split**: Create separate clouds for in-range and out-of-range points
 *
 * This is useful for:
 * - Removing noise based on sensor values
 * - Extracting specific height ranges
 * - Selecting points by intensity
 *
 * @author Daniel Girardeau-Montaut
 */

#include <ui_filterByValueDlg.h>

// Qt
#include <QDialog>

/**
 * @brief Dialog for filtering points by scalar value range.
 *
 * @details Provides a UI for filtering point clouds based on
 * scalar field values.
 *
 * The dialog shows:
 * - Current min/max values in the cloud
 * - User-selected filter range
 * - Histogram visualization (optional)
 *
 * @extends QDialog
 * @extends Ui::FilterByValueDialog
 */
class ccFilterByValueDlg : public QDialog
    , public Ui::FilterByValueDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Filter modes.
	 */
	enum Mode
	{
		EXPORT, //!< Export only in-range points
		SPLIT,  //!< Split into two clouds
		CANCEL   //!< Cancel operation
	};

	/**
	 * @brief Construct the filter dialog.
	 *
	 * @param[in] minRange Minimum value in the range selector.
	 * @param[in] maxRange Maximum value in the range selector.
	 * @param[in] minVal Minimum possible value (SF minimum).
	 * @param[in] maxVal Maximum possible value (SF maximum).
	 * @param[in] parent Parent widget.
	 */
	ccFilterByValueDlg(double   minRange,
	                   double   maxRange,
	                   double   minVal = -1.0e9,
	                   double   maxVal = 1.0e9,
	                   QWidget* parent = nullptr);

	/**
	 * @brief Get the selected filter mode.
	 * @return Mode selected by user.
	 */
	Mode mode() const
	{
		return m_mode;
	}

  protected slots:
	/**
	 * @brief Handle export button.
	 */
	void onExport()
	{
		m_mode = EXPORT;
		accept();
	}

	/**
	 * @brief Handle split button.
	 */
	void onSplit()
	{
		m_mode = SPLIT;
		accept();
	}

  protected:
	//! Selected mode
	Mode m_mode;
};

#endif // CC_FILTER_BY_VALUE_DIALOG_HEADER
