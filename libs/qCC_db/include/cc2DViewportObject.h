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

#ifndef CC_2D_VIEWPORT_OBJECT_HEADER
#define CC_2D_VIEWPORT_OBJECT_HEADER

/**
 * @file cc2DViewportObject.h
 *
 * @brief 2D viewport object
 *
 * Base class for 2D viewport overlays.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "ccGenericGLDisplay.h"
#include "ccHObject.h"

/**
 * @brief 2D viewport object
 *
 * Base class for 2D viewport overlays and labels.
 */
class QCC_DB_LIB_API cc2DViewportObject : public ccHObject
{
  public:
	/**
	 * @brief Create viewport object
	 * @param[in] name Object name
	 */
	cc2DViewportObject(QString name = QString());

	/**
	 * @brief Copy constructor
	 * @param[in] viewport Source object
	 */
	cc2DViewportObject(const cc2DViewportObject& viewport);

	// inherited from ccHObject
	/// Get class type
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::VIEWPORT_2D_OBJECT;
	}
	/// Is serializable
	virtual bool isSerializable() const override
	{
		return true;
	}

	/**
	 * @brief Set parameters
	 * @param[in] params Viewport parameters
	 */
	void setParameters(const ccViewportParameters& params)
	{
		m_params = params;
	}

	/**
	 * @brief Get parameters
	 * @return Viewport parameters
	 */
	const ccViewportParameters& getParameters() const
	{
		return m_params;
	}

  protected:
	// inherited from ccHObject
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;

	/// Viewport parameters
	ccViewportParameters m_params;
};

#endif // CC_2D_VIEWPORT_OBJECT_HEADER
