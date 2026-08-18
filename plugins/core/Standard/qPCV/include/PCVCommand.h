#pragma once

//##########################################################################
//#                                                                        #
//#                      CLOUDCOMPARE PLUGIN                               #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                  COPYRIGHT: CloudCompare project                       #
//#                                                                        #
//##########################################################################

/**
 * @file PCVCommand.h
 *
 * @brief PCV CLI command
 *
 * Command line interface for Potentially Visible Set computation.
 */

#include "ccCommandLineInterface.h"

class ccProgressDialog;
class ccMainAppInterface;

//qCC_db
#include <ccHObject.h>

/**
 * @class PCVCommand
 *
 * @brief PCV command
 *
 * CLI command for Potentially Visible Set computation.
 */
class PCVCommand : public ccCommandLineInterface::Command
{
public:
	/// Constructor
	PCVCommand();

	/// Destructor
	~PCVCommand() override = default;

	/**
	 * @brief Process PCV
	 * @param[in] candidates Candidate objects
	 * @param[in] rays View rays
	 * @param[in] meshIsClosed Whether mesh is closed
	 * @param[in] resolution Resolution
	 * @param[in] progressDlg Progress dialog
	 * @param[in] app Main application interface
	 * @return Success
	 */
	static bool Process(	const ccHObject::Container& candidates,
						const std::vector<CCVector3d>& rays,
						bool meshIsClosed,
						unsigned resolution,
						ccProgressDialog* progressDlg = nullptr,
						ccMainAppInterface* app = nullptr);

	/// Process command
	bool process(ccCommandLineInterface& cmd) override;
};
