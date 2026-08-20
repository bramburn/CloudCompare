// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
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
 * @file ccGBLSensor.h
 *
 * @brief Ground-based LiDAR sensor class.
 *
 * @details Represents a ground-based laser scanner with two rotation axes
 * (yaw and pitch). Handles depth maps from terrestrial LiDAR
 * surveys.
 *
 * ## Overview
 *
 * Ground-based LiDAR (GBL) sensors use two rotation axes:
 * - **Yaw**: Horizontal rotation
 * - **Pitch**: Vertical rotation
 *
 * ## Rotation Order
 *
 * - **YAW_THEN_PITCH**: Most modern scanners (Leica, Riegl, Faro)
 * - **PITCH_THEN_YAW**: Legacy scanners (old Mensi Soisic)
 *
 * ## Depth Buffer
 *
 * The sensor stores a depth map (ccDepthBuffer) representing
 * the 3D points captured from each scan position.
 *
 * ## Usage
 *
 * @code
 * // Create GBL sensor
 * ccGBLSensor* sensor = new ccGBLSensor(ccGBLSensor::YAW_THEN_PITCH);
 *
 * // Set scanning parameters
 * sensor->setYawRange(-M_PI, M_PI);
 * sensor->setPitchRange(-M_PI/4, M_PI/4);
 * sensor->setYawStep(dTheta);
 * sensor->setPitchStep(dPhi);
 *
 * // Set sensor position
 * sensor->setRigidTransformation(transformation);
 *
 * // Apply viewport
 * sensor->applyViewport(display);
 *
 * // Check point visibility
 * unsigned char vis = sensor->checkVisibility(point);
 * @endcode
 *
 * @extends ccSensor
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccSensor for base class
 * @see ccDepthBuffer for depth map storage
 */

#ifndef CC_GROUND_LIDAR_SENSOR_HEADER
#define CC_GROUND_LIDAR_SENSOR_HEADER

// Local
#include "ccDepthBuffer.h"
#include "ccSensor.h"

// CCCoreLib
#include <GenericCloud.h>

class ccPointCloud;

/**
 * @brief Ground-based LiDAR sensor.
 *
 * @details Represents a terrestrial laser scanner with two perpendicular
 * rotation axes (yaw and pitch).
 *
 * @extends ccSensor
 */
class QCC_DB_LIB_API ccGBLSensor : public ccSensor
{
  public:
	/**
	 * @brief Rotation order for the sensor axes.
	 */
	enum ROTATION_ORDER
	{
		YAW_THEN_PITCH = 0, //!< Yaw then pitch (most modern scanners).
		PITCH_THEN_YAW = 1  //!< Pitch then yaw (legacy scanners).
	};

	/**
	 * @brief Create a GBL sensor.
	 *
	 * @param[in] rotOrder Rotation order of axes.
	 */
	explicit ccGBLSensor(ROTATION_ORDER rotOrder = YAW_THEN_PITCH);

	/**
	 * @brief Copy constructor.
	 *
	 * @param[in] sensor Sensor to copy.
	 * @param[in] copyDepthBuffer Whether to copy depth buffer.
	 *
	 * @warning The depth buffer is not copied by default!
	 */
	ccGBLSensor(const ccGBLSensor& sensor, bool copyDepthBuffer = false);

	/**
	 * @brief Destructor.
	 */
	~ccGBLSensor() override = default;

	// inherited from ccHObject
	/**
	 * @brief Get class type.
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::GBL_SENSOR;
	}

	/**
	 * @brief Check if serializable.
	 */
	bool isSerializable() const override
	{
		return true;
	}

	/**
	 * @brief Get own bounding box.
	 */
	ccBBox getOwnBB(bool withGLFeatures = false) override;

	/**
	 * @brief Get fitted bounding box.
	 */
	ccBBox getOwnFitBB(ccGLMatrix& trans) override;

	// inherited from ccSensor
	/**
	 * @brief Apply viewport for rendering.
	 */
	bool applyViewport(ccGenericGLDisplay* win = nullptr) const override;

	/**
	 * @brief Check point visibility.
	 *
	 * Determines a 3D point visibility relative to the sensor FOV.
	 * Relies on the associated depth buffer.
	 *
	 * @param[in] P Point to test.
	 *
	 * @return Visibility (POINT_VISIBLE, POINT_HIDDEN, POINT_OUT_OF_RANGE, POINT_OUT_OF_FOV).
	 */
	unsigned char checkVisibility(const CCVector3& P) const override;

	/**
	 * @brief Compute angular range and max range automatically.
	 *
	 * @param[in] theCloud Cloud to compute from.
	 *
	 * @return true if successful.
	 *
	 * @warning Uses the cloud global iterator.
	 * @note Only computes if values are zero.
	 */
	bool computeAutoParameters(CCCoreLib::GenericCloud* theCloud);

	/**
	 * @brief Get error string for error code.
	 *
	 * @param[in] errorCode Error code.
	 *
	 * @return Error description.
	 */
	static QString GetErrorString(int errorCode);

  public: // setters and getters
	/**
	 * @brief Set pitch scanning limits.
	 *
	 * @param[in] minPhi Min pitch angle (radians).
	 * @param[in] maxPhi Max pitch angle (radians).
	 */
	void setPitchRange(PointCoordinateType minPhi, PointCoordinateType maxPhi);

	/**
	 * @brief Get min pitch limit.
	 *
	 * @return Min pitch (radians).
	 */
	inline PointCoordinateType getMinPitch() const
	{
		return m_phiMin;
	}

	/**
	 * @brief Get max pitch limit.
	 *
	 * @return Max pitch (radians).
	 */
	inline PointCoordinateType getMaxPitch() const
	{
		return m_phiMax;
	}

	/**
	 * @brief Set pitch step.
	 *
	 * @param[in] dPhi Pitch step (radians).
	 */
	void setPitchStep(PointCoordinateType dPhi);

	/**
	 * @brief Get pitch step.
	 *
	 * @return Pitch step (radians).
	 */
	inline PointCoordinateType getPitchStep() const
	{
		return m_deltaPhi;
	}

	/**
	 * @brief Check if pitch angles are shifted.
	 *
	 * @return true if shifted to [0, 2pi].
	 */
	bool pitchIsShifted() const
	{
		return m_pitchAnglesAreShifted;
	}

	/**
	 * @brief Set yaw scanning limits.
	 *
	 * @param[in] minTheta Min yaw angle (radians).
	 * @param[in] maxTheta Max yaw angle (radians).
	 */
	void setYawRange(PointCoordinateType minTheta, PointCoordinateType maxTheta);

	/**
	 * @brief Get min yaw limit.
	 *
	 * @return Min yaw (radians).
	 */
	inline PointCoordinateType getMinYaw() const
	{
		return m_thetaMin;
	}

	/**
	 @brief Get max yaw limit.
	 *
	 * @return Max yaw (radians).
	 */
	inline PointCoordinateType getMaxYaw() const
	{
		return m_thetaMax;
	}

	/**
	 * @brief Set yaw step.
	 *
	 * @param[in] dTheta Yaw step (radians).
	 */
	void setYawStep(PointCoordinateType dTheta);

	/**
	 * @brief Get yaw step.
	 *
	 * @return Yaw step (radians).
	 */
	inline PointCoordinateType getYawStep() const
	{
		return m_deltaTheta;
	}

	/**
	 * @brief Check if yaw angles are shifted.
	 *
	 * @return true if shifted to [0, 2pi].
	 */
	bool yawIsShifted() const
	{
		return m_yawAnglesAreShifted;
	}

	/**
	 * @brief Get sensor max range.
	 *
	 * @return Max range.
	 */
	inline PointCoordinateType getSensorRange() const
	{
		return m_sensorRange;
	}

	/**
	 * @brief Set sensor max range.
	 *
	 * @param[in] range Max range.
	 */
	inline void setSensorRange(PointCoordinateType range)
	{
		m_sensorRange = range;
	}

	/**
	 * @brief Get rotation order.
	 *
	 * @return Rotation order.
	 */
	inline ROTATION_ORDER getRotationOrder() const
	{
		return m_rotOrder;
	}

	// ... (continues in implementation)

  protected:
	// ... (continues in implementation)
};

#endif // CC_GROUND_LIDAR_SENSOR_HEADER
