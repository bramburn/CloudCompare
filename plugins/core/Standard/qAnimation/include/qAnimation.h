#pragma once

// ##########################################################################
// #                                                                        #
// #                   CLOUDCOMPARE PLUGIN: qAnimation                      #
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
// #             COPYRIGHT: Ryan Wicks, 2G Robotics Inc., 2015              #
// #                                                                        #
// ##########################################################################

/**
 * @file qAnimation.h
 *
 * @brief Camera animation capture plugin
 *
 * Captures camera animation sequences from the 3D view by interpolating
 * between saved viewpoint objects (cc2DViewportObject).
 *
 * Requires at least 2 cc2DViewportObject entities to be selected.
 * The plugin interpolates camera parameters (position, orientation, zoom)
 * between the selected viewports to create smooth animation sequences.
 *
 * Export formats: image sequence (PNG/JPG) or video (AVI/MP4).
 *
 * Use case: documenting scan processing workflows, creating turntable
 * demonstrations, fly-through animations of 3D models.
 *
 * @author Ryan Wicks, 2G Robotics Inc.
 */

#include "ccStdPluginInterface.h"

#include <QObject>
#include <vector>

// Local
#include "cc2DViewportObject.h"

/**
 * @class qAnimation
 *
 * @brief Camera animation capture plugin
 *
 * Interpolates camera paths between saved viewport objects and exports
 * the resulting animation as images or video.
 *
 * Activation requirements:
 * - Exactly 2+ cc2DViewportObject entities must be selected
 * - An active 3D view must be open
 *
 * Workflow:
 * 1. User selects 2+ saved viewport objects in the DB tree
 * 2. Triggers the "Animation" action
 * 3. Plugin opens qAnimationDlg with the selected viewports
 * 4. User configures frame rate, output format, etc.
 * 5. Plugin iterates through interpolated camera positions, captures each frame
 *
 * @extends QObject
 * @extends ccStdPluginInterface
 * @implements ccStdPluginInterface
 */
class qAnimation : public QObject
    , public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccStdPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qAnimation" FILE "../info.json")

  public:
	//! Extended viewport (typedef for the dialog's viewports container)
	typedef std::vector<ExtendedViewport> ViewPortList;

	//! Get selected viewport objects from a selection container
	static ViewPortList GetSelectedViewPorts(const ccHObject::Container& selectedEntities);

  public:
	/**
	 * @brief Construct the animation plugin
	 *
	 * @param[in] parent QObject parent
	 */
	qAnimation(QObject* parent = nullptr);

	/**
	 * @brief Destructor
	 */
	virtual ~qAnimation() = default;

	// ccStdPluginInterface

	/**
	 * @brief Handle new entity selection
	 *
	 * Enables the action when at least 2 cc2DViewportObject entities
	 * are selected. Otherwise shows a tooltip explaining the requirement.
	 *
	 * @param[in] selectedEntities Current selection
	 */
	void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/**
	 * @brief Get the plugin's actions
	 *
	 * Returns the "Animation" action for the Plugins menu.
	 *
	 * @return List containing the animation action
	 */
	virtual QList<QAction*> getActions() override;

  private:
	/**
	 * @brief Execute the animation tool
	 *
	 * Validates active view, backs up GL state, opens animation dialog,
	 * restores GL state on exit, optionally exports the camera path as polyline.
	 */
	void doAction();

	//! "Animation" action for the Plugins menu
	QAction* m_action;
};
