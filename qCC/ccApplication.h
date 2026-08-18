#ifndef CCAPPLICATION_H
#define CCAPPLICATION_H

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

// Common
#include <ccApplicationBase.h>

/**
 * @file ccApplication.h
 *
 * @brief CloudCompare Qt Application class
 *
 * Extends ccApplicationBase with CloudCompare-specific application functionality.
 * Handles application-wide settings, file version compatibility, and platform-specific
 * events (e.g., macOS file open events).
 *
 * @see ccApplicationBase for base class implementation
 */

class ccApplication : public ccApplicationBase
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the CloudCompare application
	 *
	 * @param[in] argc Reference to argument count from main()
	 * @param[in] argv Array of argument strings from main()
	 * @param[in] isCommandLine True if running in command-line (non-GUI) mode
	 */
	ccApplication(int& argc, char** argv, bool isCommandLine);

	/**
	 * @brief Get minimum CloudCompare version for a file format version
	 *
	 * Returns the earliest CloudCompare version that can read files
	 * saved with a specific file format version. This is used to warn
	 * users when opening files from older versions.
	 *
	 * @param[in] fileVersion Internal file format version number
	 * @return QString describing the minimum required CloudCompare version
	 *
	 * @note File format versions are independent of CloudCompare version numbers.
	 *       A file saved with CloudCompare 2.8 may have a different fileVersion
	 *       than a file saved with CloudCompare 2.10.
	 */
	static QString GetMinCCVersionForFileVersion(short fileVersion);

  protected:
	/**
	 * @brief Handle application-level events
	 *
	 * Processes special Qt events, particularly:
	 * - QEvent::FileOpen: macOS file association handling
	 *
	 * @param[in] inEvent The event to process
	 * @return true if the event was handled, false otherwise
	 */
	bool event(QEvent* inEvent) override;
};

#endif
