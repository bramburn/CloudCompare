// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccCameraSensor.h
 *
 * @brief Camera and depth sensor classes for 3D reconstruction.
 *
 * @details Represents camera and depth sensors for:
 * - Projective geometry (intrinsic/extrinsic parameters)
 * - Image-to-3D registration
 * - Depth map processing
 * - Sensor fusion
 *
 * ## Sensor Types
 *
 * ### Camera Sensors
 * - Standard cameras (perspective projection)
 * - Fisheye lenses
 * - Omnidirectional cameras
 *
 * ### Depth Sensors
 * - Kinect (Xbox 360)
 * - Kinect v2 (Xbox One)
 * - PrimeSense
 * - Generic stereo cameras
 *
 * ## Intrinsic Parameters
 *
 * Internal camera parameters:
 * - Focal length (vertical)
 * - Pixel size
 * - Principal point
 * - Field of view
 * - Distortion coefficients
 *
 * ## Extrinsic Parameters
 *
 * Camera position and orientation in world coordinates.
 *
 * ## Distortion Models
 *
 * - **NO_DISTORTION_MODEL**: No lens distortion
 * - **SIMPLE_RADIAL_DISTORTION**: k1, k2 coefficients
 * - **BROWN_DISTORTION**: Brown's model (k1, k2, k3, p1, p2)
 * - **EXTENDED_RADIAL_DISTORTION**: k1, k2, k3 coefficients
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccSensor for base sensor class
 */

#ifndef CC_CAMERA_SENSOR_HEADER
#define CC_CAMERA_SENSOR_HEADER

// local
#include "ccOctree.h"
#include "ccSensor.h"

// system
#include <unordered_set>

class ccImage;
class ccMesh;
class ccPointCloud;

class QDir;

/**
 * @brief Camera (projective) sensor.
 *
 * @details Models camera geometry including intrinsic parameters
 * (focal length, sensor size, distortion) and extrinsic parameters
 * (position, orientation).
 *
 * Supports:
 * - Perspective projection
 * - Multiple distortion models
 * - Depth map generation
 * - Image-to-3D projection
 *
 * @extends ccSensor
 */
class QCC_DB_LIB_API ccCameraSensor : public ccSensor
{
  public: // general
	/**
	 * @brief Intrinsic camera parameters.
	 *
	 * @details Defines the internal camera geometry.
	 */
	struct QCC_DB_LIB_API IntrinsicParameters
	{
		/**
		 * @brief Default constructor.
		 */
		IntrinsicParameters();

		/**
		 * @brief Initialize with Kinect defaults.
		 *
		 * @param[out] params Parameters to initialize.
		 */
		static void GetKinectDefaults(IntrinsicParameters& params);

		float vertFocal_pix;       //!< Vertical focal length (pixels)
		float pixelSize_mm[2];     //!< Pixel size (width, height) in mm
		float skew;                //!< Skew factor
		float vFOV_rad;            //!< Vertical field of view (radians)
		float zNear_mm;            //!< Near clipping plane (mm)
		float zFar_mm;             //!< Far clipping plane (mm)
		int   arrayWidth;         //!< Image width (pixels)
		int   arrayHeight;         //!< Image height (pixels)
		float principal_point[2];  //!< Principal point (cx, cy) in pixels

		/**
		 * @brief Get horizontal focal length.
		 *
		 * @return Focal length in pixels.
		 */
		inline float horizFocal_pix() const
		{
			assert(pixelSize_mm[1] > 0);
			return (vertFocal_pix * pixelSize_mm[0]) / pixelSize_mm[1];
		}
	};

	/**
	 * @brief Lens distortion model types.
	 */
	enum DistortionModel
	{
		NO_DISTORTION_MODEL        = 0, //!< No distortion
		SIMPLE_RADIAL_DISTORTION   = 1, //!< Simple radial (k1, k2)
		BROWN_DISTORTION           = 2, //!< Brown's model (k1, k2, k3, ...)
		EXTENDED_RADIAL_DISTORTION = 3  //!< Extended radial (k1, k2, k3)
	};

	//! Lens distortion parameters base class.
	struct LensDistortionParameters
	{
		//! Shared pointer type.
		using Shared = QSharedPointer<LensDistortionParameters>;

		//! Virtual destructor.
		virtual ~LensDistortionParameters() = default;

		/**
		 * @brief Get distortion model type.
		 */
		virtual DistortionModel getModel() const = 0;
	};

	//! Simple radial distortion (k1, k2).
	struct QCC_DB_LIB_API RadialDistortionParameters : LensDistortionParameters
	{
		//! Shared pointer type.
		using Shared = QSharedPointer<RadialDistortionParameters>;

		/**
		 * @brief Default constructor.
		 */
		RadialDistortionParameters()
		    : k1(0)
		    , k2(0)
		{
		}

		/**
		 * @brief Get model type.
		 */
		inline DistortionModel getModel() const override
		{
			return SIMPLE_RADIAL_DISTORTION;
		}

		float k1; //!< 1st radial coefficient.
		float k2; //!< 2nd radial coefficient.
	};

	//! Extended radial distortion (k1, k2, k3).
	struct QCC_DB_LIB_API ExtendedRadialDistortionParameters : RadialDistortionParameters
	{
		//! Shared pointer type.
		using Shared = QSharedPointer<RadialDistortionParameters>;

		/**
		 * @brief Default constructor.
		 */
		ExtendedRadialDistortionParameters()
		    : RadialDistortionParameters()
		    , k3(0)
		{
		}

		/**
		 * @brief Get model type.
		 */
		inline DistortionModel getModel() const override
		{
			return EXTENDED_RADIAL_DISTORTION;
		}

		float k3; //!< 3rd radial coefficient.
	};

	//! Brown's distortion model + linear disparity (for Kinect).
	struct QCC_DB_LIB_API BrownDistortionParameters : LensDistortionParameters
	{
		//! Shared pointer type.
		using Shared = QSharedPointer<BrownDistortionParameters>;

		/**
		 * @brief Default constructor.
		 */
		BrownDistortionParameters();

		/**
		 * @brief Get model type.
		 */
		inline DistortionModel getModel() const override
		{
			return BROWN_DISTORTION;
		}

		/**
		 * @brief Initialize with Kinect defaults.
		 */
		static void GetKinectDefaults(BrownDistortionParameters& params);

		float principalPointOffset[2];  //!< Principal point offset (meters).
		float linearDisparityParams[2]; //!< A, B for 1/Z = A*d' + B.
		float K_BrownParams[3];         //!< Radial distortion (k1, k2, k3).
		float P_BrownParams[2];         //!< Tangential distortion (p1, p2).
	};

	//! Frustum visualization information.
	struct QCC_DB_LIB_API FrustumInformation
	{
		FrustumInformation()
		    : frustum(nullptr)
		    , frustumHeight(0)
		{
		}

		ccMesh* frustum;       //!< Frustum mesh.
		float frustumHeight;    //!< Frustum height for display.
	};

	// ... (continues in implementation)

  public:
	// ... (continues in implementation)
};

#endif // CC_CAMERA_SENSOR_HEADER
