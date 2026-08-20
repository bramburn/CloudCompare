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
 * @brief Plane edit dialog
 *
 * Dialog for creating or editing plane parameters.
 *
 * @author SAGE Ingenierie
 */

// Local
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
 * @brief Plane edit dialog
 *
 * Create or edit plane parameters.
 */
class ccPlaneEditDlg : public QDialog
    , public ccPickingListener
    , public Ui::PlaneEditDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] pickingHub Picking hub
	 * @param[in] parent Parent widget
	 */
	explicit ccPlaneEditDlg(ccPickingHub* pickingHub, QWidget* parent);

	/// Destructor
	virtual ~ccPlaneEditDlg();

	/**
	 * @brief Initialize with existing plane
	 * @param[in] plane Plane to edit
	 */
	void initWithPlane(ccPlane* plane);

	/**
	 * @brief Update plane with current parameters
	 * @param[in] plane Plane to update
	 */
	void updatePlane(ccPlane* plane);

	/// Handle item picked
	virtual void onItemPicked(const PickedItem& pi) override;

  public:
	/// Pick point as center
	void pickPointAsCenter(bool);
	/// Handle dip direction changed
	void onDipDirChanged(double);
	/// Handle dip direction modified
	void onDipDirModified(bool);
	/// Handle normal changed
	void onNormalChanged(double);

  protected:
	/// Save params and accept
	void saveParamsAndAccept();

  protected: // members
	/// Picking window
	ccGLWindowInterface* m_pickingWin;

	/// Associated plane
	ccPlane* m_associatedPlane;

	/// Picking hub
	ccPickingHub* m_pickingHub;
};

#endif
