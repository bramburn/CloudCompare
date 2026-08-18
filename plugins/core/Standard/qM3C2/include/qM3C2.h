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

/**
 * @file qM3C2.h
 *
 * @brief M3C2 plugin
 *
 * Multiscale Model to Model Cloud Comparison (M3C2) algorithm.
 *
 * Reference: "Accurate 3D comparison of complex topography with terrestrial laser scanner:
 * application to the Rangitikei canyon (N-Z)", Lague, D., Brodu, N. and Leroux, J.,
 * ISPRS journal of Photogrammetry and Remote Sensing, 2013.
 *
 * @author Universite Europeenne de Bretagne
 */

#include "ccStdPluginInterface.h"

#include <ccHObject.h>

/**
 * @class qM3C2Plugin
 *
 * @brief M3C2 plugin
 *
 * Multiscale Model to Model Cloud Comparison algorithm.
 */
class qM3C2Plugin : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )

	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qM3C2" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	qM3C2Plugin(QObject* parent = nullptr);

	/// Destructor
	virtual ~qM3C2Plugin() = default;

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;
	
	/// Get plugin actions
	virtual QList<QAction *> getActions() override;
	
	/// Register command line commands
	virtual void registerCommands(ccCommandLineInterface* cmd) override;

private:
	/// Execute M3C2 action
	void doAction();

	QAction* m_action;
	ccHObject::Container m_selectedEntities;
};

#endif //Q_M3C2_PLUGIN_HEADER
