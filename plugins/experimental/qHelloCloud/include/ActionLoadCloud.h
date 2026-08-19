//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: qHelloCloud                        #
//#                                                                        #
//##########################################################################

#pragma once

class ccMainAppInterface;

namespace HelloCloud
{
	//! Pops a QFileDialog, loads the selected file via the standard CC
	//! filter chain, adds the result to the DB tree, and zooms the active
	//! 3D view to fit it. No other UI.
	void performActionLoadCloud( ccMainAppInterface *appInterface );
}
