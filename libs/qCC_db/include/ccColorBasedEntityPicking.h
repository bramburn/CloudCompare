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
// #                      COPYRIGHT: CloudCompare project                   #
// #                                                                        #
// ##########################################################################

#include "ccColorTypes.h"

// Qt
#include <QMap>

class ccHObject;

/**
 * @file ccColorBasedEntityPicking.h
 *
 * @brief Color-based entity picking
 *
 * RGB color-based entity picking mechanism.
 *
 * @author CloudCompare project
 */

/**
 * @brief Color-based entity picker
 *
 * Maps entities to RGB colors for picking.
 */
class ccColorBasedEntityPicking
{
  public:
	/// Unique ID type (24-bit RGB colors)
	typedef uint32_t ID_TYPE;

	/**
	 * @brief Default constructor
	 */
	ccColorBasedEntityPicking()
	    : lastID(0)
	{
	}

	/// Reset picker
	void reset()
	{
		entities.clear();
		ids.clear();
		lastID = 0;
	}

	/**
	 * @brief Convert ID to color
	 * @param[in] id Entity ID
	 * @return RGB color
	 */
	static inline ccColor::Rgb IDToColor(ID_TYPE id)
	{
		assert(id < (1 << 24));
		return ccColor::Rgb(static_cast<unsigned char>(id & 255),
		                    static_cast<unsigned char>((id >> 8) & 255),
		                    static_cast<unsigned char>((id >> 16) & 255));
	}

	/**
	 * @brief Convert color to ID
	 * @param[in] col RGB color
	 * @return Entity ID
	 */
	static inline ID_TYPE ColorToID(const ccColor::Rgb& col)
	{
		return (static_cast<ID_TYPE>(col.r))
		       | (static_cast<ID_TYPE>(col.g) << 8)
		       | (static_cast<ID_TYPE>(col.b) << 16);
	}

	/**
	 * @brief Register entity
	 * @param[in] obj Entity to register
	 * @return Assigned color
	 */
	ccColor::Rgb registerEntity(ccHObject* obj)
	{
		if (ids.contains(obj))
		{
			return IDToColor(ids[obj]);
		}
		entities[++lastID] = obj;
		ids[obj] = lastID;
		return IDToColor(lastID);
	}

	//! Returns the entity corresponding to a given color
	inline ccHObject* objectFromColor(ccColor::Rgb color) const
	{
		return entities[ColorToID(color)];
	}

	//! Returns the last generated ID
	inline ID_TYPE getLastID() const
	{
		return lastID;
	}

  protected:
	//! ID/object association map
	QMap<ID_TYPE, ccHObject*> entities;

	//! object/ID association map
	QMap<ccHObject*, ID_TYPE> ids;

	//! Biggest ID value used during the last picking/rendering process
	ID_TYPE lastID;
};
