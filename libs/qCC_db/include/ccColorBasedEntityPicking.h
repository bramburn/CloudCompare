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
// #                      COPYRIGHT: CloudCompare project                   #
// #                                                                        //
// ##########################################################################

/**
 * @file ccColorBasedEntityPicking.h
 *
 * @brief RGB color-based entity picking mechanism.
 *
 * @details Maps entities to unique RGB colors for picking in OpenGL.
 *
 * ## Overview
 *
 * Color-based picking uses unique colors as identifiers:
 * - Each entity gets a unique RGB color
 * - Render entities with their pick colors
 * - Read pixel color under cursor
 * - Look up entity from color
 *
 * ## Color Encoding
 *
 * 24-bit RGB color encodes entity ID:
 * - R: bits 0-7
 * - G: bits 8-15
 * - B: bits 16-23
 *
 * Maximum ~16 million unique IDs.
 *
 * ## Usage
 *
 * @code
 * // Create picker
 * ccColorBasedEntityPicking picker;
 *
 * // Register entities
 * for (auto* entity : entities) {
 *     ccColor::Rgb color = picker.registerEntity(entity);
 *     entity->setColor(color);
 * }
 *
 * // Render for picking
 * glClear(GL_COLOR_BUFFER_BIT);
 * for (auto* entity : entities) {
 *     ccColor::Rgb color = picker.ColorToID(entity->getColor());
 *     glColor3ub(color.r, color.g, color.b);
 *     entity->draw();
 * }
 *
 * // Get picked entity
 * GLint pixel[4];
 * glReadPixels(mouseX, mouseY, 1, 1, GL_RGBA, GL_INT, pixel);
 * ccColor::Rgb color(pixel[0], pixel[1], pixel[2]);
 * ccHObject* picked = picker.objectFromColor(color);
 * @endcode
 *
 * @author CloudCompare project
 */

#pragma once

#include "ccColorTypes.h"

// Qt
#include <QMap>

class ccHObject;

/**
 * @brief Color-based entity picker.
 *
 * @details Maps entities to RGB colors for OpenGL picking.
 *
 * This technique works by:
 * 1. Assigning unique RGB colors to entities
 * 2. Rendering entities with their pick colors
 * 3. Reading pixel color under cursor
 * 4. Looking up entity from color
 */
class ccColorBasedEntityPicking
{
  public:
	/**
	 * @brief Unique ID type (24-bit RGB).
	 */
	typedef uint32_t ID_TYPE;

	/**
	 * @brief Default constructor.
	 */
	ccColorBasedEntityPicking()
	    : lastID(0)
	{
	}

	/**
	 * @brief Reset the picker.
	 *
	 * Clears all registered entities.
	 */
	void reset()
	{
		entities.clear();
		ids.clear();
		lastID = 0;
	}

	/**
	 * @brief Convert ID to RGB color.
	 *
	 * @param[in] id Entity ID.
	 *
	 * @return RGB color encoding the ID.
	 *
	 * @note ID must be < 2^24.
	 */
	static inline ccColor::Rgb IDToColor(ID_TYPE id)
	{
		assert(id < (1 << 24));
		return ccColor::Rgb(static_cast<unsigned char>(id & 255),
		                    static_cast<unsigned char>((id >> 8) & 255),
		                    static_cast<unsigned char>((id >> 16) & 255));
	}

	/**
	 * @brief Convert RGB color to ID.
	 *
	 * @param[in] col RGB color.
	 *
	 * @return Entity ID.
	 */
	static inline ID_TYPE ColorToID(const ccColor::Rgb& col)
	{
		return (static_cast<ID_TYPE>(col.r))
		       | (static_cast<ID_TYPE>(col.g) << 8)
		       | (static_cast<ID_TYPE>(col.b) << 16);
	}

	/**
	 * @brief Register an entity.
	 *
	 * @param[in] obj Entity to register.
	 *
	 * @return Assigned RGB color.
	 *
	 * @note Returns existing color if already registered.
	 */
	ccColor::Rgb registerEntity(ccHObject* obj)
	{
		if (ids.contains(obj))
		{
			return IDToColor(ids[obj]);
		}
		entities[++lastID] = obj;
		ids[obj]           = lastID;
		return IDToColor(lastID);
	}

	/**
	 * @brief Get entity from color.
	 *
	 * @param[in] color RGB color.
	 *
	 * @return Entity, or nullptr if not found.
	 */
	inline ccHObject* objectFromColor(ccColor::Rgb color) const
	{
		return entities[ColorToID(color)];
	}

	/**
	 * @brief Get last assigned ID.
	 */
	inline ID_TYPE getLastID() const
	{
		return lastID;
	}

  private:
	//! ID to entity map.
	QMap<ID_TYPE, ccHObject*> entities;

	//! Entity to ID map.
	QMap<ccHObject*, ID_TYPE> ids;

	//! Last assigned ID.
	ID_TYPE lastID;
};
