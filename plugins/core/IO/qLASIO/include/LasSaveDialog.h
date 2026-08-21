/**
 * @file LasSaveDialog.h
 *
 * @brief LAS/LAZ file export configuration dialog
 *
 * Modal dialog for configuring LAS/LAZ export options:
 * - **LAS version**: 1.2, 1.3, or 1.4
 * - **Point format**: varies by version (formats 0-10)
 * - **Coordinate encoding**: scale and offset factors
 * - **RGB**: enable color export
 * - **Extra scalar fields**: select which fields to save as extra attributes
 * - **Waveform data**: include waveform packets
 *
 * ## Version/Format Matrix
 *
 * | Version | Formats | Notes |
 * |---------|---------|-------|
 * | 1.2     | 0-5    |       |
 * | 1.3     | 0-5    | Same as 1.2 |
 * | 1.4     | 0-10   | Extended formats |
 *
 * Point format 6 or higher requires LAS 1.4.
 *
 * @extends QDialog
 * @author Thomas Montaigu
 */

#pragma once

// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        //
// ##########################################################################

#include "LasDetails.h"
#include "LasExtraScalarField.h"
#include "LasScalarField.h"
#include "ui_lassavedialog.h"

// Qt
#include <QDialog>

// CCCoreLib
#include <CCGeom.h>

class QStringListModel;
class ccScalarField;
class ccPointCloud;

class MappingLabel;
class LasExtraScalarFieldCard;

/**
 * @brief LAS save dialog.
 *
 * @details Dialog for configuring LAS file export options.
 *
 * @extends QDialog
 */
class LasSaveDialog : public QDialog
    , public Ui::LASSaveDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog.
	 *
	 * @param[in] cloud Point cloud to save.
	 * @param[in] parent Parent widget.
	 */
	explicit LasSaveDialog(ccPointCloud* cloud, QWidget* parent = nullptr);

	/**
	 * @brief Set version and point format.
	 *
	 * @param[in] versionAndFmt LAS version.
	 */
	void setVersionAndPointFormat(const LasDetails::LasVersion versionAndFmt);

	/**
	 * @brief Set optimal scale.
	 *
	 * @param[in] scale Optimal scale factor.
	 * @param[in] autoCheck Auto-check checkbox.
	 */
	void setOptimalScale(const CCVector3d& scale, bool autoCheck = false);

	/**
	 * @brief Set original scale from file.
	 *
	 * @param[in] scale Original scale factor.
	 * @param[in] canUseScale Can use original scale.
	 * @param[in] autoCheck Auto-check checkbox.
	 */
	void setOriginalScale(const CCVector3d& scale, bool canUseScale, bool autoCheck = true);

	/**
	 * @brief Set extra scalar fields.
	 *
	 * @param[in] extraScalarFields Extra fields from original file.
	 */
	void setExtraScalarFields(const std::vector<LasExtraScalarField>& extraScalarFields);

	/**
	 * @brief Offset type enumeration.
	 */
	enum Offset
	{
		GLOBAL_SHIFT,    //!< Global shift.
		ORIGN_LAS_OFFSET, //!< Original LAS offset.
		MIN_BB_CORNER,    //!< Min bounding box corner.
		BB_CENTER,       //!< Bounding box center.
		CUSTOM_LAS_OFFSET //!< Custom offset.
	};

	/**
	 * @brief Set available offsets.
	 *
	 * @param[in] availableOffsets Available offset options.
	 * @param[in] selectedOffsetType Selected offset type.
	 */
	void setOffsets(const QMap<Offset, CCVector3d>& availableOffsets, Offset selectedOffsetType);

	/**
	 * @brief Get selected point format.
	 *
	 * @return Point format version.
	 */
	uint8_t selectedPointFormat() const;

	/**
	 * @brief Get selected version.
	 *
	 * @param[out] versionMajor Version major.
	 * @param[out] versionMinor Version minor.
	 */
	void selectedVersion(uint8_t& versionMajor, uint8_t& versionMinor) const;

	/**
	 * @brief Get chosen scale.
	 *
	 * @return Scale vector.
	 */
	CCVector3d chosenScale() const;

	/**
	 * @brief Check if should save RGB.
	 *
	 * @return true if RGB should be saved.
	 */
	bool shouldSaveRGB() const;

	/**
	 * @brief Check if should save waveforms.
	 *
	 * @return true if waveforms should be saved.
	 */
	bool shouldSaveWaveform() const;

	/**
	 * @brief Check if should save normals.
	 *
	 * @return true if normals should be saved as extra field.
	 */
	bool shouldSaveNormalsAsExtraScalarField() const;

	/**
	 * @brief Get chosen offset.
	 *
	 * @param[out] offsetType Offset type.
	 *
	 * @return Offset vector.
	 */
	CCVector3d chosenOffset(Offset& offsetType) const;

	/**
	 * @brief Get scalar fields to save.
	 *
	 * @return Scalar fields to save.
	 */
	std::vector<LasScalarField> fieldsToSave() const;

	/**
	 * @brief Get extra fields to save.
	 *
	 * @return Extra fields to save.
	 */
	std::vector<LasExtraScalarField> extraFieldsToSave() const;

  public Q_SLOTS:
	/**
	 * @brief Handle version change.
	 */
	void handleSelectedVersionChange(const QString&);

	/**
	 * @brief Handle point format change.
	 */
	void handleSelectedPointFormatChange(int index);

	/**
	 * @brief Handle combo box change.
	 */
	void handleComboBoxChange(int index);

	/**
	 * @brief Handle custom scale toggle.
	 */
	void handleCustomScaleButtontoggled(bool checked);

	/**
	 * @brief Add extra scalar field card.
	 *
	 * @return New card widget.
	 */
	LasExtraScalarFieldCard* addExtraScalarFieldCard();

  private:
	/**
	 * @brief Create extra field card.
	 *
	 * @return New card widget.
	 */
	LasExtraScalarFieldCard* createCard() const;

	/**
	 * @brief Assign leftover scalar fields.
	 *
	 * Assigns unassigned SFs as extra fields.
	 */
	void assignLeftoverScalarFieldsAsExtra();

	/**
	 * @brief Unassign default fields.
	 */
	void unassignDefaultFields();

	/**
	 * @brief Check auto-assignment preference.
	 *
	 * @return true if should auto-assign.
	 */
	bool shouldAutomaticallyAssignLeftoverSFsAsExtra() const;

  private:
	//! Point cloud.
	ccPointCloud* m_cloud{nullptr};

	//! Scalar field names model.
	QStringListModel* m_scalarFieldsNamesModel{nullptr};

	//! Extra fields data types model.
	QStringListModel* m_extraFieldsDataTypesModel{nullptr};

	//! Optimal scale.
	CCVector3d m_optimalScale;

	//! Original scale.
	CCVector3d m_originalScale;

	//! Scalar field mapping.
	std::vector<std::pair<MappingLabel*, QComboBox*>> m_scalarFieldMapping;

	//! Output offsets.
	QMap<Offset, CCVector3d> outputOffsets;
};
