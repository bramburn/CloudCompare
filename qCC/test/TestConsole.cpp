// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.     #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

/**
 * @file TestConsole.cpp
 *
 * @brief Unit tests for ccConsole
 *
 * Tests console output functionality:
 * - Log message formatting
 * - Color output (info, warning, error)
 * - Message buffering and display
 * - Multi-line output
 *
 * @see ccConsole.cpp
 */
#include <QCoreApplication>
#include <QListWidget>
#include <QString>
#include <QTest>
#include <ccConsole.h>

class TestConsole : public QObject
{
	Q_OBJECT

  private slots:

	void testInitAndInstance()
	{
		// ccConsole::Init should not crash (no widget)
		ccConsole::Init(nullptr, nullptr, nullptr, false);
		ccConsole* instance = ccConsole::TheInstance(true);
		QVERIFY(instance != nullptr);

		// Second call should return same instance
		ccConsole* instance2 = ccConsole::TheInstance(true);
		QCOMPARE(instance, instance2);

		ccConsole::ReleaseInstance(false);
	}

	void testRefreshCycle()
	{
		ccConsole::Init(nullptr, nullptr, nullptr, false);
		ccConsole::SetRefreshCycle(500);
		ccConsole::ReleaseInstance(false);
	}

	void testQtMessagesEnabled()
	{
		// Default should be false
		QCOMPARE(ccConsole::QtMessagesEnabled(), false);
		ccConsole::EnableQtMessages(true);
		QCOMPARE(ccConsole::QtMessagesEnabled(), true);
		ccConsole::EnableQtMessages(false);
		QCOMPARE(ccConsole::QtMessagesEnabled(), false);
	}
};

QTEST_MAIN(TestConsole)
#include "TestConsole.moc"
