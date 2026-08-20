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
 * @brief Implementation of recent files manager.
 *
 * @details Implements the ccRecentFiles class for managing the recent
 * files menu in CloudCompare.
 *
 * @see ccRecentFiles.h
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

/** @brief Settings key for recent files persistence. */
QString ccRecentFiles::s_settingKey("RecentFiles");

/**
 * @brief Construct a recent files manager.
 *
 * @param[in] parent Parent widget (typically the main window).
 */
ccRecentFiles::ccRecentFiles(QWidget* parent)
    : QObject(parent)
{
	m_menu = new QMenu(tr("Open Recent..."), parent);

	m_actionClearMenu = new QAction(tr("Clear Menu"), this);

	connect(m_actionClearMenu, &QAction::triggered, this, [this]()
	        {
		m_settings.remove( s_settingKey );
		
		updateMenu(); });

	updateMenu();
}

/**
 * @brief Get the recent files menu.
 *
 * @return Pointer to the QMenu containing recent file entries.
 */
QMenu* ccRecentFiles::menu()
{
	return m_menu;
}

/**
 * @brief Add a file to the recent files list.
 *
 * @param[in] filePath Absolute path to the file to add.
 *
 * @details Moves the file to the front of the list. If the file
 * is already in the list, it's moved to the top. The list is
 * automatically limited to 10 entries.
 */
void ccRecentFiles::addFilePath(const QString& filePath)
{
	QStringList list = m_settings.value(s_settingKey).toStringList();

	list.removeAll(filePath);
	list.prepend(filePath);

	// only save the last ten files
	if (list.count() > 10)
	{
		list = list.mid(0, 10);
	}

	m_settings.setValue(s_settingKey, list);

	updateMenu();
}

/**
 * @brief Update the menu with current recent files.
 *
 * @details Clears and rebuilds the menu with entries from the
 * stored settings. Removes entries for files that no longer exist.
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

/**
 * @brief Open a file triggered from the menu.
 *
 * @details Called when a recent file menu entry is clicked.
 * Loads the file into CloudCompare using MainWindow::addToDB().
 */
void ccRecentFiles::openFileFromAction()
{
	QAction* action = qobject_cast<QAction*>(sender());

	Q_ASSERT(action);

	QString fileName = action->data().toString();

	if (!QFile::exists(fileName))
	{
		return;
	}

	QStringList fileListOfOne{fileName};

	MainWindow::TheInstance()->addToDB(fileListOfOne);
}

/**
 * @brief Get the list of recent files from settings.
 *
 * @return List of recent file paths.
 *
 * @details Filters out files that no longer exist on disk.
 */
QStringList ccRecentFiles::listRecent()
{
	QStringList list = m_settings.value(s_settingKey).toStringList();

	QStringList::iterator iter = list.begin();

	while (iter != list.end())
	{
		const QString filePath = *iter;

		if (!QFile::exists(filePath))
		{
			iter = list.erase(iter);
			continue;
		}

		++iter;
	}

	return list;
}

/**
 * @brief Contract a file path for display.
 *
 * @param[in] filePath The file path to contract.
 * @return The contracted path (with home directory as ~).
 *
 * @details Replaces the home directory path with ~ for
 * cleaner display in the menu.
 */
QString ccRecentFiles::contractFilePath(const QString& filePath)
{
	QString homePath = QDir::toNativeSeparators(QDir::homePath());
	QString newPath  = QDir::toNativeSeparators(filePath);

	if (newPath.startsWith(homePath))
	{
		return newPath.replace(0, QDir::homePath().length(), '~');
	}

	return filePath;
}

/**
 * @brief Expand a contracted file path.
 *
 * @param[in] filePath The contracted file path.
 * @return The expanded absolute path.
 *
 * @details Replaces ~ with the actual home directory path.
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
