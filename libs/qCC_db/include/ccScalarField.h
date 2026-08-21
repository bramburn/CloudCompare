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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccScalarField.h
 *
 * @brief Scalar field with display parameters for point clouds
 *
 * A scalar field stores one numerical value per point in a point cloud.
 * Common uses include:
 * - Distance measurements (C2C, C2M)
 * - Curvature or roughness values
 * - LiDAR intensity
 * - Classification labels
 * - Per-point error metrics
 *
 * This class extends CCCoreLib::ScalarField with CloudCompare-specific
 * display features:
 * - Color scale (color ramp) assignment
 * - Display range (values outside shown as grey or hidden)
 * - Saturation range (relative color scale boundaries)
 * - Logarithmic scale option
 * - Symmetrical scale (centered on zero)
 * - Histogram for statistics display
 *
 * Visualization flow:
 * getColor(value) → normalize(value) → colorScale->getColorByRelativePos()
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 * @see CCCoreLib::ScalarField for the core data storage
 * @see ccPointCloud for owner implementation
 * @see ccColorScale for color ramp implementation
 */

// CCCoreLib
#include <ScalarField.h>

// qCC_db
#include "ccColorScale.h"

/**
 * @brief Scalar field with display parameters
 *
 * Extends CCCoreLib::ScalarField with CloudCompare-specific display settings:
 * color scale, display/saturation ranges, log scale, histogram, and
 * modification tracking. Serializable to .bin files.
 *
 * The scalar field manages three ranges:
 * - **Bounds**: absolute min/max of the data (set by computeMinAndMax)
 * - **Display range**: values shown on screen (user-adjustable)
 * - **Saturation range**: controls relative color scale boundaries
 *
 * @see CCCoreLib::ScalarField
 */
class QCC_DB_LIB_API ccScalarField : public CCCoreLib::ScalarField
    , public ccSerializableObject
{
  public:
	/**
	 * @brief Create a scalar field
	 * @param[in] name Human-readable name for this scalar field
	 */
	explicit ccScalarField(const std::string& name = std::string());

	/**
	 * @brief Copy constructor
	 *
	 * Performs a deep copy of the scalar values and display parameters.
	 * Shares the color scale (reference counted via QSharedPointer).
	 *
	 * @param[in] sf Scalar field to copy
	 * @throws std::bad_alloc if value array allocation fails
	 */
	ccScalarField(const ccScalarField& sf);

	/*** Scalar values display handling ***/

	/**
	 * @brief Display/saturation range structure
	 *
	 * Manages min/max bounds, display window (start/stop), and
	 * saturation boundaries for color mapping.
	 *
	 * The four values are ordered: min ≤ start ≤ stop ≤ max.
	 * The display window [start, stop] maps to the color gradient.
	 * Values outside the display window are shown in grey (or hidden).
	 */
	class QCC_DB_LIB_API Range
	{
	  public:
		/**
		 * @brief Default constructor
		 *
		 * Initializes all values to 0 (meaningless state until setBounds is called).
		 */
		Range()
		    : m_min(0)
		    , m_start(0)
		    , m_stop(0)
		    , m_max(0)
		    , m_range(1)
		{
		}

		// Getters

		/**
		 * @brief Get the absolute minimum bound
		 * @return Minimum bound value
		 */
		inline ScalarType min() const { return m_min; }

		/**
		 * @brief Get the display window start
		 * @return Start of display window
		 */
		inline ScalarType start() const { return m_start; }

		/**
		 * @brief Get the display window end
		 * @return End of display window
		 */
		inline ScalarType stop() const { return m_stop; }

		/**
		 * @brief Get the absolute maximum bound
		 * @return Maximum bound value
		 */
		inline ScalarType max() const { return m_max; }

		/**
		 * @brief Get the display window size
		 * @return stop - start (clamped to >= ZERO_TOLERANCE_SCALAR)
		 */
		inline ScalarType range() const { return m_range; }

		/**
		 * @brief Get the total data range
		 * @return max - min
		 */
		inline ScalarType maxRange() const { return m_max - m_min; }

		// Setters

		/**
		 * @brief Set the absolute bounds
		 *
		 * @param[in] minVal         New minimum bound
		 * @param[in] maxVal         New maximum bound
		 * @param[in] resetStartStop If true, reset start=min and stop=max;
		 *                            otherwise clamp existing start/stop to new bounds
		 */
		void setBounds(ScalarType minVal, ScalarType maxVal, bool resetStartStop = true)
		{
			assert(std::isnan(minVal) || std::isnan(maxVal) || minVal <= maxVal);
			m_min = minVal;
			m_max = maxVal;
			if (resetStartStop)
			{
				m_start = m_min;
				m_stop  = m_max;
			}
			else
			{
				m_start = inbound(m_start);
				m_stop  = inbound(m_stop);
			}
			updateRange();
		}

		/**
		 * @brief Set the display window start
		 * @param[in] value New start (clamped to [min, max])
		 */
		inline void setStart(ScalarType value)
		{
			m_start = inbound(value);
			if (m_stop < m_start)
				m_stop = m_start;
			updateRange();
		}

		/**
		 * @brief Set the display window end
		 * @param[in] value New stop (clamped to [min, max])
		 */
		inline void setStop(ScalarType value)
		{
			m_stop = inbound(value);
			if (m_stop < m_start)
				m_start = m_stop;
			updateRange();
		}

		/**
		 * @brief Clamp a value to the [min, max] interval
		 * @param[in] val Value to clamp
		 * @return Clamped value
		 */
		inline ScalarType inbound(ScalarType val) const
		{
			return (val < m_min ? m_min : (val > m_max ? m_max : val));
		}

		/**
		 * @brief Check if a value is within [min, max]
		 * @param[in] val Value to check
		 * @return true if min ≤ val ≤ max
		 */
		inline bool isInbound(ScalarType val) const { return (val >= m_min && val <= m_max); }

		/**
		 * @brief Check if a value is within the display window [start, stop]
		 * @param[in] val Value to check
		 * @return true if start ≤ val ≤ stop
		 */
		inline bool isInRange(ScalarType val) const { return (val >= m_start && val <= m_stop); }

	  protected:
		/**
		 * @brief Update m_range from start/stop
		 *
		 * Sets m_range = max(stop - start, ZERO_TOLERANCE_SCALAR).
		 * Called by setStart, setStop, and setBounds.
		 */
		inline void updateRange()
		{
			m_range = std::max(m_stop - m_start, CCCoreLib::ZERO_TOLERANCE_SCALAR);
		}

		ScalarType m_min;   //!< Absolute minimum bound
		ScalarType m_start; //!< Display window start (clamped to [min, max])
		ScalarType m_stop;  //!< Display window end (clamped to [min, max])
		ScalarType m_max;   //!< Absolute maximum bound
		ScalarType m_range; //!< stop - start (minimum ZERO_TOLERANCE_SCALAR)
	};

	/**
	 * @brief Get the display range
	 *
	 * Values outside [displayRange.start, displayRange.stop] are shown
	 * in grey or hidden depending on showNaNValuesInGrey().
	 *
	 * @return Reference to the display range
	 */
	inline const Range& displayRange() const { return m_displayRange; }

	/**
	 * @brief Get the saturation range (linear or log, depending on logScale())
	 *
	 * Relative color scales map [saturationRange.start, saturationRange.stop]
	 * to the full color gradient.
	 *
	 * @return Reference to the saturation range
	 */
	inline const Range& saturationRange() const { return m_logScale ? m_logSaturationRange : m_saturationRange; }

	/**
	 * @brief Get the log-scale saturation range
	 * @return Reference to the log saturation range
	 */
	inline const Range& logSaturationRange() const { return m_logSaturationRange; }

	/**
	 * @brief Set the minimum displayed value
	 *
	 * Sets displayRange.start() to val (clamped to [min, max]).
	 *
	 * @param[in] val New minimum display value
	 */
	void setMinDisplayed(ScalarType val);

	/**
	 * @brief Set the maximum displayed value
	 *
	 * Sets displayRange.stop() to val (clamped to [min, max]).
	 *
	 * @param[in] val New maximum display value
	 */
	void setMaxDisplayed(ScalarType val);

	/**
	 * @brief Set the saturation range start
	 *
	 * @param[in] val New saturation start
	 */
	void setSaturationStart(ScalarType val);

	/**
	 * @brief Set the saturation range end
	 *
	 * @param[in] val New saturation stop
	 */
	void setSaturationStop(ScalarType val);

	/**
	 * @brief Get the color for a scalar value
	 *
	 * Maps a scalar value to an RGB color using the current color scale
	 * and normalization parameters. NaN or out-of-range values are
	 * handled according to showNaNValuesInGrey().
	 *
	 * @param[in] value Scalar value to look up
	 * @return Pointer to RGB color (never nullptr if colorScale is set)
	 *
	 * @note A color scale must be set before calling this method.
	 * @see setColorScale(), normalize()
	 */
	inline const ccColor::Rgb* getColor(ScalarType value) const
	{
		assert(m_colorScale);
		return m_colorScale->getColorByRelativePos(
		    normalize(value),
		    m_colorRampSteps,
		    m_showNaNValuesInGrey ? &ccColor::lightGreyRGB : nullptr);
	}

	/**
	 * @brief Get the color for a point's scalar value
	 *
	 * Convenience method: reads the scalar value at an index and
	 * maps it to a color.
	 *
	 * @param[in] index Point index in the scalar field
	 * @return Pointer to RGB color for the value at this index
	 * @see getColor(ScalarType)
	 */
	inline const ccColor::Rgb* getValueColor(unsigned index) const
	{
		return getColor(getValue(index));
	}

	/**
	 * @brief Control how out-of-range / NaN values are displayed
	 *
	 * @param[in] state true = show in grey, false = hide (transparent)
	 */
	void showNaNValuesInGrey(bool state);

	/**
	 * @brief Check how out-of-range / NaN values are displayed
	 * @return true if shown in grey, false if hidden
	 */
	inline bool areNaNValuesShownInGrey() const { return m_showNaNValuesInGrey; }

	/**
	 * @brief Force the color scale to always show zero
	 *
	 * If enabled, the color scale is anchored so that zero maps to
	 * the appropriate color regardless of the display range.
	 *
	 * @param[in] state true to force zero visibility
	 */
	void alwaysShowZero(bool state);

	/**
	 * @brief Check if zero is forced visible
	 * @return true if zero is always shown
	 */
	inline bool isZeroAlwaysShown() const { return m_alwaysShowZero; }

	/**
	/**
	 * @brief Set the color scale symmetry
	 *
	 * When enabled (for relative color scales only), the scale is
	 * centered on zero with equal extent above and below.
	 *
	 * @param[in] state true to enable symmetric scale
	 */
	void setSymmetricalScale(bool state);

	/**
	 * @brief Check if the scale is symmetric
	 * @return true if symmetric, false otherwise
	 */
	inline bool symmetricalScale() const { return m_symmetricalScale; }

	/**
	 * @brief Enable or disable logarithmic display
	 *
	 * In log mode, the saturation range uses a logarithmic mapping
	 * (logScale version) instead of linear.
	 *
	 * @param[in] state true to enable log scale
	 */
	void setLogScale(bool state);

	/**
	 * @brief Check if log scale is enabled
	 * @return true if log scale is active
	 */
	inline bool logScale() const { return m_logScale; }

	// inherited from ScalarField
	/**
	 * @brief Recompute min and max from the underlying data array
	 *
	 * Scans all scalar values to find the global min and max,
	 * then sets the display and saturation ranges accordingly.
	 */
	void computeMinAndMax() override;

	/**
	 * @brief Get the active color scale
	 * @return Shared pointer to the current color scale
	 */
	inline const ccColorScale::Shared& getColorScale() const { return m_colorScale; }

	/**
	 * @brief Set the active color scale
	 * @param[in] scale Shared pointer to a color scale
	 */
	void setColorScale(ccColorScale::Shared scale);

	/**
	 * @brief Get the number of color ramp steps
	 * @return Number of discrete color steps in the ramp
	 */
	inline unsigned getColorRampSteps() const { return m_colorRampSteps; }

	/**
	 * @brief Set the number of color ramp steps
	 *
	 * Controls how many discrete steps are used when sampling
	 * the color ramp for rendering.
	 *
	 * @param[in] steps Number of steps (default is typically 256)
	 */
	void setColorRampSteps(unsigned steps);

	/**
	 * @brief Histogram data structure for display
	 *
	 * Inherits from std::vector<unsigned> with an added maxValue field.
	 * Each element represents the count of points in a bin.
	 */
	struct Histogram : std::vector<unsigned>
	{
		//! Maximum bin count across all bins
		unsigned maxValue = 0;
	};

	/**
	 * @brief Get the histogram for display in the scalar field dialog
	 * @return Reference to the histogram vector
	 */
	inline const Histogram& getHistogram() const { return m_histogram; }

	/**
	 * @brief Check if any values may be hidden by the current display settings
	 *
	 * Returns true if NaN values exist or if the display range
	 * ([start, stop]) excludes part of the data range ([min, max]).
	 *
	 * @return true if some values may be hidden
	 */
	bool mayHaveHiddenValues() const;

	/**
	 * @brief Set the modification flag
	 *
	 * Indicates that the scalar field values or display parameters
	 * have changed since the last save.
	 *
	 * @param[in] state New modification state
	 */
	inline void setModificationFlag(bool state) { m_modified = state; }

	/**
	 * @brief Get the modification flag
	 * @return true if the scalar field has been modified
	 */
	inline bool getModificationFlag() const { return m_modified; }

	/**
	 * @brief Copy display parameters from another scalar field
	 *
	 * Copies: color scale, display/saturation ranges, log scale,
	 * symmetrical scale, NaN display mode, and color ramp steps.
	 * Does NOT copy scalar values.
	 *
	 * @param[in] sf Source scalar field to copy parameters from
	 */
	void importParametersFrom(const ccScalarField* sf);

	// from ccSerializableObject
	inline bool isSerializable() const override { return true; }

	/**
	 * @brief Serialize to binary file
	 *
	 * Saves scalar values, name, and all display parameters including
	 * color scale, display range, saturation range, and flags.
	 *
	 * @param[in] out        Output file
	 * @param[in] dataVersion File format version
	 * @return true on success
	 */
	bool toFile(QFile& out, short dataVersion) const override;

	/**
	 * @brief Deserialize from binary file
	 *
	 * @param[in]  in              Input file
	 * @param[in]  dataVersion     File format version
	 * @param[in]  flags           Deserialization flags
	 * @param[out] oldToNewIDMap  ID remapping table
	 * @return true on success
	 */
	bool fromFile(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;

	/**
	 * @brief Minimum file version needed to save this state
	 * @return Minimum compatible file version
	 */
	short minimumFileVersion() const override;

  protected:
	/**
	 * @brief Protected destructor
	 *
	 * Destruction should go through release() (inherited from ScalarField).
	 */
	~ccScalarField() override = default;

	/**
	 * @brief Recompute saturation bounds from display bounds
	 */
	void updateSaturationBounds();

	/**
	 * @brief Normalize a scalar value to [0, 1]
	 *
	 * Maps a scalar value to a relative position within the display
	 * range [start, stop], accounting for log scale if enabled.
	 *
	 * @param[in] val Scalar value to normalize
	 * @return 0.0–1.0 if within display range, -1.0 otherwise
	 */
	ScalarType normalize(ScalarType val) const;

  protected: // members
	//! Display range: controls which values are shown vs. hidden/grey
	Range m_displayRange;

	//! Saturation range: controls relative color scale boundaries (linear)
	Range m_saturationRange;

	//! Saturation range: controls relative color scale boundaries (log mode)
	Range m_logSaturationRange;

	//! How NaN / out-of-range values are displayed
	bool m_showNaNValuesInGrey;

	//! Whether color scale is symmetric around zero
	bool m_symmetricalScale;

	//! Whether display uses logarithmic scale
	bool m_logScale;

	//! Whether zero is forced visible in the color scale
	bool m_alwaysShowZero;

	//! Active color scale (reference-counted)
	ccColorScale::Shared m_colorScale;

	//! Number of discrete color ramp steps
	unsigned m_colorRampSteps;

	//! Histogram data for statistics dialog
	Histogram m_histogram;

	//! Modification flag: true if values or parameters changed since last save
	bool m_modified;
};
