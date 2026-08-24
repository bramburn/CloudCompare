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
 * @file ccRecentFiles.cpp
 *
 * @brief Recent files manager implementation
 *
 * Implements the "Open Recent" menu: persistent storage of recent files,
 * automatic cleanup of missing files, path contraction with ~, and
 * menu rebuild on file open.
 *
 * @see ccRecentFiles
 */

#include "ccRecentFiles.h"

#include "mainwindow.h"

#include <QAction>
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QSettings>
#include <QString>
#include <QStringList>

//! Settings key for the recent files list
QString ccRecentFiles::s_settingKey("RecentFiles");

// ccRecentFiles::ccRecentFiles
/**
 * @brief Construct the recent files manager
 *
 * Creates the menu and the "Clear Menu" action. The "Clear Menu" action
 * removes all entries from settings and rebuilds the menu.
 */
ccRecentFiles::ccRecentFiles(QWidget* parent)
    : QObject(parent)
{
	m_menu = new QMenu(tr("Open Recent..."), parent);

	m_actionClearMenu = new QAction(tr("Clear Menu"), this);

	connect(m_actionClearMenu, &QAction::triggered, this, [this]()
	        {
		m_settings.remove(s_settingKey);
		updateMenu(); });

	updateMenu();
}

// ccRecentFiles::menu
/**
 * @brief Get the recent files menu
 * @return Pointer to the "Open Recent..." QMenu
 */
QMenu* ccRecentFiles::menu()
{
	return m_menu;
}

// ccRecentFiles::addFilePath
/**
 * @brief Add a file to the recent files list
 *
 * Moves the file to the front (most recent). If already present,
 * removes the existing entry first (so it doesn't appear twice).
 * Trims to 10 entries max.
 *
 * @param[in] filePath Absolute path of the file to add
 */
void ccRecentFiles::addFilePath(const QString& filePath)
{
	QStringList list = m_settings.value(s_settingKey).toStringList();

	// Remove if already present (will be re-added at front)
	list.removeAll(filePath);
	// Add at front
	list.prepend(filePath);

	// Keep only the 10 most recent
	if (list.count() > 10)
	{
		list = list.mid(0, 10);
	}

	m_settings.setValue(s_settingKey, list);
	updateMenu();
}

// ccRecentFiles::updateMenu
/**
 * @brief Rebuild the recent files menu
 *
 * Clears all actions, reads the current file list (with automatic
 * removal of missing files), creates menu actions with contracted
 * paths (~ notation), and appends a separator + "Clear Menu" action.
 */
void ccRecentFiles::updateMenu()
{
	m_menu->clear();

	const QStringList recentList = listRecent();

	for (const QString& recentFile : recentList)
	{
		QAction* recentAction = new QAction(contractFilePath(recentFile), this);
		recentAction->setData(recentFile);
		connect(recentAction, &QAction::triggered, this, &ccRecentFiles::openFileFromAction);
		m_menu->addAction(recentAction);
	}

	if (!m_menu->actions().isEmpty())
	{
		m_menu->addSeparator();
		m_menu->addAction(m_actionClearMenu);
	}

	m_menu->setEnabled(!m_menu->actions().isEmpty());
}

// ccRecentFiles::openFileFromAction
/**
 * @brief Load a file when its menu action is triggered
 *
 * Extracts the file path from the action's data and passes it to
 * MainWindow::addToDB(). Silently ignores files that no longer exist.
 */
void ccRecentFiles::openFileFromAction()
{
	QAction* action = qobject_cast<QAction*>(sender());
	Q_ASSERT(action);

	QString fileName = action->data().toString();

	// Skip files that no longer exist
	if (!QFile::exists(fileName))
	{
		return;
	}

	MainWindow::TheInstance()->addToDB(QStringList{fileName});
}

// ccRecentFiles::listRecent
/**
 * @brief Read recent files from settings, filtering out missing files
 *
 * Reads the file list from QSettings and filters out any entries
 * that no longer exist on disk. Saves the cleaned list back to settings.
 *
 * @return List of existing file paths
 */
QStringList ccRecentFiles::listRecent()
{
	QStringList list = m_settings.value(s_settingKey).toStringList();

	// Filter out files that no longer exist
	auto iter = list.begin();
	while (iter != list.end())
	{
		if (!QFile::exists(*iter))
		{
			iter = list.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// Save cleaned list back to settings
	if (list.size() != m_settings.value(s_settingKey).toStringList().size())
	{
		m_settings.setValue(s_settingKey, list);
	}

	return list;
}

// ccRecentFiles::contractFilePath
/**
 * @brief Contract a path for menu display
 *
 * Replaces the user's home directory prefix with ~.
 *
 * @param[in] filePath Absolute file path
 * @return Path with home directory replaced by ~
 */
QString ccRecentFiles::contractFilePath(const QString& filePath)
{
	QString homePath = QDir::toNativeSeparators(QDir::homePath());
	QString newPath = QDir::toNativeSeparators(filePath);

	if (newPath.startsWith(homePath))
	{
		return newPath.replace(0, QDir::homePath().length(), "~");
	}

	return filePath;
}

// ccRecentFiles::expandFilePath
/**
 * @brief Expand a ~-prefixed path back to absolute
 *
 * @param[in] filePath Path starting with ~
 * @return Absolute file path
 */
QString ccRecentFiles::expandFilePath(const QString& filePath)
{
	QString newPath = QDir::toNativeSeparators(filePath);

	if (newPath.startsWith('~'))
	{
		QString homePath = QDir::toNativeSeparators(QDir::homePath());
		return newPath.replace(0, 1, homePath);
	}

	return filePath;
}
