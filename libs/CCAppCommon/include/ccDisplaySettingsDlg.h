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

#include "CCAppCommon.h"

/**
 * @file ccDisplaySettingsDlg.h
 *
 * @brief Display settings dialog
 *
 * Modal dialog for configuring all CloudCompare appearance settings:
 * - GL lighting parameters (ambient, diffuse, specular colors and shininess)
 * - Entity default colors (points, meshes, text, bounding boxes)
 * - 3D label appearance (font size, background, opacity, marker)
 * - GL rendering options (VBO usage, octree auto-compute, cursor size)
 * - Application behavior (zoom speed, number precision, log verbosity)
 *
 * Settings are stored persistently via QSettings and applied globally
 * via ccGui::Parameters and ccOptions singletons.
 *
 * @see ccGui::ParamStruct for the GL rendering parameters
 * @see ccOptions for application-level options
 * @see ccMainAppInterface::showDisplayOptions() for dialog invocation
 */

#include "ccOptions.h"

#include <QDialog>
#include <ccGuiParameters.h>

namespace Ui
{
	class DisplaySettingsDlg;
}

/**
 * @class ccDisplaySettingsDlg
 *
 * @brief Display settings configuration dialog
 *
 * Tabbed or sectioned dialog that exposes all ccGui and ccOptions
 * settings. Changes are applied immediately (apply()) or on OK (doAccept()).
 * Cancel reverts to the snapshot taken at dialog open.
 *
 * Key color settings:
 * - GL light model: ambient, diffuse, specular
 * - Mesh: separate front/back diffuse colors
 * - Default entity colors: points, text, background
 * - Label styling: background, marker, font size, opacity
 *
 * Key rendering settings:
 * - VBO usage: enable/disable vertex buffer objects
 * - Picking cursor size
 * - Zoom speed
 * - Auto-compute octree on load
 *
 * @extends QDialog
 */
class CCAPPCOMMON_LIB_API ccDisplaySettingsDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the display settings dialog
	 *
	 * Captures current settings (m_oldParameters, m_oldOptions) for
	 * potential revert on cancel.
	 *
	 * @param[in] parent Parent widget
	 */
	explicit ccDisplaySettingsDlg(QWidget* parent);

	/**
	 * @brief Destructor
	 */
	~ccDisplaySettingsDlg() override;

  signals:
	/**
	 * @brief Emitted when any display parameter changes
	 *
	 * Connected to ccGLWindow slots to trigger viewport redraw.
	 */
	void aspectHasChanged();

  protected:
	// Color change slots (each opens a QColorDialog)
	/**
	 * @brief Change light diffuse color (3D view lighting)
	 */
	void changeLightDiffuseColor();
	/**
	 * @brief Change light ambient color (3D view lighting)
	 */
	void changeLightAmbientColor();
	/**
	 * @brief Change light specular color (3D view lighting)
	 */
	void changeLightSpecularColor();
	/**
	 * @brief Change mesh front-face diffuse color
	 */
	void changeMeshFrontDiffuseColor();
	/**
	 * @brief Change mesh back-face diffuse color
	 */
	void changeMeshBackDiffuseColor();
	/**
	 * @brief Change mesh specular color and shininess
	 */
	void changeMeshSpecularColor();
	/**
	 * @brief Change default point color (when cloud has no per-point color)
	 */
	void changePointsColor();
	/**
	 * @brief Change bounding box wireframe color
	 */
	void changeBBColor();
	/**
	 * @brief Change default text color (labels, annotations)
	 */
	void changeTextColor();
	/**
	 * @brief Change 3D view background color
	 */
	void changeBackgroundColor();
	/**
	 * @brief Change label background rectangle color
	 */
	void changeLabelBackgroundColor();
	/**
	 * @brief Change label leader line/marker color
	 */
	void changeLabelMarkerColor();

	// Numeric parameter slots
	/**
	 * @brief Change max triangle count for mesh display
	 *
	 * Meshes above this count are replaced with bounding box.
	 *
	 * @param[in] sizeIn Thousands of triangles
	 */
	void changeMaxMeshSize(double sizeIn);
	/**
	 * @brief Change max point count for point cloud display
	 *
	 * Clouds above this count are subsampled for display.
	 *
	 * @param[in] sizeIn Thousands of points
	 */
	void changeMaxCloudSize(double sizeIn);
	/**
	 * @brief Toggle vertex buffer object usage
	 *
	 * VBO can improve rendering performance for large meshes.
	 *
	 * @param[in] useVBO 0 = off, 1 = on
	 */
	void changeVBOUsage();
	/**
	 * @brief Change scalar field color scale bar width in pixels
	 *
	 * @param[in] width New width
	 */
	void changeColorScaleRampWidth(int width);
	/**
	 * @brief Change picking crosshair cursor size
	 *
	 * @param[in] sizeInPixels Crosshair half-size
	 */
	void changePickingCursor(int sizeInPixels);
	/**
	 * @brief Change console log verbosity level
	 *
	 * @param[in] level 0 = errors only, 1 = warnings, 2 = standard, 3 = verbose
	 */
	void changeLogVerbosity(int level);

	// Font and label styling slots
	/**
	 * @brief Change default application font size
	 *
	 * @param[in] pointSize Font point size
	 */
	void changeDefaultFontSize(int pointSize);
	/**
	 * @brief Change 3D label font size
	 *
	 * @param[in] pointSize Font point size
	 */
	void changeLabelFontSize(int pointSize);
	/**
	 * @brief Change numeric coordinate precision in labels
	 *
	 * @param[in] precision Number of decimal places
	 */
	void changeNumberPrecision(int precision);
	/**
	 * @brief Change label background opacity
	 *
	 * @param[in] opacity 0 = transparent, 255 = opaque
	 */
	void changeLabelOpacity(int opacity);
	/**
	 * @brief Change label leader line marker size
	 *
	 * @param[in] markerSizeInPixels Marker half-size
	 */
	void changeLabelMarkerSize(int markerSizeInPixels);

	/**
	 * @brief Change mouse wheel zoom sensitivity
	 *
	 * @param[in] speed Zoom speed factor
	 */
	void changeZoomSpeed(double speed);

	/**
	 * @brief Change automatic octree computation after cloud loading
	 *
	 * @param[in] mode 0 = never, 1 = always, 2 = ask user
	 */
	void changeAutoComputeOctreeOption(int mode);

	/**
	 * @brief Apply changes and close dialog (OK button)
	 */
	void doAccept();
	/**
	 * @brief Revert changes and close dialog (Cancel button)
	 */
	void doReject();
	/**
	 * @brief Apply changes without closing (Apply button)
	 */
	void apply();
	/**
	 * @brief Reset all settings to defaults
	 */
	void reset();

  protected:
	/**
	 * @brief Refresh all dialog controls from current parameters
	 *
	 * Called at dialog open and after apply/reset.
	 */
	void refresh();

	// Color state
	QColor m_lightDiffuseColor;
	QColor m_lightAmbientColor;
	QColor m_lightSpecularColor;
	QColor m_meshFrontDiff;
	QColor m_meshBackDiff;
	QColor m_meshSpecularColor;
	QColor m_pointsDefaultCol;
	QColor m_textDefaultCol;
	QColor m_backgroundCol;
	QColor m_labelBackgroundCol;
	QColor m_labelMarkerCol;
	QColor m_bbDefaultCol;

	//! Current GUI parameters (applied on accept/apply)
	ccGui::ParamStruct m_parameters;
	//! Current application options (applied on accept/apply)
	ccOptions m_options;

	//! Snapshot at dialog open (reverted on cancel)
	ccGui::ParamStruct m_oldParameters;
	//! Snapshot at dialog open (reverted on cancel)
	ccOptions m_oldOptions;

	//! Style index at dialog open (for style revert)
	int m_defaultAppStyleIndex;

  private:
	//! Qt Designer-generated UI
	Ui::DisplaySettingsDlg* m_ui;
};
