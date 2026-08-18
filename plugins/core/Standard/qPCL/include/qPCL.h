//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qPCL                        #
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
//#                        COPYRIGHT: Luca Penasa                          #
//#                                                                        #
//##########################################################################

#ifndef Q_PCL_PLUGIN_HEADER
#define Q_PCL_PLUGIN_HEADER

/**
 * @file qPCL.h
 *
 * @brief Point Cloud Library (PCL) bridge plugin
 *
 * Bridge to the Point Cloud Library for advanced point cloud processing.
 *
 * @author Luca Penasa
 */

#include "ccStdPluginInterface.h"

//Qt
#include <QObject>
#include <QtGui>

class BaseFilter;

/**
 * @class qPCL
 *
 * @brief PCL bridge plugin
 *
 * Bridge to PCL for point cloud processing.
 */
class qPCL : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )

	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qPCL" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	qPCL(QObject* parent = nullptr);
	
	/// Destructor
	virtual ~qPCL();

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;
	
	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

	/**
	 * @brief Add a filter
	 * @param[in] filter Filter to add
	 * @return Filter index
	 */
	int addFilter(BaseFilter* filter);

public:
	/// Handle new entity
	void handleNewEntity(ccHObject*);

	/// Handle entity modification
	void handleEntityChange(ccHObject*);

	/// Handle error message
	void handleErrorMessage(QString);

protected:
	std::vector<BaseFilter*> m_filters;
};

#endif//END Q_PCL_PLUGIN_HEADER
