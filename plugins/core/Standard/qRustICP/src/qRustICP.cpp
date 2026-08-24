// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: qRustICP                          #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                    COPYRIGHT: (C) ICELABZ 2026                         #
// #                                                                        #
// ##########################################################################

/**
 * @file qRustICP.cpp
 *
 * @brief Rust ICP plugin entry point
 *
 * ICP (Iterative Closest Point) registration implemented in Rust
 * and exposed via CXX FFI. Uses the kiddo KD-tree crate for
 * nearest-neighbor queries.
 *
 * ## Algorithm
 *
 * Point-to-plane ICP with kiddo KD-tree acceleration.
 * Falls back to brute-force NN when point count < 2000.
 *
 * @extends QObject
 * @extends ccStdPluginInterface
 */
#include "qRustICP.h"

#include "RustICP.h"

#include <QtGui>

// Default constructor:
//  - pass the Qt resource path to the info.json file
//  - the constructor's main job is to initialise the action
qRustICP::qRustICP(QObject* parent)
    : QObject(parent)
    , ccStdPluginInterface(":/CC/plugin/qRustICP/info.json")
    , m_action(nullptr)
{
}

// Enable the action only when exactly 2 point clouds are selected.
void qRustICP::onNewSelection(const ccHObject::Container& selectedEntities)
{
	if (m_action == nullptr)
	{
		return;
	}

	int cloudCount = 0;
	for (ccHObject* obj : selectedEntities)
	{
		if (obj && obj->isA(CC_TYPES::POINT_CLOUD))
		{
			++cloudCount;
		}
	}

	// The action runs on (model, data) — exactly two point clouds.
	m_action->setEnabled(cloudCount == 2);
}

QList<QAction*> qRustICP::getActions()
{
	if (!m_action)
	{
		m_action = new QAction(getName(), this);
		m_action->setToolTip(getDescription());
		m_action->setIcon(getIcon());

		connect(m_action, &QAction::triggered, this, [this]()
		        {
			// Pull the selected point clouds from the database.
			ccHObject::Container selected = m_app->getSelectedEntities();
			ccPointCloud *model = nullptr;
			ccPointCloud *data = nullptr;
			for ( ccHObject *obj : selected )
			{
				if ( !obj || !obj->isA( CC_TYPES::POINT_CLOUD ) )
				{
					continue;
				}
				if ( !model )
				{
					model = static_cast<ccPointCloud *>( obj );
				}
				else if ( !data )
				{
					data = static_cast<ccPointCloud *>( obj );
				}
			}

			if ( !model || !data )
			{
				m_app->dispToConsole(
					"[qRustICP] Select exactly 2 point clouds (model + data).",
					ccMainAppInterface::ERR_CONSOLE_MESSAGE );
				return;
			}

			// Run the ICP. The Rust binary does the heavy lifting;
			// the plugin just orchestrates the subprocess and
			// applies the recovered transform in place.
			RustICP::performRustICP( m_app, model, data, 50 ); });
	}

	return {m_action};
}
