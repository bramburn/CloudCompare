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
 * @file ccColorScaleEditorWidget.h
 *
 * @brief Color scale editor widgets for creating custom color ramps.
 *
 * @details Provides widgets for visually editing color scales used in
 * scalar field visualization.
 *
 * ## Overview
 *
 * The color scale editor consists of several components:
 * - **ColorBarWidget**: Displays the color gradient
 * - **SlidersWidget**: Draggable color stops
 * - **SliderLabelWidget**: Labels showing position values
 * - **ccColorScaleEditorWidget**: Main editor combining all components
 *
 * ## Color Scale Structure
 *
 * Color scales are defined by:
 * - A set of color stops at relative positions [0.0, 1.0]
 * - Each stop has a position and a color
 * - Linear interpolation between stops
 *
 * ## Usage
 *
 * @code
 * ccColorScale::Shared scale = ccColorScale::Create("My Scale");
 *
 * ccColorScaleEditorWidget editor(this);
 * editor.importColorScale(scale);
 *
 * if (editor.exec() == QDialog::Accepted) {
 *     editor.exportColorScale(scale);
 * }
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccColorScale for the color scale data structure
 */

#pragma once

// Inspired from ccColorScaleEditorWidget by Richard Steffen (LGPL 2.1)

#include "CCPluginAPI.h"

// Qt
#include <QWidget>

// qCC_db
#include <ccColorScale.h>

/**
 * @brief Single color stop as an interactive widget.
 *
 * @details A draggable widget representing a single color stop
 * in a color scale.
 *
 * @extends QWidget
 */
class CCPLUGIN_LIB_API ColorScaleElementSlider : public QWidget
    , public ccColorScaleElement
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct a color element slider.
	 *
	 * @param[in] relativePos Position (0.0-1.0).
	 * @param[in] color Element color.
	 * @param[in] parent Parent widget.
	 * @param[in] orientation Horizontal or vertical.
	 */
	ColorScaleElementSlider(double          relativePos = 0.0,
	                        QColor          color       = Qt::black,
	                        QWidget*        parent      = nullptr,
	                        Qt::Orientation orientation = Qt::Horizontal);

	/**
	 * @brief Set selection state.
	 *
	 * @param[in] state Selection state.
	 */
	void setSelected(bool state)
	{
		m_selected = state;
	}

	/**
	 * @brief Get selection state.
	 *
	 * @return true if selected.
	 */
	bool isSelected() const
	{
		return m_selected;
	}

	/**
	 * @brief Compare two elements by position.
	 */
	static bool IsSmaller(const ColorScaleElementSlider* e1, const ColorScaleElementSlider* e2)
	{
		return e1->getRelativePos() < e2->getRelativePos();
	}

  protected:
	//! Paint the element.
	void paintEvent(QPaintEvent* e) override;

  private:
	//! Selection state
	bool m_selected;

	//! Orientation
	Qt::Orientation m_orientation;
};

/**
 * @brief Container for color element sliders.
 *
 * @details Manages a set of color stop widgets with sorting
 * and selection capabilities.
 */
class CCPLUGIN_LIB_API ColorScaleElementSliders
{
  public:
	//! Type for the set of elements.
	using Set = QList<ColorScaleElementSlider*>;

	/**
	 * @brief Add a slider and sort.
	 *
	 * @param[in] slider Slider to add.
	 */
	void addSlider(ColorScaleElementSlider* slider);

	/**
	 * @brief Get number of sliders.
	 *
	 * @return Count.
	 */
	int size() const
	{
		return m_list.size();
	}

	/**
	 * @brief Sort sliders by position.
	 */
	void sort();

	/**
	 * @brief Clear all sliders.
	 */
	void clear();

	/**
	 * @brief Remove slider at index.
	 *
	 * @param[in] i Index.
	 */
	void removeAt(int i);

	/**
	 * @brief Get selected slider index.
	 *
	 * @return Index, or -1.
	 */
	int selected() const;

	/**
	 * @brief Get index of slider.
	 *
	 * @param[in] slider Slider.
	 *
	 * @return Index.
	 */
	int indexOf(ColorScaleElementSlider* slider);

	/**
	 * @brief Get slider at index.
	 *
	 * @param[in] index Slider index.
	 *
	 * @return Slider widget.
	 */
	ColorScaleElementSlider* element(int index)
	{
		return m_list.at(index);
	}

	/**
	 * @brief Get slider at index (const).
	 */
	const ColorScaleElementSlider* element(int index) const
	{
		return m_list.at(index);
	}

	/**
	 * @brief Get all sliders.
	 */
	Set& elements()
	{
		return m_list;
	}

	/**
	 * @brief Get all sliders (const).
	 */
	const Set& elements() const
	{
		return m_list;
	}

  private:
	//! Set of slider widgets.
	Set m_list;
};

//! Shared pointer to slider set.
using SharedColorScaleElementSliders = QSharedPointer<ColorScaleElementSliders>;

/**
 * @brief Base widget with margin.
 *
 * @details Base class for editor components with configurable margin.
 */
class CCPLUGIN_LIB_API ColorScaleEditorBaseWidget : public QWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct base widget.
	 *
	 * @param[in] sliders Slider set.
	 * @param[in] orientation Layout orientation.
	 * @param[in] margin Content margin.
	 * @param[in] parent Parent widget.
	 */
	ColorScaleEditorBaseWidget(SharedColorScaleElementSliders sliders,
	                           Qt::Orientation                orientation,
	                           int                            margin,
	                           QWidget*                       parent = nullptr)
	    : QWidget(parent)
	    , m_sliders(sliders)
	    , m_orientation(orientation)
	    , m_margin(margin)
	{
	}

	/**
	 * @brief Get content length.
	 *
	 * @return Length minus margins.
	 */
	int length() const
	{
		return (m_orientation == Qt::Horizontal ? contentsRect().width() : contentsRect().height()) - 2 * m_margin;
	}

	/**
	 * @brief Set sliders.
	 *
	 * @param[in] sliders New slider set.
	 */
	virtual void setSliders(SharedColorScaleElementSliders sliders)
	{
		m_sliders = sliders;
		update();
	}

	/**
	 * @brief Get orientation.
	 */
	Qt::Orientation getOrientation() const
	{
		return m_orientation;
	}

	/**
	 * @brief Get margin.
	 */
	int getMargin() const
	{
		return m_margin;
	}

  protected:
	//! Slider set.
	SharedColorScaleElementSliders m_sliders;

	//! Orientation.
	Qt::Orientation m_orientation;

	//! Margin.
	int m_margin;
};

/**
 * @brief Color gradient bar display.
 *
 * @details Shows the color gradient from start to end.
 */
class CCPLUGIN_LIB_API ColorBarWidget : public ColorScaleEditorBaseWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct color bar.
	 *
	 * @param[in] sliders Slider set.
	 * @param[in] parent Parent widget.
	 * @param[in] orientation Layout orientation.
	 */
	ColorBarWidget(SharedColorScaleElementSliders sliders, QWidget* parent = nullptr, Qt::Orientation orientation = Qt::Horizontal);

  signals:
	/**
	 * @brief Emitted when bar is clicked.
	 *
	 * @param[in] relativePos Click position (0.0-1.0).
	 */
	void pointClicked(double relativePos);

  protected:
	//! Paint the gradient.
	void paintEvent(QPaintEvent* e) override;

	//! Handle mouse click.
	void mousePressEvent(QMouseEvent* e) override;
};

/**
 * @brief Interactive color stop sliders.
 *
 * @details Displays draggable color stop widgets for editing
 * a color scale.
 */
class CCPLUGIN_LIB_API SlidersWidget : public ColorScaleEditorBaseWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct sliders widget.
	 *
	 * @param[in] sliders Slider set.
	 * @param[in] parent Parent widget.
	 * @param[in] orientation Layout orientation.
	 */
	SlidersWidget(SharedColorScaleElementSliders sliders, QWidget* parent = nullptr, Qt::Orientation orientation = Qt::Horizontal);

	/**
	 * @brief Select a slider.
	 *
	 * @param[in] index Slider index.
	 * @param[in] silent Suppress signals.
	 */
	void select(int index, bool silent = false);

	/**
	 * @brief Add a new color stop.
	 *
	 * @param[in] relativePos Position (0.0-1.0).
	 * @param[in] color Stop color.
	 *
	 * @return Created slider.
	 */
	ColorScaleElementSlider* addNewSlider(double relativePos, QColor color);

	/**
	 * @brief Update slider position.
	 *
	 * @param[in] index Slider index.
	 */
	void updateSliderPos(int index);

	/**
	 * @brief Update all slider positions.
	 */
	void updateAllSlidersPos();

  signals:
	/**
	 * @brief Emitted when slider changes.
	 *
	 * @param[in] index Slider index.
	 */
	void sliderModified(int index);

	/**
	 * @brief Emitted when slider selected.
	 *
	 * @param[in] index Slider index.
	 */
	void sliderSelected(int index);

  protected:
	//! Handle mouse press.
	void mousePressEvent(QMouseEvent* e) override;

	//! Handle mouse move.
	void mouseMoveEvent(QMouseEvent* e) override;

	//! Handle double-click.
	void mouseDoubleClickEvent(QMouseEvent* e) override;

	//! Handle resize.
	void resizeEvent(QResizeEvent* e) override;
};

/**
 * @brief Labels for slider positions.
 *
 * @details Shows position values below sliders.
 */
class CCPLUGIN_LIB_API SliderLabelWidget : public ColorScaleEditorBaseWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct label widget.
	 *
	 * @param[in] sliders Slider set.
	 * @param[in] parent Parent widget.
	 * @param[in] orientation Layout orientation.
	 */
	SliderLabelWidget(SharedColorScaleElementSliders sliders, QWidget* parent = nullptr, Qt::Orientation orientation = Qt::Horizontal);

	/**
	 * @brief Set text color.
	 *
	 * @param[in] color Text color.
	 */
	void setTextColor(QColor color)
	{
		m_textColor = color;
	}

	/**
	 * @brief Set number precision.
	 *
	 * @param[in] precision Decimal places.
	 */
	void setPrecision(int precision)
	{
		m_precision = precision;
	}

  protected:
	//! Paint labels.
	void paintEvent(QPaintEvent* e) override;

  private:
	//! Text color.
	QColor m_textColor;

	//! Decimal precision.
	int m_precision;
};

/**
 * @brief Main color scale editor widget.
 *
 * @details Complete editor for creating and editing color scales
 * with interactive color stops.
 */
class CCPLUGIN_LIB_API ccColorScaleEditorWidget : public ColorScaleEditorBaseWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the editor.
	 *
	 * @param[in] parent Parent widget.
	 * @param[in] orientation Layout orientation.
	 */
	ccColorScaleEditorWidget(QWidget* parent = nullptr, Qt::Orientation orientation = Qt::Horizontal);

	/**
	 * @brief Destructor.
	 */
	~ccColorScaleEditorWidget() override = default;

	/**
	 * @brief Get number of color steps.
	 *
	 * @return Step count.
	 */
	int getStepCount() const
	{
		return (m_sliders ? m_sliders->size() : 0);
	}

	/**
	 * @brief Get step at index.
	 *
	 * @param[in] index Step index.
	 *
	 * @return Slider widget.
	 */
	const ColorScaleElementSlider* getStep(int index)
	{
		return m_sliders ? m_sliders->elements().at(index) : nullptr;
	}

	/**
	 * @brief Set step color.
	 *
	 * @param[in] index Step index.
	 * @param[in] color New color.
	 */
	void setStepColor(int index, QColor color);

	/**
	 * @brief Set step position.
	 *
	 * @param[in] index Step index.
	 * @param[in] relativePos New position (0.0-1.0).
	 */
	void setStepRelativePosition(int index, double relativePos);

	/**
	 * @brief Get selected step index.
	 *
	 * @return Index, or -1.
	 */
	int getSelectedStepIndex() const
	{
		return m_sliders ? m_sliders->selected() : -1;
	}

	/**
	 * @brief Set selected step.
	 *
	 * @param[in] index Step index.
	 * @param[in] silent Suppress signals.
	 */
	void setSelectedStepIndex(int index, bool silent = false);

	/**
	 * @brief Delete a step.
	 *
	 * @param[in] index Step to delete.
	 *
	 * @note First and last steps cannot be deleted.
	 */
	void deleteStep(int index);

	/**
	 * @brief Export to color scale.
	 *
	 * @param[out] destScale Destination scale.
	 */
	void exportColorScale(ccColorScale::Shared& destScale) const;

	/**
	 * @brief Import from color scale.
	 *
	 * @param[in] scale Source scale.
	 */
	void importColorScale(ccColorScale::Shared scale);

	/**
	 * @brief Show/hide labels.
	 *
	 * @param[in] state Show state.
	 */
	void showLabels(bool state);

	/**
	 * @brief Set label color.
	 *
	 * @param[in] color Label color.
	 */
	void setLabelColor(QColor color);

	/**
	 * @brief Set label precision.
	 *
	 * @param[in] precision Decimal places.
	 */
	void setLabelPrecision(int precision);

	// From ColorScaleEditorBaseWidget
	/**
	 * @brief Set sliders.
	 */
	void setSliders(SharedColorScaleElementSliders sliders) override;

  signals:
	/**
	 * @brief Emitted when step selected.
	 *
	 * @param[in] index Step index.
	 */
	void stepSelected(int index);

	/**
	 * @brief Emitted when step modified.
	 *
	 * @param[in] index Step index.
	 */
	void stepModified(int index);

  protected slots:
	//! Handle point clicked.
	void onPointClicked(double relativePos);

	//! Handle slider modified.
	void onSliderModified(int sliderIndex);

	//! Handle slider selected.
	void onSliderSelected(int sliderIndex);

  private:
	//! Color bar widget.
	ColorBarWidget* m_colorBarWidget;

	//! Sliders widget.
	SlidersWidget* m_slidersWidget;

	//! Labels widget.
	SliderLabelWidget* m_labelsWidget;
};
