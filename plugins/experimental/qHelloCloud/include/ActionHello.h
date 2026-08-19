//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: qHelloCloud                        #
//#                                                                        #
//##########################################################################

#pragma once

class ccMainAppInterface;

namespace HelloCloud
{
	//! Logs a hello-world line to the CC console and pops a brief info dialog.
	void performActionHello( ccMainAppInterface *appInterface );
}
