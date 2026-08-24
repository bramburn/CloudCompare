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

#ifndef CC_COLOR_SCALES_MANAGER_HEADER
#define CC_COLOR_SCALES_MANAGER_HEADER

/**
 * @file ccColorScalesManager.h
 *
 * @brief Color scales manager singleton
 *
 * Manages color scale presets for scalar field visualization.
 * Provides access to pre-defined color ramps like BGYR, grey,
 * HSV, Viridis, and ASPRS classification colors.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Local
#include "ccColorScale.h"

// Qt
#include <QMap>

/**
 * @brief Color scales manager singleton
 *
 * Manages and provides access to pre-defined color scales.
 */
class QCC_DB_LIB_API ccColorScalesManager
{
  public:
	/**
	 * @brief Get singleton instance
	 * @return Pointer to the manager
	 */
	static ccColorScalesManager* GetUniqueInstance();

	/**
	 * @brief Release singleton instance
	 */
	static void ReleaseUniqueInstance();

	/**
	 * @brief Destructor
	 */
	virtual ~ccColorScalesManager();

	/**
	 * @brief Pre-defined color scales
	 *
	 * All scales use relative values (0-1) that expand
	 * to the actual scalar field range.
	 */
	enum DEFAULT_SCALES
	{
		BGYR = 0,               //!< Blue-Green-Yellow-Red (distances)
		GREY = 1,               //!< Grey ramp
		BWR = 2,                //!< Blue-White-Red (signed SF)
		RY = 3,                 //!< Red-Yellow
		RW = 4,                 //!< Red-White
		ABS_NORM_GREY = 5,      //!< Absolute normalized grey
		HSV_360_DEG = 6,        //!< HSV 0-360 degrees
		VERTEX_QUALITY = 7,     //!< Mesh vertex quality
		DIP_BRYW = 8,           //!< Dip 0-90 degrees
		DIP_DIR_REPEAT = 9,     //!< Dip direction 0-360 degrees
		VIRIDIS = 10,           //!< Matplotlib Viridis
		BROWN_YELLOW = 11,      //!< Brown-Yellow
		YELLOW_BROWN = 12,      //!< Yellow-Brown
		TOPO_LANDSERF = 13,     //!< Topo Landserf
		HIGH_CONTRAST = 14,     //!< High contrast
		CIVIDIS = 15,           //!< Matplotlib Cividis
		ASPRS_CLASSES = 16,     //!< ASPRS classes
		ASPRS_WITH_LABELS = 17, //!< ASPRS with labels
		GREY_INV = 18,          //!< Inverted grey
	};

	//! Returns a pre-defined color scale UUID
	static QString GetDefaultScaleUUID(int scale)
	{
		return QString::number(scale);
	}

	//! Returns a pre-defined color scale (static shortcut)
	static ccColorScale::Shared GetDefaultScale(DEFAULT_SCALES scale = BGYR)
	{
		ccColorScalesManager* instance = GetUniqueInstance();
		return instance ? instance->getDefaultScale(scale) : ccColorScale::Shared(nullptr);
	}

	//! Returns a pre-defined color scale
	ccColorScale::Shared getDefaultScale(DEFAULT_SCALES scale) const
	{
		return getScale(GetDefaultScaleUUID(scale));
	}

	//! Returns a color scale based on its UUID
	ccColorScale::Shared getScale(QString UUID) const;

	//! Adds a new color scale
	void addScale(ccColorScale::Shared scale);

	//! Removes a color scale
	/** Warning: can't remove default scales!
	 **/
	void removeScale(QString UUID);

	//! Color scales map type
	typedef QMap<QString, ccColorScale::Shared> ScalesMap;

	//! Access to the internal map
	ScalesMap& map()
	{
		return m_scales;
	}

	//! Access to the internal map (const)
	const ScalesMap& map() const
	{
		return m_scales;
	}

	//! Loads custom color scales from persistent settings
	void fromPersistentSettings();

	//! Save custom color scales to persistent settings
	void toPersistentSettings() const;

  protected:
	//! Default constructor
	ccColorScalesManager();

	//! Creates a pre-defined color scale
	static ccColorScale::Shared Create(DEFAULT_SCALES scaleType);

	//! Color scales
	ScalesMap m_scales;
};

#endif // CC_COLOR_SCALES_MANAGER_HEADER
