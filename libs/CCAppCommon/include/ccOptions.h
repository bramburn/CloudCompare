#pragma once
// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
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
// #                 COPYRIGHT: Daniel Girardeau-Montaut                    #
// #                                                                        #
// ##########################################################################

/**
 * @file ccOptions.h
 *
 * @brief Application-level options singleton
 *
 * Global settings that persist across CloudCompare sessions. Unlike
 * ccGui::ParamStruct (which holds per-3D-view GL rendering parameters),
 * ccOptions holds cross-cutting application behavior settings.
 *
 * Persistence: stored in QSettings under the "ccOptions" group.
 * Loaded on app startup (fromPersistentSettings) and optionally saved
 * immediately after changes (toPersistentSettings).
 *
 * Thread safety: the singleton is not thread-safe. Access from the
 * main thread only.
 *
 * @see ccDisplaySettingsDlg for the UI that modifies these options
 * @see ccGui::ParamStruct for per-view GL rendering parameters
 */

#include "CCAppCommon.h"

#include <QString>

/**
 * @class ccOptions
 *
 * @brief Application-level settings singleton
 *
 * Holds application behavior settings that apply globally to all
 * CloudCompare instances. Options are loaded from QSettings at startup
 * and can be saved at any time.
 *
 * The singleton is exposed via Instance() for read access and
 * InstanceNonConst()/Set() for write access.
 *
 * @note This class deliberately has no constructor with all defaults
 * at the class level (members are set in the constructor body).
 * Use reset() to restore factory defaults.
 */
class CCAPPCOMMON_LIB_API ccOptions
{
  public:
	// Display options

	/**
	 * @brief Whether to display normals by default when loading a new entity
	 *
	 * When true, loaded clouds/meshes with normals will have their
	 * normals displayed automatically.
	 *
	 * @default false
	 */
	bool normalsDisplayedByDefault;

	/**
	 * @brief Whether to use the OS native file dialog
	 *
	 * When true, uses QFileDialog::getOpenFileName etc. (native OS dialog).
	 * When false, uses Qt's custom file dialog.
	 *
	 * @default true
	 */
	bool useNativeDialogs;

	/**
	 * @brief Whether to show a confirmation dialog when quitting
	 *
	 * @default true
	 */
	bool confirmQuit;

	/**
	 * @brief Whether to show a confirmation dialog before deleting entities
	 *
	 * @default true
	 */
	bool confirmDelete;

  public:
	/**
	 * @brief Construct with default values
	 *
	 * Calls reset() to initialize all members to their defaults.
	 */
	ccOptions();

	/**
	 * @brief Reset all options to factory defaults
	 *
	 * Restores all boolean members to their default values.
	 */
	void reset();

	/**
	 * @brief Load options from persistent QSettings
	 *
	 * Reads the "ccOptions" group from QSettings and populates
	 * all members. Missing keys use their default values.
	 */
	void fromPersistentSettings();

	/**
	 * @brief Save options to persistent QSettings
	 *
	 * Writes all current member values to the "ccOptions" group
	 * in QSettings. Call after Set() to persist changes.
	 */
	void toPersistentSettings() const;

  public:
	/**
	 * @brief Get read-only access to the singleton instance
	 *
	 * @return Reference to the current options
	 */
	static const ccOptions& Instance()
	{
		return InstanceNonConst();
	}

	/**
	 * @brief Release the singleton instance
	 *
	 * Deletes the singleton. Called during application shutdown.
	 */
	static void ReleaseInstance();

	/**
	 * @brief Replace the singleton with new values
	 *
	 * @param[in] options New option values
	 * @param[in] saveToPersistentSettings If true, immediately persist to QSettings
	 */
	static void Set(const ccOptions& options, bool saveToPersistentSettings = false);

  protected:
	/**
	 * @brief Get mutable singleton reference
	 *
	 * Internal use only. Creates the singleton on first access.
	 *
	 * @return Mutable reference to the options
	 */
	static ccOptions& InstanceNonConst();
};
