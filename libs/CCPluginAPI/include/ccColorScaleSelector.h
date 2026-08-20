// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
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
 * @file ccColorScaleSelector.h
 *
 * @brief Color scale selector widget for scalar field visualization.
 *
 * @details Widget for selecting and managing color scales used to
 * display scalar field values in 3D views.
 *
 * ## Overview
 *
 * Color scales map scalar field values to colors. This widget provides:
 * - Dropdown selection of available scales
 * - Access to the color scale editor
 * - Integration with color scales manager
 *
 * ## Built-in Scales
 *
 * CloudCompare includes several built-in color scales:
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
 * ccColorScaleSelector* selector = new ccColorScaleSelector(manager, this);
 * selector->init();
 *
 * connect(selector, &ccColorScaleSelector::colorScaleSelected,
 *         this, &MyClass::onScaleSelected);
 *
 * ccColorScale::Shared scale = selector->getSelectedScale();
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccColorScale for color scale implementation
 * @see ccColorScalesManager for scale management
 */

#pragma once

#include "CCPluginAPI.h"

// Qt
#include <QFrame>

// qCC_db
#include <ccColorScale.h>

class QComboBox;
class QToolButton;
class ccColorScalesManager;

/**
 * @brief Color scale selector widget.
 *
 * @details Provides a combo box for selecting color scales with
 * a button to open the color scale editor.
 *
 * Features:
 * - Dropdown scale selection
 * - Built-in and custom scales
 * - Editor access
 *
 * @extends QFrame
 */
class CCPLUGIN_LIB_API ccColorScaleSelector : public QFrame
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the color scale selector.
	 *
	 * @param[in] manager Color scales manager.
	 * @param[in] parent Parent widget.
	 * @param[in] defaultButtonIconPath Path to button icon.
	 */
	ccColorScaleSelector(ccColorScalesManager* manager, QWidget* parent, QString defaultButtonIconPath = QString());

	/**
	 * @brief Initialize the selector.
	 *
	 * Populates the combo box with available scales.
	 */
	void init();

	/**
	 * @brief Set selected scale by UUID.
	 *
	 * @param[in] uuid Scale UUID string.
	 */
	void setSelectedScale(QString uuid);

	/**
	 * @brief Get the selected color scale.
	 *
	 * @return Selected scale, or nullptr.
	 */
	ccColorScale::Shared getSelectedScale() const;

	/**
	 * @brief Get a color scale by index.
	 *
	 * @param[in] index Scale index in combo box.
	 *
	 * @return Scale, or nullptr.
	 */
	ccColorScale::Shared getScale(int index) const;

  signals:
	/**
	 * @brief Emitted when a color scale is selected.
	 *
	 * @param[in] index Selected index.
	 */
	void colorScaleSelected(int);

	/**
	 * @brief Emitted when editor button is clicked.
	 */
	void colorScaleEditorSummoned();

  private:
	//! Color scales manager
	ccColorScalesManager* m_manager;

	//! Scale combo box
	QComboBox* m_comboBox;

	//! Editor button
	QToolButton* m_button;
};
