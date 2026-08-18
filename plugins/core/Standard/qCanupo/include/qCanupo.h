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
 * @brief CANUPO point cloud classification plugin
 *
 * Multi-scale dimensionality classification for 3D terrestrial lidar data.
 *
 * Reference: "3D Terrestrial lidar data classification of complex natural scenes
 * using a multi-scale dimensionality criterion", N. Brodu, D. Lague, 2012.
 *
 * @author Universite Europeenne de Bretagne / CNRS
 */

#include <ccStdPluginInterface.h>

#include <ccHObject.h>

/**
 * @class qCanupoPlugin
 *
 * @brief CANUPO plugin
 *
 * Multi-scale point cloud classification.
 */
class qCanupoPlugin : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )

	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qCanupo" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	qCanupoPlugin(QObject* parent = nullptr);

	/// Handle new selection
	void onNewSelection(const ccHObject::Container& selectedEntities) override;
	
	/// Get plugin actions
	virtual QList<QAction*> getActions() override;
	
	/// Register command line commands
	virtual void registerCommands(ccCommandLineInterface* cmd) override;

protected:
	/// Execute classification action
	void doClassifyAction();
	
	/// Execute training action
	void doTrainAction();

protected:
	QAction* m_classifyAction;
	QAction* m_trainAction;

	ccHObject::Container m_selectedEntities;
};

#endif //Q_CANUPO_HEADER
