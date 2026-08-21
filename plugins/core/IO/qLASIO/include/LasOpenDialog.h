#pragma once

// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        #
// ##########################################################################

/**
 * @file LasOpenDialog.h
 *
 * @brief LAS/LAZ file open dialog — field selection, COPC, and tiling options
 *
 * Modal dialog presented to the user when opening a LAS/LAZ file.
 * Supports three modes (selected by tabs):
 *
 * 1. **Load tab**: Select which scalar fields and extra attributes to import.
 *    - Standard LAS fields (Intensity, Return Number, Classification, etc.)
 *    - Extra attributes (from LAS 1.4 EDR)
 *    - Extra fields as normals (RGB normals from attributes)
 *    - Color handling: 8-bit vs 16-bit RGB
 *    - Classification decomposition (byte → individual flag bits)
 *    - Ignore fields with default values
 *
 * 2. **COPC tab** (shown only for COPC LAZ files):
 *    - Max level: LOD constraint (load coarser/more detailed levels)
 *    - Spatial extent: clip to a sub-region (X/Y/Z bounds)
 *
 * 3. **Tile tab**: Split the file into a grid of output tiles.
 *    - Dimension: XY, XZ, or YZ tiling plane
 *    - Grid size: numTiles0 × numTiles1
 *    - Output directory
 *
 * State is persistent: remembers last settings via QSettings and per-session
 * via m_shouldSkipDialog flag.
 */

#include "LasDetails.h"
#include "LasExtraScalarField.h"
#include "LasScalarField.h"
#include "LasTiler.h"

#include <ui_lasopendialog.h>

// CCCoreLib
#include <CCGeom.h>
#include <ccLog.h>

// system
#include <string>
#include <vector>

/**
 * @class LasOpenDialog
 *
 * @brief LAS/LAZ file import dialog
 *
 * Modal dialog for configuring import options. Supports:
 * - Field selection (standard + extra attributes)
 * - COPC LOD and spatial extent filtering
 * - File tiling
 *
 * @extends QDialog
 */
class LasOpenDialog : public QDialog
    , public Ui::LASOpenDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief User's chosen action
	 */
	enum class Action
	{
		Load, //!< Import into CloudCompare
		Tile, //!< Split into tiles
	};

	/**
	 * @brief Construct the dialog
	 *
	 * @param[in] parent Parent widget
	 */
	explicit LasOpenDialog(QWidget* parent = nullptr);

	/**
	 * @brief Set file metadata for display
	 *
	 * Shows version, point format, and point count to the user.
	 *
	 * @param[in] versionMinor LAS minor version (2, 3, or 4)
	 * @param[in] pointFormatId LAS point format (0-10)
	 * @param[in] numPoints Total point count
	 */
	void setInfo(int versionMinor, int pointFormatId, qulonglong numPoints);

	/**
	 * @brief Populate available fields for user selection
	 *
	 * Fills the field list checkboxes with all standard LAS scalar
	 * fields for this point format and all extra attributes.
	 *
	 * @param[in] scalarFields Available standard fields
	 * @param[in] extraScalarFields Available extra attributes
	 */
	void setAvailableScalarFields(const std::vector<LasScalarField>&      scalarFields,
	                              const std::vector<LasExtraScalarField>& extraScalarFields);

	/**
	 * @brief Remove unchecked fields from the active selection
	 *
	 * Reads the checkboxes and removes unchecked fields from
	 * the active scalar field lists (used by LasIOFilter::loadFile).
	 *
	 * @param[in,out] scalarFields Scalar fields (unchecked removed)
	 * @param[in,out] extraScalarFields Extra fields (unchecked removed)
	 */
	void filterOutNotChecked(std::vector<LasScalarField>&      scalarFields,
	                         std::vector<LasExtraScalarField>& extraScalarFields);

	/**
	 * @brief Show or hide the COPC tab
	 *
	 * @param[in] visibilityState true to show, false to hide
	 */
	void displayCopcTab(bool visibilityState);

	/**
	 * @brief Fill COPC tab with file metadata
	 *
	 * @param[in] level_point_count Points per LOD level
	 * @param[in] copcBB Bounding box of the COPC file
	 */
	void setCopcInformations(const std::vector<uint64_t>& level_point_count, const LasDetails::UnscaledExtent& copcBB);

	/**
	 * @brief Get extra fields to be loaded as normals
	 *
	 * Returns up to 3 extra scalar fields that the user selected
	 * to interpret as normal vector components (Nx, Ny, Nz).
	 *
	 * @param[in] extraScalarFields All extra fields
	 * @return Array of 3 fields (unused slots = Undocumented type)
	 */
	std::array<LasExtraScalarField, 3> getExtraFieldsToBeLoadedAsNormals(const std::vector<LasExtraScalarField>& extraScalarFields) const;

	/**
	 * @brief Ignore fields with all-default values
	 */
	bool shouldIgnoreFieldsWithDefaultValues() const;

	/**
	 * @brief Treat RGB values as 8-bit components
	 *
	 * LAS RGB can be 8-bit or 16-bit per component. If the file
	 * uses 16-bit but the user checks this, values are scaled to 8-bit.
	 */
	bool shouldForce8bitColors() const;

	/**
	 * @brief Decompose the Classification byte
	 *
	 * LAS 1.4+ stores classification as a byte with bit flags
	 * (synthetic, keypoint, withheld, etc.). If checked, the
	 * single Classification byte is split into multiple scalar fields.
	 */
	bool shouldDecomposeClassification() const;

	/**
	 * @brief Get the user's chosen action
	 *
	 * Determined by which tab is active when the dialog is accepted.
	 *
	 * @return Action::Load or Action::Tile
	 */
	Action action() const;

	/**
	 * @brief Get tiling options (valid only when action == Tile)
	 */
	LasTilingOptions tilingOptions() const;

	/**
	 * @brief Check if a valid COPC extent was set
	 */
	bool hasUsableExtent() const;

	/**
	 * @brief Get the maximum COPC LOD level to load
	 */
	uint32_t copcMaxLevel() const;

	/**
	 * @brief Get the COPC spatial extent for clipping
	 */
	LasDetails::UnscaledExtent copcExtent() const;

	/**
	 * @brief Reset the skip-dialog flag
	 *
	 * Forces the dialog to show even if the user previously
	 * chose "don't ask again".
	 */
	void resetShouldSkipDialog();

	/**
	 * @brief Check if the dialog should be skipped
	 *
	 * Returns true if the user previously selected "don't ask again"
	 * and the file structure hasn't changed.
	 */
	bool shouldSkipDialog() const;

  private:
	//! Check if a standard scalar field is checked in the UI
	bool isChecked(const LasScalarField& lasScalarField) const;

	//! Check if an extra field is checked in the UI
	bool isChecked(const LasExtraScalarField& lasExtraScalarField) const;

	//! Select/deselect all standard fields
	void doSelectAll(bool doSelect);

	//! Select/deselect all extra fields
	void doSelectAllESF(bool doSelect);

	//! Apply current settings to the next file
	void onApplyAll();

	//! Handle normal field combo box change
	void onNormalComboBoxChanged(const QString& name);

	//! Browse for tiling output directory
	void onBrowseTilingOutputDir();

	//! Handle tab change
	void onCurrentTabChanged(int index);

	//! Decompose/recompose classification fields
	void decomposeClassificationFields(bool decompose, bool autoUpdateCheckSate);

	//! Validate COPC extent inputs
	void checkExtentConsistency(double);

	//! Show/hide classification flag checkboxes
	void onDecomposeClassificationToggled(bool state);

	//! Skip dialog flag (per-session)
	bool m_shouldSkipDialog{false};
	//! Valid extent flag
	bool m_validExtent{false};
};
