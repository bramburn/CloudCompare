#ifndef BUNDLERCOMMAND_H
#define BUNDLERCOMMAND_H

// ##########################################################################
// #                                                                        #
// #                      CLOUDCOMPARE PLUGIN                               #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.     #
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
 * @file BundlerCommand.h
 *
 * @brief Bundler command
 *
 * Command line interface command for Bundler import.
 *
 * @author CloudCompare project
 */

#include "ccCommandLineInterface.h"

/**
 * @brief Bundler command
 *
 * CLI command for importing Bundler files.
 */
class BundlerCommand : public ccCommandLineInterface::Command
{
  public:
	/// Constructor
	BundlerCommand();

	/// Destructor
	virtual ~BundlerCommand()
	{
	}

	/// Process command
	virtual bool process(ccCommandLineInterface& cmd) override;
};

#endif
