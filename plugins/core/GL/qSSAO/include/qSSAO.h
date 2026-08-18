// ##########################################################################
// #                                                                        #
// #                       CLOUDCOMPARE PLUGIN: qSSAO                       #
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

#ifndef Q_SSAO_PLUGIN_HEADER
#define Q_SSAO_PLUGIN_HEADER

/**
 * @file qSSAO.h
 *
 * @brief Screen Space Ambient Occlusion plugin
 *
 * SSAO shader plugin for ambient occlusion effects.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "ccGLPluginInterface.h"

/**
 * @brief SSAO shader plugin
 *
 * Screen Space Ambient Occlusion shader.
 */
class qSSAO : public QObject
    , public ccGLPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccGLPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qSSAO" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qSSAO(QObject* parent = nullptr);

	/// Destructor
	~qSSAO() override = default;

	/// Get SSAO filter
	ccGlFilter* getFilter() override;
};

#endif
