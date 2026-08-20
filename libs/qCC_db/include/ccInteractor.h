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
 * @file ccInteractor.h
 *
 * @brief Interactor interface for 3D mouse interaction.
 *
 * @details Interface for entities that can respond to mouse interactions
 * in 3D views (clicking, dragging).
 *
 * ## Overview
 *
 * ccInteractor provides mouse interaction capabilities:
 * - **Click handling**: Accept/reject mouse clicks
 * - **2D movement**: Screen-space dragging
 * - **3D movement**: World-space transformations
 *
 * ## Implementations
 *
 * - **ccClipBox**: Interactive clipping plane box
 * - **ccPolyline**: Editable polylines
 * - **ccSphere**: Draggable primitives
 *
 * ## Usage
 *
 * @code
 * class MyInteractor : public ccInteractor {
 * public:
 *     bool acceptClick(int x, int y, Qt::MouseButton button) override {
 *         if (button == Qt::LeftButton) {
 *             m_clickPos = QPoint(x, y);
 *             return true;
 *         }
 *         return false;
 *     }
 *
 *     bool move3D(const CCVector3d& u) override {
 *         translate(u);
 *         return true;
 *     }
 * };
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#ifndef CC_INTERACTOR_HEADER
#define CC_INTERACTOR_HEADER

// Local
#include "CCGeom.h"
#include "qCC_db.h"

// Qt
#include <Qt>

/**
 * @brief Interactor interface.
 *
 * @details Interface for entities that can be interacted with
 * in 3D views (labels, clipping planes, etc.).
 */
class QCC_DB_LIB_API ccInteractor
{
  public:
	/**
	 * @brief Destructor.
	 */
	virtual ~ccInteractor() = default;

	/**
	 * @brief Handle mouse click.
	 *
	 * @param[in] x Mouse X position.
	 * @param[in] y Mouse Y position.
	 * @param[in] button Mouse button.
	 *
	 * @return true if click was handled.
	 */
	virtual bool acceptClick(int x, int y, Qt::MouseButton button)
	{
		return false;
	}

	/**
	 * @brief Handle 2D mouse movement.
	 *
	 * @param[in] x New mouse X position.
	 * @param[in] y New mouse Y position.
	 * @param[in] dx X movement delta.
	 * @param[in] dy Y movement delta.
	 * @param[in] screenWidth Screen width.
	 * @param[in] screenHeight Screen height.
	 *
	 * @return true if movement occurred.
	 */
	virtual bool move2D(int x, int y, int dx, int dy, int screenWidth, int screenHeight)
	{
		return false;
	}

	/**
	 * @brief Handle 3D mouse movement.
	 *
	 * @param[in] u Movement vector in world coordinates.
	 *
	 * @return true if movement occurred.
	 */
	virtual bool move3D(const CCVector3d& u)
	{
		return false;
	}
};

#endif // CC_INTERACTOR_HEADER
