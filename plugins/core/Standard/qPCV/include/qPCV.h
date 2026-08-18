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
 * @brief Potentially Visible Set (PCV) plugin
 *
 * Ambient occlusion computation using the ShadeVis algorithm.
 *
 * Reference: "Visibility based methods and assessment for detail-recovery",
 * M. Tarini, P. Cignoni, R. Scopigno, Visualization 2003.
 *
 * @author Daniel Girardeau-Montaut
 */

#include "ccStdPluginInterface.h"
#include "PCVCommand.h"

/**
 * @class qPCV
 *
 * @brief Potentially Visible Set plugin
 *
 * Compute ambient occlusion using ShadeVis algorithm.
 */
class qPCV : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )
	
	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qPCV" FILE "../info.json" )

public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qPCV(QObject* parent = nullptr);
	
	/// Destructor
	~qPCV()override  = default;

	/// Handle new selection
	void onNewSelection(const ccHObject::Container& selectedEntities) override;
	
	/// Get plugin actions
	QList<QAction *> getActions() override;
	
	/// Register command line commands
	void registerCommands(ccCommandLineInterface *cmd) override;

private:
	/// Execute PCV action
	void doAction();

	QAction* m_action;
};
