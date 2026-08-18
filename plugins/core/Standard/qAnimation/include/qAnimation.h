#pragma once

//##########################################################################
//#                                                                        #
//#                   CLOUDCOMPARE PLUGIN: qAnimation                      #
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
//#             COPYRIGHT: Ryan Wicks, 2G Robotics Inc., 2015              #
//#                                                                        #
//##########################################################################

/**
 * @file qAnimation.h
 *
 * @brief Animation plugin
 *
 * Animation capture plugin for CloudCompare.
 *
 * @author Ryan Wicks, 2G Robotics Inc.
 */

//qCC
#include "ccStdPluginInterface.h"

//Qt
#include <QObject>

/**
 * @brief Animation plugin
 *
 * Capture animations from CloudCompare.
 */
class qAnimation : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )

	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qAnimation" FILE "../info.json" )

public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	qAnimation(QObject* parent = nullptr);

	/// Destructor
	virtual ~qAnimation() = default;

	/// Handle new selection
	void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

private:
	/// Execute animation action
	void doAction();

	QAction* m_action;
};
