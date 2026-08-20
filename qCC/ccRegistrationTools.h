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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_REGISTRATION_TOOLS_HEADER
#define CC_REGISTRATION_TOOLS_HEADER

/**
 * @file ccRegistrationTools.h
 *
 * @brief Registration tools
 *
 * Wrapper for ICP registration tools.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// CCCoreLib
#include <RegistrationTools.h>

// qCC_db
#include <ccGLMatrix.h>

class QWidget;
class ccHObject;

/**
 * @brief Registration tools
 *
 * Wrapper for ICP registration operations.
 */
class ccRegistrationTools
{

  public:
	/**
	 * @brief Apply ICP registration
	 * @param[in] data Data entity
	 * @param[in] model Model entity
	 * @param[out] transMat Result transformation matrix
	 * @param[out] finalScale Final scale
	 * @param[out] finalRMS Final RMS
	 * @param[out] finalPointCount Final point count
	 * @param[in] inputParameters ICP parameters
	 * @param[in] useDataSFAsWeights Use data SF as weights
	 * @param[in] useModelSFAsWeights Use model SF as weights
	 * @param[in] parent Parent widget
	 * @return true if successful
	 */
	static bool ICP(ccHObject*                                         data,
	                ccHObject*                                         model,
	                ccGLMatrix&                                        transMat,
	                double&                                            finalScale,
	                double&                                            finalRMS,
	                unsigned&                                          finalPointCount,
	                const CCCoreLib::ICPRegistrationTools::Parameters& inputParameters,
	                bool                                               useDataSFAsWeights  = false,
	                bool                                               useModelSFAsWeights = false,
	                QWidget*                                           parent              = nullptr);
};

#endif // CC_REGISTRATION_TOOLS_HEADER
