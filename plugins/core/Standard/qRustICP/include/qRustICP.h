//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: qRustICP                          #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                    COPYRIGHT: (C) ICELABZ 2026                         #
//#                                                                        #
//##########################################################################

#pragma once

#include "ccStdPluginInterface.h"

/**
 * @file qRustICP.h
 *
 * @brief qRustICP standard plugin — runs the pure-Rust ICP
 * from `cc-rust/` on the selected model + data clouds.
 *
 * The plugin exposes one action: "Rust ICP" in the plugin
 * menu. The action takes the current selection (one model
 * cloud + one data cloud), serialises them to a temp CSV,
 * invokes `cc_rust_cli.exe icp` as a subprocess, parses the
 * JSON output for the recovered 4x4 transform, and applies
 * the transform to the data cloud. The result is the data
 * cloud aligned to the model, with a console message
 * containing the wall time + RMS + iteration count.
 *
 * Future work: replace the subprocess with an in-process
 * CXX FFI reverse-direction call into cc-rust (eliminating
 * the ~1-2s subprocess startup overhead). The staticlib
 * link is already in CMakeLists.txt for that path.
 */
class qRustICP : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )

	// IID should be unique. The format is
	// `ccorp.cloudcompare.plugin.<name>`.
	Q_PLUGIN_METADATA( IID "ccorp.cloudcompare.plugin.qRustICP" FILE "../info.json" )

public:
	explicit qRustICP( QObject *parent = nullptr );
	~qRustICP() override = default;

	// Inherited from ccStdPluginInterface
	void onNewSelection( const ccHObject::Container &selectedEntities ) override;
	QList<QAction *> getActions() override;

private:
	//! Single action — "Rust ICP" menu item
	QAction *m_action;
};
