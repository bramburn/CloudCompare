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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccDrawableObject.h
 *
 * @brief Drawable object interface for 3D rendering.
 *
 * @details Base interface for entities that can render themselves
 * in 3D views. Provides visibility, color, materials, and drawing
 * management.
 *
 * ## Overview
 *
 * ccDrawableObject provides:
 * - **Visibility management**: Show/hide entities
 * - **Color management**: Colors, materials, temporary colors
 * - **Normal display**: Show/hide normals
 * - **Scalar field display**: SF visualization
 * - **Display association**: Link to GL windows
 * - **GL transformation**: Temporary display transformations
 * - **Clipping planes**: OpenGL clipping
 *
 * ## Display State Stack
 *
 * Use push/pop display state to save and restore:
 * - Visibility
 * - Colors display
 * - Normals display
 * - SF display
 * - Temporary color
 * - Name display
 * - Associated display
 *
 * ## Usage
 *
 * @code
 * // Show/hide entity
 * entity->setVisible(true);
 * entity->toggleVisibility();
 *
 * // Show colors and normals
 * entity->showColors(true);
 * entity->showNormals(true);
 *
 * // Temporary color override
 * entity->setTempColor(ccColor::red, true);
 *
 * // Save and restore display state
 * entity->pushDisplayState();
 * entity->showSF(false);
 * // ... operations ...
 * entity->popDisplayState();
 *
 * // GL transformation (display only)
 * entity->setGLTransformation(glMatrix);
 * entity->rotateGL(rotation);
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccHObject for hierarchical objects
 * @see ccGenericGLDisplay for display management
 */

#ifndef CC_DRAWABLE_OBJECT_HEADER
#define CC_DRAWABLE_OBJECT_HEADER

// Local
#include "ccGLDrawContext.h"

// CCCoreLib
#include <CCGeom.h>

class ccGenericGLDisplay;

/**
 * @brief Clipping plane equation.
 */
struct ccClipPlane
{
	//! Plane equation (A, B, C, D).
	Tuple4Tpl<double> equation;
};

/**
 * @brief Container of clipping planes.
 */
using ccClipPlaneSet = std::vector<ccClipPlane>;

/**
 * @brief Generic interface for drawable entities.
 *
 * @details Provides common drawing and display functionality
 * for all entities that render in 3D views.
 */
class QCC_DB_LIB_API ccDrawableObject
{
  public:
	/**
	 * @brief Default constructor.
	 */
	ccDrawableObject();

	/**
	 * @brief Copy constructor.
	 *
	 * @param[in] object Source object.
	 */
	ccDrawableObject(const ccDrawableObject& object);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccDrawableObject() = default;

  public: // drawing and drawing options
	/**
	 * @brief Draw the entity.
	 *
	 * @param[in] context Drawing context.
	 */
	virtual void draw(CC_DRAW_CONTEXT& context) = 0;

	/**
	 * @brief Check if visible.
	 *
	 * @return true if visible.
	 */
	inline virtual bool isVisible() const
	{
		return m_visible;
	}

	/**
	 * @brief Set visibility.
	 *
	 * @param[in] state Visibility state.
	 */
	inline virtual void setVisible(bool state)
	{
		m_visible = state;
	}

	/**
	 * @brief Toggle visibility.
	 */
	inline virtual void toggleVisibility()
	{
		setVisible(!isVisible());
	}

	/**
	 * @brief Check if visibility is locked.
	 *
	 * @return true if locked.
	 */
	inline virtual bool isVisibilityLocked() const
	{
		return m_lockedVisibility;
	}

	/**
	 * @brief Lock/unlock visibility.
	 *
	 * @param[in] state Lock state.
	 */
	inline virtual void lockVisibility(bool state)
	{
		m_lockedVisibility = state;
	}

	/**
	 * @brief Check if selected.
	 */
	inline virtual bool isSelected() const
	{
		return m_selected;
	}

	/**
	 * @brief Set selection state.
	 *
	 * @param[in] state Selection state.
	 */
	inline virtual void setSelected(bool state)
	{
		m_selected = state;
	}

	/**
	 * @brief Get drawing parameters.
	 *
	 * @param[out] params Drawing parameters.
	 */
	virtual void getDrawingParameters(glDrawParams& params) const;

  public: // colors
	/**
	 * @brief Check if has colors.
	 */
	inline virtual bool hasColors() const
	{
		return false;
	}

	/**
	 * @brief Check if colors are shown.
	 */
	inline virtual bool colorsShown() const
	{
		return m_colorsDisplayed;
	}

	/**
	 * @brief Set colors visibility.
	 *
	 * @param[in] state Display state.
	 */
	inline virtual void showColors(bool state)
	{
		m_colorsDisplayed = state;
	}

	/**
	 * @brief Toggle colors display.
	 */
	inline virtual void toggleColors()
	{
		showColors(!colorsShown());
	}

  public: // normals
	/**
	 * @brief Check if has normals.
	 */
	inline virtual bool hasNormals() const
	{
		return false;
	}

	/**
	 * @brief Check if normals are shown.
	 */
	inline virtual bool normalsShown() const
	{
		return m_normalsDisplayed;
	}

	/**
	 * @brief Set normals visibility.
	 *
	 * @param[in] state Display state.
	 */
	inline virtual void showNormals(bool state)
	{
		m_normalsDisplayed = state;
	}

	/**
	 * @brief Toggle normals display.
	 */
	inline virtual void toggleNormals()
	{
		showNormals(!normalsShown());
	}

  public: // scalar fields
	/**
	 * @brief Check if has displayed scalar field.
	 */
	inline virtual bool hasDisplayedScalarField() const
	{
		return false;
	}

	/**
	 * @brief Check if has any scalar fields.
	 *
	 * @note Different from hasDisplayedScalarField().
	 */
	inline virtual bool hasScalarFields() const
	{
		return false;
	}

	/**
	 * @brief Set scalar field visibility.
	 *
	 * @param[in] state Display state.
	 */
	inline virtual void showSF(bool state)
	{
		m_sfDisplayed = state;
	}

	/**
	 * @brief Toggle SF display.
	 */
	inline virtual void toggleSF()
	{
		showSF(!sfShown());
	}

	/**
	 * @brief Check if SF is shown.
	 */
	inline virtual bool sfShown() const
	{
		return m_sfDisplayed;
	}

  public: // materials
	/**
	 * @brief Toggle material display.
	 */
	virtual void toggleMaterials()
	{
	} // does nothing by default!

  public: // name display in 3D
	/**
	 * @brief Set name display in 3D.
	 *
	 * @param[in] state Display state.
	 */
	inline virtual void showNameIn3D(bool state)
	{
		m_showNameIn3D = state;
		if (!state)
			m_nameIn3DPosIsValid = false;
	}

	/**
	 * @brief Check if name is shown in 3D.
	 */
	inline virtual bool nameShownIn3D() const
	{
		return m_showNameIn3D;
	}

	/**
	 * @brief Toggle name in 3D display.
	 */
	inline virtual void toggleShowName()
	{
		showNameIn3D(!nameShownIn3D());
	}

  public: // temporary color
	/**
	 * @brief Check if color is overridden.
	 */
	inline virtual bool isColorOverridden() const
	{
		return m_colorIsOverridden;
	}

	/**
	 * @brief Get temporary color.
	 */
	inline virtual const ccColor::Rgba& getTempColor() const
	{
		return m_tempColor;
	}

	/**
	 * @brief Set temporary RGBA color.
	 *
	 * @param[in] col RGBA color.
	 * @param[in] autoActivate Auto-activate temporary color.
	 */
	virtual void setTempColor(const ccColor::Rgba& col, bool autoActivate = true);

	/**
	 * @brief Set temporary RGB color.
	 *
	 * @param[in] col RGB color.
	 * @param[in] autoActivate Auto-activate temporary color.
	 */
	virtual void setTempColor(const ccColor::Rgb& col, bool autoActivate = true);

	/**
	 * @brief Enable/disable temporary color.
	 *
	 * @param[in] state Enable state.
	 */
	inline virtual void enableTempColor(bool state)
	{
		m_colorIsOverridden = state;
	}

  public: // display management
	/**
	 * @brief Remove from display.
	 *
	 * @param[in] win Display to remove from.
	 */
	virtual void removeFromDisplay(const ccGenericGLDisplay* win);

	/**
	 * @brief Set associated display.
	 *
	 * @param[in] win GL display.
	 */
	virtual void setDisplay(ccGenericGLDisplay* win);

	/**
	 * @brief Get associated display.
	 */
	inline virtual ccGenericGLDisplay* getDisplay() const
	{
		return m_currentDisplay;
	}

	/**
	 * @brief Redraw associated display.
	 */
	virtual void redrawDisplay();

	/**
	 * @brief Prepare display for refresh.
	 */
	virtual void prepareDisplayForRefresh();

	/**
	 * @brief Refresh display.
	 *
	 * @param[in] only2D Only refresh 2D elements.
	 */
	virtual void refreshDisplay(bool only2D = false);

  public: // GL transformation (display only)
	/**
	 * @brief Set GL transformation.
	 *
	 * @warning FOR DISPLAY PURPOSE ONLY.
	 * This transformation is only applied for display and
	 * is not considered by CCCoreLib algorithms.
	 *
	 * @param[in] trans Transformation matrix.
	 */
	virtual void setGLTransformation(const ccGLMatrix& trans);

	/**
	 * @brief Enable/disable GL transformation.
	 *
	 * @param[in] state Enable state.
	 */
	virtual void enableGLTransformation(bool state);

	/**
	 * @brief Check if GL transformation is enabled.
	 */
	inline virtual bool isGLTransEnabled() const
	{
		return m_glTransEnabled;
	}

	/**
	 * @brief Get GL transformation.
	 */
	inline virtual const ccGLMatrix& getGLTransformation() const
	{
		return m_glTrans;
	}

	/**
	 * @brief Reset GL transformation to identity.
	 */
	virtual void resetGLTransformation();

	/**
	 * @brief Multiply GL transformation.
	 *
	 * GLtrans = M * GLtrans
	 *
	 * @param[in] rotMat Rotation matrix.
	 */
	virtual void rotateGL(const ccGLMatrix& rotMat);

	/**
	 * @brief Translate GL transformation.
	 *
	 * GLtrans = GLtrans + T
	 *
	 * @param[in] trans Translation vector.
	 */
	virtual void translateGL(const CCVector3& trans);

  public: // clipping planes
	/**
	 * @brief Remove all clipping planes.
	 */
	virtual void removeAllClipPlanes()
	{
		m_clipPlanes.resize(0);
	}

	/**
	 * @brief Add clipping plane.
	 *
	 * @param[in] plane Clipping plane equation.
	 *
	 * @return true if added successfully.
	 */
	virtual bool addClipPlanes(const ccClipPlane& plane);

	/**
	 * @brief Toggle clipping planes.
	 *
	 * @param[in] context Drawing context.
	 * @param[in] enable Enable state.
	 *
	 * @warning Call AFTER modelview matrix is set.
	 */
	virtual void toggleClipPlanes(CC_DRAW_CONTEXT& context, bool enable);

  public: // display state stack
	/**
	 * @brief Display state structure.
	 */
	struct DisplayState
	{
		/**
		 * @brief Default constructor.
		 */
		DisplayState()
		{
		}

		/**
		 * @brief Copy from drawable object.
		 */
		DisplayState(const ccDrawableObject& dobj);

		//! Shared pointer type.
		using Shared = QSharedPointer<DisplayState>;

		bool visible = false;
		bool colorsDisplayed = false;
		bool normalsDisplayed = false;
		bool sfDisplayed = false;
		bool colorIsOverridden = false;
		bool showNameIn3D = false;
		ccGenericGLDisplay* display = nullptr;
	};

	/**
	 * @brief Push current display state.
	 *
	 * @return true if successful.
	 */
	virtual bool pushDisplayState();

	/**
	 * @brief Pop last pushed display state.
	 *
	 * @param[in] apply Apply the state.
	 */
	virtual void popDisplayState(bool apply = true);

	/**
	 * @brief Apply a display state.
	 *
	 * @param[in] state State to apply.
	 */
	virtual void applyDisplayState(const DisplayState& state);

  protected:
	//! Visibility state.
	bool m_visible;

	//! Selection state.
	bool m_selected;

	//! Visibility lock state.
	bool m_lockedVisibility;

	//! Colors display state.
	bool m_colorsDisplayed;

	//! Normals display state.
	bool m_normalsDisplayed;

	//! Scalar field display state.
	bool m_sfDisplayed;

	//! Temporary color.
	ccColor::Rgba m_tempColor;

	//! Temporary color activation state.
	bool m_colorIsOverridden;

	//! Current GL transformation.
	ccGLMatrix m_glTrans;

	//! GL transformation enabled state.
	bool m_glTransEnabled;

	//! Name display in 3D state.
	bool m_showNameIn3D;

	//! Last 3D name position.
	CCVector3d m_nameIn3DPos;

	//! 3D name position validity.
	bool m_nameIn3DPosIsValid;

	//! Current GL display.
	ccGenericGLDisplay* m_currentDisplay;

	//! Active clipping planes.
	ccClipPlaneSet m_clipPlanes;

	//! Display state stack.
	std::vector<DisplayState::Shared> m_displayStateStack;
};

#endif // CC_DRAWABLE_OBJECT_HEADER
