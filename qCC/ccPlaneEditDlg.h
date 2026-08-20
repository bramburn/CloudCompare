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
// #                       COPYRIGHT: SAGE INGENIERIE                       #
// #                                                                        #
// ##########################################################################

#ifndef CC_PLANE_EDIT_DLG_HEADER
#define CC_PLANE_EDIT_DLG_HEADER

/**
 * @file ccPlaneEditDlg.h
 *
 * @brief Plane edit dialog for creating or editing plane primitives.
 *
 * @details Dialog for creating new plane primitives or editing
 * existing ones. Supports both interactive picking and manual
 * parameter input.
 *
 * Plane parameters:
 * - Position (X, Y, Z)
 * - Dip direction (azimuth angle)
 * - Dip angle (inclination from horizontal)
 * - Normal vector
 *
 * Interactive features:
 * - Pick point to set plane center
 * - Adjust dip/dip direction (geological convention)
 * - Set custom normal vector
 *
 * @author SAGE Ingenierie
 *
 * @see ccPlane
 * @see ccPickingListener
 */

#include "ccPickingListener.h"

#include <ui_planeEditDlg.h>

// CCCoreLib
#include <CCGeom.h>

// Qt
#include <QDialog>

class ccGLWindowInterface;
class ccPlane;
class ccHObject;
class ccPickingHub;

/**
 * @brief Dialog for creating or editing plane parameters.
 *
 * @details Provides a UI for setting up plane primitives
 * with support for both manual input and interactive picking.
 *
 * Features:
 * - Manual parameter input (position, dip, dip direction)
 * - Interactive point picking to set plane center
 * - Normal vector specification
 * - Geological convention support (dip/dip direction)
 *
 * @extends QDialog
 * @extends ccPickingListener
 * @extends Ui::PlaneEditDlg
 */
class ccPlaneEditDlg : public QDialog
    , public ccPickingListener
    , public Ui::PlaneEditDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the plane edit dialog.
	 *
	 * @param[in] pickingHub Picking hub for interactive selection.
	 * @param[in] parent Parent widget.
	 */
	explicit ccPlaneEditDlg(ccPickingHub* pickingHub, QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccPlaneEditDlg();

	/**
	 * @brief Initialize with an existing plane.
	 *
	 * @param[in] plane Plane to edit.
	 *
	 * @details Populates the dialog fields with values
	 * from an existing plane for editing.
	 */
	void initWithPlane(ccPlane* plane);

	/**
	 * @brief Update a plane with current parameters.
	 *
	 * @param[in] plane Plane to update.
	 *
	 * @details Applies the current dialog values to the plane.
	 */
	void updatePlane(ccPlane* plane);

	/**
	 * @brief Handle item picked.
	 *
	 * @param[in] pi Picked item information.
	 */
	virtual void onItemPicked(const PickedItem& pi) override;

  public slots:
	/**
	 * @brief Pick selected point as plane center.
	 *
	 * @param[in] state Checkbox state.
	 */
	void pickPointAsCenter(bool state);

	/**
	 * @brief Handle dip direction change.
	 *
	 * @param[in] value New dip direction angle.
	 */
	void onDipDirChanged(double value);

	/**
	 * @brief Handle dip direction modification.
	 *
	 * @param[in] state Modification state.
	 */
	void onDipDirModified(bool state);

	/**
	 * @brief Handle normal vector change.
	 *
	 * @param[in] value New normal angle.
	 */
	void onNormalChanged(double value);

  protected slots:
	/**
	 * @brief Save parameters and accept dialog.
	 */
	void saveParamsAndAccept();

  protected:
	//! Picking window
	ccGLWindowInterface* m_pickingWin;

	//! Associated plane
	ccPlane* m_associatedPlane;

	//! Picking hub
	ccPickingHub* m_pickingHub;
};

#endif
