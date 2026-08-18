#pragma once

//##########################################################################
//#                                                                        #
//#                   CLOUDCOMPARE PLUGIN: qCloudLayers                    #
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
//#                     COPYRIGHT: WigginsTech 2022                        #
//#                                                                        #
//##########################################################################

/**
 * @file qCloudLayers.h
 *
 * @brief Cloud layers plugin
 *
 * Plugin for managing point cloud layers with ASPRS classification.
 */

#include <ccStdPluginInterface.h>

class ccCloudLayersDlg;

/**
 * @class qCloudLayers
 *
 * @brief Cloud layers plugin
 *
 * Manage point cloud layers with ASPRS classification.
 */
class qCloudLayers : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )
	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qCloudLayers" FILE "../info.json" )

public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qCloudLayers( QObject* parent = nullptr );
	
	/// Destructor
	~qCloudLayers() override = default;

	/// Handle new selection
	void onNewSelection( const ccHObject::Container& selectedEntities ) override;
	
	/// Get plugin actions
	QList<QAction*> getActions() override;

protected:
	/// Execute cloud layers action
	void doAction();

private:
	QAction* m_action;

	ccCloudLayersDlg* m_cloudLayersDlg;
};
