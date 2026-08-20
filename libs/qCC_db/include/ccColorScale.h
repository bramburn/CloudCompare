// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccColorScale.h
 *
 * @brief Color scale for scalar field visualization.
 *
 * @details Color scales map scalar values to colors for visualization
 * of point clouds and meshes.
 *
 * ## Overview
 *
 * A color scale consists of:
 * - **Steps**: Position-color pairs defining key colors
 * - **Interpolation**: Linear interpolation between steps
 * - **Labels**: Optional text labels at specific positions
 *
 * ## Modes
 *
 * ### Relative Mode
 * Values are normalized to [0,1] range.
 * Good for arbitrary scalar fields.
 *
 * ### Absolute Mode
 * Values map to actual scalar ranges.
 * Good for data with known min/max (e.g., elevations).
 *
 * ## Built-in Scales
 *
 * CloudCompare includes several pre-defined scales:
 * - Grey (monochrome)
 * - Fire
 * - Science (viridis-like)
 * - Ice and Fire
 * - Royal
 * - Normalize
 *
 * ## Usage
 *
 * @code
 * // Create a color scale
 * ccColorScale::Shared scale = ccColorScale::Create("My Scale");
 *
 * // Add color stops
 * scale->insert(0.0, Qt::blue);
 * scale->insert(0.5, Qt::yellow);
 * scale->insert(1.0, Qt::red);
 *
 * // Generate lookup table
 * scale->refresh();
 *
 * // Get color for a value
 * QColor color = scale->getColorByValue(0.75);
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#ifndef CC_COLOR_SCALE_HEADER
#define CC_COLOR_SCALE_HEADER

// Local
#include "ccColorTypes.h"
#include "ccSerializableObject.h"

// Qt
#include <QList>
#include <QSharedPointer>

// System
#include <set>

/**
 * @brief A single element (stop) in a color scale.
 *
 * @details Represents a position-color pair in a color gradient.
 * Position is relative [0.0, 1.0].
 */
class ccColorScaleElement
{
  public:
	/**
	 * @brief Default constructor.
	 *
	 * Creates element at position 0.0 with black color.
	 */
	ccColorScaleElement()
	    : m_relativePos(0.0)
	    , m_color(Qt::black)
	{
	}

	/**
	 * @brief Create an element.
	 *
	 * @param[in] relativePos Position in scale (0.0 to 1.0).
	 * @param[in] color Color at this position.
	 */
	ccColorScaleElement(double relativePos, const QColor& color)
	    : m_relativePos(relativePos)
	    , m_color(color)
	{
	}

	/**
	 * @brief Set position.
	 *
	 * @param[in] pos Relative position (0.0 to 1.0).
	 */
	void setRelativePos(double pos)
	{
		m_relativePos = pos;
	}

	/**
	 * @brief Get position.
	 *
	 * @return Relative position (0.0 to 1.0).
	 */
	double getRelativePos() const
	{
		return m_relativePos;
	}

	/**
	 * @brief Set color.
	 *
	 * @param[in] color Color value.
	 */
	void setColor(const QColor& color)
	{
		m_color = color;
	}

	/**
	 * @brief Get color.
	 *
	 * @return Color value.
	 */
	const QColor& getColor() const
	{
		return m_color;
	}

	/**
	 * @brief Compare two elements by position.
	 */
	static bool IsSmaller(const ccColorScaleElement& e1, const ccColorScaleElement& e2)
	{
		return e1.getRelativePos() < e2.getRelativePos();
	}

  protected:
	//! Relative position (0.0 to 1.0).
	double m_relativePos;

	//! Color at this position.
	QColor m_color;
};

/**
 * @brief Color scale for scalar field visualization.
 *
 * @details A color scale maps scalar values to colors.
 *
 * Structure:
 * - Multiple color stops at relative positions
 * - Linear interpolation between stops
 * - Pre-computed lookup table for fast access
 *
 * Requirements:
 * - At least 2 stops (0.0 and 1.0)
 * - Stops must be within [0.0, 1.0] range
 *
 * @extends ccSerializableObject
 */
class QCC_DB_LIB_API ccColorScale : public ccSerializableObject
{
  public:
	//! Shared pointer type.
	using Shared = QSharedPointer<ccColorScale>;

	/**
	 * @brief Create a new color scale.
	 *
	 * @param[in] name Scale name.
	 *
	 * @return New color scale (relative by default).
	 *
	 * @note UUID is auto-generated.
	 */
	static ccColorScale::Shared Create(const QString& name);

	/**
	 * @brief Construct a color scale.
	 *
	 * @param[in] name Scale name.
	 * @param[in] uuid UUID (auto-generated if empty).
	 *
	 * @note Scale is relative by default.
	 * @note Two extreme steps are created (0.0 and 1.0).
	 */
	ccColorScale(const QString& name, const QString& uuid = QString());

	/**
	 * @brief Destructor.
	 */
	~ccColorScale() override = default;

	/**
	 * @brief Create a copy with new UUID.
	 *
	 * @param[in] uuid New UUID (auto-generated if empty).
	 *
	 * @return Copy of this scale.
	 */
	ccColorScale::Shared copy(const QString& uuid = QString()) const;

	//! Minimum number of steps.
	static constexpr unsigned MIN_STEPS = 2;

	//! Default lookup table size.
	static constexpr unsigned DEFAULT_STEPS = 256;

	//! Maximum lookup table size.
	static constexpr unsigned MAX_STEPS = 1024;

	/**
	 * @brief Get scale name.
	 */
	const QString& getName() const
	{
		return m_name;
	}

	/**
	 * @brief Set scale name.
	 */
	void setName(const QString& name)
	{
		m_name = name;
	}

	/**
	 * @brief Get unique ID.
	 */
	const QString& getUuid() const
	{
		return m_uuid;
	}

	/**
	 * @brief Set unique ID.
	 */
	void setUuid(const QString& uuid)
	{
		m_uuid = uuid;
	}

	/**
	 * @brief Generate a new unique ID.
	 */
	void generateNewUuid();

	/**
	 * @brief Check if scale is relative.
	 *
	 * @return true if relative (0.0-1.0).
	 */
	bool isRelative() const
	{
		return m_relative;
	}

	/**
	 * @brief Set scale as relative.
	 */
	void setRelative()
	{
		m_relative = true;
	}

	/**
	 * @brief Set scale as absolute with bounds.
	 *
	 * @param[in] minVal Minimum value.
	 * @param[in] maxVal Maximum value.
	 */
	void setAbsolute(double minVal, double maxVal);

	/**
	 * @brief Get absolute scale boundaries.
	 *
	 * @param[out] minVal Minimum value.
	 * @param[out] maxVal Maximum value.
	 *
	 * @note Only valid for absolute scales.
	 */
	void getAbsoluteBoundaries(double& minVal, double& maxVal) const;

	/**
	 * @brief Check if scale is read-only.
	 */
	bool isReadOnly() const
	{
		return m_readOnly;
	}

	/**
	 * @brief Set read-only state.
	 */
	void setReadOnly(bool state)
	{
		m_readOnly = state;
	}

	/**
	 * @brief Label for a position.
	 */
	struct Label
	{
		Label(double v)
		    : value(v)
		{
		}

		Label(double v, const QString& t)
		    : value(v)
		    , text(t)
		{
		}

		double  value = 0.0;
		QString text;

		bool operator<(const Label& otherLabel) const
		{
			return value < otherLabel.value;
		}
	};

	//! Type for custom labels list.
	using LabelSet = std::set<Label>;

	/**
	 * @brief Get custom labels.
	 */
	const LabelSet& getLabels() const
	{
		return m_labels;
	}

	/**
	 * @brief Set custom labels.
	 */
	void setLabels(const LabelSet& labels);

	/**
	 * @brief Insert a color stop.
	 *
	 * @param[in] relativePos Position (0.0-1.0).
	 * @param[in] color Color at position.
	 *
	 * @return true on success.
	 */
	bool insert(double relativePos, const QColor& color);

	/**
	 * @brief Remove a color stop.
	 *
	 * @param[in] index Stop index.
	 *
	 * @return true on success.
	 *
	 * @note Cannot remove first or last stop.
	 */
	bool remove(int index);

	/**
	 * @brief Get number of color stops.
	 */
	unsigned stepCount() const
	{
		return static_cast<unsigned>(m_steps.size());
	}

	/**
	 * @brief Get a color stop.
	 *
	 * @param[in] index Stop index.
	 *
	 * @return Color stop element.
	 */
	const ccColorScaleElement& getStep(unsigned index) const;

	/**
	 * @brief Get interpolated color for position.
	 *
	 * @param[in] relativePos Position (0.0-1.0).
	 *
	 * @return Interpolated color.
	 */
	QColor getColorByRelativePos(double relativePos) const;

	/**
	 * @brief Get color for absolute value.
	 *
	 * @param[in] value Scalar value.
	 *
	 * @return Color at value.
	 *
	 * @note For relative scales, value should be normalized.
	 */
	QColor getColorByValue(double value) const;

	/**
	 * @brief Regenerate lookup table.
	 *
	 * @param[in] steps Number of steps (256 by default).
	 *
	 * @note Must be called after modifying stops.
	 */
	void refresh(unsigned steps = DEFAULT_STEPS);

	/**
	 * @brief Get lookup table size.
	 */
	unsigned getTableSize() const
	{
		return m_tableSize;
	}

	/**
	 * @brief Get color from lookup table.
	 *
	 * @param[in] index Table index.
	 *
	 * @return Color at index.
	 */
	const QColor& getColorByTableIndex(unsigned index) const;

	/**
	 * @brief Check if scale is empty.
	 */
	bool isEmpty() const
	{
		return m_steps.empty();
	}

	// Serialization
	/**
	 * @brief Serialize to binary.
	 */
	bool toFile(QFile& out) const override;

	/**
	 * @brief Deserialize from binary.
	 */
	bool fromFile(QFile& in, short dataVersion) override;

  protected:
	//! Scale name.
	QString m_name;

	//! Unique identifier.
	QString m_uuid;

	//! Color stops (sorted by position).
	std::vector<ccColorScaleElement> m_steps;

	//! Custom labels.
	LabelSet m_labels;

	//! Lookup table (pre-computed colors).
	std::vector<QColor> m_colorTable;

	//! Lookup table size.
	unsigned m_tableSize = 0;

	//! Relative mode flag.
	bool m_relative = true;

	//! Read-only flag.
	bool m_readOnly = false;

	//! Absolute mode boundaries.
	double m_minVal = 0.0;
	double m_maxVal = 1.0;
};

#endif // CC_COLOR_SCALE_HEADER
