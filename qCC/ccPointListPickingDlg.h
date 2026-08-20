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

#ifndef CC_POINT_PICKING_LIST_DIALOG_HEADER
#define CC_POINT_PICKING_LIST_DIALOG_HEADER

/**
 * @file ccPointListPickingDlg.h
 *
 * @brief Point list picking dialog for interactive point selection.
 *
 * @details Provides an interactive dialog for picking and managing a list of
 * points on a point cloud or mesh. Users can:
 * - Pick points by clicking in the 3D view
 * - Reorder picked points
 * - Export points to new entities (cloud, polyline)
 * - Export points to ASCII files in various formats
 *
 * This dialog is used for:
 * - Registration (select corresponding points on two clouds)
 * - Measurement (pick specific points for distance/angle calculation)
 * - Annotation (mark points of interest)
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPointPickingGenericInterface
 * @see ccPickingHub
 * @see cc2DLabel
 */

// GUI
#include <ui_pointListPickingDlg.h>

// Local
#include "ccPointPickingGenericInterface.h"

// qCC_db
#include <ccHObject.h>

class cc2DLabel;

/**
 * @brief Point list picking dialog for interactive point selection.
 *
 * @details An overlay dialog that allows users to pick points on a point cloud
 * or mesh by clicking in the 3D view. Picked points are stored as labels
 * and can be exported in various formats.
 *
 * Key features:
 * - Interactive point picking in 3D view
 * - Point reordering and deletion
 * - Export to new point cloud
 * - Export to polyline
 * - Export to ASCII files (xyz, ixyz, gxyz, lxyz formats)
 *
 * The dialog maintains a list of cc2DLabel objects associated with the
 * picked points. Labels store both 3D coordinates and screen position
 * for visual feedback.
 *
 * @extends ccPointPickingGenericInterface
 * @extends Ui::PointListPickingDlg
 */
class ccPointListPickingDlg : public ccPointPickingGenericInterface
    , public Ui::PointListPickingDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the point list picking dialog.
	 *
	 * @param[in] pickingHub The picking hub for handling 3D point picking.
	 * @param[in] parent Parent widget (optional).
	 */
	explicit ccPointListPickingDlg(ccPickingHub* pickingHub, QWidget* parent);

	/**
	 * @brief Link the dialog with an entity for picking.
	 *
	 * @param[in] entity Point cloud or mesh to pick points from.
	 *
	 * @details Associates the dialog with a specific entity. The user
	 * will only be able to pick points on this entity.
	 */
	void linkWithEntity(ccHObject* entity);

  protected slots:
	/**
	 * @brief Apply changes and close the dialog.
	 *
	 * @details Commits the picked points and closes the dialog.
	 * The picked labels remain associated with the entity.
	 */
	void applyAndExit();

	/**
	 * @brief Cancel changes and close the dialog.
	 *
	 * @details Discards any newly picked points and removes them
	 * from the entity. Closes the dialog.
	 */
	void cancelAndExit();

	/**
	 * @brief Export picked points to a new point cloud.
	 *
	 * @details Creates a new point cloud containing all picked points.
	 * The cloud is added to the database tree.
	 */
	void exportToNewCloud();

	/**
	 * @brief Export picked points to a new polyline.
	 *
	 * @details Creates a new polyline connecting the picked points
	 * in order. The polyline is added to the database tree.
	 */
	void exportToNewPolyline();

	/**
	 * @brief Remove the last picked point from the list.
	 *
	 * @details Deletes the most recently added point from the list.
	 */
	void removeLastEntry();

	/**
	 * @brief Export to ASCII file (XYZ format).
	 *
	 * Exports only the 3D coordinates: x y z per line.
	 */
	inline void exportToASCII_xyz()
	{
		return exportToASCII(PLP_ASCII_EXPORT_XYZ);
	}

	/**
	 * @brief Export to ASCII file (IXYZ format).
	 *
	 * Exports index and 3D coordinates: index x y z per line.
	 */
	inline void exportToASCII_ixyz()
	{
		return exportToASCII(PLP_ASCII_EXPORT_IXYZ);
	}

	/**
	 * @brief Export to ASCII file (GXYZ format).
	 *
	 * Exports global coordinates: X Y Z per line (double precision).
	 */
	inline void exportToASCII_gxyz()
	{
		return exportToASCII(PLP_ASCII_EXPORT_GXYZ);
	}

	/**
	 * @brief Export to ASCII file (LXYZ format).
	 *
	 * Exports local coordinates relative to first point: X Y Z per line.
	 */
	inline void exportToASCII_lxyz()
	{
		return exportToASCII(PLP_ASCII_EXPORT_LXYZ);
	}

	/**
	 * @brief Handle marker size spin box changes.
	 *
	 * @param[in] size New marker size.
	 *
	 * @details Updates the visual marker size for picked points.
	 */
	void markerSizeChanged(int size);

	/**
	 * @brief Handle start index spin box changes.
	 *
	 * @param[in] index New start index.
	 *
	 * @details Updates the displayed index offset for labels.
	 */
	void startIndexChanged(int);

	/**
	 * @brief Update the point list widget.
	 *
	 * @details Refreshes the list display with current picked points.
	 */
	void updateList();

  protected:
	/**
	 * @brief Process a picked point from the 3D view.
	 *
	 * @param[in] picked Information about the picked point.
	 *
	 * @details Called when the user clicks in the 3D view. Creates
	 * a new label for the picked point and adds it to the list.
	 */
	void processPickedPoint(const PickedItem& picked) override;

	/**
	 * @brief Get all currently picked points as labels.
	 *
	 * @param[out] pickedPoints Vector to store the labels.
	 * @return Number of picked points.
	 */
	unsigned getPickedPoints(std::vector<cc2DLabel*>& pickedPoints);

	/**
	 * @brief ASCII export formats.
	 */
	enum ExportFormat
	{
		PLP_ASCII_EXPORT_XYZ,  //!< x y z (float)
		PLP_ASCII_EXPORT_IXYZ, //!< index x y z (float)
		PLP_ASCII_EXPORT_GXYZ, //!< X Y Z (double precision)
		PLP_ASCII_EXPORT_LXYZ  //!< Local coordinates (relative to first point)
	};

	/**
	 * @brief Export points to an ASCII file.
	 *
	 * @param[in] format Export format to use.
	 */
	void exportToASCII(ExportFormat format);

	//! Associated point cloud or mesh for picking
	ccHObject* m_associatedEntity;

	//! Last existing label ID when dialog was opened
	unsigned m_lastPreviousID;

	//! Container for ordered labels
	ccHObject* m_orderedLabelsContainer;

	//! Points to delete if user cancels
	ccHObject::Container m_toBeDeleted;

	//! New points added by user (for proper cancel)
	ccHObject::Container m_toBeAdded;
};

#endif
