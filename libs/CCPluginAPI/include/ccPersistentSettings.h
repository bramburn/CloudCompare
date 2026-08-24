// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccPersistentSettings.h
 *
 * @brief Standardized QSettings key constants
 *
 * Central registry of all QSettings key strings used to persist application
 * preferences. Using these inline functions instead of string literals
 * ensures consistency across all modules and prevents typos.
 *
 * Usage:
 * @code
 * QSettings settings;
 * settings.setValue(ccPS::MainWinGeom(), saveGeometry());
 * settings.beginGroup(ccPS::Console());
 * bool qtMessages = settings.value("QtMessagesEnabled", false).toBool();
 * settings.endGroup();
 * @endcode
 *
 * Key groups:
 * - UI: mainWindowGeometry, mainWindowState, AppStyle
 * - Files: LoadFile, SaveFile, CurrentPath, selectedInputFilter, etc.
 * - Tools: HeightGridGeneration, VolumeCalculation, duplicatePoints
 * - Display: GlobalShift, AutoPickRotationCenter, View3dRotationAxisLocked
 * - Plugins: Plugins, Options, Translation, Shortcuts
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 * @see QSettings
 */

#pragma once

// Qt
#include <QString>

/**
 * @brief Persistent settings key constants
 *
 * Namespace providing canonical string keys for QSettings. All keys use
 * lowercase names matching the Qt-internal convention. Plugins should
 * use beginGroup/endGroup with ccPS::Plugins() for their own settings.
 *
 * @see QSettings
 */
namespace ccPS
{
	// File dialog paths
	/** @brief "LoadFile" — last directory used for opening files */
	inline const QString LoadFile()
	{
		return QStringLiteral("LoadFile");
	}
	/** @brief "SaveFile" — last directory used for saving files */
	inline const QString SaveFile()
	{
		return QStringLiteral("SaveFile");
	}

	// Window state
	/** @brief "mainWindowGeometry" — QMainWindow saveGeometry() */
	inline const QString MainWinGeom()
	{
		return QStringLiteral("mainWindowGeometry");
	}
	/** @brief "mainWindowState" — QMainWindow saveState() */
	inline const QString MainWinState()
	{
		return QStringLiteral("mainWindowState");
	}
	/** @brief "doNotRestoreWindowGeometry" — skip geometry restore at startup */
	inline const QString DoNotRestoreWindowGeometry()
	{
		return QStringLiteral("doNotRestoreWindowGeometry");
	}

	// UI
	/** @brief "AppStyle" — Qt stylesheet or style name */
	inline const QString AppStyle()
	{
		return QStringLiteral("AppStyle");
	}
	/** @brief "currentPath" — current working directory for file dialogs */
	inline const QString CurrentPath()
	{
		return QStringLiteral("currentPath");
	}

	// File filters
	/** @brief "selectedInputFilter" — last selected import filter */
	inline const QString SelectedInputFilter()
	{
		return QStringLiteral("selectedInputFilter");
	}
	/** @brief "selectedOutputFilterCloud" — last selected cloud export format */
	inline const QString SelectedOutputFilterCloud()
	{
		return QStringLiteral("selectedOutputFilterCloud");
	}
	/** @brief "selectedOutputFilterMesh" — last selected mesh export format */
	inline const QString SelectedOutputFilterMesh()
	{
		return QStringLiteral("selectedOutputFilterMesh");
	}
	/** @brief "selectedOutputFilterImage" — last selected image export format */
	inline const QString SelectedOutputFilterImage()
	{
		return QStringLiteral("selectedOutputFilterImage");
	}
	/** @brief "selectedOutputFilterPoly" — last selected polyline export format */
	inline const QString SelectedOutputFilterPoly()
	{
		return QStringLiteral("selectedOutputFilterPoly");
	}

	// Tool settings
	/** @brief "duplicatePoints" — group key for duplicate detection settings */
	inline const QString DuplicatePointsGroup()
	{
		return QStringLiteral("duplicatePoints");
	}
	/** @brief "minDist" — minimum distance for duplicate detection */
	inline const QString DuplicatePointsMinDist()
	{
		return QStringLiteral("minDist");
	}
	/** @brief "HeightGridGeneration" — group key for height grid settings */
	inline const QString HeightGridGeneration()
	{
		return QStringLiteral("HeightGridGeneration");
	}
	/** @brief "VolumeCalculation" — group key for volume calc settings */
	inline const QString VolumeCalculation()
	{
		return QStringLiteral("VolumeCalculation");
	}

	// Console
	/** @brief "Console" — group key for console settings */
	inline const QString Console()
	{
		return QStringLiteral("Console");
	}

	// Global shift / coordinates
	/** @brief "GlobalShift" — group key for global shift settings */
	inline const QString GlobalShift()
	{
		return QStringLiteral("GlobalShift");
	}
	/** @brief "MaxAbsCoord" — maximum absolute coordinate threshold */
	inline const QString MaxAbsCoord()
	{
		return QStringLiteral("MaxAbsCoord");
	}
	/** @brief "MaxAbsDiag" — maximum absolute diagonal threshold */
	inline const QString MaxAbsDiag()
	{
		return QStringLiteral("MaxAbsDiag");
	}

	// 3D view
	/** @brief "AutoPickRotationCenter" — auto-pick pivot at screen center */
	inline const QString AutoPickRotationCenter()
	{
		return QStringLiteral("AutoPickRotationCenter");
	}
	/** @brief "View3dRotationAxisLocked" — lock rotation to a fixed axis */
	inline const QString View3dRotationAxisLocked()
	{
		return QStringLiteral("View3dRotationAxisLocked");
	}
	/** @brief "View3dLockedAxisRotation" — locked rotation angle */
	inline const QString View3dLockedAxisRotation()
	{
		return QStringLiteral("View3dLockedAxisRotation");
	}

	// App-level
	/** @brief "Options" — group key for application options */
	inline const QString Options()
	{
		return QStringLiteral("Options");
	}
	/** @brief "Plugins" — group key for plugin settings */
	inline const QString Plugins()
	{
		return QStringLiteral("Plugins");
	}
	/** @brief "Translation" — current language/translation */
	inline const QString Translation()
	{
		return QStringLiteral("Translation");
	}
	/** @brief "Shortcuts" — keyboard shortcut mappings */
	inline const QString Shortcuts()
	{
		return QStringLiteral("Shortcuts");
	}
}; // namespace ccPS
