#pragma once

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

/**
 * @file ccViewportParameters.h
 *
 * @brief Viewport parameters for 3D views
 *
 * Stores all camera and view parameters for 3D visualization including
 * pivot point, camera center, rotation matrix, perspective/ortho settings,
 * field of view, and clipping planes.
 *
 * Two view modes:
 * - **Object-centered**: rotation is around the pivot point; camera stays fixed
 * - **Viewer-centered**: rotation is around the camera; pivot follows
 *
 * The focal distance is the distance from camera center to pivot point in
 * object-centered mode, and is used to compute zoom, pixel size, and
 * projection matrices.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "ccGLMatrix.h"
#include "ccSerializableObject.h"

// CCCoreLib
#include <CCGeom.h>

class QRect;

/**
 * @brief Viewport parameters
 *
 * Holds the complete state of a 3D viewport's camera and view configuration.
 * Serializable to/from .bin files for saving/loading view states.
 *
 * The camera model uses two centers:
 * - **cameraCenter**: position of the camera eye in world space
 * - **pivotPoint**: the point in world space that stays fixed during rotation
 *                   (what the user considers the "center" of rotation)
 *
 * The focal distance is the scalar distance between these two points in
 * object-centered perspective mode.
 *
 * @see ccGLWindow, ccGLMatrixd, ccSerializableObject
 */
class QCC_DB_LIB_API ccViewportParameters : public ccSerializableObject
{
  public:
	/**
	 * @brief Default constructor
	 *
	 * Initializes to a centered orthographic view:
	 * - viewMat = identity
	 * - objectCenteredView = true
	 * - perspectiveView = false
	 * - focalDistance = 1.0
	 * - cameraCenter = (0, 0, 1.0)
	 * - pivotPoint = (0, 0, 0)
	 * - fov_deg = 50.0
	 * - cameraAspectRatio = 1.0
	 */
	ccViewportParameters();

	/**
	 * @brief Copy constructor
	 * @param[in] params Source parameters to copy
	 */
	ccViewportParameters(const ccViewportParameters& params);

	// from ccSerializableObject
	bool isSerializable() const override
	{
		return true;
	}

	/**
	 * @brief Serialize to binary file
	 *
	 * Saves: viewMat, focalDistance, defaultPointSize, defaultLineWidth,
	 * perspectiveView, objectCenteredView, pivotPoint, cameraCenter, fov_deg,
	 * cameraAspectRatio, nearClippingDepth, farClippingDepth.
	 *
	 * @param[in] out        Output file
	 * @param[in] dataVersion File format version
	 * @return true on success
	 */
	bool toFile(QFile& out, short dataVersion) const override;

	/**
	 * @brief Deserialize from binary file
	 *
	 * Handles backward compatibility with older file versions:
	 * - dataVersion < 36: camera matrix stored as float (converted to double)
	 * - dataVersion < 51: pixelSize/zoom used instead of focalDistance
	 * - dataVersion < 53: no clipping depth storage
	 *
	 * @param[in]  in              Input file
	 * @param[in]  dataVersion     File format version
	 * @param[in]  flags           Deserialization flags
	 * @param[out] oldToNewIDMap   ID remapping table
	 * @return true on success
	 */
	bool fromFile(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;

	/**
	 * @brief Minimum file version needed to store this state
	 *
	 * Returns 53 if clipping depths are non-NaN (requires version 53),
	 * otherwise 51 (version 51 if both are NaN).
	 *
	 * @return Minimum compatible file version
	 */
	short minimumFileVersion() const override;

	/**
	 * @brief Set the rotation pivot point
	 *
	 * In object-centered mode, optionally updates the focal distance to
	 * preserve the visual zoom (distance from camera to pivot stays constant).
	 *
	 * @param[in] P              New pivot point in world coordinates
	 * @param[in] autoUpdateFocal If true and in object-centered mode,
	 *                             update focalDistance to maintain visual zoom
	 */
	void setPivotPoint(const CCVector3d& P, bool autoUpdateFocal);

	/**
	 * @brief Get the pivot point
	 * @return Current pivot point in world coordinates
	 */
	const CCVector3d& getPivotPoint() const
	{
		return pivotPoint;
	}

	/**
	 * @brief Set the camera center position
	 *
	 * In object-centered mode, optionally updates the focal distance.
	 *
	 * @param[in] C              New camera center in world coordinates
	 * @param[in] autoUpdateFocal If true and in object-centered mode,
	 *                             update focalDistance to maintain visual zoom
	 */
	void setCameraCenter(const CCVector3d& C, bool autoUpdateFocal);

	/**
	 * @brief Get the camera center
	 * @return Current camera center in world coordinates
	 */
	const CCVector3d& getCameraCenter() const
	{
		return cameraCenter;
	}

	/**
	 * @brief Set the focal distance
	 *
	 * In object-centered mode, automatically adjusts cameraCenter.z to
	 * maintain cameraCenter.z = pivotPoint.z + focalDistance.
	 *
	 * @param[in] distance New focal distance (must be positive)
	 */
	void setFocalDistance(double distance);

	/**
	 * @brief Get the focal distance
	 * @return Current focal distance
	 */
	double getFocalDistance() const
	{
		return focalDistance;
	}

	/**
	 * @brief Compute the OpenGL view matrix
	 *
	 * Returns the 4x4 view matrix used for gluLookAt-style rendering.
	 * The matrix is constructed as:
	 * 1. Translate to rotation center (pivot in object-centered, cameraCenter otherwise)
	 * 2. Apply rotation from viewMat
	 * 3. Translate back and shift by cameraCenter
	 *
	 * @return 4x4 view matrix as ccGLMatrixd
	 */
	ccGLMatrixd computeViewMatrix() const;

	/**
	 * @brief Compute the scale matrix for aspect ratio correction
	 *
	 * Returns a diagonal matrix that corrects the aspect ratio distortion
	 * in the GL projection. When ar < 1.0, applies a uniform (ar, ar, 1.0)
	 * scale to compensate for the taller-than-wide viewport.
	 *
	 * @param[in] glViewport Current GL viewport rectangle
	 * @return 4x4 scale matrix
	 */
	ccGLMatrixd computeScaleMatrix(const QRect& glViewport) const;

	/**
	 * @brief Get the view direction
	 *
	 * Returns the direction the camera is looking (normal to the screen,
	 * pointing into the scene). Computed as the negated 3rd row of viewMat.
	 *
	 * @return Normalized view direction vector
	 */
	CCVector3d getViewDir() const;

	/**
	 * @brief Get the screen-up direction
	 *
	 * Returns the direction that corresponds to "up" on the screen.
	 * Computed as the 2nd row of viewMat.
	 *
	 * @return Normalized up direction vector
	 */
	CCVector3d getUpDir() const;

	/**
	/**
	 * @brief Get the rotation center
	 *
	 * Returns the point around which view rotation occurs:
	 * - pivotPoint in object-centered view mode
	 * - cameraCenter in viewer-centered view mode
	 *
	 * @return Rotation center in world coordinates
	 */
	const CCVector3d& getRotationCenter() const;

	/**
	 * @brief Compute distance-to-half-width ratio
	 *
	 * Returns tan(fov / 2). Used for perspective projection depth scaling.
	 *
	 * @return half-width / distance ratio
	 */
	double computeDistanceToHalfWidthRatio() const;

	/**
	 * @brief Compute distance-to-width ratio for current aspect ratio
	 *
	 * Returns 2 * tan(fov / 2) / min(ar, 1.0), where ar is the adjusted
	 * aspect ratio (screenWidth / (screenHeight * cameraAspectRatio)).
	 *
	 * @param[in] screenWidth  Viewport width in pixels
	 * @param[in] screenHeight Viewport height in pixels
	 * @return width / distance ratio
	 */
	double computeDistanceToWidthRatio(int screenWidth, int screenHeight) const;

	/**
	 * @brief Compute the world-space width visible at the focal distance
	 *
	 * @param[in] screenWidth  Viewport width in pixels
	 * @param[in] screenHeight Viewport height in pixels
	 * @return Visible width at focal distance
	 */
	double computeWidthAtFocalDist(int screenWidth, int screenHeight) const;

	/**
	 * @brief Compute the size of one pixel in world space
	 *
	 * @param[in] screenWidth  Viewport width in pixels
	 * @param[in] screenHeight Viewport height in pixels
	 * @return World-space size of one pixel at the focal distance
	 */
	double computePixelSize(int screenWidth, int screenHeight) const;

	/**
	 * @brief Log all viewport parameters to the console
	 */
	void log() const;

  public: // variables — intentionally public for performance
	//! View rotation matrix (pure rotation, no translation)
	ccGLMatrixd viewMat;

	//! Default point size for rendering
	float defaultPointSize;
	//! Default line width for rendering
	float defaultLineWidth;

	//! true for perspective projection, false for orthographic
	bool perspectiveView;
	//! true = rotation around pivot point (object-centered); false = rotation around camera (viewer-centered)
	/** Always true for orthographic mode. */
	bool objectCenteredView;

	//! Relative zNear position (multiplied by focal distance)
	double zNearCoef;

	//! Depth of the near clipping plane relative to scene (NaN = auto)
	double nearClippingDepth;
	//! Depth of the far clipping plane relative to scene (NaN = auto)
	double farClippingDepth;

	//! Actual near clipping plane z value (computed from zNearCoef and focalDistance)
	double zNear;
	//! Actual far clipping plane z value
	double zFar;

	//! Field of view in degrees (perspective mode only)
	float fov_deg;
	//! Camera aspect ratio (width / height)
	float cameraAspectRatio;

  protected:
	//! Distance from camera center to pivot point (Z component difference in object-centered mode)
	double focalDistance;

	//! World-space point around which rotation occurs (object-centered mode)
	CCVector3d pivotPoint;

	//! World-space position of the camera eye
	CCVector3d cameraCenter;

  public:
	/**
	 * @brief Direct write access to cameraCenter (for tests)
	 *
	 * Bypasses setCameraCenter() side-effects. Needed by test code that
	 * directly constructs viewport parameters from raw struct data.
	 *
	 * @return Reference to cameraCenter
	 */
	CCVector3d& cameraCenterDirect()
	{
		return cameraCenter;
	}

	/**
	 * @brief Direct write access to focalDistance (for tests)
	 *
	 * Bypasses setFocalDistance() side-effects. Needed by test code that
	 * directly constructs viewport parameters from raw struct data.
	 *
	 * @return Reference to focalDistance
	 */
	double& focalDistanceDirect()
	{
		return focalDistance;
	}
};
