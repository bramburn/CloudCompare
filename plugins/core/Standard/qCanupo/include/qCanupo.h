//##########################################################################
//#                                                                        #
//#                     CLOUDCOMPARE PLUGIN: qCANUPO                       #
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
//#      COPYRIGHT: UEB (UNIVERSITE EUROPEENNE DE BRETAGNE) / CNRS         #
//#                                                                        #
//##########################################################################

#ifndef Q_CANUPO_PLUGIN_HEADER
#define Q_CANUPO_PLUGIN_HEADER

/**
 * @file qCanupo.h
 *
 * @brief CANUPO multi-scale dimensionality classification plugin
 *
 * Classifies 3D terrestrial LiDAR point clouds using the CANUPO
 * (Curvature And NDUtility-based classification for PointOwOls) algorithm.
 *
 * CANUPO is a dimensionality classifier: it classifies each point as
 * belonging to one of three geometric primitive classes at a given scale:
 * - **1D (linear/filamentary)**: powerlines, cables, thin vegetation
 * - **2D (planar/surface)**: ground, walls, roofs, flat terrain
 * - **3D (volumetric/scattered)**: foliage, tree crowns, complex vegetation
 *
 * Algorithm:
 * 1. Compute multi-scale eigenvalues (eigenvalue ratio analysis)
 * 2. Project onto a simplex (1D + 2D + 3D = 1)
 * 3. Classify based on proximity to class centroids
 * 4. Output scalar field with dimensionality [0=1D, 0.5=2D, 1=3D]
 *
 * Two modes:
 * - **Training**: build a custom classifier from labeled training data
 * - **Classification**: apply a trained classifier to segment the cloud
 *
 * Reference: Brodu N., Lague D. "3D Terrestrial lidar data classification
 * of complex natural scenes using a multi-scale dimensionality criterion."
 * ISPRS Journal of Photogrammetry and Remote Sensing, 2012.
 *
 * @see https://doi.org/10.1016/j.isprsjprs.2012.03.005
 *
 * @author UEB (Université Bretagne Sud) / CNRS
 */

#include <ccStdPluginInterface.h>
#include <ccHObject.h>

/**
 * @class qCanupoPlugin
 *
 * @brief CANUPO multi-scale dimensionality classification plugin
 *
 * Classifies point clouds as 1D / 2D / 3D geometry at multiple scales.
 * Activates when a point cloud is selected.
 *
 * Two actions:
 * - "CANUPO Classify": apply a pre-trained classifier
 * - "CANUPO Train": build a new classifier from labeled training data
 *
 * Training workflow:
 * 1. Select point cloud
 * 2. Choose "CANUPO Train"
 * 3. Paint labeled regions (ground, vegetation, etc.)
 * 4. Set scales to analyze
 * 5. Save classifier (.canupo file)
 *
 * Classification workflow:
 * 1. Load trained classifier
 * 2. Select point cloud
 * 3. Choose "CANUPO Classify"
 * 4. View resulting dimensionality scalar field
 *
 * @extends QObject
 * @extends ccStdPluginInterface
 * @implements ccStdPluginInterface
 */
class qCanupoPlugin : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccStdPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qCanupo" FILE "../info.json")

  public:
	/**
	 * @brief Construct the CANUPO plugin
	 *
	 * @param[in] parent QObject parent
	 */
	qCanupoPlugin(QObject* parent = nullptr);

	// ccStdPluginInterface

	/**
	 * @brief Handle new entity selection
	 *
	 * Enables both actions when at least one point cloud is selected.
	 *
	 * @param[in] selectedEntities Current selection
	 */
	void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/**
	 * @brief Get the plugin's actions
	 *
	 * Returns two actions: "CANUPO Classify" and "CANUPO Train".
	 *
	 * @return List containing the classify and train actions
	 */
	virtual QList<QAction*> getActions() override;

	/**
	 * @brief Register command-line commands
	 *
	 * Registers the -CANUPO command for batch processing.
	 *
	 * @param[in] cmd Command-line interface
	 */
	virtual void registerCommands(ccCommandLineInterface* cmd) override;

  protected:
	/**
	 * @brief Execute point cloud classification
	 *
	 * Loads a pre-trained .canupo classifier file and applies it
	 * to the selected cloud. Creates a dimensionality scalar field.
	 */
	void doClassifyAction();

	/**
	 * @brief Execute classifier training
	 *
	 * Opens the training dialog where the user paints labeled regions,
	 * configures scales, and saves a .canupo classifier file.
	 */
	void doTrainAction();

  protected:
	//! "CANUPO Classify" action
	QAction* m_classifyAction;
	//! "CANUPO Train" action
	QAction* m_trainAction;

	//! Cached selection (refreshed by onNewSelection)
	ccHObject::Container m_selectedEntities;
};

#endif //Q_CANUPO_HEADER
