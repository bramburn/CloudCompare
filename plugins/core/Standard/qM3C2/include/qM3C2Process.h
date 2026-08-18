//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qM3C2                       #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#            COPYRIGHT: UNIVERSITE EUROPEENNE DE BRETAGNE                #
//#                                                                        #
//##########################################################################

#ifndef Q_M3C2_PROCESS_HEADER
#define Q_M3C2_PROCESS_HEADER

/**
 * @file qM3C2Process.h
 *
 * @brief M3C2 process
 *
 * M3C2 computation process.
 */

#include "qM3C2Dialog.h"

class ccMainAppInterface;

/**
 * @class qM3C2Process
 *
 * @brief M3C2 process
 *
 * M3C2 computation process.
 */
class qM3C2Process
{
public:
	
	/**
	 * @brief Compute M3C2
	 * @param[in] dlg M3C2 dialog
	 * @param[out] errorMessage Error message
	 * @param[out] outputCloud Output point cloud
	 * @param[in] allowDialogs Allow dialogs
	 * @param[in] parentWidget Parent widget
	 * @param[in] app Main application interface
	 * @return Success
	 */
	static bool Compute(const qM3C2Dialog& dlg,
						QString& errorMessage,
						ccPointCloud*& outputCloud,
						bool allowDialogs,
						QWidget* parentWidget = nullptr,
						ccMainAppInterface* app = nullptr);

};

#endif //Q_M3C2_PROCESS_HEADER
