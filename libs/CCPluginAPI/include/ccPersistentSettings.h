// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        //
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccPersistentSettings.h
 *
 * @brief Standardized QSettings key constants.
 *
 * @details Provides standardized key strings for QSettings to persist
 * application preferences across sessions.
 *
 * ## Overview
 *
 * These keys ensure consistent settings storage:
 * - Window geometry and state
 * - File dialog paths
 * - Recent files
 * - Plugin settings
 * - Tool preferences
 *
 * ## Usage
 *
 * @code
 * QSettings settings;
 *
 * // Save window geometry
 * settings.setValue(ccPS::MainWinGeom(), saveGeometry());
 *
 * // Load last path
 * QString path = settings.value(ccPS::CurrentPath()).toString();
 *
 * // Save plugin setting
 * settings.beginGroup(ccPS::Plugins());
 * settings.setValue("MyPlugin/enabled", true);
 * settings.endGroup();
 * @endcode
 *
 * ## Categories
 *
 * - **UI**: Window geometry, styles
 * - **Files**: Recent files, dialog paths, filters
 * - **Tools**: Tool-specific settings
 * - **Plugins**: Plugin preferences
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#pragma once

// Qt
#include <QString>

/**
 * @brief Persistent settings key constants.
 *
 * @details Namespace containing standardized settings keys.
 *
 * Use these instead of string literals to ensure
 * consistency across the application.
 */
namespace ccPS
{
	/**
	 * @brief Get load file dialog key.
	 */
	inline const QString LoadFile()
	{
		return QStringLiteral("LoadFile");
	}

	/**
	 * @brief Get save file dialog key.
	 */
	inline const QString SaveFile()
	{
		return QStringLiteral("SaveFile");
	}

	/**
	 * @brief Get main window geometry key.
	 */
	inline const QString MainWinGeom()
	{
		return QStringLiteral("mainWindowGeometry");
	}

	/**
	 * @brief Get main window state key.
	 */
	inline const QString MainWinState()
	{
		return QStringLiteral("mainWindowState");
	}

	/**
	 * @brief Get don't restore window geometry flag key.
	 */
	inline const QString DoNotRestoreWindowGeometry()
	{
		return QStringLiteral("doNotRestoreWindowGeometry");
	}

	/**
	 * @brief Get application style key.
	 */
	inline const QString AppStyle()
	{
		return QStringLiteral("AppStyle");
	}

	/**
	 * @brief Get current working directory path key.
	 */
	inline const QString CurrentPath()
	{
		return QStringLiteral("currentPath");
	}

	/**
	 * @brief Get selected input file filter key.
	 */
	inline const QString SelectedInputFilter()
	{
		return QStringLiteral("selectedInputFilter");
	}

	/**
	 * @brief Get selected cloud output filter key.
	 */
	inline const QString SelectedOutputFilterCloud()
	{
		return QStringLiteral("selectedOutputFilterCloud");
	}

	/**
	 * @brief Get selected mesh output filter key.
	 */
	inline const QString SelectedOutputFilterMesh()
	{
		return QStringLiteral("selectedOutputFilterMesh");
	}

	/**
	 * @brief Get selected image output filter key.
	 */
	inline const QString SelectedOutputFilterImage()
	{
		return QStringLiteral("selectedOutputFilterImage");
	}

	/**
	 * @brief Get selected polyline output filter key.
	 */
	inline const QString SelectedOutputFilterPoly()
	{
		return QStringLiteral("selectedOutputFilterPoly");
	}

	/**
	 * @brief Get duplicate points group key.
	 */
	inline const QString DuplicatePointsGroup()
	{
		return QStringLiteral("duplicatePoints");
	}

	/**
	 * @brief Get min distance for duplicate detection key.
	 */
	inline const QString DuplicatePointsMinDist()
	{
		return QStringLiteral("minDist");
	}

	/**
	 * @brief Get height grid generation settings key.
	 */
	inline const QString HeightGridGeneration()
	{
		return QStringLiteral("HeightGridGeneration");
	}

	/**
	 * @brief Get volume calculation settings key.
	 */
	inline const QString VolumeCalculation()
	{
		return QStringLiteral("VolumeCalculation");
	}

	/**
	 * @brief Get console settings key.
	 */
	inline const QString Console()
	{
		return QStringLiteral("Console");
	}

	/**
	 * @brief Get global shift settings key.
	 */
	inline const QString GlobalShift()
	{
		return QStringLiteral("GlobalShift");
	}

	/**
	 * @brief Get max absolute coordinate key.
	 */
	inline const QString MaxAbsCoord()
	{
		return QStringLiteral("MaxAbsCoord");
	}

	/**
	 * @brief Get max absolute diagonal key.
	 */
	inline const QString MaxAbsDiag()
	{
		return QStringLiteral("MaxAbsDiag");
	}

	/**
	 * @brief Get auto pick rotation center key.
	 */
	inline const QString AutoPickRotationCenter()
	{
		return QStringLiteral("AutoPickRotationCenter");
	}

	/**
	 * @brief Get 3D view rotation axis locked key.
	 */
	inline const QString View3dRotationAxisLocked()
	{
		return QStringLiteral("View3dRotationAxisLocked");
	}

	/**
	 * @brief Get 3D view locked axis rotation key.
	 */
	inline const QString View3dLockedAxisRotation()
	{
		return QStringLiteral("View3dLockedAxisRotation");
	}

	/**
	 * @brief Get options/settings group key.
	 */
	inline const QString Options()
	{
		return QStringLiteral("Options");
	}

	/**
	 * @brief Get plugins settings group key.
	 */
	inline const QString Plugins()
	{
		return QStringLiteral("Plugins");
	}

	/**
	 * @brief Get translation/language key.
	 */
	inline const QString Translation()
	{
		return QStringLiteral("Translation");
	}

	/**
	 * @brief Get keyboard shortcuts key.
	 */
	inline const QString Shortcuts()
	{
		return QStringLiteral("Shortcuts");
	}
}; // namespace ccPS
