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
// #          COPYRIGHT: CloudCompare project                               //
// #                                                                        #
// ##########################################################################

/**
 * @file ccEntityAction.h
 *
 * @brief Entity actions namespace for CloudCompare.
 *
 * @details Defines actions that can be performed on entities (point clouds,
 * meshes, etc.) in CloudCompare. Each function operates on a container
 * of selected entities, typically showing a dialog for user input when needed.
 *
 * Actions are organized into categories:
 * - Color operations (set color, RGB conversion, gradient, levels)
 * - Scalar field operations (filter, convert, split, arithmetic)
 * - Normal operations (compute, invert, orient)
 * - Octree operations (compute)
 * - Property operations (clear, toggle)
 * - Statistical operations (tests, parameters)
 *
 * @author CloudCompare project
 *
 * @see ccPointCloud
 * @see ccMesh
 * @see ccHObject
 */

// qCC_db
#include <ccMesh.h>
#include <ccPointCloud.h>
#include <ccPointCloudInterpolator.h>

class QWidget;

class ccMainAppInterface;

/**
 * @brief Entity actions namespace.
 *
 * @details Contains static utility functions for performing various
 * operations on selected entities. Each function typically:
 * - Takes a container of selected entities
 * - Optionally shows a dialog for parameters
 * - Performs the operation on applicable entities
 * - Returns true on success, false on failure
 */
namespace ccEntityAction
{
	// ###############################
	// #      COLOR OPERATIONS       #
	// ###############################

	/**
	 * @brief Set or clear color on entities.
	 *
	 * @param[in] selectedEntities Entities to operate on.
	 * @param[in] colorize If true, show dialog to pick a color.
	 * @param[in] parent Parent widget for dialogs.
	 * @return true if successful.
	 */
	bool setColor(ccHObject::Container selectedEntities, bool colorize, QWidget* parent = nullptr);

	/**
	 * @brief Convert RGB colors to greyscale.
	 *
	 * @param[in] selectedEntities Point clouds with RGB colors.
	 * @return true if successful.
	 */
	bool rgbToGreyScale(ccHObject::Container selectedEntities);

	/**
	 * @brief Apply a color gradient to selected entities.
	 *
	 * @param[in] selectedEntities Entities with scalar fields.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool setColorGradient(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Change color levels (brightness, contrast, gamma).
	 *
	 * @param[in] selectedEntities Entities with colors.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool changeColorLevels(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Interpolate colors from another entity.
	 *
	 * @param[in] selectedEntities Entities to receive colors.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool interpolateColors(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Convert mesh texture to vertex colors.
	 *
	 * @param[in] selectedEntities Meshes with textures.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool convertTextureToColor(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Enhance RGB colors using intensities.
	 *
	 * @param[in] selectedEntities Point clouds with RGB and intensity.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool enhanceRGBWithIntensities(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Apply Gaussian filter to RGB colors.
	 *
	 * @param[in] selectedEntities Point clouds with RGB.
	 * @param[in] filterParams Filter parameters (size, sigma).
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool rgbGaussianFilter(ccHObject::Container selectedEntities, ccPointCloud::RgbFilterOptions filterParams, QWidget* parent = nullptr);

	// ###############################
	// #    SCALAR FIELD OPERATIONS  #
	// ###############################

	/**
	 * @brief Apply Gaussian filter to scalar field.
	 *
	 * @param[in] selectedEntities Point clouds with scalar fields.
	 * @param[in] filterParams Filter parameters.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool sfGaussianFilter(ccHObject::Container selectedEntities, ccPointCloud::RgbFilterOptions filterParams, QWidget* parent = nullptr);

	/**
	 * @brief Convert scalar field to RGB colors.
	 *
	 * @param[in] selectedEntities Point clouds with scalar fields.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool sfConvertToRGB(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Convert scalar field to random RGB colors.
	 *
	 * @param[in] selectedEntities Point clouds with scalar fields.
	 * @return true if successful.
	 */
	bool sfConvertToRandomRGB(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Rename a scalar field.
	 *
	 * @param[in] selectedEntities Point clouds.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool sfRename(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Add an ID field to scalar fields.
	 *
	 * @param[in] selectedEntities Point clouds.
	 * @param[in] storeAsInt If true, store as integer; otherwise float.
	 * @return true if successful.
	 */
	bool sfAddIdField(ccHObject::Container selectedEntities, bool storeAsInt = false);

	/**
	 * @brief Split a cloud based on scalar field values.
	 *
	 * @param[in] selectedEntities Point clouds.
	 * @param[in] app Application interface for adding results to DB.
	 * @return true if successful.
	 */
	bool sfSplitCloud(ccHObject::Container selectedEntities, ccMainAppInterface* app);

	/**
	 * @brief Set scalar field values as coordinates.
	 *
	 * @param[in] entity Single entity to operate on.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool sfSetAsCoord(ccHObject* entity, QWidget* parent = nullptr);

	/**
	 * @brief Set scalar field values as coordinates.
	 *
	 * @param[in] selectedEntities Point clouds.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool sfSetAsCoord(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Export coordinates to scalar fields.
	 *
	 * @param[in] selectedEntities Point clouds.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool exportCoordToSF(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Set scalar fields as normal vectors.
	 *
	 * @param[in] entity Single entity to operate on.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool setSFsAsNormal(ccHObject* entity, QWidget* parent = nullptr);

	/**
	 * @brief Export normals to scalar fields.
	 *
	 * @param[in] selectedEntities Entities with normals.
	 * @param[in] parent Parent widget for dialog.
	 * @param[out] exportDimensions Which dimensions to export.
	 * @return true if successful.
	 */
	bool exportNormalToSF(ccHObject::Container selectedEntities, QWidget* parent = nullptr, bool* exportDimensions = nullptr);

	/**
	 * @brief Perform scalar field arithmetic.
	 *
	 * @param[in] selectedEntities Point clouds with scalar fields.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool sfArithmetic(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Generate scalar field from color.
	 *
	 * @param[in] selectedEntities Point clouds with RGB colors.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool sfFromColor(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Generate scalar fields from color components.
	 *
	 * @param[in] selectedEntities Point clouds with RGB colors.
	 * @param[in] exportR Export red channel.
	 * @param[in] exportG Export green channel.
	 * @param[in] exportB Export blue channel.
	 * @param[in] exportAlpha Export alpha channel.
	 * @param[in] exportComposite Export composite RGB value.
	 * @return true if successful.
	 */
	bool sfFromColor(ccHObject::Container selectedEntities, bool exportR, bool exportG, bool exportB, bool exportAlpha, bool exportComposite);

	/**
	 * @brief Interpolate scalar fields between entities.
	 *
	 * @param[in] selectedEntities Entities to interpolate.
	 * @param[in] parent Application interface.
	 * @return true if successful.
	 */
	bool interpolateSFs(ccHObject::Container selectedEntities, ccMainAppInterface* parent);

	/**
	 * @brief Interpolate a scalar field between two clouds.
	 *
	 * @param[in] source Source point cloud.
	 * @param[in] dst Destination point cloud.
	 * @param[in] sfIndex Scalar field index to interpolate.
	 * @param[in] params Interpolation parameters.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool interpolateSFs(ccPointCloud* source, ccPointCloud* dst, int sfIndex, ccPointCloudInterpolator::Parameters& params, QWidget* parent = nullptr);

	/**
	 * @brief Add a constant scalar field.
	 *
	 * @param[in] cloud Point cloud to add scalar field to.
	 * @param[in] sfName Name for the scalar field.
	 * @param[in] integerValue If true, values are integers.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool sfAddConstant(ccPointCloud* cloud, QString sfName, bool integerValue, QWidget* parent = nullptr);

	/**
	 * @brief Process mesh scalar field.
	 *
	 * @param[in] selectedEntities Meshes with scalar fields.
	 * @param[in] process Processing operation to perform.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool processMeshSF(ccHObject::Container selectedEntities, ccMesh::MESH_SCALAR_FIELD_PROCESS process, QWidget* parent = nullptr);

	// ###############################
	// #      NORMAL OPERATIONS      #
	// ###############################

	/**
	 * @brief Compute normals for entities.
	 *
	 * @param[in] selectedEntities Entities to compute normals for.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool computeNormals(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Invert normal directions.
	 *
	 * @param[in] selectedEntities Entities with normals.
	 * @return true if successful.
	 */
	bool invertNormals(ccHObject::Container selectedEntities);

	/**
	 * @brief Orient normals using Fast Marching.
	 *
	 * @param[in] selectedEntities Point clouds with normals.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool orientNormalsFM(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Orient normals using Minimum Spanning Tree.
	 *
	 * @param[in] selectedEntities Point clouds with normals.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool orientNormalsMST(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Destination formats for normal conversion.
	 */
	enum class NORMAL_CONVERSION_DEST
	{
		HSV_COLORS,  //!< Convert to HSV colors
		DIP_DIR_SFS  //!< Convert to dip/direction scalar fields
	};

	/**
	 * @brief Convert normals to another representation.
	 *
	 * @param[in] selectedEntities Entities with normals.
	 * @param[in] dest Conversion destination format.
	 * @return true if successful.
	 */
	bool convertNormalsTo(ccHObject::Container selectedEntities, NORMAL_CONVERSION_DEST dest);

	// ###############################
	// #      OCTREE OPERATIONS       #
	// ###############################

	/**
	 * @brief Compute octrees for point clouds.
	 *
	 * @param[in] selectedEntities Point clouds.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool computeOctree(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	// ###############################
	// #     PROPERTY OPERATIONS      #
	// ###############################

	/**
	 * @brief Properties that can be cleared.
	 */
	enum class CLEAR_PROPERTY
	{
		COLORS,             //!< Clear all colors
		NORMALS,           //!< Clear all normals
		CURRENT_SCALAR_FIELD,  //!< Clear current scalar field
		ALL_SCALAR_FIELDS  //!< Clear all scalar fields
	};

	/**
	 * @brief Clear a property from entities.
	 *
	 * @param[in] selectedEntities Entities to modify.
	 * @param[in] property Property to clear.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool clearProperty(ccHObject::Container selectedEntities, CLEAR_PROPERTY property, QWidget* parent = nullptr);

	/**
	 * @brief Properties that can be toggled.
	 */
	enum class TOGGLE_PROPERTY
	{
		ACTIVE,        //!< Enable/disable entity
		VISIBLE,      //!< Show/hide entity
		COLOR,        //!< Show/hide color
		NORMALS,      //!< Show/hide normals
		SCALAR_FIELD, //!< Show/hide scalar field
		MATERIAL,     //!< Show/hide material
		NAME          //!< Show/hide 3D name
	};

	/**
	 * @brief Toggle a property on entities.
	 *
	 * @param[in] selectedEntities Entities to modify.
	 * @param[in] property Property to toggle.
	 * @return true if successful.
	 */
	bool toggleProperty(ccHObject::Container selectedEntities, TOGGLE_PROPERTY property);

	// ###############################
	// #     STATISTICAL OPERATIONS   #
	// ###############################

	/**
	 * @brief Perform statistical test on scalar field.
	 *
	 * @param[in] selectedEntities Point clouds with scalar fields.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool statisticalTest(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

	/**
	 * @brief Compute statistical parameters.
	 *
	 * @param[in] selectedEntities Point clouds with scalar fields.
	 * @param[in] parent Parent widget for dialog.
	 * @return true if successful.
	 */
	bool computeStatParams(ccHObject::Container selectedEntities, QWidget* parent = nullptr);

} // namespace ccEntityAction
