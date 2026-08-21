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
 * @file ccOptions.cpp
 *
 * @brief Application-wide options implementation
 *
 * Implements the ccOptions singleton for application-wide preferences:
 * - **Display**: normals shown by default
 * - **Dialogs**: use native file dialogs
 * - **Confirmation**: quit, delete
 *
 * Stored in QSettings under the "Options" group.
 *
 * ## Singleton Access
 *
 * - ccOptions::Instance(): const reference
 * - InstanceNonConst(): non-const reference (for modification)
 * - Set(params, saveToPersistentSettings): update + optional save
 * - ReleaseInstance(): cleanup on app exit
 *
 * @see ccOptions.h
 */

#include "ccOptions.h"

// ccPluginAPI
#include <ccPersistentSettings.h>

// Qt
#include <QSettings>

// qCC_db
#include <ccSingleton.h>

//! Unique instance of ccOptions
static ccSingleton<ccOptions> s_options;

/**
 * @brief Get the non-const singleton instance
 *
 * Lazy-initializes from QSettings on first access.
 */
ccOptions& ccOptions::InstanceNonConst()
{
	if (!s_options.instance)
	{
		s_options.instance = new ccOptions();
		s_options.instance->fromPersistentSettings();
	}

	return *s_options.instance;
}

void ccOptions::ReleaseInstance()
{
	s_options.release();
}

void ccOptions::Set(const ccOptions& params, bool saveToPersistentSettings /*=false*/)
{
	ccOptions& options = InstanceNonConst();
	options            = params;

	if (saveToPersistentSettings)
	{
		options.toPersistentSettings();
	}
}

ccOptions::ccOptions()
{
	reset();
}

void ccOptions::reset()
{
	normalsDisplayedByDefault = false;
	useNativeDialogs          = true;
	confirmQuit               = true;
	confirmDelete             = true;
}

/**
 * @brief Load options from QSettings
 */
void ccOptions::fromPersistentSettings()
{
	QSettings settings;
	settings.beginGroup(ccPS::Options());
	{
		normalsDisplayedByDefault = settings.value("normalsDisplayedByDefault", false).toBool();
		useNativeDialogs          = settings.value("useNativeDialogs", true).toBool();
		confirmQuit               = settings.value("confirmQuit", true).toBool();
		confirmDelete             = settings.value("confirmDelete", true).toBool();
	}
	settings.endGroup();
}

/**
 * @brief Save options to QSettings
 */
void ccOptions::toPersistentSettings() const
{
	QSettings settings;
	settings.beginGroup(ccPS::Options());
	{
		settings.setValue("normalsDisplayedByDefault", normalsDisplayedByDefault);
		settings.setValue("useNativeDialogs", useNativeDialogs);
		settings.setValue("confirmQuit", confirmQuit);
		settings.setValue("confirmDelete", confirmDelete);
	}
	settings.endGroup();
}
