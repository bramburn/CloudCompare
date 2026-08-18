// ##########################################################################
// #                                                                        #
// #                       CLOUDCOMPARE PLUGIN: qEDL                        #
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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef Q_EDL_PLUGIN_HEADER
#define Q_EDL_PLUGIN_HEADER

/**
 * @file qEDL.h
 *
 * @brief Eye Dome Lighting plugin
 *
 * EDL (Eye Dome Lighting) shader plugin.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "ccGLPluginInterface.h"

/**
 * @brief EDL shader plugin
 *
 * Eye Dome Lighting shader for enhanced depth perception.
 */
class qEDL : public QObject
    , public ccGLPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccGLPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qEDL" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qEDL(QObject* parent = nullptr);

	/// Destructor
	~qEDL() override = default;

	/// Get EDL filter
	ccGlFilter* getFilter() override;
};

#endif
