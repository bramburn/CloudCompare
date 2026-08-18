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
 * @brief Filter by value dialog
 *
 * Dialog for filtering points by scalar value.
 *
 * @author Daniel Girardeau-Montaut
 */

#include <ui_filterByValueDlg.h>

// Qt
#include <QDialog>

/**
 * @brief Filter by value dialog
 *
 * Filter points by scalar value range.
 */
class ccFilterByValueDlg : public QDialog
    , public Ui::FilterByValueDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] minRange Minimum range value
	 * @param[in] maxRange Maximum range value
	 * @param[in] minVal Minimum scalar value
	 * @param[in] maxVal Maximum scalar value
	 * @param[in] parent Parent widget
	 */
	ccFilterByValueDlg(double   minRange,
	                   double   maxRange,
	                   double   minVal = -1.0e9,
	                   double   maxVal = 1.0e9,
	                   QWidget* parent = nullptr);

	/// Mode
	enum Mode
	{
		EXPORT, //!< Export filtered
		SPLIT,  //!< Split at boundaries
		CANCEL   //!< Cancel
	};

	/**
	 * @brief Get mode
	 * @return Mode
	 */
	Mode mode() const
	{
		return m_mode;
	}

  protected:
	void onExport()
	{
		m_mode = EXPORT;
		accept();
	}
	void onSplit()
	{
		m_mode = SPLIT;
		accept();
	}

  protected:
	Mode m_mode;
};

#endif // CC_FILTER_BY_VALUE_DIALOG_HEADER
