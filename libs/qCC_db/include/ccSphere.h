// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_SPHERE_PRIMITIVE_HEADER
#define CC_SPHERE_PRIMITIVE_HEADER

/**
 * @file ccSphere.h
 *
 * @brief Sphere primitive for visualization and sphere fitting
 *
 * Represents a tessellated 3D sphere (UV sphere mesh) positioned
 * in 3D space by the transformation matrix. Used for:
 * - Creating reference geometry
 * - Fitting spheres to point clouds (sphere fitting tool)
 * - Displaying approximated circular regions
 *
 * The sphere mesh uses latitude/longitude tessellation with a configurable
 * number of angular steps (default 24). The mesh is centered at the origin
 * in local space; translation in the transformation matrix positions it.
 *
 * @extends ccGenericPrimitive
 */

#include "ccGenericPrimitive.h"

/**
 * @class ccSphere
 *
 * @brief Tessellated UV sphere primitive
 *
 * A sphere is parameterized by:
 * - Radius: spatial extent
 * - Precision: number of angular steps for tessellation (more = smoother)
 * - Transformation: position and orientation
 *
 * Mesh: UV sphere tessellation with triangular facets.
 * Normals are computed per-vertex for smooth shading.
 *
 * @extends ccGenericPrimitive
 */
class QCC_DB_LIB_API ccSphere : public ccGenericPrimitive
{
  public:
	/**
	 * @brief Create a sphere with specified radius
	 *
	 * @param[in] radius Sphere radius
	 * @param[in] transMat Optional transformation matrix
	 * @param[in] name Display name
	 * @param[in] precision Number of angular steps (default 24)
	 * @param[in] uniqueID Optional unique ID
	 */
	ccSphere(PointCoordinateType radius,
	         const ccGLMatrix* transMat = nullptr,
	         QString name = QString("Sphere"),
	         unsigned precision = 24,
	         unsigned uniqueID = ccUniqueIDGenerator::InvalidUniqueID);

	/**
	 * @brief Simplified constructor for ccHObject factory
	 *
	 * @param[in] name Display name
	 */
	explicit ccSphere(QString name = QString("Sphere"));

	// ccHObject
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::SPHERE;
	}

	// ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Sphere";
	}
	virtual bool hasDrawingPrecision() const override
	{
		return true; // Precision affects the mesh tessellation
	}
	virtual ccGenericPrimitive* clone() const override;

	/**
	 * @brief Get the sphere radius
	 */
	inline PointCoordinateType getRadius() const
	{
		return m_radius;
	}

	/**
	 * @brief Set the sphere radius
	 *
	 * @warning Triggers mesh rebuild via updateRepresentation()
	 *
	 * @param[in] radius New radius
	 */
	void setRadius(PointCoordinateType radius);

  protected:
	// ccGenericPrimitive
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool buildUp() override;

	// ccHObject
	virtual void drawNameIn3D(CC_DRAW_CONTEXT& context) override;

	//! Sphere radius
	PointCoordinateType m_radius;
};

#endif // CC_SPHERE_PRIMITIVE_HEADER
