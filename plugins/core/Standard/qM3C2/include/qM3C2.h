//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qM3C2                       #
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
//#            COPYRIGHT: UNIVERSITE EUROPEENNE DE BRETAGNE                #
//#                                                                        #
//##########################################################################

#ifndef Q_M3C2_PLUGIN_HEADER
#define Q_M3C2_PLUGIN_HEADER

//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qM3C2                       #
//#                                                                        #
//##########################################################################

#ifdef QM3C2_PLUGIN_EXPORTS
#  define QM3C2_LIB_API Q_DECL_EXPORT
#else
#  define QM3C2_LIB_API Q_DECL_IMPORT
#endif

/**
 * @file qM3C2.h
 *
 * @brief Multiscale Model to Model Cloud Comparison (M3C2) plugin
 *
 * Implements the M3C2 algorithm for comparing two 3D point cloud datasets.
 *
 * The M3C2 algorithm:
 * 1. Projects points from cloud 2 onto the normal direction of each point in cloud 1
 * 2. Computes local density at multiple scales (using a core point approach)
 * 3. Calculates the mean 3D distance between corresponding point sets
 * 4. Provides confidence intervals based on point density variation
 *
 * Key outputs:
 * - Mean 3D distance (signed, along local normal)
 * - Standard deviation of distances
 * - Point density at each scale
 * - Confidence levels (95% CI)
 *
 * Reference: Lague D., Brodu N., Leroux J. "Accurate 3D comparison of complex
 * topography with terrestrial laser scanner: application to the Rangitikei canyon
 * (N-Z)." ISPRS Journal of Photogrammetry and Remote Sensing, 2013.
 *
 * @see https://doi.org/10.1016/j.isprsjprs.2013.04.009
 */

#include "ccStdPluginInterface.h"
#include <ccHObject.h>

/**
 * @class qM3C2Plugin
 *
 * @brief Multiscale Model to Model Cloud Comparison (M3C2) algorithm plugin
 *
 * Compares two point clouds using the M3C2 algorithm. Activates when
 * two or more point clouds are selected. Adds a "M3C2" action to the
 * Plugins menu.
 *
 * Parameter dialog options:
 * - Core point selection: use cloud 1 or cloud 2 as reference
 * - Normal scale: spatial scale for normal computation
 * - Search scales: radii for multi-scale comparison
 * - Max distance: maximum search distance threshold
 * - Precision: level of detail for density estimation
 *
 * Output: a new cloud (or scalar field) with M3C2 distances and confidence values.
 *
 * @extends QObject
 * @extends ccStdPluginInterface
 * @implements ccStdPluginInterface
 */
class qM3C2Plugin : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccStdPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qM3C2" FILE "../info.json")

  public:
	/**
	 * @brief Construct the M3C2 plugin
	 *
	 * @param[in] parent QObject parent
	 */
	qM3C2Plugin(QObject* parent = nullptr);

	/**
	 * @brief Destructor
	 */
	virtual ~qM3C2Plugin() = default;

	// ccStdPluginInterface

	/**
	 * @brief Handle new entity selection
	 *
	 * Enables the "M3C2" action when two or more point clouds
	 * are selected.
	 *
	 * @param[in] selectedEntities Current selection
	 */
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/**
	 * @brief Get the plugin's actions
	 *
	 * Returns the "M3C2" action for the Plugins menu.
	 *
	 * @return List containing the M3C2 action
	 */
	virtual QList<QAction*> getActions() override;

	/**
	 * @brief Register command-line commands
	 *
	 * Registers the -M3C2 command for batch processing.
	 *
	 * @param[in] cmd Command-line interface
	 */
	virtual void registerCommands(ccCommandLineInterface* cmd) override;

  private:
	/**
	 * @brief Execute the M3C2 comparison
	 *
	 * Opens the parameter dialog, computes the M3C2 distances,
	 * and creates output clouds with distance scalar fields.
	 */
	void doAction();

	//! "M3C2" action for the Plugins menu
	QAction* m_action;

	//! Cached selection (refreshed by onNewSelection)
	ccHObject::Container m_selectedEntities;
};

#endif //Q_M3C2_PLUGIN_HEADER
