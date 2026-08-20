/**
 * @file ccPlanarEntityInterface.h
 *
 * @brief Planar entity interface for entities with planar geometry.
 *
 * @details Interface for entities that lie on a plane.
 *
 * ## Overview
 *
 * Provides a common interface for entities with planar geometry:
 * - Access to plane normal vector
 * - Normal visualization toggle
 * - OpenGL normal rendering
 *
 * ## Implementations
 *
 * - **ccPlane**: Infinite plane primitive
 * - **ccFacet**: Planar surface extracted from point cloud
 * - **ccPolyline**: Planar polyline
 *
 * ## Usage
 *
 * @code
 * class MyPlanarEntity : public ccHObject, public ccPlanarEntityInterface {
 * public:
 *     CCVector3 getNormal() const override {
 *         return m_normal;
 *     }
 * };
 *
 * // Show/hide normal
 * entity->showNormalVector(true);
 * if (entity->normalVectorIsShown()) {
 *     // Normal is visible
 * }
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPlane for plane primitive
 * @see ccFacet for planar surface
 */

#ifndef PLANAR_ENTITY_INTERFACE_HEADER
#define PLANAR_ENTITY_INTERFACE_HEADER

// CCCoreLib
#include <CCGeom.h>

// qCC_gl
#include <ccGLDrawContext.h>

/**
 * @brief Planar entity interface.
 *
 * @details Interface for entities with a planar geometry.
 */
class ccPlanarEntityInterface
{
  public:
	/**
	 * @brief Default constructor.
	 */
	ccPlanarEntityInterface();

	/**
	 * @brief Show/hide normal vector.
	 *
	 * @param[in] state Show state.
	 */
	inline void showNormalVector(bool state)
	{
		m_showNormalVector = state;
	}

	/**
	 * @brief Check if normal is shown.
	 *
	 * @return true if shown.
	 */
	inline bool normalVectorIsShown() const
	{
		return m_showNormalVector;
	}

	/**
	 * @brief Get entity normal.
	 *
	 * @return Normal vector.
	 */
	virtual CCVector3 getNormal() const = 0;

	/**
	 * @brief Destructor.
	 */
	virtual ~ccPlanarEntityInterface() = default;

  protected: // methods
	/**
	 * @brief Draw normal vector (OpenGL).
	 *
	 * @param[in] context Drawing context.
	 * @param[in] pos Position to draw from.
	 * @param[in] scale Normal vector scale.
	 * @param[in] color Optional color override.
	 */
	void glDrawNormal(CC_DRAW_CONTEXT& context, const CCVector3& pos, float scale, const ccColor::Rgb* color = 0);

  protected: // members
	//! Show normal vector flag.
	bool m_showNormalVector;
};

#endif // PLANAR_ENTITY_INTERFACE_HEADER
