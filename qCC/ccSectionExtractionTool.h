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
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccSectionExtractionTool.h
 *
 * @brief Section extraction tool for interactive profile extraction.
 *
 * @details Tool for extracting 2D sections/profiles from point clouds
 * by drawing polylines in the 3D view.
 *
 * Features:
 * - Interactive polyline drawing
 * - Multiple section support
 * - Section envelope extraction
 * - Orthogonal section generation
 * - Unfolding to 2D
 *
 * Usage:
 * 1. Add clouds to the pool
 * 2. Draw polylines to define sections
 * 3. Configure extraction parameters
 * 4. Extract points or envelopes
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccOverlayDialog
 * @see ccEnvelopeExtractor
 */

#include "ccEnvelopeExtractor.h"
#include "ccOverlayDialog.h"

#include <ccHObject.h>

class ccGenericPointCloud;
class ccPointCloud;
class ccGLWindowInterface;

namespace Ui
{
	class SectionExtractionDlg;
}

/**
 * @brief Section extraction tool for profile generation.
 *
 * @details Provides an overlay dialog for interactive section extraction.
 *
 * Workflow:
 * 1. Add clouds to process
 * 2. Draw polyline paths defining sections
 * 3. Set extraction parameters
 * 4. Extract points along sections
 * 5. Optionally extract envelopes
 *
 * @extends ccOverlayDialog
 */
class ccSectionExtractionTool : public ccOverlayDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Process states.
	 */
	enum ProcessStates
	{
		PAUSED  = 32,
		STARTED = 64,
		RUNNING = 128,
	};

	/**
	 * @brief Construct the section extraction tool.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccSectionExtractionTool(QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	~ccSectionExtractionTool() override;

	/**
	 * @brief Add a cloud to the clouds pool.
	 *
	 * @param[in] cloud Cloud to add.
	 * @param[in] alreadyInDB Whether cloud is already in DB.
	 * @return true on success.
	 */
	bool addCloud(ccGenericPointCloud* cloud, bool alreadyInDB = true);

	/**
	 * @brief Add a polyline to the sections pool.
	 *
	 * @param[in] poly Polyline to add.
	 * @param[in] alreadyInDB Whether polyline is already in DB.
	 * @return true on success.
	 *
	 * @warning If true, ownership is taken by this class.
	 */
	bool addPolyline(ccPolyline* poly, bool alreadyInDB = true);

	/**
	 * @brief Remove all entities.
	 */
	void removeAllEntities();

	// inherited from ccOverlayDialog
	/**
	 * @brief Link with a 3D window.
	 */
	bool linkWith(ccGLWindowInterface* win) override;

	/**
	 * @brief Start the tool.
	 */
	bool start() override;

	/**
	 * @brief Stop the tool.
	 *
	 * @param[in] accepted Whether to apply changes.
	 */
	void stop(bool accepted) override;

  protected slots:
	/**
	 * @brief Undo last action.
	 */
	void undo();

	/**
	 * @brief Reset the tool.
	 *
	 * @param[in] askForConfirmation Prompt user.
	 * @return true on success.
	 */
	bool reset(bool askForConfirmation = true);

	/**
	 * @brief Apply extraction.
	 */
	void apply();

	/**
	 * @brief Cancel and close.
	 */
	void cancel();

	/**
	 * @brief Add point to current polyline.
	 *
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 */
	void addPointToPolyline(int x, int y);

	/**
	 * @brief Close the current polyline.
	 *
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 */
	void closePolyLine(int x = 0, int y = 0);

	/**
	 * @brief Update polyline preview.
	 *
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 * @param[in] buttons Mouse buttons.
	 */
	void updatePolyLine(int x, int y, Qt::MouseButtons buttons);

	/**
	 * @brief Toggle section editing mode.
	 *
	 * @param[in] enabled Enable/disable.
	 */
	void enableSectionEditingMode(bool enabled);

	/**
	 * @brief Import polylines from DB.
	 */
	void doImportPolylinesFromDB();

	/**
	 * @brief Set vertical dimension.
	 *
	 * @param[in] index Dimension index.
	 */
	void setVertDimension(int index);

	/**
	 * @brief Handle entity selection.
	 *
	 * @param[in] entity Selected entity.
	 */
	void entitySelected(ccHObject* entity);

	/**
	 * @brief Generate orthogonal sections.
	 */
	void generateOrthoSections();

	/**
	 * @brief Extract points along sections.
	 */
	void extractPoints();

	/**
	 * @brief Unfold sections to 2D.
	 */
	void unfoldPoints();

	/**
	 * @brief Export sections.
	 */
	void exportSections();

	/**
	 * @brief Handle shortcut trigger.
	 *
	 * @param[in] id Shortcut ID.
	 */
	void onShortcutTriggered(int id);

  protected:
	/**
	 * @brief Cancel current polyline.
	 */
	void cancelCurrentPolyline();

	/**
	 * @brief Delete selected polyline.
	 */
	void deleteSelectedPolyline();

	/**
	 * @brief Add undo step.
	 */
	void addUndoStep();

	/**
	 * @brief Extract section cloud.
	 *
	 * @param[in] refClouds Reference clouds.
	 * @param[in] sectionIndex Section index.
	 * @param[out] cloudGenerated Whether cloud was generated.
	 * @return true on success.
	 */
	bool extractSectionCloud(const std::vector<CCCoreLib::ReferenceCloud*>& refClouds,
	                         unsigned                                       sectionIndex,
	                         bool&                                          cloudGenerated);

	/**
	 * @brief Extract section envelope.
	 *
	 * @param[in] originalSection Original polyline.
	 * @param[in] originalSectionCloud Cloud along section.
	 * @param[in] unrolledSectionCloud 2D cloud (Z=0).
	 * @param[in] sectionIndex Section index.
	 * @param[in] type Envelope type.
	 * @param[in] maxEdgeLength Max edge length.
	 * @param[in] multiPass Use multi-pass.
	 * @param[in] splitEnvelope Split envelope.
	 * @param[out] envelopeGenerated Whether envelope was generated.
	 * @param[in] visualDebugMode Debug visualization.
	 * @return true on success.
	 */
	bool extractSectionEnvelope(const ccPolyline*                 originalSection,
	                            const ccPointCloud*               originalSectionCloud,
	                            ccPointCloud*                     unrolledSectionCloud,
	                            unsigned                          sectionIndex,
	                            ccEnvelopeExtractor::EnvelopeType type,
	                            PointCoordinateType               maxEdgeLength,
	                            bool                              multiPass,
	                            bool                              splitEnvelope,
	                            bool&                             envelopeGenerated,
	                            bool                              visualDebugMode = false);

	/**
	 * @brief Get export group.
	 *
	 * @param[out] defaultGroupID Group ID.
	 * @param[in] defaultName Default name.
	 * @return Export group.
	 */
	ccHObject* getExportGroup(unsigned& defaultGroupID, const QString& defaultName);

	/**
	 * @brief Imported entity wrapper.
	 */
	template <class EntityType>
	struct ImportedEntity
	{
		ImportedEntity()
		    : entity(nullptr)
		    , originalDisplay(nullptr)
		    , isInDB(false)
		    , backupColorShown(false)
		    , backupWidth(0)
		{
		}

		ImportedEntity(const ImportedEntity& section)
		    : entity(section.entity)
		    , originalDisplay(section.originalDisplay)
		    , isInDB(section.isInDB)
		    , backupColorShown(section.backupColorShown)
		    , backupWidth(section.backupWidth)
		{
			backupColor = section.backupColor;
		}

		ImportedEntity(EntityType* e, bool alreadyInDB)
		    : entity(e)
		    , originalDisplay(e->getDisplay())
		    , isInDB(alreadyInDB)
		    , backupColorShown(false)
		    , backupWidth(0)
		{
			if (e->isA(CC_TYPES::POLY_LINE))
			{
				ccPolyline* poly = reinterpret_cast<ccPolyline*>(e);
				backupColor      = poly->getColor();
				backupColorShown = poly->colorsShown();
				backupWidth = poly->getWidth();
			}
		}

		bool operator==(const ImportedEntity& ie) const
		{
			return entity == ie.entity;
		}

		EntityType*         entity;
		ccGenericGLDisplay* originalDisplay;
		bool                isInDB;
		ccColor::Rgb        backupColor;
		bool                backupColorShown;
		PointCoordinateType backupWidth;
	};

	//! Section type
	using Section = ImportedEntity<ccPolyline>;

	//! Cloud type
	using Cloud = ImportedEntity<ccGenericPointCloud>;

	//! Section pool
	using SectionPool = QList<Section>;

	//! Cloud pool
	using CloudPool = QList<Cloud>;

	/**
	 * @brief Release a polyline.
	 *
	 * @param[in] section Section to release.
	 */
	void releasePolyline(Section* section);

	/**
	 * @brief Select a polyline.
	 *
	 * @param[in] poly Polyline to select.
	 * @param[in] autoRefreshDisplay Auto-refresh.
	 */
	void selectPolyline(Section* poly, bool autoRefreshDisplay = true);

	/**
	 * @brief Update clouds bounding box.
	 */
	void updateCloudsBox();

  private:
	//! UI definition
	Ui::SectionExtractionDlg* m_UI;

	//! Section pool
	SectionPool m_sections;

	//! Selected polyline
	Section* m_selectedPoly;

	//! Cloud pool
	CloudPool m_clouds;

	//! Process state
	unsigned m_state;

	//! Undo count per section
	std::vector<size_t> m_undoCount;

	//! Currently edited polyline
	ccPolyline* m_editedPoly;

	//! Edited polyline vertices
	ccPointCloud* m_editedPolyVertices;

	//! Clouds bounding box
	ccBBox m_cloudsBox;
};
