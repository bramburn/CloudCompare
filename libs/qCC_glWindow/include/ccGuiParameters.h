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
 * @file ccGuiParameters.h
 *
 * @brief Per-3D-view GL rendering and display parameters
 *
 * Holds the rendering parameters that apply to each ccGLWindow individually.
 * Unlike ccOptions (cross-cutting application settings), these are stored
 * per view and control the visual appearance of entities in the 3D view.
 *
 * Groups:
 * - **Lighting**: ambient/diffuse/specular colors, double-sided lighting
 * - **Mesh defaults**: front/back diffuse colors, specular
 * - **Color defaults**: text, points, background, labels, bounding boxes
 * - **Level of Detail (LoD)**: mesh/cloud decimation during interaction
 * - **Scalar field display**: color scale bar, histogram, shader usage
 * - **Font and labels**: sizes, precision, opacity, marker size
 * - **Interaction**: zoom speed, octree auto-compute, picking cursor
 * - **Logging**: verbosity level
 *
 * Persistence: stored in QSettings. Loaded/saved via
 * ParamStruct::fromPersistentSettings() / toPersistentSettings().
 *
 * Global access: ccGui::Parameters() returns the current singleton.
 * Each ccGLWindow stores its own copy (ccViewportParameters::guiParams).
 *
 * @see ccDisplaySettingsDlg for the UI that modifies these parameters
 * @see ccOptions for application-level (not per-view) settings
 */

#include "qCC_glWindow.h"

#include <QString>
#include <ccColorTypes.h>
#include <ccLog.h>

/**
 * @class ccGui
 *
 * @brief Per-view GL rendering and display parameters
 *
 * Manages the ParamStruct singleton for per-3D-view rendering settings.
 * Access via Parameters() for read, Set() for write.
 */
class CCGLWINDOW_LIB_API ccGui
{
  public:
	/**
	 * @struct ParamStruct
	 *
	 * @brief GL rendering and display parameters
	 *
	 * These parameters control the visual appearance of entities in the
	 * 3D view. Each ccGLWindow has its own copy (linked at construction).
	 */
	struct CCGLWINDOW_LIB_API ParamStruct
	{
		// --- Lighting ---
		//! GL light diffuse color (RGBA float)
		ccColor::Rgbaf lightDiffuseColor;
		//! GL light ambient color (RGBA float)
		ccColor::Rgbaf lightAmbientColor;
		//! GL light specular color (RGBA float)
		ccColor::Rgbaf lightSpecularColor;
		//! Enable double-sided lighting (both face normals)
		bool lightDoubleSided;

		// --- Mesh defaults ---
		//! Default mesh front-face diffuse color
		ccColor::Rgbaf meshFrontDiff;
		//! Default mesh back-face diffuse color
		ccColor::Rgbaf meshBackDiff;
		//! Default mesh specular color
		ccColor::Rgbaf meshSpecular;

		// --- Entity colors ---
		//! Default 3D text color
		ccColor::Rgba textDefaultCol;
		//! Default uncolored point color
		ccColor::Rgba pointsDefaultCol;
		//! 3D view background color (RGB unsigned byte)
		ccColor::Rgbub backgroundCol;
		//! 3D label background rectangle color
		ccColor::Rgba labelBackgroundCol;
		//! 3D label leader line/marker color
		ccColor::Rgba labelMarkerCol;
		//! Default bounding box wireframe color
		ccColor::Rgba bbDefaultCol;

		// --- Rendering options ---
		//! Draw vertical gradient background
		bool drawBackgroundGradient;
		//! Auto-decimate meshes during drag (LoD)
		bool decimateMeshOnMove;
		//! Min triangle count below which mesh LoD is disabled
		unsigned minLoDMeshSize;
		//! Auto-decimate clouds during drag (LoD)
		bool decimateCloudOnMove;
		//! Min point count below which cloud LoD is disabled
		unsigned minLoDCloudSize;
		//! Draw crosshair at screen center
		bool displayCross;
		//! Use vertex buffer objects (faster for large meshes)
		bool useVBOs;

		// --- Label styling ---
		//! 3D label marker (cross/dot) size in pixels
		unsigned labelMarkerSize;

		// --- Scalar field display ---
		//! Show histogram alongside the color scale bar
		bool colorScaleShowHistogram;
		//! Use GLSL shader for color scale (faster), falls back to software
		bool colorScaleUseShader;
		//! Whether the GPU supports the color scale shader
		bool colorScaleShaderSupported;
		//! Color scale bar width in pixels
		unsigned colorScaleRampWidth;

		// --- Font and text ---
		//! Default application font size (UI font)
		unsigned defaultFontSize;
		//! 3D label font point size
		unsigned labelFontSize;
		//! Number of decimal places for displayed coordinates
		unsigned displayedNumPrecision;
		//! Label background opacity (0=transparent, 255=opaque)
		unsigned labelOpacity;

		// --- Interaction ---
		//! Mouse wheel zoom sensitivity multiplier
		double zoomSpeed;

		//! Octree auto-compute for picking behavior
		enum ComputeOctreeForPicking
		{
			ALWAYS = 0,   //!< Always compute octree before picking
			ASK_USER = 1, //!< Prompt user if octree is missing
			NEVER = 2     //!< Never compute (use brute force)
		};
		//! Auto-compute octree when picking (for large clouds)
		ComputeOctreeForPicking autoComputeOctree;

		//! Draw points as circles (slower) vs squares
		bool drawRoundedPoints;

		//! Enable single-click object picking (may be slow for large clouds)
		bool singleClickPicking;

		//! Picking cursor shape (Qt::CursorShape)
		Qt::CursorShape pickingCursorShape;

		//! Console log verbosity level
		ccLog::MessageLevelFlags logVerbosityLevel;

		/**
		 * @brief Construct with default values
		 */
		ParamStruct();

		/**
		 * @brief Reset all members to factory defaults
		 */
		void reset();

		/**
		 * @brief Load from QSettings
		 */
		void fromPersistentSettings();

		/**
		 * @brief Save to QSettings
		 */
		void toPersistentSettings() const;

		/**
		 * @brief Check if a specific parameter is stored in QSettings
		 *
		 * @param[in] paramName Parameter name (QString)
		 * @return true if the key exists in settings
		 */
		bool isInPersistentSettings(QString paramName) const;
	};

	/**
	 * @brief Get the current parameter values
	 */
	static const ParamStruct& Parameters();

	/**
	 * @brief Replace the current parameters
	 *
	 * @param[in] params New parameter values
	 */
	static void Set(const ParamStruct& params);

	/**
	 * @brief Release the singleton
	 */
	static void ReleaseInstance();

  protected:
	//! Current parameter values
	ParamStruct params;
};
