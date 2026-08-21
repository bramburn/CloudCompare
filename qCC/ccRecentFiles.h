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
 * @brief Recent files manager
 *
 * Manages the "Open Recent" menu, storing up to 10 recently opened files
 * in persistent Qt settings. Automatically removes entries for files
 * that no longer exist (moved or deleted).
 *
 * Path display: paths under the user's home directory are contracted
 * to ~ notation for brevity (e.g., ~/Documents/scan.ply).
 *
 * @author CloudCompare project
 */

#include <QObject>
#include <QSettings>

class QAction;
class QMenu;
class QString;

/**
 * @brief Recent files manager
 *
 * Manages the "Open Recent" menu. Stores up to 10 most recently opened
 * files in QSettings under the "RecentFiles" key. On every menu update,
 * entries for files that no longer exist on disk are automatically removed.
 *
 * Signals: none (slots only for internal use)
 */
class ccRecentFiles : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Create the recent files manager
	 *
	 * Creates the "Open Recent..." menu and populates it from persistent
	 * settings. Registers the "Clear Menu" action.
	 *
	 * @param[in] parent Parent widget (typically the main window)
	 */
	ccRecentFiles(QWidget* parent);

	/**
	 * @brief Get the recent files menu
	 * @return Pointer to the "Open Recent..." QMenu
	 */
	QMenu* menu();

	/**
	 * @brief Add a file to the recent files list
	 *
	 * Moves the file to the front of the list (most recent position).
	 * If the file is already in the list, it is moved to the front.
	 * The list is trimmed to 10 entries. Settings are saved immediately.
	 *
	 * @param[in] filePath Absolute path to the file
	 */
	void addFilePath(const QString& filePath);

  private:
	/**
	 * @brief Rebuild the menu from current settings
	 *
	 * Clears all actions, re-reads the recent files list, creates
	 * actions for each file, and adds a separator + "Clear Menu" action.
	 * Removes non-existent files from the list before populating.
	 */
	void updateMenu();

	/**
	 * @brief Handle a menu action being triggered
	 *
	 * Loads the file associated with the triggered action into
	 * the main window via MainWindow::addToDB().
	 */
	void openFileFromAction();

	/**
	 * @brief Read the recent files list from settings
	 *
	 * Reads from QSettings, filtering out files that no longer exist.
	 *
	 * @return List of existing file paths
	 */
	QStringList listRecent();

	/**
	 * @brief Contract a file path for display
	 *
	 * Replaces the user's home directory prefix with ~.
	 * e.g., /home/user/Documents/scan.ply → ~/Documents/scan.ply
	 *
	 * @param[in] filePath Absolute path
	 * @return Contracted path (or original if not under home)
	 */
	QString contractFilePath(const QString& filePath);

	/**
	 * @brief Expand a contracted path back to absolute
	 *
	 * Replaces ~ with the user's home directory.
	 *
	 * @param[in] filePath Path with ~ prefix
	 * @return Absolute path
	 */
	QString expandFilePath(const QString& filePath);

	//! QSettings key for the recent files list
	static QString s_settingKey;

	//! Persistent settings (recent files storage)
	QSettings m_settings;

	//! The "Open Recent..." menu
	QMenu* m_menu;

	//! "Clear Menu" action
	QAction* m_actionClearMenu;
};

#endif
