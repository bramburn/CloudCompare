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
// #                    COPYRIGHT: Daniel Girardeau-Montaut                 #
// #                                                                        #
// ##########################################################################

#ifndef CC_SCALE_DLG_HEADER
#define CC_SCALE_DLG_HEADER

/**
 * @file ccScaleDlg.h
 *
 * @brief Scale dialog
 *
 * Dialog for scaling entities.
 *
 * @author Daniel Girardeau-Montaut
 */

// CC_Lib
#include <CCGeom.h>

// Qt
#include <QDialog>

namespace Ui
{
	class ScaleDialog;
}

/**
 * @brief Scale dialog
 *
 * Scale or multiply entities.
 */
class ccScaleDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccScaleDlg(QWidget* parent = nullptr);

	/// Destructor
	~ccScaleDlg();

	/// Get scales
	CCVector3d getScales() const;

	/// Get keep in place flag
	bool keepInPlace() const;

	/// Get rescale global shift flag
	bool rescaleGlobalShift() const;

	/// Save state
	void saveState();

  private:
	/// Handle all dims toggled
	void allDimsAtOnceToggled(bool);
	/// Handle fx updated
	void fxUpdated(double);

	Ui::ScaleDialog* m_ui;
};

#endif // CC_SCALE_DLG_HEADER
