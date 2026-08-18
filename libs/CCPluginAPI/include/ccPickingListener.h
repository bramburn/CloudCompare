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
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        #
// ##########################################################################

#include "CCPluginAPI.h"

/**
 * @file ccPickingListener.h
 *
 * @brief Picking listener interface
 *
 * Interface for tools that respond to point/triangle
 * clicks in 3D views.
 *
 * @author CloudCompare project
 */
// CCCoreLib
#include <CCGeom.h>

// Qt
#include <QPoint>

class ccHObject;

/**
 * @brief Picking listener interface
 *
 * Interface for handling point/triangle picks in 3D views.
 */
class CCPLUGIN_LIB_API ccPickingListener
{
  public:
	/**
	 * @brief Destructor
	 */
	virtual ~ccPickingListener() = default;

	/**
	 * @brief Information about a picked item
	 */
	struct PickedItem
	{
		/**
		 * @brief Default constructor
		 */
		PickedItem()
		    : entity(nullptr)
		    , itemIndex(0)
		    , entityCenter(false)
		{
		}

		QPoint     clickPoint;   //!< Mouse click position
		ccHObject* entity;       //!< Picked entity
		unsigned   itemIndex;    //!< Point or triangle index
		CCVector3  P3D;          //!< 3D picked point
		CCVector3d uvw;          //!< Barycentric coords (for triangles)
		bool       entityCenter; //!< True if entity center was picked
	};

	/**
	 * @brief Handle picked item
	 * @param[in] pi Information about the pick
	 */
	virtual void onItemPicked(const PickedItem& pi) = 0;
};
