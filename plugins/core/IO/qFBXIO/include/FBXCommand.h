#ifndef FBXCOMMAND_H
#define FBXCOMMAND_H

// ##########################################################################
// #                                                                        #
// #                      CLOUDCOMPARE PLUGIN                               #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

/**
 * @file FBXCommand.h
 *
 * @brief FBX command
 *
 * Command line interface command for FBX import.
 *
 * @author CloudCompare project
 */

#include "ccCommandLineInterface.h"

/**
 * @class FBXCommand
 *
 * @brief FBX CLI command
 *
 * CLI command for importing FBX files.
 */
class FBXCommand : public ccCommandLineInterface::Command
{
  public:
	/// Constructor
	FBXCommand();

	/// Destructor
	~FBXCommand() override = default;

	/// Process command
	bool process(ccCommandLineInterface& cmd) override;
};

#endif
