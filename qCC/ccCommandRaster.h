#ifndef COMMAND_LINE_RASTER_HEADER
#define COMMAND_LINE_RASTER_HEADER

/**
 * @file ccCommandRaster.h
 *
 * @brief Raster command-line interface commands.
 *
 * @details Command-line interface commands for raster processing:
 * - **CommandRasterize**: Convert point clouds to raster grids
 * - **CommandVolume25D**: Compute 2.5D volumes between surfaces
 *
 * These commands enable batch processing of raster operations
 * from the command line without the GUI.
 *
 * @author CloudCompare project
 *
 * @see ccCommandLineInterface
 */

#include "ccCommandLineInterface.h"

/**
 * @brief Rasterize command.
 *
 * @details Converts point clouds to raster grids via CLI.
 *
 * Usage:
 * @code
 * CloudCompare -o cloud.bin - RASTERIZE -param value [...]
 * @endcode
 *
 * @extends ccCommandLineInterface::Command
 */
struct CommandRasterize : public ccCommandLineInterface::Command
{
	/**
	 * @brief Constructor.
	 */
	CommandRasterize();

	/**
	 * @brief Process the rasterize command.
	 *
	 * @param[in] cmd Command line interface.
	 * @return true on success.
	 */
	bool process(ccCommandLineInterface& cmd) override;
};

/**
 * @brief 2.5D volume command.
 *
 * @details Computes the volume between two surfaces or a surface
 * and a reference plane.
 *
 * Usage:
 * @code
 * CloudCompare -o surface1.bin -o surface2.bin - VOLUME_25D
 * @endcode
 *
 * @extends ccCommandLineInterface::Command
 */
struct CommandVolume25D : public ccCommandLineInterface::Command
{
	/**
	 * @brief Constructor.
	 */
	CommandVolume25D();

	/**
	 * @brief Process the volume command.
	 *
	 * @param[in] cmd Command line interface.
	 * @return true on success.
	 */
	bool process(ccCommandLineInterface& cmd) override;
};

#endif // COMMAND_LINE_RASTER_HEADER
