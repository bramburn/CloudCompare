#pragma once

//#######################################################################################
//#                                                                                     #
//#                              CLOUDCOMPARE PLUGIN: qCSF                              #
//#                                                                                     #
//#        This program is free software; you can redistribute it and/or modify         #
//#        it under the terms of the GNU General Public License as published by         #
//#        the Free Software Foundation; version 2 or later of the License.             #
//#                                                                                     #
//#        This program is distributed in the hope that it will be useful,              #
//#        but WITHOUT ANY WARRANTY; without even the implied warranty of               #
//#        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 #
//#        GNU General Public License for more details.                                 #
//#                                                                                     #
//#        Please cite the following paper, If you use this plugin in your work.        #
//#                                                                                     #
//#  Zhang W, Qi J, Wan P, Wang H, Xie D, Wang X, Yan G. An Easy-to-Use Airborne LiDAR  #
//#  Data Filtering Method Based on Cloth Simulation. Remote Sensing. 2016; 8(6):501.   #
//#                                                                                     #
//#                                     Copyright ©                                     #
//#               RAMM laboratory, School of Geography, Beijing Normal University       #
//#                               (http://ramm.bnu.edu.cn/)                             #
//#                                                                                     #
//#                      Wuming Zhang; Jianbo Qi; Peng Wan; Hongtao Wang                #
//#                                                                                     #
//#                      contact us: 2009zwm@gmail.com; wpqjbzwm@126.com                #
//#                                                                                     #
//#######################################################################################

/**
 * @file qCSF.h
 *
 * @brief Cloth Simulation Filter plugin
 *
 * Point cloud filtering using Cloth Simulation Filter (CSF) algorithm
 * for ground/terrain filtering in LiDAR data.
 *
 * Reference: Zhang W, Qi J, Wan P, Wang H, Xie D, Wang X, Yan G.
 * An Easy-to-Use Airborne LiDAR Data Filtering Method Based on Cloth Simulation.
 * Remote Sensing. 2016; 8(6):501.
 */

#include "ccStdPluginInterface.h"

/**
 * @brief Cloth Simulation Filter plugin
 *
 * Ground filtering algorithm using cloth simulation.
 */
class qCSF : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )
	
	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qCSF" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qCSF(QObject* parent = nullptr);

	/// Destructor
	virtual ~qCSF() = default;

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

	/// Register command line commands
	virtual void registerCommands(ccCommandLineInterface* cmd) override;

protected:
	/// Execute CSF action
	void doAction();

protected:
	QAction* m_action;
};
