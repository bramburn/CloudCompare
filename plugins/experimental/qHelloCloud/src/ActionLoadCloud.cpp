//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: qHelloCloud                        #
//#                                                                        #
//##########################################################################

#include "ActionLoadCloud.h"

#include "ccHObject.h"
#include "ccMainAppInterface.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QString>

namespace qHelloCloud
{
	void performActionLoadCloud( ccMainAppInterface *appInterface )
	{
		if ( appInterface == nullptr )
		{
			Q_ASSERT( false );
			return;
		}

		// Permissive filter - the actual decoding is handled by whichever
		// FileIOFilter the standard CC load chain picks based on the file
		// extension / magic bytes. We just need to give the user a sensible
		// default place to look.
		const QString filter = QStringLiteral(
			"Point cloud / mesh (*.ply *.bin *.las *.laz *.obj *.e57 *.vtk *.stl *.pcd);;"
			"All files (*.*)"
		);

		const QString filename = QFileDialog::getOpenFileName(
			nullptr,
			QStringLiteral( "Open point cloud" ),
			QString(),
			filter
		);

		if ( filename.isEmpty() )
		{
			// User cancelled - nothing to do.
			return;
		}

		// Standard load path: goes through the same FileIOFilter chain as
		// File -> Open. Returns nullptr if no filter claims the file.
		ccHObject* entity = appInterface->loadFile( filename, /*silent=*/false );
		if ( entity == nullptr )
		{
			appInterface->dispToConsole(
				QStringLiteral( "[qHelloCloud] loadFile() returned null for: %1" ).arg( filename ),
				ccMainAppInterface::ERR_CONSOLE_MESSAGE
			);
			return;
		}

		// Add to the main DB tree. updateZoom=true reframes the active 3D
		// view around the new entity; autoExpandDBTree=true expands the
		// tree so the new node is visible; autoRedraw=true triggers the
		// redraw (no need to call refreshAll() manually afterwards).
		appInterface->addToDB(
			entity,
			/*updateZoom=*/       true,
			/*autoExpandDBTree=*/ true,
			/*checkDimensions=*/  false,
			/*autoRedraw=*/       true
		);

		appInterface->dispToConsole(
			QStringLiteral( "[qHelloCloud] Loaded and displayed: %1" ).arg( QFileInfo( filename ).fileName() ),
			ccMainAppInterface::STD_CONSOLE_MESSAGE
		);
	}
}
