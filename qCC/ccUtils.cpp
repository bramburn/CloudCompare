// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

/**
 * @file ccUtils.cpp
 *
 * @brief Implementation of CloudCompare utility functions
 *
 * @see ccUtils
 */

#include "ccUtils.h"

#include "ccConsole.h"

// Qt
#include <QApplication>
#include <QClipboard>

namespace ccUtils
{
	// ccUtils::DisplayLockedVerticesWarning
	/**
	 * @brief Display a warning about locked vertices
	 *
	 * Warns the user that mesh vertices are locked (shared) and
	 * operations should be applied directly to the vertex cloud.
	 *
	 * @param[in] meshName Name of the mesh
	 * @param[in] displayAsError Show as error (red) instead of warning (yellow)
	 */
	void DisplayLockedVerticesWarning(const QString& meshName, bool displayAsError)
	{
		QString message = QString(
		                      "Vertices of mesh '%1' are locked (they may be shared by "
		                      "multiple entities for instance).\n"
		                      "You should call this method directly on the vertices cloud.\n"
		                      "(warning: all entities depending on this cloud will be "
		                      "impacted!)")
		                      .arg(meshName);

		if (displayAsError)
			ccConsole::Error(message);
		else
			ccConsole::Warning(message);
	}

	// ccUtils::GetVectorFromClipboard
	/**
	 * @brief Read a 3D vector from the system clipboard
	 *
	 * Parses the clipboard text as a 3D vector using three fallback
	 * separator formats:
	 * 1. Space-separated: "x y z"
	 * 2. Semicolon-separated: "x;y;z"
	 * 3. Comma-separated: "x,y,z"
	 *
	 * Brackets are stripped if present: "[x y z]", "(x,y,z)", "{x;y;z}"
	 *
	 * @param[out] vector Parsed vector
	 * @param[in] sendErrors Log errors to console if true
	 * @return true if parsing succeeded
	 */
	bool GetVectorFromClipboard(CCVector3d& vector, bool sendErrors)
	{
		const QClipboard* clipboard = QApplication::clipboard();
		if (!clipboard)
		{
			if (sendErrors)
			{
				ccLog::Error("Clipboard not available");
			}
			return false;
		}

		// Strip surrounding brackets: [x y z], (x y z), {x;y;z}
		QString text = clipboard->text().trimmed();
		if (text.startsWith('[') || text.startsWith('{') || text.startsWith('('))
		{
			text = text.right(text.length() - 1);
		}
		if (text.endsWith(']') || text.endsWith('}') || text.endsWith(')'))
		{
			text = text.left(text.length() - 1);
		}

		if (text.isEmpty())
		{
			return false;
		}

		// Try space-separated first
		QStringList tokens = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
		if (tokens.size() != 3)
		{
			// Fall back to semicolon separator
			tokens = text.split(';', Qt::SkipEmptyParts);
			if (tokens.size() != 3)
			{
				// Fall back to comma separator
				tokens = text.split(',', Qt::SkipEmptyParts);
				if (tokens.size() != 3)
				{
					// Truncate long strings for the error message
					if (text.length() > 64)
					{
						text.truncate(61);
						text += "...";
					}
					if (sendErrors)
					{
						ccLog::Error("Unrecognized format: " + text);
					}
					return false;
				}
			}
		}

		// Parse each component as a double
		for (unsigned char i = 0; i < 3; ++i)
		{
			bool ok = false;
			vector.u[i] = tokens[i].toDouble(&ok);
			if (!ok)
			{
				if (sendErrors)
				{
					ccLog::Error("Invalid value: " + tokens[i]);
				}
				return false;
			}
		}

		return true;
	}

} // namespace ccUtils
