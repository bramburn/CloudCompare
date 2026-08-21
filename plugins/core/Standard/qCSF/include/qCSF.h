#pragma once

#ifdef QCSF_PLUGIN_EXPORTS
#  define QCSF_LIB_API Q_DECL_EXPORT
#else
#  define QCSF_LIB_API Q_DECL_IMPORT
#endif

//#######################################################################################
//#                                                                                     #
//#                              CLOUDCOMPARE PLUGIN: qCSF                              #
//#                                                                                     #
//#        This program is free software; you can redistribute it and/or modify         #
//#        it under the terms of the GNU General Public License as published by         #
//#        the Free Software Foundation; version 2 or later of the License.             #
//#                                                                                     #
//#        This program is distributed in the hope that it will be useful,              #
//#        but WITHOUT ANY WARRANTY; without even the implied warranty of               #
//#        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 #
//#        GNU General Public License for more details.                                 #
//#                                                                                     #
//#        Please cite the following paper, If you use this plugin in your work.        #
//#                                                                                     #
//#  Zhang W, Qi J, Wan P, Wang H, Xie D, Wang X, Yan G. An Easy-to-Use Airborne LiDAR  #
//#  Data Filtering Method Based on Cloth Simulation. Remote Sensing. 2016; 8(6):501.   #
//#                                                                                     #
//#                                     Copyright ©                                     #
//#               RAMM laboratory, School of Geography, Beijing Normal University       #
//#                               (http://ramm.bnu.edu.cn/)                             #
//#                                                                                     #
//#                      Wuming Zhang; Jianbo Qi; Peng Wan; Hongtao Wang                #
//#                                                                                     #
//#                      contact us: 2009zwm@gmail.com; wpqjbzwm@126.com                #
//#                                                                                     #
//#######################################################################################

/**
 * @file qCSF.h
 *
 * @brief Cloth Simulation Filter plugin
 *
 * Implements the Cloth Simulation Filter (CSF) algorithm for ground/non-ground
 * classification of point clouds, particularly airborne LiDAR data.
 *
 * Algorithm overview:
 * 1. Simulate a cloth draped over the point cloud (inverted Z axis)
 * 2. Points that intersect the cloth = ground points
 * 3. Points below the cloth = non-ground (vegetation, buildings, etc.)
 *
 * Key parameters:
 * - Cloth resolution: grid size for the simulation
 * - Max iteration: maximum iterations for cloth relaxation
 * - Rigidness: cloth rigidity factor
 * - Threshold: distance threshold for ground classification
 *
 * Reference: Zhang et al. "An Easy-to-Use Airborne LiDAR Data Filtering Method
 * Based on Cloth Simulation." Remote Sensing 2016.
 *
 * @see https://doi.org/10.3390/rs8060501
 */

#include "ccStdPluginInterface.h"

/**
 * @brief Cloth Simulation Filter plugin
 *
 * Implements the CSF ground filtering algorithm as a CloudCompare Standard plugin.
 * Activates when a point cloud is selected. Adds a "Cloth Simulation Filter"
 * action to the Plugins menu.
 *
 * The plugin:
 * - Checks for single-point-cloud selection
 * - Opens a parameter dialog (resolution, rigidness, threshold)
 * - Runs the CSF algorithm
 * - Splits the cloud into ground and non-ground groups
 *
 * @extends QObject
 * @extends ccStdPluginInterface
 * @implements ccStdPluginInterface
 */
class qCSF : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccStdPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qCSF" FILE "../info.json")

  public:
	/**
	 * @brief Construct the CSF plugin
	 *
	 * @param[in] parent QObject parent
	 */
	explicit qCSF(QObject* parent = nullptr);

	/**
	 * @brief Destructor
	 */
	virtual ~qCSF() = default;

	// ccStdPluginInterface

	/**
	 * @brief Handle new entity selection
	 *
	 * Checks if a single point cloud is selected and enables/disables
	 * the plugin action accordingly.
	 *
	 * @param[in] selectedEntities Current selection
	 */
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/**
	 * @brief Get the list of plugin actions
	 *
	 * Returns the "Cloth Simulation Filter" action for the Plugins menu.
	 *
	 * @return List containing the filter action
	 */
	virtual QList<QAction*> getActions() override;

	/**
	 * @brief Register command-line commands
	 *
	 * Registers the -CSF command for batch processing.
	 *
	 * @param[in] cmd Command-line interface
	 */
	virtual void registerCommands(ccCommandLineInterface* cmd) override;

  protected:
	/**
	 * @brief Execute the CSF filter
	 *
	 * Opens the parameter dialog, runs the algorithm, and
	 * creates ground/non-ground groups in the DB tree.
	 */
	void doAction();

  protected:
	//! "Cloth Simulation Filter" action for the Plugins menu
	QAction* m_action;
};
