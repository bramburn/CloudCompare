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
 * Manages recently opened files menu.
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
 * Manages the recently opened files menu.
 */
class ccRecentFiles : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Create recent files manager
	 * @param[in] parent Parent widget
	 */
	ccRecentFiles(QWidget* parent);

	/// Get recent files menu
	QMenu* menu();

	/// Add file to recent files
	void addFilePath(const QString& filePath);

  private:
	/// Update menu
	void updateMenu();

	/// Open file from action
	void openFileFromAction();

	/// List recent files
	QStringList listRecent();

	/// Contract file path
	QString contractFilePath(const QString& filePath);

	/// Expand file path
	QString expandFilePath(const QString& filePath);

	/// Settings key
	static QString s_settingKey;

	/// Settings
	QSettings m_settings;

	/// Menu
	QMenu* m_menu;

	/// Clear menu action
	QAction* m_actionClearMenu;
};

#endif
