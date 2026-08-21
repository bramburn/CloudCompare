//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qHPR                        #
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
//#                  COPYRIGHT: Daniel Girardeau-Montaut                   #
//#                                                                        #
//##########################################################################

#ifndef Q_HPR_PLUGIN_HEADER
#define Q_HPR_PLUGIN_HEADER

/**
 * @file qHPR.h
 *
 * @brief Hidden Point Removal (HPR) plugin
 *
 * Implements the Hidden Point Removal algorithm for approximating
 * point cloud visibility from a given viewpoint.
 *
 * The HPR algorithm:
 * 1. Performs spherical inversion of the point cloud around the viewpoint
 * 2. Computes the convex hull of the inverted points
 * 3. Points on the hull = visible from the viewpoint
 *
 * Useful for view-dependent point cloud reduction and visibility queries
 * without a full mesh.
 *
 * Reference: Katz S., Tal A., Basri R. "Direct Visibility of Point Sets."
 * ACM SIGGRAPH 2007.
 *
 * @see https://doi.org/10.1145/1275808.1276405
 */

#include "ccStdPluginInterface.h"
#include <ReferenceCloud.h>

/**
 * @class qHPR
 *
 * @brief Hidden Point Removal plugin
 *
 * Approximates which points in a point cloud are visible from a given
 * viewpoint using the HPR spherical inversion algorithm.
 *
 * Activates when a point cloud is selected. Adds an "Hidden Point Removal"
 * action to the Plugins menu.
 *
 * Output: a ReferenceCloud containing only the visible points.
 *
 * @extends QObject
 * @extends ccStdPluginInterface
 * @implements ccStdPluginInterface
 */
class qHPR : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccStdPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qHPR" FILE "../info.json")

  public:
	/**
	 * @brief Construct the HPR plugin
	 *
	 * @param[in] parent QObject parent
	 */
	explicit qHPR(QObject* parent = nullptr);

	/**
	 * @brief Destructor
	 */
	virtual ~qHPR() = default;

	// ccStdPluginInterface

	/**
	 * @brief Handle new entity selection
	 *
	 * Enables the action when exactly one point cloud is selected.
	 *
	 * @param[in] selectedEntities Current selection
	 */
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/**
	 * @brief Get the plugin's actions
	 *
	 * Returns the "Hidden Point Removal" action for the Plugins menu.
	 *
	 * @return List containing the HPR action
	 */
	virtual QList<QAction*> getActions() override;

  protected:
	/**
	 * @brief Execute the HPR algorithm
	 *
	 * Runs the full pipeline: parameter dialog, HPR computation,
	 * and output cloud creation.
	 */
	void doAction();

	/**
	 * @brief Core HPR algorithm
	 *
	 * Implements the HPR spherical inversion:
	 * 1. Invert points around viewPoint
	 * 2. Compute convex hull of inverted points
	 * 3. Return original points whose inverted counterparts are on the hull
	 *
	 * @param[in] theCloud Input point cloud (must not be nullptr)
	 * @param[in] viewPoint Camera/viewpoint position\n
	 *                      Used as the center of spherical inversion\n
	 *                      Tip: use the camera center from the active 3D view
	 * @param[in] fParam HPR sharpness parameter (>0).\n
	 *                    Controls how aggressively points are removed.\n
	 *                    Larger values = more points kept as "visible"
	 * @return ReferenceCloud of visible points (owned by caller, may be null)
	 */
	CCCoreLib::ReferenceCloud* removeHiddenPoints(CCCoreLib::GenericIndexedCloudPersist* theCloud,
	                                               const CCVector3d&                     viewPoint,
	                                               double                                 fParam);

  protected:
	//! "Hidden Point Removal" action for the Plugins menu
	QAction* m_action;
};

#endif
