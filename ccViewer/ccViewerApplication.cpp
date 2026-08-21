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
 * @file ccViewerApplication.cpp
 *
 * @brief ccViewer application entry point
 *
 * The ccViewer application is a lightweight read-only point cloud
 * and mesh viewer built on the CloudCompare core libraries.
 *
 * ## Differences from qCC
 *
 * - No editing capabilities (read-only)
 * - Separate plugin path (plugins are loaded from ccViewer's
 *   own plugin directory, not qCC's)
 * - Simpler menu: open, view modes, screenshot, exit
 * - Faster startup for quick visualization
 *
 * @see main.cpp
 */
#include <QtGlobal>

#ifdef Q_OS_MAC
#include <QFileOpenEvent>
#endif

#include "ccViewerApplication.h"
#include "ccviewer.h"

ccViewerApplication::ccViewerApplication(int& argc, char** argv, bool isCommandLine)
    : ccApplicationBase(argc, argv, isCommandLine, QString("1.42.beta (%1)").arg(__DATE__))
{
	setApplicationName("CloudCompareViewer");
}

void ccViewerApplication::setViewer(ccViewer* inViewer)
{
	mViewer = inViewer;
}

bool ccViewerApplication::event(QEvent* inEvent)
{
#ifdef Q_OS_MAC
	switch (inEvent->type())
	{
	case QEvent::FileOpen:
	{
		if (mViewer == nullptr)
		{
			return false;
		}

		mViewer->addToDB({static_cast<QFileOpenEvent*>(inEvent)->file()});
		return true;
	}

	default:
		break;
	}
#endif

	return ccApplicationBase::event(inEvent);
}
