//##########################################################################
//#                                                                        #
//#                  CLOUDCOMPARE PLUGIN: qPoissonRecon                    #
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

#ifndef Q_POISSON_RECON_PLUGIN_HEADER
#define Q_POISSON_RECON_PLUGIN_HEADER

/**
 * @file qPoissonRecon.h
 *
 * @brief Poisson surface reconstruction plugin
 *
 * Surface reconstruction using the Poisson surface reconstruction algorithm.
 *
 * Reference: "Poisson Surface Reconstruction", M. Kazhdan, M. Bolitho, and H. Hoppe,
 * Symposium on Geometry Processing 2006.
 *
 * @author Daniel Girardeau-Montaut
 */

#include "ccStdPluginInterface.h"

/**
 * @class qPoissonRecon
 *
 * @brief Poisson surface reconstruction plugin
 *
 * Surface reconstruction from point clouds using Poisson algorithm.
 */
class qPoissonRecon : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )
	
	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qPoissonRecon" FILE "../info.json" )

public:

	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qPoissonRecon(QObject* parent = nullptr);

	/// Destructor
	virtual ~qPoissonRecon() = default;

	/// Handle new selection
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;
	
	/// Get plugin actions
	virtual QList<QAction *> getActions() override;

protected:
	/// Execute reconstruction action
	void doAction();

protected:
	QAction* m_action;

};

#endif
