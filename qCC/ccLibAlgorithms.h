#ifndef CCLIBALGORITHMS_H
#define CCLIBALGORITHMS_H
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
 * @file ccLibAlgorithms.h
 *
 * @brief Common algorithms for point cloud processing.
 *
 * @details Provides common algorithms used throughout CloudCompare
 * for point cloud processing tasks:
 *
 * - Geometric characteristic computation (curvature, roughness, etc.)
 * - Scalar field gradient computation
 * - Scale matching algorithms
 *
 * These are wrapper functions that interface with CCCoreLib
 * algorithms while handling CloudCompare-specific UI and progress.
 *
 * @author CloudCompare project
 *
 * @see CCCoreLib::GeometricalAnalysisTools
 */

#include "ccHObject.h"

#include <GeometricalAnalysisTools.h>

class QWidget;

class ccGenericPointCloud;
class ccProgressDialog;

/**
 * @brief Library algorithms namespace.
 *
 * @details Contains common point cloud processing algorithms.
 */
namespace ccLibAlgorithms
{
	/**
	 * @brief Get default kernel size for a single cloud.
	 *
	 * @param[in] cloud Point cloud.
	 * @param[in] knn Number of neighbors (default 12).
	 * @return Default kernel size based on cloud characteristics.
	 */
	double GetDefaultCloudKernelSize(ccGenericPointCloud* cloud, unsigned knn = 12);

	/**
	 * @brief Get default kernel size for multiple clouds.
	 *
	 * @param[in] entities Container of point clouds.
	 * @param[in] knn Number of neighbors (default 12).
	 * @return Default kernel size based on cloud characteristics.
	 */
	double GetDefaultCloudKernelSize(const ccHObject::Container& entities, unsigned knn = 12);

	/*** CCCoreLib "standalone" algorithms ***/

	/**
	 * @brief Geometric characteristic with sub-option.
	 */
	struct GeomCharacteristic
	{
		/**
		 * @brief Construct a geometric characteristic.
		 *
		 * @param[in] c Characteristic type.
		 * @param[in] option Sub-option for the characteristic.
		 */
		GeomCharacteristic(CCCoreLib::GeometricalAnalysisTools::GeomCharacteristic c, int option = 0)
		    : charac(c)
		    , subOption(option)
		{
		}

		//! Characteristic type
		CCCoreLib::GeometricalAnalysisTools::GeomCharacteristic charac;

		//! Sub-option
		int subOption = 0;
	};

	//! Set of geometric characteristics
	typedef std::vector<GeomCharacteristic> GeomCharacteristicSet;

	/**
	 * @brief Compute multiple geometric characteristics.
	 *
	 * @param[in] characteristics Characteristics to compute.
	 * @param[in] radius Neighborhood radius.
	 * @param[in,out] entities Entities to process.
	 * @param[in] roughnessUpDir Roughness computation up direction.
	 * @param[in] parent Parent widget.
	 * @return true on success.
	 */
	bool ComputeGeomCharacteristics(const GeomCharacteristicSet& characteristics,
	                                PointCoordinateType          radius,
	                                ccHObject::Container&      entities,
	                                const CCVector3*           roughnessUpDir = nullptr,
	                                QWidget*                   parent         = nullptr);

	/**
	 * @brief Compute a single geometric characteristic.
	 *
	 * @param[in] algo Characteristic to compute.
	 * @param[in] subOption Sub-option for the characteristic.
	 * @param[in] radius Neighborhood radius.
	 * @param[in,out] entities Entities to process.
	 * @param[in] roughnessUpDir Roughness computation up direction.
	 * @param[in] parent Parent widget.
	 * @param[in] progressDialog Progress dialog.
	 * @return true on success.
	 */
	bool ComputeGeomCharacteristic(CCCoreLib::GeometricalAnalysisTools::GeomCharacteristic algo,
	                               int                                                     subOption,
	                               PointCoordinateType                                     radius,
	                               ccHObject::Container&                                  entities,
	                               const CCVector3*                                       roughnessUpDir = nullptr,
	                               QWidget*                                               parent         = nullptr,
	                               ccProgressDialog*                                      progressDialog = nullptr);

	/**
	 * @brief CCCoreLib algorithms handled by ApplyCCLibAlgorithm.
	 */
	enum CC_LIB_ALGORITHM
	{
		CCLIB_ALGO_SF_GRADIENT, //!< Scalar field gradient
	};

	/**
	 * @brief Apply a CCCoreLib algorithm.
	 *
	 * @param[in] algo Algorithm to apply.
	 * @param[in,out] entities Entities to process.
	 * @param[in] parent Parent widget.
	 * @param[in] additionalParameters Algorithm-specific parameters.
	 * @return true on success.
	 */
	bool ApplyCCLibAlgorithm(CC_LIB_ALGORITHM      algo,
	                         ccHObject::Container& entities,
	                         QWidget*              parent               = nullptr,
	                         void**                additionalParameters = nullptr);

	/**
	 * @brief Scale matching algorithms.
	 */
	enum ScaleMatchingAlgorithm
	{
		BB_MAX_DIM,  //!< Match using bounding box max dimension
		BB_VOLUME,   //!< Match using bounding box volume
		PCA_MAX_DIM, //!< Match using PCA max dimension
		ICP_SCALE    //!< Match using ICP scale computation
	};

	/**
	 * @brief Apply a scale matching algorithm.
	 *
	 * @param[in] algo Scale matching algorithm.
	 * @param[in,out] entities Entities to match.
	 * @param[in] icpRmsDiff ICP RMS difference threshold.
	 * @param[in] icpFinalOverlap ICP final overlap.
	 * @param[in] refEntityIndex Reference entity index.
	 * @param[in] parent Parent widget.
	 * @return true on success.
	 */
	bool ApplyScaleMatchingAlgorithm(ScaleMatchingAlgorithm algo,
	                                 ccHObject::Container&  entities,
	                                 double                 icpRmsDiff,
	                                 int                    icpFinalOverlap,
	                                 unsigned               refEntityIndex = 0,
	                                 QWidget*              parent         = nullptr);
} // namespace ccLibAlgorithms

#endif
