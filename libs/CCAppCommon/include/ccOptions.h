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
// #                 COPYRIGHT: Daniel Girardeau-Montaut                    #
// #                                                                        #
// ##########################################################################

/**
 * @file ccOptions.h
 *
 * @brief Application options/settings
 *
 * Global application settings persisted to disk.
 *
 * @author Daniel Girardeau-Montaut
 */

#include "CCAppCommon.h"

// Qt
#include <QString>

/**
 * @brief Application options singleton
 *
 * Global settings for the application.
 */
class CCAPPCOMMON_LIB_API ccOptions
{
  public: // parameters
	
	/// Display normals by default
	bool normalsDisplayedByDefault;

	/// Use native file dialogs
	bool useNativeDialogs;

	/// Confirm on quit
	bool confirmQuit;

	/// Confirm on delete
	bool confirmDelete;

  public: // methods
	/**
	 * @brief Create options
	 */
	ccOptions();

	/**
	 * @brief Reset to defaults
	 */
	void reset();

	/**
	 * @brief Load from persistent settings
	 */
	void fromPersistentSettings();

	/**
	 * @brief Save to persistent settings
	 */
	void toPersistentSettings() const;

  public: // static methods
	/**
	 * @brief Get options instance
	 * @return Options reference
	 */
	static const ccOptions& Instance()
	{
		return InstanceNonConst();
	}

	/**
	 * @brief Release instance
	 */
	static void ReleaseInstance();

	/**
	 * @brief Set options
	 * @param[in] options New options
	 * @param[in] saveToPersistentSettings Save to disk
	 */
	static void Set(const ccOptions& options, bool saveToPersistentSettings = false);

  protected: // methods
	/**
	 * @brief Get mutable instance
	 * @return Options reference
	 */
	static ccOptions& InstanceNonConst();
};
