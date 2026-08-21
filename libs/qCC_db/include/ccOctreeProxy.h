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

#ifndef CC_OCTREE_PROXY_HEADER
#define CC_OCTREE_PROXY_HEADER

/**
 * @file ccOctreeProxy.h
 *
 * @brief Octree proxy for the DB hierarchy
 *
 * Wraps a ccOctree (from CCCoreLib) as a ccHObject child in the
 * DB tree. Allows the octree to appear as a named entity, be
 * serialized alongside its parent cloud, and be displayed/drawn.
 *
 * The actual spatial subdivision logic lives in CCCoreLib::DgmOctree.
 * This class is purely the ccHObject adapter.
 *
 * Lifecycle:
 * - Created by ccGenericPointCloud::computeOctree() or setOctree()
 * - Deleted when the parent cloud's deleteOctree() is called
 * - Stored as a child of the point cloud (DP_PARENT_OF_OTHER dependency)
 */

#include "ccHObject.h"
#include <ccOctree.h>

/**
 * @class ccOctreeProxy
 *
 * @brief Scene-graph adapter for a spatial octree
 *
 * Holds a QSharedPointer to a CCCoreLib::DgmOctree and presents it
 * as a ccHObject in the DB tree. This enables:
 * - The octree to be serialized with the cloud (same .bin file)
 * - The octree to have a name and icon in the DB tree
 * - Octree-level operations to be invoked from the hierarchy
 *
 * @extends ccHObject
 */
class QCC_DB_LIB_API ccOctreeProxy : public ccHObject
{
  public:
	/**
	 * @brief Construct an octree proxy
	 *
	 * @param[in] octree Shared octree to wrap (may be null)
	 * @param[in] name Display name in the DB tree
	 */
	ccOctreeProxy(ccOctree::Shared octree = ccOctree::Shared(nullptr), QString name = "Octree");

	/**
	 * @brief Destructor
	 */
	virtual ~ccOctreeProxy() = default;

	/**
	 * @brief Replace the wrapped octree
	 *
	 * @param[in] octree New octree (may be null)
	 */
	inline void setOctree(ccOctree::Shared octree)
	{
		m_octree = octree;
	}

	/**
	 * @brief Get the wrapped octree
	 *
	 * @return Shared pointer to the octree
	 */
	inline ccOctree::Shared getOctree() const
	{
		return m_octree;
	}

	// ccHObject

	/**
	 * @brief Returns CC_TYPES::POINT_OCTREE
	 */
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::POINT_OCTREE;
	}

	/**
	 * @brief Get bounding box from the octree
	 *
	 * Delegates to DgmOctree::getBoundingBox() if octree is present.
	 *
	 * @param[in] withGLFeatures Ignored
	 * @return Bounding box of the octree
	 */
	virtual ccBBox getOwnBB(bool withGLFeatures = false) override;

  protected:
	// ccHObject

	/**
	 * @brief Draw the octree bounding box wireframe
	 *
	 * Draws a colored wireframe box representing the octree bounds.
	 *
	 * @param[in] context GL draw context
	 */
	virtual void drawMeOnly(CC_DRAW_CONTEXT& context) override;

  protected:
	//! Wrapped octree (from CCCoreLib::DgmOctree)
	ccOctree::Shared m_octree;
};

#endif // CC_OCTREE_PROXY_HEADER
