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

#ifndef CC_OCTREE_SPIN_BOX_HEADER
#define CC_OCTREE_SPIN_BOX_HEADER

/**
 * @file ccOctreeSpinBox.h
 *
 * @brief Octree level spin box
 *
 * Spin box for selecting octree subdivision level.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "qCC_db.h"

// CCCoreLib
#include <DgmOctree.h>

// Qt
#include <QSpinBox>

class ccGenericPointCloud;

/**
 * @brief Octree level spin box
 *
 * Spin box for selecting octree subdivision level.
 */
class QCC_DB_LIB_API ccOctreeSpinBox : public QSpinBox
{
	Q_OBJECT

  public:
	/**
	 * @brief Create spin box
	 * @param[in] parent Parent widget
	 */
	explicit ccOctreeSpinBox(QWidget* parent = nullptr);

	/**
	 * @brief Set cloud
	 * @param[in] cloud Point cloud
	 */
	void setCloud(ccGenericPointCloud* cloud);

	/**
	 * @brief Set octree
	 * @param[in] octree Octree
	 */
	void setOctree(CCCoreLib::DgmOctree* octree);

  protected:
	/// Handle value change
	void onValueChange(int);

  protected:
	/// Octree box width
	double m_octreeBoxWidth;
};

#endif // CC_OCTREE_SPIN_BOX_HEADER
