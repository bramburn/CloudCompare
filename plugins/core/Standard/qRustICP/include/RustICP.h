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

class ccMainAppInterface;
class ccPointCloud;

namespace RustICP
{
	/**
	 * @brief Run pure-Rust ICP on (model, data) via a subprocess
	 * call to `cc_rust_cli.exe icp`.
	 *
	 * @param appInterface  CC's main app interface (for console
	 *                      output, status bar, error reporting).
	 * @param modelCloud    The reference cloud (won't move).
	 * @param dataCloud     The cloud to register (will move).
	 * @param iterations    Max ICP iterations (forwarded to
	 *                      cc_rust_cli as `-i`).
	 * @return  true on success (data cloud was transformed in
	 *          place), false on error (logged to console).
	 *
	 * The recovered 4x4 transform is applied to `dataCloud` via
	 * `ccPointCloud::applyRigidTransformation`. The wall time,
	 * RMS, and iteration count are reported to the console.
	 */
	bool performRustICP(
		ccMainAppInterface *appInterface,
		ccPointCloud *modelCloud,
		ccPointCloud *dataCloud,
		int iterations = 50 );

	/**
	 * @brief Locate the `cc_rust_cli.exe` binary.
	 *
	 * Resolution order:
	 *   1. CC_RUST_CLI_BIN env var (explicit override).
	 *   2. `<repo>/cc-rust/target/release/cc_rust_cli.exe`
	 *   3. `<repo>/cc-rust/target/release/cc_rust_cli(.exe)`
	 *      (without arch suffix).
	 *
	 * @return  Absolute path to the binary, or empty string if
	 *          not found.
	 */
	QString locateCcRustCliBinary();
}
