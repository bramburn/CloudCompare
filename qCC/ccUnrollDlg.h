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
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccUnrollDlg.h
 *
 * @brief Unroll dialog
 *
 * Dialog for unrolling clouds on cylinder or cone.
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
 * @brief Unroll dialog
 *
 * Unroll clouds on cylinder or cone.
 */
class ccUnrollDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] dbRootEntity Database root entity
	 * @param[in] parent Parent widget
	 */
	explicit ccUnrollDlg(ccHObject* dbRootEntity, QWidget* parent = nullptr);
	
	/// Destructor
	~ccUnrollDlg() override;

	/// Get unroll type
	ccPointCloud::UnrollMode getType() const;
	/// Get axis
	CCVector3d getAxis() const;
	/// Get axis position auto flag
	bool isAxisPositionAuto() const;
	/// Get use arbitrary output CS flag
	bool useArbitraryOutputCS() const;
	/// Get remove stretched triangles flag
	bool removeStretchedTriangles() const;
	/// Get axis position
	CCVector3 getAxisPosition() const;
	/// Get angle range
	void getAngleRange(double& start_deg, double& stop_deg) const;
	/// Get radius
	double getRadius() const;
	/// Get cone half angle
	double getConeHalfAngle() const;
	/// Get export deviation SF flag
	bool exportDeviationSF() const;
	/// Get conical proj span ratio
	double getConicalProjSpanRatio() const;

	/// Save to persistent settings
	void toPersistentSettings() const;
	/// Load from persistent settings
	void fromPersistentSettings();
	/// Set configuration
	void setConfiguration(bool cloudsOnly);

  protected:
	/// Handle shape type changed
	void shapeTypeChanged(int index);
	/// Handle projection type changed
	void projectionTypeChanged(int index);
	/// Handle axis dimension changed
	void axisDimensionChanged(int index);
	/// Handle axis auto state changed
	void axisAutoStateChanged(int checkState);
	/// Load parameters from entity
	void loadParametersFromEntity();
	/// Set axis from clipboard
	void axisFromClipboard();
	/// Set center from clipboard
	void centerFromClipboard();

  protected:
	Ui::UnrollDialog* m_ui;
	ccHObject*        m_dbRootEntity;
};
