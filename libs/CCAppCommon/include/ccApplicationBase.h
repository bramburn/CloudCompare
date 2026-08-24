#pragma once
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
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

/**
 * @file ccApplicationBase.h
 *
 * @brief CloudCompare application base class
 *
 * Base class for both qCC (full GUI) and ccViewer (read-only viewer).
 * Provides:
 * - Qt application lifecycle (inherits QApplication)
 * - OpenGL initialization (InitOpenGL must be called before constructing)
 * - Translation and shader resource paths
 * - Plugin search path setup
 * - Command-line vs GUI mode detection
 *
 * The global macro ccApp casts QCoreApplication::instance() to
 * ccApplicationBase* for easy access from anywhere in the app.
 *
 * Init sequence:
 * 1. QApplication(argc, argv)
 * 2. ccApplicationBase::InitOpenGL() — must precede step 3
 * 3. new ccApplicationBase(argc, argv, isCmd, version)
 *
 * @see ccApplication for the qCC-specific subclass
 * @see ccApp macro for global singleton access
 */

#include "CCAppCommon.h"

// Qt
#include <QApplication>

/// Global application instance accessor
#define ccApp (static_cast<ccApplicationBase*>(QCoreApplication::instance()))

/**
 * @brief Application base class
 *
 * Base class for CloudCompare application.
 */
class CCAPPCOMMON_LIB_API ccApplicationBase : public QApplication
{
  public:
	/**
	 * @brief Initialize OpenGL
	 *
	 * Must be called before instantiating the application class.
	 */
	static void InitOpenGL();

	/**
	 * @brief Create application
	 * @param[in] argc Argument count
	 * @param[in] argv Argument values
	 * @param[in] isCommandLine Command line mode
	 * @param[in] version Version string
	 */
	ccApplicationBase(int& argc, char** argv, bool isCommandLine, const QString& version);

	/**
	 * @brief Check if command line mode
	 * @return true if command line
	 */
	inline bool isCommandLine() const
	{
		return m_isCommandLine;
	}

	/**
	 * @brief Get translation path
	 * @return Path to translations
	 */
	inline const QString& translationPath() const
	{
		return m_translationPath;
	}

	/**
	 * @brief Get version string
	 * @return Version
	 */
	inline const QString& versionStr() const
	{
		return m_versionStr;
	}

	/**
	 * @brief Get full version string for display
	 *
	 * @param[in] includeOS If true, appends " (Windows/macOS/Linux)"
	 * @return Formatted version string (e.g. "CloudCompare 2.14.0")
	 */
	QString versionLongStr(bool includeOS) const;

	/**
	 * @brief Set application style
	 * @param[in] styleKey QStyleFactory key
	 * @return true on success
	 */
	bool setAppStyle(QString styleKey);

  private:
	/// Setup paths
	void setupPaths();

	/// Version string
	const QString m_versionStr;

	/// Shader path
	QString m_shaderPath;

	/// Translation path
	QString m_translationPath;

	/// Plugin paths
	QStringList m_pluginPaths;

	/// Command line mode flag
	const bool m_isCommandLine;
};
