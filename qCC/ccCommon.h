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

#ifndef CC_COMMON_HEADER
#define CC_COMMON_HEADER

/**
 * @file ccCommon.h
 *
 * @brief Common constants and default scalar field names for CloudCompare.
 *
 * @details This header defines standard string constants used throughout
 * CloudCompare for scalar field names, distance computation results,
 * and other shared identifiers.
 *
 * Scalar field names are used to identify specific data types attached
 * to point clouds and meshes, such as distances, curvatures, densities,
 * and other measurements computed during processing.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPointCloud
 * @see ccScalarField
 */

/**
 * @defgroup scalar_field_names Standard Scalar Field Names
 *
 * @brief Standard names for scalar field data attached to point clouds.
 *
 * @details These constants ensure consistency across the application
 * and plugins when naming scalar fields. When a scalar field is
 * created or accessed by name, these constants should be used.
 *
 * @note Scalar fields are arrays of per-point/per-vertex values stored
 *       in ccPointCloud objects. They can represent distances, curvatures,
 *       colors, or any other scalar measurement.
 *
 * @{
 */

/**
 * @brief Default scalar field name when no specific name is applicable.
 *
 * Used as a fallback when a scalar field is created without a specific
 * identifier or when the type is unknown.
 */
#define CC_DEFAULT_SF_NAME "Unknown"

/**
 * @brief Scalar field name for Cloud-to-Cloud absolute distances.
 *
 * Represents the absolute distance from each point in one cloud to
 * its nearest point in another cloud.
 */
#define CC_CLOUD2CLOUD_DISTANCES_DEFAULT_SF_NAME "C2C absolute distances"

/**
 * @brief Scalar field name for temporary approximate distances.
 *
 * Used during iterative computations where distances are temporarily
 * stored before final processing.
 */
#define CC_TEMP_APPROX_DISTANCES_DEFAULT_SF_NAME "Approx. distances"

/**
 * @brief Scalar field name for temporary distances.
 *
 * Another form of temporary distance storage during computations.
 */
#define CC_TEMP_DISTANCES_DEFAULT_SF_NAME "Temp. approx. distances"

/**
 * @brief Scalar field name for Cloud-to-Cloud approximate distances.
 *
 * An approximation of cloud-to-cloud distances, typically computed
 * more quickly than exact distances.
 */
#define CC_CLOUD2CLOUD_APPROX_DISTANCES_DEFAULT_SF_NAME "C2C approx. distances"

/**
 * @brief Scalar field name for Cloud-to-Mesh absolute distances.
 *
 * Represents the absolute distance from each point to the nearest
 * triangle in a mesh.
 */
#define CC_CLOUD2MESH_DISTANCES_DEFAULT_SF_NAME "C2M absolute distances"

/**
 * @brief Scalar field name for Cloud-to-Mesh signed distances.
 *
 * Similar to C2M distances but with sign information indicating
 * which side of the mesh surface the point is on.
 */
#define CC_CLOUD2MESH_SIGNED_DISTANCES_DEFAULT_SF_NAME "C2M signed distances"

/**
 * @brief Scalar field name for Cloud-to-Mesh approximate distances.
 *
 * An approximation of cloud-to-mesh distances.
 */
#define CC_CLOUD2MESH_APPROX_DISTANCES_DEFAULT_SF_NAME "C2M approx. distances"

/**
 * @brief Scalar field name for Cloud-to-Primitive absolute distances.
 *
 * Distance from each point to the nearest geometric primitive
 * (plane, sphere, cylinder, etc.).
 */
#define CC_CLOUD2PRIMITIVE_DISTANCES_DEFAULT_SF_NAME "C2Prim absolute distances"

/**
 * @brief Scalar field name for Cloud-to-Primitive signed distances.
 *
 * Signed distance to the nearest geometric primitive.
 */
#define CC_CLOUD2PRIMITIVE_SIGNED_DISTANCES_DEFAULT_SF_NAME "C2Prim signed distances"

/**
 * @brief Scalar field name for Chi2 distances.
 *
 * Chi-squared distance metric used in statistical comparisons.
 */
#define CC_CHI2_DISTANCES_DEFAULT_SF_NAME "Chi2 distances"

/**
 * @brief Scalar field name for connected component labels.
 *
 * Integer labels identifying which connected component each point
 * belongs to after segmentation.
 */
#define CC_CONNECTED_COMPONENTS_DEFAULT_LABEL_NAME "CC labels"

/**
 * @brief Scalar field name for local k-nearest-neighbor density.
 *
 * The number of neighbors within a local neighborhood (k-NN density).
 */
#define CC_LOCAL_KNN_DENSITY_FIELD_NAME "Number of neighbors"

/**
 * @brief Scalar field name for surface density.
 *
 * Density measurement normalized by surface area.
 */
#define CC_LOCAL_SURF_DENSITY_FIELD_NAME "Surface density"

/**
 * @brief Scalar field name for volume density.
 *
 * Density measurement normalized by volume.
 */
#define CC_LOCAL_VOL_DENSITY_FIELD_NAME "Volume density"

/**
 * @brief Scalar field name for roughness measurement.
 *
 * Surface roughness at each point, typically computed from
 * local neighborhood geometry.
 */
#define CC_ROUGHNESS_FIELD_NAME "Roughness"

/**
 * @brief Scalar field name for first-order moment.
 *
 * First-order statistical moment of local geometry.
 */
#define CC_MOMENT_ORDER1_FIELD_NAME "1st order moment"

/**
 * @brief Scalar field name for Gaussian curvature.
 *
 * Gaussian curvature (product of principal curvatures) at each point.
 */
#define CC_CURVATURE_GAUSSIAN_FIELD_NAME "Gaussian curvature"

/**
 * @brief Scalar field name for mean curvature.
 *
 * Mean curvature (average of principal curvatures) at each point.
 */
#define CC_CURVATURE_MEAN_FIELD_NAME "Mean curvature"

/**
 * @brief Scalar field name for normal change rate.
 *
 * Rate of change of surface normals in the local neighborhood.
 */
#define CC_CURVATURE_NORM_CHANGE_RATE_FIELD_NAME "Normal change rate"

/**
 * @brief Scalar field name for gradient norms.
 *
 * Magnitude of the scalar field gradient at each point.
 */
#define CC_GRADIENT_NORMS_FIELD_NAME "Gradient norms"

/**
 * @brief Scalar field name for geodesic distances.
 *
 * Distance along the surface mesh from a reference point.
 */
#define CC_GEODESIC_DISTANCES_FIELD_NAME "Geodesic distances"

/**
 * @brief Scalar field name for scattering angles in radians.
 *
 * Scattering angle measurement in radians.
 */
#define CC_DEFAULT_RAD_SCATTERING_ANGLES_SF_NAME "Scattering angles (rad)"

/**
 * @brief Scalar field name for scattering angles in degrees.
 *
 * Scattering angle measurement in degrees.
 */
#define CC_DEFAULT_DEG_SCATTERING_ANGLES_SF_NAME "Scattering angles (deg)"

/**
 * @brief Scalar field name for range values.
 *
 * Range measurements, typically from laser scanning.
 */
#define CC_DEFAULT_RANGES_SF_NAME "Ranges"

/**
 * @brief Scalar field name for squared range values.
 *
 * Range measurements squared, useful for certain computations.
 */
#define CC_DEFAULT_SQUARED_RANGES_SF_NAME "Ranges (squared)"

/**
 * @brief Scalar field name for dip angle in degrees.
 *
 * Geological dip angle measurement in degrees.
 */
#define CC_DEFAULT_DIP_SF_NAME "Dip (degrees)"

/**
 * @brief Scalar field name for dip direction in degrees.
 *
 * Geological dip direction measurement in degrees.
 */
#define CC_DEFAULT_DIP_DIR_SF_NAME "Dip direction (degrees)"

/**
 * @brief Scalar field name for mesh vertex flags.
 *
 * Integer flags indicating vertex type or properties.
 */
#define CC_DEFAULT_MESH_VERT_FLAGS_SF_NAME "Vertex type"

/**
 * @brief Scalar field name for entity ID.
 *
 * Unique identifier for each entity in a comparison or analysis.
 */
#define CC_DEFAULT_ID_SF_NAME "Id"

/**
 * @brief Scalar field name for original cloud index.
 *
 * Index linking points back to their original point cloud when
 * multiple clouds have been merged.
 */
#define CC_ORIGINAL_CLOUD_INDEX_SF_NAME "Original cloud index"

/** @} */ // end of scalar_field_names

#endif // CC_COMMON_HEADER
