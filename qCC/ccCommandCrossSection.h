#ifndef COMMAND_CROSS_SECTION_HEADER
#define COMMAND_CROSS_SECTION_HEADER

/**
 * @file ccCommandCrossSection.h
 *
 * @brief Cross section command for the command-line interface.
 *
 * @details Command-line interface command for extracting cross sections
 * from point clouds.
 *
 * Cross sections are 2D slices through 3D point clouds, useful for:
 * - Profile analysis
 * - Sectional views
 * - Structural geology
 * - Surveying cross-sections
 *
 * The command supports:
 * - Defining section planes by point pairs
 * - Multiple section generation
 * - Section export
 *
 * @author CloudCompare project
 *
 * @see ccCommandLineInterface
 */

#include "ccCommandLineInterface.h"

class QString;
class QXmlStreamAttributes;

/**
 * @brief Command-line cross section processing command.
 *
 * @details Implements the command-line interface command for
 * generating cross sections from point clouds.
 *
 * Usage:
 * @code
 * CloudCompare -o input.bin - CROSS_SECTION -xml sections.xml
 * @endcode
 *
 * @extends ccCommandLineInterface::Command
 */
struct CommandCrossSection : public ccCommandLineInterface::Command
{
	/**
	 * @brief Constructor.
	 */
	CommandCrossSection();

	/**
	 * @brief Process the cross section command.
	 *
	 * @param[in] cmd Command line interface.
	 * @return true on success.
	 */
	bool process(ccCommandLineInterface& cmd) override;

  private:
	/**
	 * @brief Read vector from XML attributes.
	 *
	 * @param[in] attributes XML attributes.
	 * @param[out] P Output vector.
	 * @param[in] element Element name for error messages.
	 * @param[in] cmd Command line interface.
	 * @return true on success.
	 */
	bool readVector(const QXmlStreamAttributes& attributes, CCVector3& P, QString element, const ccCommandLineInterface& cmd);
};

#endif // COMMAND_CROSS_SECTION_HEADER
