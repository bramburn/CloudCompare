/**
 * @file main.cpp
 *
 * @brief CloudCompare main entry point
 *
 * ## Application Initialization Sequence
 *
 * 1. Qt application: setAttribute, setOrganizationName, QApplication construction
 * 2. Sentry crash handler: sentry_init() (if CC_USE_SENTRY=ON)
 * 3. Command-line parsing: qCC::CommandLineParser if --SILENT or --CMD
 * 4. QSettings: restore last-used language
 * 5. OpenGL: ccGLUtils::SetForwardCompatibleGL for the GPU
 * 6. Normal vectors table: ccNormalVectors::GetUnique()
 * 7. Color scales manager: ccColorScalesManager::Initialize()
 * 8. Main window: MainWindow construction
 * 9. Plugins: ccPluginManager::loadPlugins()
 * 10. Splash screen: shown during initialization, hidden when ready
 * 11. ccConsole::Init(): message sink registration
 * 12. ccLog: startup banner (version, date, Qt version)
 * 13. ccCommandLineParser: parse CLI args if in command mode
 * 14. exec(): Qt event loop
 * 15. Cleanup: sentry_close()
 *
 * ## Command-Line Modes
 *
 * - **Silent mode** (--SILENT): batch processing without GUI
 * - **Command mode** (--CMD): CLI with console
 * - **Normal mode**: full GUI with splash screen
 *
 * ## Environment Variables
 *
 * - CC_SENTRY_DSN: Sentry DSN for crash reporting
 * - CC_USE_SENTRY: enable/disable Sentry
 * - QT_QPA_PLATFORM: offscreen on headless servers
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#include <ccIncludeGL.h>

// Qt
#include <QDir>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QSplashScreen>
#include <QTime>
#include <QTimer>
#include <QTranslator>

// qCC_db
#include <ccColorScalesManager.h>
#include <ccLog.h>
#include <ccNormalVectors.h>
#include <ccPointCloud.h>

// qCC_io
#include <FileIOFilter.h>
#include <ccGlobalShiftManager.h>

// local
#include "ccApplication.h"
#include "ccCommandLineParser.h"
#include "ccGuiParameters.h"
#include "ccPersistentSettings.h"
#include "ccTranslationManager.h"
#include "mainwindow.h"

// plugins
#include "ccPluginInterface.h"
#include "ccPluginManager.h"

// Sentry crash reporting
#ifdef CC_USE_SENTRY
#include <QDir>
#include <sentry.h>
#endif

#ifdef USE_VLD
#include <vld.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

/**
 * @brief Determines if the application is running in command-line mode
 *
 * On macOS, the Finder sometimes adds a "process serial number" argument
 * (-psn_*) when double-clicking the application. This function filters out
 * such system-generated arguments to accurately detect CLI mode.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return true if command-line mode is detected, false otherwise
 */
static bool IsCommandLine(int argc, char** argv)
{
#ifdef Q_OS_MAC
	// On macOS, when double-clicking the application, the Finder (sometimes!) adds a command-line parameter
	// like "-psn_0_582385" which is a "process serial number".
	// We need to recognize this and discount it when determining if we are running on the command line or not.

	int numRealArgs = argc;

	for (int i = 1; i < argc; ++i)
	{
		if (strncmp(argv[i], "-psn_", 5) == 0)
		{
			--numRealArgs;
		}
	}

	return (numRealArgs > 1) && (argv[1][0] == '-');
#else
	return (argc > 1) && (argv[1][0] == '-');
#endif
}

/**
 * @brief CloudCompare application entry point
 *
 * Initializes the Qt application, sets up OpenGL context, loads plugins,
 * and either enters GUI mode (with MainWindow) or command-line processing mode.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Application exit code (0 for success, non-zero for failure)
 *
 * @note On Windows, automatically attaches to parent console for stdout/stderr
 *       redirection if launched from command line.
 * @note Handles special commands: -LANG for translation loading, -VERBOSITY for log level
 * @see ccCommandLineParser::Parse() for CLI mode processing
 * @see MainWindow for GUI mode initialization
 */
int main(int argc, char** argv)
{
#ifdef _WIN32 // This will allow printf to function on windows when opened from command line
	DWORD stdout_type = GetFileType(GetStdHandle(STD_OUTPUT_HANDLE));
	if (AttachConsole(ATTACH_PARENT_PROCESS))
	{
		if (stdout_type == FILE_TYPE_UNKNOWN) // this will allow std redirection (./executable > out.txt)
		{
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
		}
	}
#endif

	// -------------------------------------------------------------------------
	// Sentry crash reporting — must be initialised before anything else
	// so that crashes during startup are captured.
	// CC_USE_SENTRY is set by CMake when sentry-native is found.
	// -------------------------------------------------------------------------
#ifdef CC_USE_SENTRY
	{
		sentry_options_t* options = sentry_options_new();
		sentry_options_set_dsn(options, CC_SENTRY_DSN);
		// Store crash DB next to the executable so it travels with the install
		sentry_options_set_database_path(options, QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/.sentry-native").toUtf8().constData());
		sentry_options_set_release(options, CC_SENTRY_RELEASE);
#ifdef _DEBUG
		sentry_options_set_debug(options, 1);
#endif
		sentry_init(options);
		// Ensure Sentry flushes any queued events on normal shutdown
		QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, []
		                 { sentry_close(); });
	}
#endif // CC_USE_SENTRY

	bool commandLine = IsCommandLine(argc, argv);

	// Convert the input arguments to QString before the application is initialized
	// (as it will force utf8, which might prevent from properly reading filenames from the command line)
	QStringList argumentsLocal8Bit;
	for (int i = 0; i < argc; ++i)
	{
		argumentsLocal8Bit << QString::fromLocal8Bit(argv[i]);
	}

	// specific commands
	int lastArgumentIndex = 1;
	if (commandLine)
	{
		// translation file selection
		if (lastArgumentIndex < argumentsLocal8Bit.size()
		    && argumentsLocal8Bit[lastArgumentIndex].toUpper() == "-LANG")
		{
			// remove verified local option
			argumentsLocal8Bit.removeAt(lastArgumentIndex);

			if (lastArgumentIndex >= argumentsLocal8Bit.size())
			{
				ccLog::Error(QObject::tr("Missing argument after %1: language file").arg("-LANG"));
				return EXIT_FAILURE;
			}

			// remove verified arguments so that -SILENT will be the first one (if present)...
			QString langFilename = argumentsLocal8Bit.takeAt(lastArgumentIndex);

			ccTranslationManager::Get().loadTranslation(langFilename);
			commandLine = false;
		}

		if (lastArgumentIndex < argumentsLocal8Bit.size()
		    && argumentsLocal8Bit[lastArgumentIndex].toUpper() == "-VERBOSITY")
		{
			// remove verified local option
			argumentsLocal8Bit.removeAt(lastArgumentIndex);

			if (lastArgumentIndex >= argumentsLocal8Bit.size())
			{
				ccLog::Error(QObject::tr("Missing argument after %1: verbosity level").arg("-VERBOSITY"));
				return EXIT_FAILURE;
			}

			// remove verified arguments so that -SILENT will be the first one (if present)...
			QString verbosityLevelStr = argumentsLocal8Bit.takeAt(lastArgumentIndex);

			bool ok = false;
			int verbosityLevel = verbosityLevelStr.toInt(&ok);
			if (!ok || verbosityLevel < 0)
			{
				ccLog::Warning(QObject::tr("Invalid verbosity level: %1").arg(verbosityLevelStr));
			}
			else
			{
				ccLog::SetVerbosityLevel(verbosityLevel);
			}
		}
	}

#ifdef Q_OS_WIN
	// enables automatic scaling based on the monitor's pixel density
	ccApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

	ccApplication::InitOpenGL();

	ccApplication app(argc, argv, commandLine);

	if (!commandLine)
	{
		// if not in CLI mode, we set the default log verbosity level
		ccLog::SetVerbosityLevel(ccGui::Parameters().logVerbosityLevel);
	}

	// store the log message until a valid logging instance is registered
	ccLog::EnableMessageBackup(true);

	// splash screen - initialized to nullptr for now
	QScopedPointer<QSplashScreen> splash(nullptr);

	// GUI mode initialization
	if (!commandLine)
	{
		/**
		 * @brief OpenGL Context Validation
		 *
		 * CloudCompare requires OpenGL 2.1 or higher for 3D rendering.
		 * This code creates a temporary OpenGL context to verify hardware support
		 * before proceeding with the full application initialization.
		 *
		 * @see ccGLWindow for the main rendering window
		 */
		QOpenGLContext context;
		if (!context.create())
		{
			QMessageBox::critical(nullptr, "Error", "This application needs OpenGL to run!");
			return EXIT_FAILURE;
		}

		auto* glFunc = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_2_1>(&context);
		// Check if we have at least OpenGL 2.1
		if (!glFunc)
		{
			QMessageBox::critical(nullptr, "Error", "This application needs OpenGL 2.1 at least to run!");
			return EXIT_FAILURE;
		}

		// init splash screen - displays logo while application loads
		QPixmap pixmap(QString::fromUtf8(":/CC/images/imLogoV2Qt.png"));
		splash.reset(new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint));
		splash->show();
	}

	/**
	 * @brief Global Structures Initialization
	 *
	 * Initializes core subsystems that must be available throughout the
	 * application lifetime. These are singleton instances that are lazily
	 * created but forced here to ensure they're ready before any UI operations.
	 *
	 * @note FileIOFilter::InitInternalFilters() loads built-in file format
	 *       handlers. Plugin-based I/O handlers are loaded separately by
	 *       ccPluginManager::loadPlugins()
	 * @see ccNormalVectors for normal vector computation utilities
	 * @see ccColorScalesManager for color scale management
	 */
	FileIOFilter::InitInternalFilters();       // load all known I/O filters (plugins will come later!)
	ccNormalVectors::GetUniqueInstance();      // force pre-computed normals array initialization
	ccColorScalesManager::GetUniqueInstance(); // force pre-computed color tables initialization

	/**
	 * @brief Plugin System Initialization
	 *
	 * Loads all registered plugins from the plugins/ directory.
	 * Plugins extend CloudCompare's functionality with additional
	 * file formats, processing algorithms, and UI components.
	 *
	 * @see ccPluginManager for plugin discovery and management
	 * @see ccPluginInterface for plugin interface definition
	 */
	ccPluginManager& pluginManager = ccPluginManager::Get();
	pluginManager.loadPlugins();

	// restore some global parameters
	{
		QSettings settings;
		settings.beginGroup(ccPS::GlobalShift());
		double maxAbsCoord = settings.value(ccPS::MaxAbsCoord(), ccGlobalShiftManager::MaxCoordinateAbsValue()).toDouble();
		double maxAbsDiag = settings.value(ccPS::MaxAbsDiag(), ccGlobalShiftManager::MaxBoundgBoxDiagonal()).toDouble();
		settings.endGroup();

		ccLog::Print(QString("[Global Shift] Max abs. coord = %1 / max abs. diag = %2").arg(maxAbsCoord, 0, 'e', 0).arg(maxAbsDiag, 0, 'e', 0));

		ccGlobalShiftManager::SetMaxCoordinateAbsValue(maxAbsCoord);
		ccGlobalShiftManager::SetMaxBoundgBoxDiagonal(maxAbsDiag);
	}

	int result = 0;

	/**
	 * @brief Application Execution Path
	 *
	 * CloudCompare can run in two modes:
	 * - Command-line mode: batch processing without GUI (faster for scripts)
	 * - GUI mode: interactive 3D visualization and editing
	 */
	// command line mode
	if (commandLine)
	{
		/**
		 * @brief Command-Line Processing
		 *
		 * Parses and executes command-line arguments for batch processing.
		 * Supported commands include file loading, export, and various
		 * point cloud processing operations.
		 *
		 * @param[in] argumentsLocal8Bit Command-line arguments as QStringList
		 * @param[in] pluginManager Reference to loaded plugins for extended functionality
		 * @return Exit code from command processing
		 * @see ccCommandLineParser::Parse()
		 */
		result = ccCommandLineParser::Parse(argumentsLocal8Bit, pluginManager.pluginList());
	}
	else
	{
		/**
		 * @brief GUI Mode Initialization
		 *
		 * Creates and displays the main application window (MainWindow).
		 * Initializes plugin UI components, shows the window, and processes
		 * any additional command-line arguments as filenames to open.
		 *
		 * @see MainWindow::TheInstance()
		 * @see MainWindow::initPlugins()
		 * @see MainWindow::addToDB()
		 */
		// main window initialization
		MainWindow* mainWindow = MainWindow::TheInstance();
		if (!mainWindow)
		{
			QMessageBox::critical(nullptr, "Error", "Failed to initialize the main application window?!");
			return EXIT_FAILURE;
		}
		mainWindow->initPlugins();
		mainWindow->show();
		QCoreApplication::processEvents();

		// show current Global Shift parameters in Console
		{
			ccLog::Print(QString("[Global Shift] Max abs. coord = %1 / max abs. diag = %2")
			                 .arg(ccGlobalShiftManager::MaxCoordinateAbsValue(), 0, 'e', 0)
			                 .arg(ccGlobalShiftManager::MaxBoundgBoxDiagonal(), 0, 'e', 0));
		}

		if (splash)
		{
			splash->close();
		}

		if (argc > lastArgumentIndex)
		{
			// any additional argument is assumed to be a filename --> we try to load it/them
			QStringList filenames;
			for (int i = lastArgumentIndex; i < argc; ++i)
			{
				QString arg = argumentsLocal8Bit[i];

				// special command: auto start a plugin
				if (arg.startsWith(":start-plugin:"))
				{
					QString pluginName = arg.mid(14);
					QString pluginNameUpper = pluginName.toUpper();
					// look for this plugin
					bool found = false;
					for (ccPluginInterface* plugin : pluginManager.pluginList())
					{
						if (plugin->getName().replace(' ', '_').toUpper() == pluginNameUpper)
						{
							found = true;
							bool success = plugin->start();
							if (!success)
							{
								ccLog::Error(QString("Failed to start the plugin '%1'").arg(plugin->getName()));
							}
							break;
						}
					}

					if (!found)
					{
						ccLog::Error(QString("Couldn't find the plugin '%1'").arg(pluginName.replace('_', ' ')));
					}
				}
				else
				{
					filenames << arg;
				}
			}

			mainWindow->addToDB(filenames);
		}

		/**
		 * @brief Working Directory Setup
		 *
		 * Sets the application's working directory to its own location.
		 * On macOS, navigates up from the MacOS bundle directory to the
		 * application root, as macOS bundles place executables in a
		 * non-standard location.
		 *
		 * @note This is done AFTER processing command-line arguments to ensure
		 *       relative paths in arguments are resolved correctly.
		 */
		// change the default path to the application one (do this AFTER processing the command line)
		QDir workingDir = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
		// This makes sure that our "working directory" is not within the application bundle
		if (workingDir.dirName() == "MacOS")
		{
			workingDir.cdUp();
			workingDir.cdUp();
			workingDir.cdUp();
		}
#endif

		QDir::setCurrent(workingDir.absolutePath());

		/**
		 * @brief Main Event Loop
		 *
		 * Enters Qt's event loop. All GUI interactions and plugin operations
		 * are processed here. Exceptions are caught and displayed to the user
		 * with a crash notification dialog.
		 *
		 * @return Exit code returned from the event loop
		 * @note Plugins are stopped gracefully when the event loop exits
		 */
		// let's rock!
		try
		{
			result = QApplication::exec();
		}
		catch (const std::exception& e)
		{
			QMessageBox::warning(nullptr, "CC crashed!", QString("Hum, it seems that CC has crashed... Sorry about that :)\n") + e.what());
		}
		catch (...)
		{
			QMessageBox::warning(nullptr, "CC crashed!", "Hum, it seems that CC has crashed... Sorry about that :)");
		}

		// release the plugins - gracefully shut down all loaded plugins
		for (ccPluginInterface* plugin : pluginManager.pluginList())
		{
			plugin->stop(); // just in case
		}
	}

	/**
	 * @brief Cleanup and Shutdown
	 *
	 * Releases all global resources before application exit:
	 * - ccPointCloud::ReleaseShaders() must be called before OpenGL context destruction
	 * - MainWindow::DestroyInstance() cleans up the main window singleton
	 * - FileIOFilter::UnregisterAll() releases file format handlers
	 *
	 * @note Order matters! Shaders must be released while OpenGL context is still valid.
	 */
	// release global structures
	ccPointCloud::ReleaseShaders(); // must be done before the OpenGL context is released (i.e. before the windows is destroyed)
	MainWindow::DestroyInstance();
	FileIOFilter::UnregisterAll();

#ifdef CC_TRACK_ALIVE_SHARED_OBJECTS
	// for debug purposes
	unsigned alive = CCShareable::GetAliveCount();
	if (alive > 1)
	{
		printf("Error: some shared objects (%u) have not been released on program end!", alive);
		system("PAUSE");
	}
#endif

	return result;
}
