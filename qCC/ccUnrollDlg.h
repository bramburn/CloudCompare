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
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccUnrollDlg.h
 *
 * @brief Unroll dialog for projecting clouds onto developable surfaces.
 *
 * @details Dialog for unrolling (projecting) point clouds onto
 * developable surfaces like cylinders and cones.
 *
 * "Unrolling" transforms a 3D curved surface into a 2D plane
 * by projecting points radially. Useful for:
 * - Cylindrical objects (pipes, tunnels)
 * - Conical surfaces
 * - Unwrapping scanned surfaces
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

// qCC_db
#include <ccPointCloud.h>

namespace Ui
{
	class UnrollDialog;
}

/**
 * @brief Dialog for unrolling clouds onto developable surfaces.
 *
 * @details Provides a UI for configuring surface unrolling parameters.
 *
 * Features:
 * - Cylindrical unrolling
 * - Conical unrolling
 * - Custom axis definition
 * - Angle range limiting
 * - Export deviation scalar field
 *
 * @extends QDialog
 */
class ccUnrollDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the unroll dialog.
	 *
	 * @param[in] dbRootEntity Database root for entity selection.
	 * @param[in] parent Parent widget.
	 */
	explicit ccUnrollDlg(ccHObject* dbRootEntity, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccUnrollDlg() override;

	/**
	 * @brief Get the unroll type.
	 * @return Unroll mode (cylinder, cone, etc.).
	 */
	ccPointCloud::UnrollMode getType() const;

	/**
	 * @brief Get the axis direction.
	 * @return Axis vector.
	 */
	CCVector3d getAxis() const;

	/**
	 * @brief Check if axis position is automatic.
	 * @return true if auto.
	 */
	bool isAxisPositionAuto() const;

	/**
	 * @brief Check if using arbitrary output CS.
	 * @return true if arbitrary.
	 */
	bool useArbitraryOutputCS() const;

	/**
	 * @brief Check if stretched triangles should be removed.
	 * @return true if removing.
	 */
	bool removeStretchedTriangles() const;

	/**
	 * @brief Get the axis position.
	 * @return Axis position vector.
	 */
	CCVector3 getAxisPosition() const;

	/**
	 * @brief Get the angle range.
	 *
	 * @param[out] start_deg Start angle in degrees.
	 * @param[out] stop_deg Stop angle in degrees.
	 */
	void getAngleRange(double& start_deg, double& stop_deg) const;

	/**
	 * @brief Get the cylinder radius.
	 * @return Radius value.
	 */
	double getRadius() const;

	/**
	 * @brief Get the cone half angle.
	 * @return Angle in degrees.
	 */
	double getConeHalfAngle() const;

	/**
	 * @brief Check if exporting deviation SF.
	 * @return true if exporting.
	 */
	bool exportDeviationSF() const;

	/**
	 * @brief Get conical projection span ratio.
	 * @return Span ratio.
	 */
	double getConicalProjSpanRatio() const;

	/**
	 * @brief Save settings.
	 */
	void toPersistentSettings() const;

	/**
	 * @brief Load settings.
	 */
	void fromPersistentSettings();

	/**
	 * @brief Set configuration.
	 *
	 * @param[in] cloudsOnly Only show cloud-related options.
	 */
	void setConfiguration(bool cloudsOnly);

  protected slots:
	/**
	 * @brief Handle shape type change.
	 * @param[in] index New type.
	 */
	void shapeTypeChanged(int index);

	/**
	 * @brief Handle projection type change.
	 * @param[in] index New type.
	 */
	void projectionTypeChanged(int index);

	/**
	 * @brief Handle axis dimension change.
	 * @param[in] index New dimension.
	 */
	void axisDimensionChanged(int index);

	/**
	 * @brief Handle axis auto state change.
	 * @param[in] checkState Auto state.
	 */
	void axisAutoStateChanged(int checkState);

	/**
	 * @brief Load parameters from selected entity.
	 */
	void loadParametersFromEntity();

	/**
	 * @brief Load axis from clipboard.
	 */
	void axisFromClipboard();

	/**
	 * @brief Load center from clipboard.
	 */
	void centerFromClipboard();

  protected:
	//! UI definition
	Ui::UnrollDialog* m_ui;

	//! Database root entity
	ccHObject* m_dbRootEntity;
};
