#pragma once

//##########################################################################
//#                                                                        #
//#                      CLOUDCOMPARE PLUGIN: qSRA                         #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                           COPYRIGHT: EDF                               #
//#                                                                        #
//##########################################################################

/**
 * @file qSRA.h
 *
 * @brief Surface of Revolution Analysis plugin
 *
 * Analyze surfaces of revolution by comparing point clouds to reference profiles.
 *
 * @author EDF
 */

#include "ccStdPluginInterface.h"

class ccPointCloud;
class ccPolyline;

/**
 * @class qSRA
 *
 * @brief Surface of Revolution Analysis plugin
 *
 * Analyze point clouds against revolution surfaces.
 */
class qSRA : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )
	
	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qSRA" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qSRA(QObject* parent = nullptr);

	/// Destructor
	virtual ~qSRA() = default;

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;
	
	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

protected:
	/// Load profile from file
	void loadProfile() const;

	/// Compute cloud-to-profile radial distances
	void computeCloud2ProfileRadialDist() const;

	/// Project distances into 2D grid
	void projectCloudDistsInGrid() const;

protected:
	/// Project distances into 2D grid
	void doProjectCloudDistsInGrid(ccPointCloud* cloud, ccPolyline* polyline) const;

	/// Compute radial distances
	bool doComputeRadialDists(ccPointCloud* cloud, ccPolyline* polyline) const;

	QAction* m_doLoadProfile;
	QAction* m_doCompareCloudToProfile;
	QAction* m_doProjectCloudDists;
};
