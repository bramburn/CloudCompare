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

/**
 * @file ccOctreeProxy.cpp
 *
 * @brief Octree proxy implementation
 *
 * Implements getOwnBB() and drawMeOnly() for the octree proxy.
 *
 * @see ccOctreeProxy.h
 */

#include "ccOctreeProxy.h"

// ccOctreeProxy::ccOctreeProxy
/**
 * @brief Construct an octree proxy
 *
 * The proxy starts hidden (not displayed by default). Call setVisible(true)
 * to show the octree bounding box in the 3D view.
 *
 * @param[in] octree Octree to wrap (may be null)
 * @param[in] name DB tree display name
 */
ccOctreeProxy::ccOctreeProxy(ccOctree::Shared octree /*=ccOctree::Shared(nullptr)*/,
                             QString          name /*="Octree"*/)
    : ccHObject(name)
    , m_octree(octree)
{
	setVisible(false);
	lockVisibility(false);
}

// ccOctreeProxy::getOwnBB
/**
 * @brief Get the octree bounding box
 *
 * @param[in] withGLFeatures true = include GL features (returns square BB), false = point BB only
 * @return Octree bounding box
 */
ccBBox ccOctreeProxy::getOwnBB(bool withGLFeatures /*=false*/)
{
	if (!m_octree)
	{
		assert(false);
		return ccBBox();
	}

	return withGLFeatures ? m_octree->getSquareBB() : m_octree->getPointsBB();
}

// ccOctreeProxy::drawMeOnly
/**
 * @brief Draw the octree visualization
 *
 * Draws the octree structure in the GL window. Handles:
 * - Entity picking mode (color-based selection highlighting)
 * - Fast entity picking (no detailed drawing during box select)
 * - OpenGL 2.1 functions context
 * - Delegation to DgmOctree::draw()
 *
 * @param[in] context GL draw context
 */
void ccOctreeProxy::drawMeOnly(CC_DRAW_CONTEXT& context)
{
	if (!m_octree)
	{
		assert(false);
		return;
	}

	if (!MACRO_Draw3D(context))
		return;

	// get the set of OpenGL functions (version 2.1)
	QOpenGLFunctions_2_1* glFunc = context.glFunctions<QOpenGLFunctions_2_1>();
	assert(glFunc != nullptr);
	if (glFunc == nullptr)
		return;

	// entity picking mode
	bool         entityPickingMode = MACRO_EntityPicking(context);
	ccColor::Rgb pickingColor;
	if (entityPickingMode)
	{
		if (MACRO_FastEntityPicking(context))
			return; // no detailed draw during fast picking

		pickingColor = context.entityPicking.registerEntity(this);
	}

	m_octree->draw(context, entityPickingMode ? &pickingColor : nullptr);
}
