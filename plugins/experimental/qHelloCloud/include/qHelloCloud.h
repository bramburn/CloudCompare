//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: qHelloCloud                        #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                             COPYRIGHT: ICELABZ                         #
//#                                                                        #
//##########################################################################

#pragma once

#include "ccStdPluginInterface.h"

//! qHelloCloud - experimental smoke-test plugin
/** Two actions:
	- "Hello world"   -> logs a greeting to the CC console
	- "Load point cloud" -> opens a file dialog, loads the file through the
	  standard CC filter chain, adds it to the DB tree, and zooms the active
	  3D view to fit it.

	This plugin exists to prove the experimental plugin scaffold works end
	to end. Use it as the copy-paste template for new experiments under
	plugins/experimental/.
**/
class qHelloCloud : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )

	Q_PLUGIN_METADATA( IID "ccorp.cloudcompare.plugin.qHelloCloud" FILE "../info.json" )

public:
	explicit qHelloCloud( QObject *parent = nullptr );
	~qHelloCloud() override = default;

	// Inherited from ccStdPluginInterface
	void onNewSelection( const ccHObject::Container &selectedEntities ) override;
	QList<QAction *> getActions() override;

private:
	QAction* m_actionHello   = nullptr;
	QAction* m_actionLoad    = nullptr;
};
