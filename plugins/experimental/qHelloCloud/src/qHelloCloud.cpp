//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: qHelloCloud                        #
//#                                                                        #
//##########################################################################

#include <QtGui>

#include "qHelloCloud.h"

#include "ActionHello.h"
#include "ActionLoadCloud.h"

qHelloCloud::qHelloCloud( QObject *parent )
	: QObject( parent )
	, ccStdPluginInterface( ":/CC/plugin/qHelloCloud/info.json" )
	, m_actionHello( nullptr )
	, m_actionLoad( nullptr )
{
}

void qHelloCloud::onNewSelection( const ccHObject::Container &selectedEntities )
{
	// Both actions are always available - the load action doesn't need a
	// selection, and the hello action certainly doesn't.
	Q_UNUSED( selectedEntities );
	if ( m_actionHello )
	{
		m_actionHello->setEnabled( true );
	}
	if ( m_actionLoad )
	{
		m_actionLoad->setEnabled( true );
	}
}

QList<QAction *> qHelloCloud::getActions()
{
	if ( !m_actionHello )
	{
		m_actionHello = new QAction( QStringLiteral( "Hello world" ), this );
		m_actionHello->setToolTip( QStringLiteral( "Log a hello-world line to the CloudCompare console." ) );
		m_actionHello->setIcon( getIcon() );

		connect( m_actionHello, &QAction::triggered, this, [this]()
		{
			HelloCloud::performActionHello( m_app );
		} );
	}

	if ( !m_actionLoad )
	{
		m_actionLoad = new QAction( QStringLiteral( "Load point cloud" ), this );
		m_actionLoad->setToolTip( QStringLiteral( "Open a point cloud / mesh file and display it in the active 3D view." ) );
		m_actionLoad->setIcon( getIcon() );

		connect( m_actionLoad, &QAction::triggered, this, [this]()
		{
			HelloCloud::performActionLoadCloud( m_app );
		} );
	}

	return { m_actionHello, m_actionLoad };
}
