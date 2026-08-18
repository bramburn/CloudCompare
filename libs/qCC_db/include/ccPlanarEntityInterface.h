#ifndef PLANAR_ENTITY_INTERFACE_HEADER
#define PLANAR_ENTITY_INTERFACE_HEADER

/**
 * @file ccPlanarEntityInterface.h
 *
 * @brief Planar entity interface
 *
 * Interface for entities that lie on a plane.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// CCCoreLib
#include <CCGeom.h>

// qCC_gl
#include <ccGLDrawContext.h>

/**
 * @brief Planar entity interface
 *
 * Interface for entities with a planar geometry.
 */
class ccPlanarEntityInterface
{
  public:
	/// Default constructor
	ccPlanarEntityInterface();

	/**
	 * @brief Show/hide normal vector
	 * @param[in] state Show state
	 */
	inline void showNormalVector(bool state)
	{
		m_showNormalVector = state;
	}
	
	/**
	 * @brief Check if normal is shown
	 * @return true if shown
	 */
	inline bool normalVectorIsShown() const
	{
		return m_showNormalVector;
	}

	/// Get entity normal
	virtual CCVector3 getNormal() const = 0;

	/// Destructor
	virtual ~ccPlanarEntityInterface() = default;

  protected: // members
	/// Draw normal vector (OpenGL)
	void glDrawNormal(CC_DRAW_CONTEXT& context, const CCVector3& pos, float scale, const ccColor::Rgb* color = 0);

	/// Show normal vector
	bool m_showNormalVector;
};

#endif // PLANAR_ENTITY_INTERFACE_HEADER
