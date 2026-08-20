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
// #                                                                        //
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        //
// ##########################################################################

/**
 * @file ccViewerApplication.h
 *
 * @brief ccViewer Qt application class.
 *
 * @details Application class for the ccViewer lightweight viewer.
 *
 * Manages:
 * - Qt event processing
 * - Application-wide settings
 * - Viewer instance lifecycle
 *
 * @extends ccApplicationBase
 *
 * @see ccViewer for the main window
 */

#ifndef CCVIEWERAPPLICATION_H
#define CCVIEWERAPPLICATION_H

// Common
#include <ccApplicationBase.h>

class ccViewer;

/**
 * @brief Application class for ccViewer.
 *
 * @details Qt application subclass for the ccViewer
 * lightweight point cloud viewer.
 *
 * Handles:
 * - Application initialization
 * - Event processing
 * - Viewer instance management
 *
 * @extends ccApplicationBase
 */
class ccViewerApplication : public ccApplicationBase
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the application.
	 *
	 * @param[in] argc Argument count.
	 * @param[in] argv Argument vector.
	 * @param[in] isCommandLine CLI mode flag.
	 */
	ccViewerApplication(int& argc, char** argv, bool isCommandLine);

	/**
	 * @brief Set the viewer instance.
	 *
	 * @param[in] inViewer Viewer instance.
	 */
	void setViewer(ccViewer* inViewer);

  protected:
	/**
	 * @brief Handle Qt events.
	 *
	 * @param[in] inEvent Event to process.
	 *
	 * @return true if event was handled.
	 */
	bool event(QEvent* inEvent) override;

  private:
	//! Viewer instance
	ccViewer* mViewer;
};

#endif // CCVIEWERAPPLICATION_H
