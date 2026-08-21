//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qBroom                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#      COPYRIGHT: Wesley Grimes (Collision Engineering Associates)       #
//#                                                                        #
//##########################################################################

#ifndef Q_BROOM_PLUGIN_HEADER
#define Q_BROOM_PLUGIN_HEADER

/**
 * @file qBroom.h
 *
 * @brief Virtual broom plugin for point cloud noise removal
 *
 * Provides a brush-style tool for interactively sweeping noise points
 * from a point cloud in the 3D view. Unlike static filters (CSF, M3C2),
 * the broom operates in real-time as the user drags across the point cloud.
 *
 * Key parameters:
 * - Brush radius: spatial extent of the broom
 * - Force/power: how aggressively points are pushed away
 * - Direction: allow 3D brush direction or constrain to screen-aligned
 *
 * Algorithm (simplified):
 * 1. User drags in the 3D view with the broom active
 * 2. Points within brush radius are identified (octree-accelerated)
 * 3. Points are displaced away from the brush center along the brush axis
 * 4. Points pushed beyond a threshold are marked as hidden
 *
 * Use case: removing isolated noise spikes from terrestrial scans,
 * e.g. pedestrians or traffic that passed during data acquisition.
 *
 * @author Wesley Grimes (Collision Engineering Associates)
 */

#include "ccStdPluginInterface.h"

/**
 * @class qBroom
 *
 * @brief Virtual broom for interactive noise removal
 *
 * Activates when a point cloud is selected. Adds a "Broom" action to
 * the Plugins menu. After clicking, the user drags in the 3D view
 * to sweep noise away.
 *
 * Shows a disclaimer on first use (qBroomDisclaimerDialog).
 *
 * @extends QObject
 * @extends ccStdPluginInterface
 * @implements ccStdPluginInterface
 */
class qBroom : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccStdPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qBroom" FILE "../info.json")

  public:
	/**
	 * @brief Construct the broom plugin
	 *
	 * @param[in] parent QObject parent
	 */
	explicit qBroom(QObject* parent = nullptr);

	/**
	 * @brief Destructor
	 */
	virtual ~qBroom() = default;

	// ccStdPluginInterface

	/**
	 * @brief Handle new entity selection
	 *
	 * Enables the broom action when exactly one point cloud is selected.
	 *
	 * @param[in] selectedEntities Current selection
	 */
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/**
	 * @brief Get the plugin's actions
	 *
	 * Returns the "Broom" action for the Plugins menu.
	 *
	 * @return List containing the broom action
	 */
	virtual QList<QAction*> getActions() override;

  protected:
	/**
	 * @brief Execute the broom tool
	 *
	 * Shows disclaimer (first-use only), then enters the broom mode.
	 * While active, mouse drags in the 3D view displace/sweep points.
	 */
	void doAction();

  protected:
	//! "Broom" action for the Plugins menu
	QAction* m_action;
};

#endif //Q_BROOM_PLUGIN_HEADER
