#pragma once

//##########################################################################
//#                                                                        #
//#                   CLOUDCOMPARE PLUGIN: qMeshBoolean                    #
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
//#                  COPYRIGHT: Daniel Girardeau-Montaut                   #
//#                                                                        #
//##########################################################################

/**
 * @file qMeshBoolean.h
 *
 * @brief Mesh boolean operations plugin
 *
 * Mesh boolean operations (CSG) using libIGL.
 *
 * Reference: https://libigl.github.io/
 *
 * @author Daniel Girardeau-Montaut
 */

//Qt
#include <QObject>

#include "ccStdPluginInterface.h"

class QAction;

/**
 * @class qMeshBoolean
 *
 * @brief Mesh boolean operations plugin
 *
 * Perform boolean operations on meshes using libIGL.
 */
class qMeshBoolean : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )

	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qMeshBoolean" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qMeshBoolean(QObject* parent = nullptr);

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities);
	
	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

protected:
	/// Execute boolean operation
	void doAction();

protected:
	QAction* m_action;
};
