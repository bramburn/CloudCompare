//##########################################################################
//#                                                                        #
//#                   CLOUDCOMPARE PLUGIN: qHoughNormals                   #
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

#ifndef QHOUGH_NORMALS_PLUGIN_HEADER
#define QHOUGH_NORMALS_PLUGIN_HEADER

/**
 * @file qHoughNormals.h
 *
 * @brief Hough normals plugin
 *
 * Normal estimation using the Hough transform method.
 *
 * Reference: "Deep Learning for Robust Normal Estimation in Unstructured Point Clouds"
 * by Alexandre Boulch and Renaud Marlet, SGP 2016.
 *
 * @author Daniel Girardeau-Montaut
 */

#include "ccStdPluginInterface.h"

/**
 * @class qHoughNormals
 *
 * @brief Hough normals plugin
 *
 * Normal estimation using deep learning and Hough transform.
 */
class qHoughNormals : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )
	
	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qHoughNormals" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qHoughNormals(QObject* parent = nullptr);

	/// Destructor
	virtual ~qHoughNormals() = default;

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;
	
	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

protected:
	/// Execute action
	void doAction();

protected:
	QAction* m_action;
};

#endif //QHOUGH_NORMALS_PLUGIN_HEADER
