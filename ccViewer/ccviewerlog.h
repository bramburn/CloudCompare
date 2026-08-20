// ##########################################################################
// #                                                                        #
// #                   CLOUDCOMPARE LIGHT VIEWER                            #
// #                                                                        #
// #  This project has been initiated under funding from ANR/CIFRE          #
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
// #      +++ COPYRIGHT: EDF R&D + TELECOM ParisTech (ENST-TSI) +++         #
// #                                                                        //
// ##########################################################################

/**
 * @file ccviewerlog.h
 *
 * @brief Minimalist logger for ccViewer.
 *
 * @details Provides a lightweight logging implementation for the
 * ccViewer application that only displays error messages.
 *
 * Unlike the full CloudCompare console, ccViewer shows only
 * critical errors via QMessageBox for simplicity.
 *
 * @extends ccLog
 */

#ifndef CCVIEWER_LOG_HEADER
#define CCVIEWER_LOG_HEADER

// Qt
#include <QMainWindow>
#include <QMessageBox>

// qCC_db
#include <ccLog.h>

/**
 * @brief Minimalist logger for ccViewer.
 *
 * @details Only displays error messages via QMessageBox.
 *
 * This is a simplified logger compared to ccConsole used
 * in the full CloudCompare application. It provides a
 * cleaner user experience for viewing tasks.
 *
 * @extends ccLog
 */
class ccViewerLog : public ccLog
{
  public:
	/**
	 * @brief Construct the viewer log.
	 *
	 * @param[in] parentWindow Parent window for message boxes.
	 */
	explicit ccViewerLog(QMainWindow* parentWindow = 0)
	    : ccLog()
	    , m_parentWindow(parentWindow)
	{
	}

  protected:
	/**
	 * @brief Handle log message.
	 *
	 * @param[in] message Message text.
	 * @param[in] level Message level.
	 *
	 * Only displays messages at LOG_ERROR level.
	 */
	virtual void logMessage(const QString& message, int level) override
	{
		if (level & LOG_ERROR)
		{
			QMessageBox::warning(m_parentWindow, "Error", message);
		}
	}

  private:
	//! Parent window for dialogs
	QMainWindow* m_parentWindow;
};

#endif // CCVIEWER_LOG_HEADER
