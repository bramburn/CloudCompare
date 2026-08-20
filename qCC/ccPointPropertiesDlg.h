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

#include "ccPointPickingGenericInterface.h"

/**
 * @file ccPointPropertiesDlg.h
 *
 * @brief Point properties dialog for picking, distance, and angle measurement.
 *
 * @details Dialog for interactive point picking with measurement capabilities:
 * - Point information display (coordinates, scalar values)
 * - Point-to-point distance measurement
 * - Angle measurement between three points
 * - Rectangular zone picking
 *
 * Extends ccPointPickingGenericInterface for the picking infrastructure.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPointPickingGenericInterface
 * @see cc2DLabel
 */

#include <ui_pointPropertiesDlg.h>

class cc2DLabel;
class cc2DViewportLabel;
class ccHObject;

/**
 * @brief Dialog for point picking and measurements.
 *
 * @details Provides interactive point picking with multiple modes:
 *
 * 1. **Point Info Mode**: Click on a point to see its properties
 *    (coordinates, scalar field values, color, etc.)
 *
 * 2. **Point-to-Point Distance**: Click two points to measure
 *    the distance between them
 *
 * 3. **Angle Mode**: Click three points to measure the angle
 *    at the middle point
 *
 * 4. **Rectangular Zone**: Define a rectangular region by
 *    clicking two opposite corners
 *
 * Labels are created for each measurement and can be exported.
 *
 * @extends ccPointPickingGenericInterface
 * @extends Ui::PointPropertiesDlg
 */
class ccPointPropertiesDlg : public ccPointPickingGenericInterface
    , public Ui::PointPropertiesDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Default constructor.
	 *
	 * @param[in] pickingHub Picking hub for point selection.
	 * @param[in] parent Parent widget.
	 */
	explicit ccPointPropertiesDlg(ccPickingHub* pickingHub, QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccPointPropertiesDlg();

	// inherited from ccPointPickingGenericInterface
	/**
	 * @brief Start the dialog.
	 * @return true on success.
	 */
	virtual bool start() override;

	/**
	 * @brief Stop the dialog.
	 * @param[in] state Final state.
	 */
	virtual void stop(bool state) override;

	/**
	 * @brief Link with a 3D window.
	 * @param[in] win Window to link with.
	 * @return true on success.
	 */
	virtual bool linkWith(ccGLWindowInterface* win) override;

  signals:
	/**
	 * @brief Emitted when a new label is created.
	 * @param[in] label The created label.
	 */
	void newLabel(ccHObject* label);

  protected slots:
	/**
	 * @brief Close the dialog.
	 */
	void onClose();

	/**
	 * @brief Activate point properties display mode.
	 */
	void activatePointPropertiesDisplay();

	/**
	 * @brief Activate distance display mode.
	 */
	void activateDistanceDisplay();

	/**
	 * @brief Activate angle display mode.
	 */
	void activateAngleDisplay();

	/**
	 * @brief Activate 2D zone picking mode.
	 */
	void activate2DZonePicking();

	/**
	 * @brief Initialize the dialog state.
	 */
	void initializeState();

	/**
	 * @brief Export the current label.
	 */
	void exportCurrentLabel();

	/**
	 * @brief Update 2D zone rectangle.
	 * @param[in] x X coordinate.
	 * @param[in] y Y coordinate.
	 * @param[in] buttons Mouse buttons pressed.
	 */
	void update2DZone(int x, int y, Qt::MouseButtons buttons);

	/**
	 * @brief Process clicked point.
	 * @param[in] x X coordinate.
	 * @param[in] y Y coordinate.
	 */
	void processClickedPoint(int x, int y);

	/**
	 * @brief Close the 2D zone.
	 */
	void close2DZone();

	/**
	 * @brief Handle shortcut trigger.
	 * @param[in] id Shortcut ID.
	 */
	void onShortcutTriggered(int id);

  protected:
	/**
	 * @brief Picking modes.
	 */
	enum Mode
	{
		POINT_INFO,         //!< Display point information
		POINT_POINT_DISTANCE, //!< Measure distance between two points
		POINTS_ANGLE,        //!< Measure angle at middle point
		RECT_ZONE            //!< Define rectangular zone
	};

	// inherited from ccPointPickingGenericInterface
	/**
	 * @brief Process a picked point.
	 * @param[in] picked Picked item information.
	 */
	void processPickedPoint(const PickedItem& picked) override;

	//! Current picking mode
	Mode m_pickingMode;

	//! 3D label for displaying measurements
	cc2DLabel* m_label;

	//! 2D viewport label for rectangular zone
	cc2DViewportLabel* m_rect2DLabel;
};
