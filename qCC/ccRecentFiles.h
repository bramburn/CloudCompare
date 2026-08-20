#ifndef CCRECENTFILES_H
#define CCRECENTFILES_H

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
 * @file ccRecentFiles.h
 *
 * @brief Recent files manager for CloudCompare.
 *
 * @details Manages the \"Open Recent...\" menu in CloudCompare, which displays
 * a list of recently opened files. This class handles:
 * - Persisting the list of recent files to application settings
 * - Creating and updating the recent files menu
 * - Cleaning up entries for files that no longer exist
 * - Path contraction (showing ~ for home directory)
 *
 * @author CloudCompare project
 *
 * @see MainWindow
 * @see QSettings
 */

#include <QObject>
#include <QSettings>

class QAction;
class QMenu;
class QString;

/**
 * @brief Manages the recently opened files menu.
 *
 * @details The ccRecentFiles class maintains a list of recently opened files
 * and displays them in a QMenu. It automatically:
 * - Persists the list to QSettings
 * - Removes entries for files that no longer exist
 * - Limits the list to 10 most recent files
 * - Contracts paths to show ~ for home directory
 *
 * @par Usage
 * @code
 * ccRecentFiles* recentFiles = new ccRecentFiles(this);
 * QMenu* recentMenu = recentFiles->menu();
 * menuBar()->addMenu(recentMenu);
 *
 * // After opening a file
 * recentFiles->addFilePath(filePath);
 * @endcode
 *
 * @extends QObject
 */
class ccRecentFiles : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct a recent files manager.
	 *
	 * @param[in] parent Parent widget (typically the main window).
	 *
	 * @details Creates the recent files menu and initializes it with
	 * the stored list from previous sessions.
	 */
	ccRecentFiles(QWidget* parent);

	/**
	 * @brief Get the recent files menu.
	 *
	 * @return Pointer to the QMenu containing recent file entries.
	 *
	 * @note The menu should be added to a menu bar or similar container.
	 */
	QMenu* menu();

	/**
	 * @brief Add a file to the recent files list.
	 *
	 * @param[in] filePath Absolute path to the file to add.
	 *
	 * @details Moves the file to the front of the list (most recent).
	 * If the file is already in the list, it's moved to the top.
	 * The list is automatically limited to 10 entries.
	 *
	 * @note Files that no longer exist are automatically cleaned up
	 * when the menu is next updated.
	 */
	void addFilePath(const QString& filePath);

  private:
	/**
	 * @brief Update the menu with current recent files.
	 *
	 * @details Clears and rebuilds the menu with entries from the
	 * stored settings. Automatically removes entries for files
	 * that no longer exist.
	 */
	void updateMenu();

	/**
	 * @brief Open a file triggered from the menu.
	 *
	 * @details Called when a recent file menu entry is clicked.
	 * Loads the file into CloudCompare using MainWindow::addToDB().
	 */
	void openFileFromAction();

	/**
	 * @brief Get the list of recent files from settings.
	 *
	 * @return List of recent file paths.
	 *
	 * @details Filters out files that no longer exist on disk.
	 */
	QStringList listRecent();

	/**
	 * @brief Contract a file path for display.
	 *
	 * @param[in] filePath The file path to contract.
	 * @return The contracted path (with home directory as ~).
	 *
	 * @details Replaces the home directory path with ~ for
	 * cleaner display in the menu.
	 */
	QString contractFilePath(const QString& filePath);

	/**
	 * @brief Expand a contracted file path.
	 *
	 * @param[in] filePath The contracted file path.
	 * @return The expanded absolute path.
	 *
	 * @details Replaces ~ with the actual home directory path.
	 */
	QString expandFilePath(const QString& filePath);

	/** @brief Settings key for storing recent files list. */
	static QString s_settingKey;

	/** @brief Application settings for persistence. */
	QSettings m_settings;

	/** @brief The recent files menu. */
	QMenu* m_menu;

	/** @brief Action to clear the recent files menu. */
	QAction* m_actionClearMenu;
};

#endif
