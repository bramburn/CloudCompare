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
// ##########################################################################

#ifndef CC_GENERIC_POINT_CLOUD_HEADER
#define CC_GENERIC_POINT_CLOUD_HEADER

// Local
#include "ccAdvancedTypes.h"
#include "ccOctree.h"
#include "ccShiftedObject.h"

// System
#include <vector>

namespace CCCoreLib
{
	class GenericProgressCallback;
	class ReferenceCloud;
} // namespace CCCoreLib

class ccOctreeProxy;

/**
 * @file ccGenericPointCloud.h
 *
 * @brief Generic point cloud interface
 *
 * Base interface for all point cloud types in CloudCompare.
 * Provides geometric point access with associated features:
 * - Per-point colors (RGB or RGBA)
 * - Per-point normals (compressed or full 3D)
 * - Per-point visibility (for segmentation)
 * - Octree spatial index
 * - Scalar fields (from ccPointCloud)
 *
 * Point clouds are ccHObject subclasses, so they participate in the
 * scene graph hierarchy. The abstract interface (this class) separates
 * the cloud concept from the concrete ccPointCloud implementation.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 * @see ccPointCloud for concrete implementation
 * @see CCCoreLib::GenericIndexedCloudPersist for point access interface
 */

// Default lighting parameters for clouds with normals
#define CC_DEFAULT_CLOUD_AMBIENT_COLOR ccColor::bright
#define CC_DEFAULT_CLOUD_SPECULAR_COLOR ccColor::bright
#define CC_DEFAULT_CLOUD_DIFFUSE_COLOR ccColor::bright
#define CC_DEFAULT_CLOUD_EMISSION_COLOR ccColor::night
#define CC_DEFAULT_CLOUD_SHININESS 50.0f

/**
 * @brief Generic point cloud interface
 *
 * Abstract base class for all point cloud types. Combines:
 * - ccShiftedObject: provides global shift for large coordinates
 * - CCCoreLib::GenericIndexedCloudPersist: provides point access
 * - ccHObject: scene graph integration
 *
 * Key features:
 * - Octree spatial index (lazy-computed on demand)
 * - Per-point visibility for segmentation
 * - Color, normal, and scalar field access
 * - Point picking (brute-force or octree-accelerated)
 * - Rigid transformation and cropping
 *
 * Subclasses: ccPointCloud (the main implementation)
 */
class QCC_DB_LIB_API ccGenericPointCloud : public ccShiftedObject
    , public CCCoreLib::GenericIndexedCloudPersist
{
	friend class ccMesh;

  public:
	/**
	 * @brief Construct a generic point cloud
	 * @param[in] name     Human-readable name
	 * @param[in] uniqueID Unique ID (auto-generated if InvalidUniqueID)
	 */
	ccGenericPointCloud(QString name = QString(), unsigned uniqueID = ccUniqueIDGenerator::InvalidUniqueID);

	/**
	 * @brief Copy constructor
	 * @param[in] cloud Source cloud
	 */
	ccGenericPointCloud(const ccGenericPointCloud& cloud);

	/**
	 * @brief Destructor
	 */
	~ccGenericPointCloud() override;

	/*** Clone/Copy ***/

	/**
	 * @brief Create a deep copy of this cloud
	 *
	 * Clones all features: colors, normals, scalar fields, meta-data,
	 * and the bounding box. Does NOT clone the octree or visibility table.
	 *
	 * @param[in] destCloud     Optional pre-allocated destination of the same type
	 * @param[in] ignoreChildren If true, skip children (children are cloned by default)
	 * @return Pointer to the new cloud (caller owns the memory)
	 */
	virtual ccGenericPointCloud* clone(ccGenericPointCloud* destCloud = nullptr, bool ignoreChildren = false) = 0;

	/*** Features deletion/clearing ***/

	/**
	 * @brief Clear all data and reset display parameters
	 *
	 * Removes all points, colors, normals, scalar fields, octree,
	 * and visibility information.
	 */
	virtual void clear();

	/*** Octree management ***/

	/**
	 * @brief Compute the spatial octree index
	 *
	 * The octree bounding box is automatically set to the smallest
	 * cube that fully encloses the cloud. Previous octree (if any)
	 * is deleted before computation.
	 *
	 * @param[in] progressCb   Progress callback (may be nullptr)
	 * @param[in] autoAddChild If true, automatically add octree as a child entity
	 * @return Shared pointer to the computed octree (nullptr on failure)
	 * @warning Previous octree is deleted even if computation fails
	 */
	virtual ccOctree::Shared computeOctree(CCCoreLib::GenericProgressCallback* progressCb = nullptr, bool autoAddChild = true);

	/**
	 * @brief Get the associated octree
	 * @return Octree shared pointer, or nullptr if not computed
	 */
	virtual ccOctree::Shared getOctree() const;

	/**
	 * @brief Set the octree
	 * @param[in] octree       Octree to associate
	 * @param[in] autoAddChild If true, add as a child entity
	 */
	virtual void setOctree(ccOctree::Shared octree, bool autoAddChild = true);

	/**
	 * @brief Get the octree proxy (for GL display)
	 * @return Proxy pointer, or nullptr if no octree
	 */
	virtual ccOctreeProxy* getOctreeProxy() const;

	/**
	 * @brief Delete the octree
	 */
	virtual void deleteOctree();

	/*** Features getters ***/

	/**
	 * @brief Get the color for a scalar value
	 *
	 * Maps a scalar value to RGB using the current scalar field's
	 * color scale and display parameters.
	 *
	 * @param[in] d Scalar value to look up
	 * @return Pointer to RGB color (may be grey or null if out of range)
	 *
	 * @warning A scalar field must be active and visible
	 */
	virtual const ccColor::Rgb* getScalarValueColor(ScalarType d) const = 0;

	/**
	 * @brief Get the color for a point's scalar value
	 *
	 * @param[in] pointIndex Point index
	 * @return Pointer to RGB color (may be grey or null if out of range)
	 *
	 * @warning A scalar field must be active and visible
	 */
	virtual const ccColor::Rgb* getPointScalarValueColor(unsigned pointIndex) const = 0;

	/**
	 * @brief Get the displayed scalar distance for a point
	 *
	 * Returns the scalar field value mapped to the display range.
	 * Returns 0 if the value is outside the display range.
	 *
	 * @param[in] pointIndex Point index
	 * @return Displayed scalar distance (0 if hidden)
	 *
	 * @warning A scalar field must be active and visible
	 */
	virtual ScalarType getPointDisplayedDistance(unsigned pointIndex) const = 0;

	/**
	 * @brief Get the color for a point
	 * @param[in] pointIndex Point index
	 * @return RGBA color
	 *
	 * @warning Colors must be enabled (hasColors() == true)
	 */
	virtual const ccColor::Rgba& getPointColor(unsigned pointIndex) const = 0;

	/**
	 * @brief Get the compressed normal index for a point
	 *
	 * The compressed index maps to a direction via ccNormalVectors.
	 *
	 * @param[in] pointIndex Point index
	 * @return Compressed normal index
	 *
	 * @warning Normals must be enabled (hasNormals() == true)
	 */
	virtual const CompressedNormType& getPointNormalIndex(unsigned pointIndex) const = 0;

	/**
	 * @brief Get the full 3D normal for a point
	 * @param[in] pointIndex Point index
	 * @return Normal vector (3D)
	 *
	 * @warning Normals must be enabled (hasNormals() == true)
	 */
	virtual const CCVector3& getPointNormal(unsigned pointIndex) const = 0;

	/*** Visibility array ***/

	/**
	 * @brief Type alias for per-point visibility table
	 *
	 * Each byte stores POINT_VISIBLE, POINT_HIDDEN, or POINT_OUTDATED.
	 * If the table is not allocated, all points are considered visible.
	 */
	using VisibilityTableType = std::vector<unsigned char>;

	/**
	 * @brief Get the visibility array (mutable)
	 * @return Reference to the visibility vector
	 */
	virtual inline VisibilityTableType& getTheVisibilityArray()
	{
		return m_pointsVisibility;
	}

	/**
	 * @brief Get the visibility array (const)
	 * @return Const reference to the visibility vector
	 */
	virtual inline const VisibilityTableType& getTheVisibilityArray() const
	{
		return m_pointsVisibility;
	}

	/**
	 * @brief Get visible points as a ReferenceCloud
	 *
	 * Creates a ReferenceCloud containing only the points marked as visible.
	 *
	 * @param[in] visTable   Visibility table to use (nullptr = cloud's default)
	 * @param[in] silent     Suppress warning if no visible points
	 * @param[in] selection  Pre-allocated ReferenceCloud to fill (nullptr = create new)
	 * @return ReferenceCloud of visible points (nullptr on error or empty)
	 */
	virtual CCCoreLib::ReferenceCloud* getTheVisiblePoints(const VisibilityTableType* visTable = nullptr,
	                                                       bool silent = false,
	                                                       CCCoreLib::ReferenceCloud* selection = nullptr) const;

	/**
	 * @brief Check if the visibility table is allocated
	 * @return true if allocated, false if all points are considered visible
	 */
	virtual bool isVisibilityTableInstantiated() const;

	/**
	 * @brief Reset the visibility array to all-visible
	 *
	 * Allocates the array if not already allocated, and sets all
	 * entries to POINT_VISIBLE.
	 *
	 * @return true on success
	 */
	virtual bool resetVisibilityArray();

	/**
	 * @brief Invert the visibility array
	 *
	 * All POINT_VISIBLE become POINT_HIDDEN and vice versa.
	 */
	virtual void invertVisibilityArray();

	/**
	 * @brief Free the visibility array
	 *
	 * After calling this, all points are considered visible
	 * (visibility array is not instantiated).
	 */
	virtual void unallocateVisibilityArray();

	/*** Other methods ***/

	// from ccHObject
	/**
	 * @brief Get the bounding box
	 *
	 * @param[in] withGLFeatures Include GL-specific features in bounding box
	 * @return Bounding box (own BB only, not including children)
	 */
	ccBBox getOwnBB(bool withGLFeatures = false) override;

	/**
	 * @brief Force recomputation of the bounding box
	 */
	virtual void refreshBB() = 0;

	/**
	 * @brief Create a new cloud from visibility selection
	 *
	 * Extracts visible points into a new cloud. Optionally removes
	 * those points from this cloud.
	 *
	 * @param[in]     removeSelectedPoints       If true, remove visible points from this cloud
	 * @param[in]     visTable                   Visibility table (nullptr = default)
	 * @param[out]    newIndexesOfRemainingPoints New indices of kept points (nullptr = ignore)
	 * @param[in]     silent                     Suppress warning if nothing to keep
	 * @param[out]    selection                  ReferenceCloud of visible points (nullptr = ignore)
	 * @return New cloud with visible points, or this cloud if all visible
	 */
	virtual ccGenericPointCloud* createNewCloudFromVisibilitySelection(bool removeSelectedPoints = false,
	                                                                   VisibilityTableType* visTable = nullptr,
	                                                                   std::vector<int>* newIndexesOfRemainingPoints = nullptr,
	                                                                   bool silent = false,
	                                                                   CCCoreLib::ReferenceCloud* selection = nullptr) = 0;

	/**
	 * @brief Remove visible points from this cloud
	 *
	 * @param[in]     visTable Visibility table (nullptr = default)
	 * @param[out]    newIndexes New indices of kept points (-1 = removed)
	 * @return true on success
	 */
	virtual bool removeVisiblePoints(VisibilityTableType* visTable = nullptr,
	                                 std::vector<int>* newIndexes = nullptr) = 0;

	/**
	 * @brief Apply a rigid transformation (rotation + translation)
	 *
	 * Transforms all point coordinates and the global shift vector.
	 *
	 * @param[in] trans 4x4 transformation matrix
	 */
	virtual void applyRigidTransformation(const ccGLMatrix& trans) = 0;

	/**
	 * @brief Crop the cloud to a bounding box
	 *
	 * @param[in] box    Cropping box
	 * @param[in] inside If true, keep points inside box; if false, keep outside
	 * @return ReferenceCloud of points inside/outside the box (may be empty)
	 */
	virtual CCCoreLib::ReferenceCloud* crop(const ccBBox& box, bool inside = true) = 0;

	/**
	 * @brief Scale the cloud coordinates
	 *
	 * @param[in] fx     X scale factor
	 * @param[in] fy     Y scale factor
	 * @param[in] fz     Z scale factor
	 * @param[in] center Scaling center (default: origin)
	 *
	 * @warning Associated octree may be deleted after scaling
	 */
	virtual void scale(PointCoordinateType fx, PointCoordinateType fy, PointCoordinateType fz, CCVector3 center = CCVector3(0, 0, 0)) = 0;

	// from ccSerializableObject
	bool isSerializable() const override
	{
		return true;
	}

	/**
	 * @brief Set the OpenGL point size for rendering
	 *
	 * If 0, uses the current OpenGL point size. Otherwise,
	 * overrides it per-cloud.
	 *
	 * @param[in] size Point size in pixels
	 */
	void setPointSize(unsigned size = 0)
	{
		m_pointSize = static_cast<unsigned char>(size);
	}

	/**
	 * @brief Get the current point size
	 * @return Point size (0 = use OpenGL default)
	 */
	unsigned char getPointSize() const
	{
		return m_pointSize;
	}

	/**
	 * @brief Copy display/feature parameters from another cloud
	 *
	 * Copies: colors, normals, scalar fields, visibility table.
	 * Does NOT copy points or octree.
	 *
	 * @param[in] cloud Source cloud to copy parameters from
	 */
	void importParametersFrom(const ccGenericPointCloud* cloud);

	/**
	 * @brief Pick the nearest point to a screen click position
	 *
	 * Projects all cloud points and finds the one closest to clickPos.
	 * Can use octree acceleration if autoComputeOctree is true.
	 *
	 * @param[in]  clickPos           2D screen position (center of picking region)
	 * @param[in]  camera             GL camera parameters (projection + view matrices)
	 * @param[out] nearestPointIndex   Index of the nearest point
	 * @param[out] nearestSquareDist   Squared distance to the nearest point
	 * @param[in]  pickWidth          Picking region width in pixels
	 * @param[in]  pickHeight        Picking region height in pixels
	 * @param[in]  autoComputeOctree  If true, compute octree if missing
	 * @return true if a point was found within the picking region
	 *
	 * @note Square distance is returned to avoid the sqrt cost
	 */
	bool pointPicking(const CCVector2d& clickPos,
	                  const ccGLCameraParameters& camera,
	                  int& nearestPointIndex,
	                  double& nearestSquareDist,
	                  double pickWidth = 2.0,
	                  double pickHeight = 2.0,
	                  bool autoComputeOctree = false);

  protected:
	// from ccHObject
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;

	//! Per-point visibility table (POINT_VISIBLE / POINT_HIDDEN / POINT_OUTDATED)
	/** If not allocated, all points are implicitly visible. */
	VisibilityTableType m_pointsVisibility;

	//! Per-cloud OpenGL point size override (0 = use OpenGL default)
	unsigned char m_pointSize;
};

#endif // CC_GENERIC_POINT_CLOUD_HEADER
