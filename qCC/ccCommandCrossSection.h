#ifndef COMMAND_CROSS_SECTION_HEADER
#define COMMAND_CROSS_SECTION_HEADER

/**
 * @file ccCommandCrossSection.h
 *
 * @brief Cross section command
 *
 * Command-line cross section command.
 *
 * @author CloudCompare project
 */

#include "ccCommandLineInterface.h"

class QString;
class QXmlStreamAttributes;

/**
 * @brief Cross section command
 *
 * Process cross section from command line.
 */
struct CommandCrossSection : public ccCommandLineInterface::Command
{
	/// Constructor
	CommandCrossSection();

	/// Process command
	bool process(ccCommandLineInterface& cmd) override;

  private:
	bool readVector(const QXmlStreamAttributes& attributes, CCVector3& P, QString element, const ccCommandLineInterface& cmd);
};

#endif // COMMAND_CROSS_SECTION_HEADER
