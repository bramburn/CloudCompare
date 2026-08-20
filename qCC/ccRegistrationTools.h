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
 * @brief Registration tools for ICP-based point cloud alignment.
 *
 * @details Wrapper for ICP (Iterative Closest Point) registration
 * operations in CCCoreLib.
 *
 * ICP is an iterative algorithm that finds the optimal rigid
 * transformation (rotation + translation, optionally scale) to
 * align two point clouds:
 *
 * 1. For each point in the "data" cloud, find the closest
 *    point in the "model" cloud
 * 2. Compute the transformation that minimizes the
 *    distances between point pairs
 * 3. Apply the transformation to the data cloud
 * 4. Repeat until convergence or maximum iterations
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see CCCoreLib::ICPRegistrationTools
 * @see ccPointPairRegistrationDlg
 */

// CCCoreLib
#include <RegistrationTools.h>

// qCC_db
#include <ccGLMatrix.h>

class QWidget;
class ccHObject;

/**
 * @brief Registration tools wrapper.
 *
 * @details Provides CloudCompare-specific wrappers for CCCoreLib
 * registration algorithms.
 */
class ccRegistrationTools
{

  public:
	/**
	 * @brief Apply ICP (Iterative Closest Point) registration.
	 *
	 * @param[in] data Data cloud (will be transformed).
	 * @param[in] model Reference/model cloud.
	 * @param[out] transMat Result transformation matrix.
	 * @param[out] finalScale Final scale factor (for similarity transform).
	 * @param[out] finalRMS Final root mean square error.
	 * @param[out] finalPointCount Number of matched point pairs.
	 * @param[in] inputParameters ICP parameters.
	 * @param[in] useDataSFAsWeights Use data cloud SF as weights.
	 * @param[in] useModelSFAsWeights Use model cloud SF as weights.
	 * @param[in] parent Parent widget.
	 *
	 * @return true if registration succeeded.
	 *
	 * @details Runs ICP registration to align the data cloud
	 * to the model cloud. The transformation is returned in
	 * transMat and should be applied to the data cloud.
	 *
	 * ICP Parameters control:
	 * - Maximum iterations
	 * - Convergence threshold (RMS change)
	 * - Min/max point distances
	 * - Transformation model (rigid, similarity, etc.)
	 *
	 * @note The caller should apply the transformation to
	 * the data cloud after successful registration.
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
