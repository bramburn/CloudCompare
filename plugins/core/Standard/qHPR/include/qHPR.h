//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qHPR                        #
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

#ifndef Q_HPR_PLUGIN_HEADER
#define Q_HPR_PLUGIN_HEADER

/**
 * @file qHPR.h
 *
 * @brief Hidden Point Removal plugin
 *
 * Hidden Point Removal algorithm for approximating points visibility.
 *
 * Reference: "Direct Visibility of Point Sets", Katz, Tal, Basri.
 * SIGGRAPH 2007
 *
 * @author Daniel Girardeau-Montaut
 */

#include "ccStdPluginInterface.h"

//CCCoreLib
#include <ReferenceCloud.h>

/**
 * @class qHPR
 *
 * @brief Hidden Point Removal plugin
 *
 * Approximate points visibility using the HPR algorithm.
 */
class qHPR : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )

	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qHPR" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qHPR(QObject* parent = nullptr);

	/// Destructor
	virtual ~qHPR() = default;

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;
	
	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

protected:
	/// Execute HPR action
	void doAction();

	/**
	 * @brief Remove hidden points
	 * @param[in] theCloud Input point cloud
	 * @param[in] viewPoint Viewpoint
	 * @param[in] fParam HPR parameter
	 * @return Visible points
	 */
	CCCoreLib::ReferenceCloud* removeHiddenPoints(CCCoreLib::GenericIndexedCloudPersist* theCloud, const CCVector3d& viewPoint, double fParam);

protected:
	QAction* m_action;
};

#endif
