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

#ifndef CC_2D_VIEWPORT_LABEL_HEADER
#define CC_2D_VIEWPORT_LABEL_HEADER

/**
 * @file cc2DViewportLabel.h
 *
 * @brief 2D viewport label
 *
 * Label overlay for 2D viewports.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "cc2DViewportObject.h"
// System
#include <array>

/**
 * @brief 2D viewport label
 *
 * Label overlay with ROI for 2D viewports.
 */
class QCC_DB_LIB_API cc2DViewportLabel : public cc2DViewportObject
{
  public:
	/**
	 * @brief Create label
	 * @param[in] name Label name
	 */
	explicit cc2DViewportLabel(QString name = QString());

	/**
	 * @brief Copy constructor
	 * @param[in] viewportLabel Source label
	 */
	explicit cc2DViewportLabel(const cc2DViewportLabel& viewportLabel);

	// inherited from ccHObject
	/// Get class type
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::VIEWPORT_2D_LABEL;
	}
	/// Is serializable
	virtual bool isSerializable() const override
	{
		return true;
	}

	/// ROI type (x, y, width, height)
	typedef std::array<float, 4> ROI;

	/**
	 * @brief Get ROI
	 * @return ROI (relative to screen)
	 */
	inline const ROI& roi() const
	{
		return m_roi;
	}

	/**
	 * @brief Set ROI
	 * @param[in] roi New ROI
	 */
	inline void setRoi(const ROI& roi)
	{
		m_roi = roi;
	}

  protected:
	// inherited from ccHObject
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;

	/// Draw the entity
	virtual void drawMeOnly(CC_DRAW_CONTEXT& context) override;

	/// Label ROI
	ROI m_roi;
};

#endif // CC_2D_VIEWPORT_LABEL_HEADER
