//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qBroom                      #
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
//#      COPYRIGHT: Wesley Grimes (Collision Engineering Associates)       #
//#                                                                        #
//##########################################################################

#ifndef Q_BROOM_PLUGIN_HEADER
#define Q_BROOM_PLUGIN_HEADER

/**
 * @file qBroom.h
 *
 * @brief Broom plugin
 *
 * Virtual broom plugin for removing noise from point clouds.
 *
 * @author Wesley Grimes (Collision Engineering Associates)
 */

#include "ccStdPluginInterface.h"

/**
 * @brief Broom plugin
 *
 * Virtual broom tool for cleaning point clouds.
 */
class qBroom : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )
	
	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qBroom" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qBroom(QObject* parent = nullptr);

	/// Destructor
	virtual ~qBroom() = default;

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

protected:
	/// Execute broom action
	void doAction();

protected:
	QAction* m_action;
};

#endif //Q_BROOM_PLUGIN_HEADER
