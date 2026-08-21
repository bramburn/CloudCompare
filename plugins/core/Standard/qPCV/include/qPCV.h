#pragma once

//##########################################################################
//#                                                                        #
//#                      CLOUDCOMPARE PLUGIN                               #
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
//#                  COPYRIGHT: Daniel Girardeau-Montaut                   #
//#                                                                        #
//##########################################################################

/**
 * @file qPCV.h
 *
 * @brief Potentially Visible Set (PCV) / ShadeVis ambient occlusion plugin
 *
 * Computes ambient occlusion on point clouds and meshes using the
 * Potentially Visible Set (PCV) ShadeVis algorithm.
 *
 * Algorithm (ShadeVis):
 * 1. Generate ray directions (spherical sampling, or use per-point normals)
 * 2. For each point, count how many rays hit nearby geometry within a radius
 * 3. Occlusion score = number of blocked rays / total rays
 * 4. Result stored as a scalar field on each cloud/mesh
 *
 * Parameters:
 * - Ray count: number of directions to sample (default 256)
 * - Resolution: spatial subdivision for the PCV grid (default 1024)
 * - 180° vs 360° mode: hemisphere vs full sphere sampling
 * - Closed mesh: treat mesh surfaces as opaque vs semi-transparent
 * - Ray source: generated uniformly, or from another cloud's normals
 *
 * Reference: Tarini M., Cignoni P., Scopigno R. "Visibility based methods
 * and assessment for detail-recovery." Visualization 2003.
 *
 * @see https://doi.org/10.1109/VISUAL.2003.1250388
 */

#include "ccStdPluginInterface.h"
#include "PCVCommand.h"

/**
 * @class qPCV
 *
 * @brief Potentially Visible Set (PCV) ambient occlusion plugin
 *
 * Computes ambient occlusion scalar fields on point clouds and meshes
 * using the ShadeVis algorithm. Activates when a cloud or mesh is selected.
 *
 * Key capabilities:
 * - Works on point clouds and meshes
 * - Multiple entities processed in one run
 * - Ray directions from generated spherical sampling or another cloud's normals
 * - Semi-persistent dialog parameters
 *
 * Output: per-point scalar field with occlusion values [0,1]
 * (0 = fully occluded, 1 = fully visible)
 *
 * @extends QObject
 * @extends ccStdPluginInterface
 * @implements ccStdPluginInterface
 */
class qPCV : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccStdPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qPCV" FILE "../info.json")

  public:
	/**
	 * @brief Construct the PCV plugin
	 *
	 * @param[in] parent QObject parent
	 */
	explicit qPCV(QObject* parent = nullptr);

	/**
	 * @brief Destructor
	 */
	~qPCV() override = default;

	// ccStdPluginInterface

	/**
	 * @brief Handle new entity selection
	 *
	 * Enables the action when at least one point cloud or mesh
	 * is selected.
	 *
	 * @param[in] selectedEntities Current selection
	 */
	void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/**
	 * @brief Get the plugin's actions
	 *
	 * Returns the "PCV / ShadeVis" action for the Plugins menu.
	 *
	 * @return List containing the PCV action
	 */
	QList<QAction*> getActions() override;

	/**
	 * @brief Register command-line commands
	 *
	 * Registers the -PCV command for batch processing.
	 *
	 * @param[in] cmd Command-line interface
	 */
	void registerCommands(ccCommandLineInterface* cmd) override;

  private:
	/**
	 * @brief Execute the PCV computation
	 *
	 * Pipeline:
	 * 1. Filter selection to clouds and meshes with real point clouds
	 * 2. Populate clouds-with-normals list from DB tree for ray-source option
	 * 3. Show parameter dialog (semi-persistent state)
	 * 4. Generate ray set: either from normals of another cloud, or PCV::GenerateRays
	 * 5. Call PCVCommand::Process() to run ShadeVis on all candidates
	 */
	void doAction();

	//! "PCV / ShadeVis" action for the Plugins menu
	QAction* m_action;
};
