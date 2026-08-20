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
// #                                                                        //
// ##########################################################################

#ifndef CC_SCALE_DLG_HEADER
#define CC_SCALE_DLG_HEADER

/**
 * @file ccScaleDlg.h
 *
 * @brief Scale dialog for scaling entities.
 *
 * @details Dialog for configuring entity scaling parameters.
 *
 * Scaling allows uniformly or non-uniformly scaling entities:
 * - Uniform: Same scale factor for X, Y, Z
 * - Non-uniform: Different scale factors per axis
 *
 * Options:
 * - Keep in place: Scale around center vs. origin
 * - Rescale global shift: Adjust coordinate system offset
 *
 * @author Daniel Girardeau-Montaut
 */

#include <CCGeom.h>

// Qt
#include <QDialog>

namespace Ui
{
	class ScaleDialog;
}

/**
 * @brief Dialog for scaling entities.
 *
 * @details Provides a UI for configuring entity scaling parameters.
 *
 * Features:
 * - Separate X, Y, Z scale factors
 * - "All dims at once" for uniform scaling
 * - Keep in place toggle (scale around center)
 * - Global shift adjustment option
 *
 * @extends QDialog
 */
class ccScaleDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the scale dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccScaleDlg(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccScaleDlg();

	/**
	 * @brief Get the scale factors.
	 * @return Scale factors for X, Y, Z.
	 */
	CCVector3d getScales() const;

	/**
	 * @brief Check if scaling around center.
	 * @return true if keeping entities in place.
	 */
	bool keepInPlace() const;

	/**
	 * @brief Check if global shift should be rescaled.
	 * @return true if rescaling global shift.
	 */
	bool rescaleGlobalShift() const;

	/**
	 * @brief Save dialog state.
	 */
	void saveState();

  private slots:
	/**
	 * @brief Handle all dims toggle.
	 * @param[in] checked Toggle state.
	 */
	void allDimsAtOnceToggled(bool checked);

	/**
	 * @brief Handle X dimension update.
	 * @param[in] value New value.
	 */
	void fxUpdated(double value);

  private:
	//! UI definition
	Ui::ScaleDialog* m_ui;
};

#endif // CC_SCALE_DLG_HEADER
