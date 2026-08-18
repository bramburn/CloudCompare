#ifndef COMMAND_LINE_RASTER_HEADER
#define COMMAND_LINE_RASTER_HEADER

/**
 * @file ccCommandRaster.h
 *
 * @brief Raster commands
 *
 * Command line raster processing commands.
 *
 * @author CloudCompare project
 */

#include "ccCommandLineInterface.h"

/// Rasterize command
struct CommandRasterize : public ccCommandLineInterface::Command
{
	/// Constructor
	CommandRasterize();

	/// Process command
	bool process(ccCommandLineInterface& cmd) override;
};

/// 2.5D volume command
struct CommandVolume25D : public ccCommandLineInterface::Command
{
	/// Constructor
	CommandVolume25D();

	/// Process command
	bool process(ccCommandLineInterface& cmd) override;
};

#endif // COMMAND_LINE_RASTER_HEADER
