//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qCork                       #
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

#ifndef Q_CORK_PLUGIN_HEADER
#define Q_CORK_PLUGIN_HEADER

/**
 * @file qCork.h
 *
 * @brief Mesh boolean operations plugin
 *
 * Mesh boolean operations (CSG) plugin using Cork library.
 *
 * Reference: https://github.com/cloudcompare/cork
 *
 * @author Daniel Girardeau-Montaut
 */

//Qt
#include <QObject>

#include "ccStdPluginInterface.h"

class QAction;

/**
 * @class qCork
 *
 * @brief Mesh boolean operations plugin
 *
 * Perform boolean operations on meshes using the Cork library.
 */
class qCork : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )

	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qCork" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qCork(QObject* parent = nullptr);

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities);
	
	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

protected:
	/// Execute mesh boolean operation
	void doAction();

protected:
	QAction* m_action;
};

#endif //Q_CORK_PLUGIN_HEADER
