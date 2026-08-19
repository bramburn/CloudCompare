//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: qHelloCloud                        #
//#                                                                        #
//##########################################################################

#include "ActionHello.h"

#include "ccMainAppInterface.h"

#include <QMessageBox>

namespace HelloCloud
{
	void performActionHello( ccMainAppInterface *appInterface )
	{
		if ( appInterface == nullptr )
		{
			Q_ASSERT( false );
			return;
		}

		const QString message = QStringLiteral( "[qHelloCloud] Hello world from the experimental plugin scaffold." );

		// Log to the CC console (no popup).
		appInterface->dispToConsole( message, ccMainAppInterface::STD_CONSOLE_MESSAGE );

		// Brief popup so the user can see the action fired even with the
		// console collapsed. This is the only "UI" in this plugin.
		QMessageBox::information( nullptr, QStringLiteral( "qHelloCloud" ), message );
	}
}
