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

/**
 * @file RustICP.cpp
 *
 * @brief Rust ICP algorithm implementation
 *
 * ICP (Iterative Closest Point) registration implemented in Rust
 * via CXX FFI. Exposes the `icp_register` function.
 *
 * ## Algorithm
 *
 * Point-to-plane ICP using kiddo KD-tree for nearest-neighbor.
 * Iteratively:
 * 1. Find correspondences (nearest neighbors)
 * 2. Compute rigid transform (SVD / Horn's method)
 * 3. Apply transform
 * 4. Repeat until convergence
 *
 * @see RustICP.h
 */
#include "RustICP.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>

#include <ccPointCloud.h>
#include <ccMainAppInterface.h>
#include <ccScalarField.h>

namespace RustICP
{

// ---------------------------------------------------------------------------
// CSV serialisation. The Rust `cc_rust_cli` reads `x,y,z` per line. We
// also include an "index" column for the optional Qty column the CLI
// might emit in the future; today it expects just three floats per row.
// ---------------------------------------------------------------------------
static bool writeCloudToCsv( const ccPointCloud *cloud, const QString &path )
{
	QFile file( path );
	if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
	{
		return false;
	}
	QTextStream out( &file );
	const unsigned n = cloud->size();
	for ( unsigned i = 0; i < n; ++i )
	{
		const CCVector3 *p = cloud->getPoint( i );
		out << p->x << ',' << p->y << ',' << p->z << '\n';
	}
	file.close();
	return true;
}

// ---------------------------------------------------------------------------
// Locate the cc_rust_cli binary. The plugin auto-detects the most common
// build locations. Set `CC_RUST_CLI_BIN` to override (CI, dev workflow).
// ---------------------------------------------------------------------------
QString locateCcRustCliBinary()
{
	// 1. Explicit env override.
	const QByteArray envOverride = qgetenv( "CC_RUST_CLI_BIN" );
	if ( !envOverride.isEmpty() && QFile::exists( envOverride ) )
	{
		return QString::fromLocal8Bit( envOverride );
	}

	// 2. Walk up from the plugin's .so/.dll location to find cc-rust/.
	//    We try a few common layouts:
	//      <repo>/cc-rust/target/release/cc_rust_cli(.exe)
	//      <repo>/cc-rust/target/release/cc_rust_cli-*/cc_rust_cli(.exe)
	//    (the hash-suffixed variant is what cargo produces by default).
	const QStringList probePaths = {
		// From <repo>/plugins/<...>/qRustICP/<hash>/ → <repo>/cc-rust/target/release/
		QDir::cleanPath( QCoreApplication::applicationDirPath()
			+ "/../../../../cc-rust/target/release" ),
		// From the build dir's deployqt/ (when the plugin is deployed with the GUI).
		QDir::cleanPath( QCoreApplication::applicationDirPath()
			+ "/../cc-rust" ),
		// Hard-coded fallback for the dev environment on this machine.
		"C:/dev/CloudCompare/cc-rust/target/release",
	};

	for ( const QString &dir : probePaths )
	{
		QDir d( dir );
		if ( !d.exists() )
		{
			continue;
		}
		// Direct file.
		#ifdef Q_OS_WIN
		const QString direct = d.absoluteFilePath( "cc_rust_cli.exe" );
		if ( QFile::exists( direct ) )
		{
			return direct;
		}
		#else
		const QString direct = d.absoluteFilePath( "cc_rust_cli" );
		if ( QFile::exists( direct ) )
		{
			return direct;
		}
		#endif
		// Hash-suffixed (cargo test builds).
		const QStringList entries = d.entryList( { "cc_rust_cli*" },
			QDir::Files );
		for ( const QString &e : entries )
		{
			#ifdef Q_OS_WIN
			if ( e.endsWith( ".exe" ) && e.startsWith( "cc_rust_cli-" ) )
			{
				return d.absoluteFilePath( e );
			}
			#else
			if ( e.startsWith( "cc_rust_cli-" ) )
			{
				return d.absoluteFilePath( e );
			}
			#endif
		}
	}

	return QString(); // Not found.
}

// ---------------------------------------------------------------------------
// performRustICP — the main entry point. Serialises the two clouds to
// CSV, invokes cc_rust_cli as a subprocess, parses the JSON output,
// and applies the recovered 4x4 transform to the data cloud.
// ---------------------------------------------------------------------------
bool performRustICP( ccMainAppInterface *appInterface,
                    ccPointCloud *modelCloud,
                    ccPointCloud *dataCloud,
                    int iterations )
{
	if ( appInterface == nullptr || modelCloud == nullptr || dataCloud == nullptr )
	{
		return false;
	}

	// Find the binary.
	const QString binary = locateCcRustCliBinary();
	if ( binary.isEmpty() )
	{
		appInterface->dispToConsole(
			"[qRustICP] Could not find cc_rust_cli.exe — set CC_RUST_CLI_BIN or build cc-rust (cargo build --release).",
			ccMainAppInterface::ERR_CONSOLE_MESSAGE );
		return false;
	}

	// Write the two clouds to temp CSVs.
	const QString tempDir = QDir::cleanPath( QStandardPaths::writableLocation(
		QStandardPaths::TempLocation ) + "/qRustICP" );
	QDir().mkpath( tempDir );
	const QString modelCsv = QDir( tempDir ).absoluteFilePath( "model.csv" );
	const QString dataCsv = QDir( tempDir ).absoluteFilePath( "data.csv" );
	const QString outputJson = QDir( tempDir ).absoluteFilePath( "output.json" );

	if ( !writeCloudToCsv( modelCloud, modelCsv ) || !writeCloudToCsv( dataCloud, dataCsv ) )
	{
		appInterface->dispToConsole(
			"[qRustICP] Failed to write temp CSV files.",
			ccMainAppInterface::ERR_CONSOLE_MESSAGE );
		return false;
	}

	// Invoke the Rust binary.
	QProcess process;
	QStringList args;
	args << "icp"
		 << "--model" << modelCsv
		 << "--data" << dataCsv
		 << "--iterations" << QString::number( iterations )
		 << "--json" << outputJson;
	process.start( binary, args );
	if ( !process.waitForStarted( 30000 ) )
	{
		appInterface->dispToConsole(
			"[qRustICP] Failed to start cc_rust_cli — see console for details.",
			ccMainAppInterface::ERR_CONSOLE_MESSAGE );
		return false;
	}
	if ( !process.waitForFinished( -1 ) )
	{
		appInterface->dispToConsole(
			"[qRustICP] cc_rust_cli did not finish in time.",
			ccMainAppInterface::ERR_CONSOLE_MESSAGE );
		return false;
	}

	if ( process.exitCode() != 0 )
	{
		const QByteArray stderrOutput = process.readAllStandardError();
		appInterface->dispToConsole(
			"[qRustICP] cc_rust_cli failed (exit=" + QString::number( process.exitCode() )
				+ "): " + QString::fromLocal8Bit( stderrOutput ),
			ccMainAppInterface::ERR_CONSOLE_MESSAGE );
		return false;
	}

	// Parse the JSON output. The Rust binary writes the recovered
	// transform as a 16-element flat array in column-major order,
	// matching `glam::Mat4`'s layout.
	QFile jsonFile( outputJson );
	if ( !jsonFile.open( QIODevice::ReadOnly ) )
	{
		appInterface->dispToConsole(
			"[qRustICP] Could not read cc_rust_cli output JSON.",
			ccMainAppInterface::ERR_CONSOLE_MESSAGE );
		return false;
	}
	const QJsonDocument doc = QJsonDocument::fromJson( jsonFile.readAll() );
	jsonFile.close();
	if ( !doc.isObject() )
	{
		appInterface->dispToConsole(
			"[qRustICP] Invalid JSON output from cc_rust_cli.",
			ccMainAppInterface::ERR_CONSOLE_MESSAGE );
		return false;
	}
	const QJsonObject root = doc.object();
	const QJsonArray transformArr = root.value( "transform" ).toArray();
	if ( transformArr.size() != 16 )
	{
		appInterface->dispToConsole(
			"[qRustICP] JSON transform is not 16 elements.",
			ccMainAppInterface::ERR_CONSOLE_MESSAGE );
		return false;
	}

	// Build a 4x4 column-major matrix in ccGLMatrix form.
	// ccGLMatrix is row-major internally; we transpose on apply.
	float m[16];
	for ( int i = 0; i < 16; ++i )
	{
		m[i] = static_cast<float>( transformArr[i].toDouble() );
	}

	// Apply to the data cloud. ccPointCloud::applyRigidTransformation
	// expects a 4x4 matrix in row-major form. We transpose here.
	ccGLMatrix glMat;
	for ( int row = 0; row < 4; ++row )
	{
		for ( int col = 0; col < 4; ++col )
		{
			glMat.setValue( row, col, m[col * 4 + row] );
		}
	}
	dataCloud->applyRigidTransformation( glMat );

	// Report.
	const double rms = root.value( "rms" ).toDouble( -1.0 );
	const int iters = root.value( "iterations" ).toInt( -1 );
	const double wall = root.value( "wall_seconds" ).toDouble( -1.0 );
	appInterface->dispToConsole(
		QString( "[qRustICP] Done. iters=%1 rms=%2 wall=%3s" )
			.arg( iters )
			.arg( rms, 0, 'g', 6 )
			.arg( wall, 0, 'f', 3 ),
		ccMainAppInterface::STD_CONSOLE_MESSAGE );

	// Refresh the view.
	dataCloud->prepareDisplayForRefresh();
	if ( dataCloud->getParent() )
	{
		dataCloud->getParent()->prepareDisplayForRefresh();
	}

	return true;
}

} // namespace RustICP
