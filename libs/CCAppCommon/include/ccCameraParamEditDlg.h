#pragma once
// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccCameraParamEditDlg.h
 *
 * @brief Camera pose editing overlay dialog
 *
 * An overlay dialog that floats on top of a 3D view, letting the user
 * directly edit the camera pose parameters (rotation angles, pivot point,
 * camera center, field of view, and clipping planes).
 *
 * Features:
 * - Spherical coordinates: Theta (azimuth), Psi (elevation), Phi (roll)
 * - Pivot point: can be picked from the 3D scene or entered manually
 * - Camera center: eye position in world space
 * - Field of view (FOV): in degrees
 * - Near/far clipping planes: can be locked or free
 * - Preset orientations: front, back, left, right, top, bottom, ISO1, ISO2
 * - Push/revert: save current matrix, revert to saved
 *
 * Lifecycle:
 * 1. Construct with parent widget and picking hub
 * 2. linkWith(ccGLWindowInterface*) to attach to a 3D view
 * 3. start() to show the dialog
 * 4. User edits parameters; reflectParamChange() applies them in real-time
 * 5. Close via Stop button or Escape key
 *
 * @extends ccOverlayDialog
 * @extends ccPickingListener
 */

#include "CCAppCommon.h"
#include "ccOverlayDialog.h"
#include "ccPickingListener.h"

#include <ccGLMatrix.h>
#include <ccGLUtils.h>
#include <map>

class QMdiSubWindow;
class ccGLWindowInterface;
class ccHObject;
class ccPickingHub;

namespace Ui
{
	class CameraParamDlg;
}

/**
 * @class ccCameraParamEditDlg
 *
 * @brief Camera pose parameter editor
 *
 * Floating overlay dialog for direct manipulation of the camera pose.
 * Changes are applied in real-time to the linked 3D view.
 *
 * @extends ccOverlayDialog
 * @extends ccPickingListener
 */
class CCAPPCOMMON_LIB_API ccCameraParamEditDlg : public ccOverlayDialog
    , public ccPickingListener
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the dialog
	 *
	 * @param[in] parent Parent widget
	 * @param[in] pickingHub Picking hub for 3D point picking
	 */
	explicit ccCameraParamEditDlg(QWidget* parent, ccPickingHub* pickingHub);

	//! Destructor
	~ccCameraParamEditDlg() override;

	//! Make the dialog frameless (for use as overlay)
	void makeFrameless();

	/**
	 * @brief Get the current camera matrix from dialog values
	 *
	 * Encodes the current rotation angles, pivot, and camera center
	 * into a 4x4 homogeneous transformation matrix.
	 *
	 * @return Camera transformation matrix
	 */
	ccGLMatrixd getMatrix();

	// ccOverlayDialog

	/**
	 * @brief Show the dialog and begin accepting input
	 *
	 * Calls linkWith() for the current window and enables controls.
	 */
	bool start() override;

	/**
	 * @brief Attach to a 3D window
	 *
	 * Saves the current camera matrix for this window (for revert).
	 *
	 * @param[in] win Target GL window
	 * @return true on success
	 */
	bool linkWith(ccGLWindowInterface* win) override;

	// ccPickingListener

	/**
	 * @brief Handle picked 3D point
	 *
	 * Called when the user picks a point in the 3D view while the
	 * "pick point as pivot" option is active.
	 *
	 * @param[in] pi Picked item data
	 */
	void onItemPicked(const PickedItem& pi) override;

  public:
	/**
	 * @brief Link to an MDI sub-window
	 *
	 * @param[in] qWin Target sub-window
	 */
	void linkWith(QMdiSubWindow* qWin);

	/**
	 * @brief Initialize from an existing camera matrix
	 *
	 * Decodes the rotation angles, pivot, and camera center from
	 * the matrix and populates the dialog fields.
	 *
	 * @param[in] mat Source camera matrix
	 */
	void initWithMatrix(const ccGLMatrixd& mat);

	/**
	 * @brief Update the pivot point display
	 *
	 * @param[in] P New pivot point (world coordinates)
	 */
	void updatePivotPoint(const CCVector3d& P);

	/**
	 * @brief Update the camera center display
	 *
	 * @param[in] P New camera center (world coordinates)
	 */
	void updateCameraCenter(const CCVector3d& P);

	//! Update the view mode label (e.g. "front", "ISO 1")
	void updateViewMode();

	/**
	 * @brief Update the FOV display
	 *
	 * @param[in] fov_deg Field of view in degrees
	 */
	void updateWinFov(float fov_deg);

	/**
	 * @brief Update the near clipping plane depth
	 *
	 * @param[in] depth Near clipping depth
	 */
	void updateNearClippingDepth(double depth);

	/**
	 * @brief Update the far clipping plane depth
	 *
	 * @param[in] depth Far clipping depth
	 */
	void updateFarClippingDepth(double depth);

	//! Set view to the +X direction (Front)
	void setFrontView();
	//! Set view to the -Z direction (Bottom)
	void setBottomView();
	//! Set view to the +Y direction (Top)
	void setTopView();
	//! Set view to the -X direction (Back)
	void setBackView();
	//! Set view to the -X direction (Left)
	void setLeftView();
	//! Set view to the +X direction (Right)
	void setRightView();
	//! Set view to ISO 1 (top-left diagonal)
	void setIso1View();
	//! Set view to ISO 2 (top-right diagonal)
	void setIso2View();

	// Spherical coordinate slot handlers
	///@{
	//! Theta (azimuth) changed via integer spinbox
	void iThetaValueChanged(int);
	//! Psi (elevation) changed via integer spinbox
	void iPsiValueChanged(int);
	//! Phi (roll) changed via integer spinbox
	void iPhiValueChanged(int);
	//! Theta changed via double spinbox
	void dThetaValueChanged(double);
	//! Psi changed via double spinbox
	void dPsiValueChanged(double);
	//! Phi changed via double spinbox
	void dPhiValueChanged(double);
	///@}

	//! Near clipping depth changed
	void nearClippingDepthChanged(double);
	//! Near clipping checkbox toggled
	void nearClippingCheckBoxToggled(bool);
	//! Clipping planes global toggle
	void clippingPlanesToggled(bool);
	//! Far clipping depth changed
	void farClippingDepthChanged(double);
	//! Far clipping checkbox toggled
	void farClippingCheckBoxToggled(bool);
	//! Pivot point changed
	void pivotChanged();
	//! Camera center changed
	void cameraCenterChanged();
	//! Field of view changed
	void fovChanged(double);

	/**
	 * @brief Toggle pick-point-as-pivot mode
	 *
	 * When enabled, clicking in the 3D view picks a point to use
	 * as the pivot.
	 *
	 * @param[in] state true to enable picking mode
	 */
	void pickPointAsPivot(bool);

	/**
	 * @brief Process a picked item from the picking hub
	 *
	 * @param[in] obj Clicked entity
	 * @param[in] subObjID Index of clicked sub-object
	 * @param[in] x Screen X coordinate
	 * @param[in] y Screen Y coordinate
	 * @param[in] P Approximate 3D picked point
	 * @param[in] P3D Precise 3D picked point
	 */
	void processPickedItem(ccHObject*, unsigned subObjID, int x, int y, const CCVector3& P, const CCVector3d& P3D);

  protected:
	/**
	 * @brief Reflect a parameter change in the 3D view
	 *
	 * Called by slot handlers to apply the current dialog values
	 * to the linked GL window.
	 */
	void reflectParamChange();

	/**
	 * @brief Set camera to a preset orientation
	 *
	 * @param[in] orientation Named orientation (CC_TOP, CC_FRONT, etc.)
	 */
	void setView(CC_VIEW_ORIENTATION orientation);

	/**
	 * @brief Push/save the current matrix for the current window
	 *
	 * Saves the matrix so it can be reverted to later.
	 */
	void pushCurrentMatrix();

	/**
	 * @brief Revert to the saved matrix for the current window
	 */
	void revertToPushedMatrix();

	/**
	 * @brief Initialize from a GL window
	 *
	 * @param[in] win Source window (reads current camera pose)
	 */
	void initWith(ccGLWindowInterface* win);

	//! Pushed (saved) matrices per window for revert functionality
	using PushedMatricesMapType = std::map<ccGLWindowInterface*, ccGLMatrixd>;
	//! Element type for the pushed matrices map
	using PushedMatricesMapElement = std::pair<ccGLWindowInterface*, ccGLMatrixd>;

	//! Per-window saved camera matrices (for revert)
	PushedMatricesMapType pushedMatrices;
	//! Picking hub for 3D point picking
	ccPickingHub* m_pickingHub;

  private:
	//! UI form generated from .ui file
	Ui::CameraParamDlg* m_ui;
};
