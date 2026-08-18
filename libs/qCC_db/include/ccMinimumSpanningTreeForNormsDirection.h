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

#ifndef CC_MST_FOR_NORMS_DIRECTION_HEADER
#define CC_MST_FOR_NORMS_DIRECTION_HEADER

/**
 * @file ccMinimumSpanningTreeForNormsDirection.h
 *
 * @brief Minimum Spanning Tree for normals direction
 *
 * MST-based normal orientation algorithm.
 */

class ccPointCloud;
class ccProgressDialog;

/**
 * @class ccMinimumSpanningTreeForNormsDirection
 *
 * @brief Minimum Spanning Tree for normals direction
 *
 * Minimum Spanning Tree for normals direction resolution.
 * See http://people.maths.ox.ac.uk/wendland/research/old/reconhtml/node3.html
 */
class ccMinimumSpanningTreeForNormsDirection
{
  public:
	/**
	 * @brief Orient normals
	 * @param[in] cloud Point cloud
	 * @param[in] kNN Number of nearest neighbors
	 * @param[in] progressDlg Progress dialog
	 * @return Success
	 */
	static bool OrientNormals(ccPointCloud*     cloud,
	                          unsigned          kNN         = 6,
	                          ccProgressDialog* progressDlg = nullptr);
};

#endif // CC_MST_FOR_NORMS_DIRECTION_HEADER
