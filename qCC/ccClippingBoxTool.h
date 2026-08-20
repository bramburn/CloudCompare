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

#ifndef CC_CLIPPING_BOX_TOOL_HEADER
#define CC_CLIPPING_BOX_TOOL_HEADER

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
 * @file ccClippingBoxTool.h
 *
 * @brief Clipping box tool for entity slicing and contour extraction.
 *
 * @details Interactive tool for managing a clipping box that can slice
 * point clouds and meshes. Provides:
 * - Interactive box manipulation in 3D view
 * - Slice extraction at box faces
 * - Contour/envelope extraction from slices
 * - Level set computation
 *
 * The clipping box can be translated and rotated, and slices
 * can be extracted from the intersection with entities.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccClipBox
 * @see ccEnvelopeExtractor
 */

// common
#include <ccOverlayDialog.h>

// local
#include "ccEnvelopeExtractor.h"

#include <ui_clippingBoxDlg.h>

// qCC_db
#include <ccGLUtils.h>

// system
#include <vector>

class ccGenericPointCloud;
class ccGenericMesh;
class ccProgressDialog;
class ccGLWindowInterface;
class ccHObject;
class ccClipBox;
class ccPolyline;
class ccBBox;

/**
 * @brief Clipping box tool for entity slicing and contour extraction.
 *
 * @details Interactive overlay tool that provides a clipping box
 * for slicing entities and extracting contours/envelopes.
 *
 * Features:
 * - Interactive clipping box manipulation
 * - Single and multi-slice extraction
 * - Contour/envelope extraction (alpha-shapes, crust, etc.)
 * - Level set computation
 * - Export slices and contours
 *
 * Usage:
 * 1. Create the tool and link to a 3D view
 * 2. Add entities to clip
 * 3. Adjust the box position/orientation
 * 4. Extract slices and/or contours
 *
 * @extends ccOverlayDialog
 * @extends Ui::ClippingBoxDlg
 */
class ccClippingBoxTool : public ccOverlayDialog
    , public Ui::ClippingBoxDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the clipping box tool.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccClippingBoxTool(QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccClippingBoxTool();

	// inherited from ccOverlayDialog
	/**
	 * @brief Link with a 3D view.
	 *
	 * @param[in] win Window to link with.
	 * @return true on success.
	 */
	virtual bool linkWith(ccGLWindowInterface* win) override;

	/**
	 * @brief Start the tool.
	 *
	 * @return true on success.
	 */
	virtual bool start() override;

	/**
	 * @brief Stop the tool.
	 *
	 * @param[in] state Final state to apply.
	 */
	virtual void stop(bool state) override;

	/**
	 * @brief Get the clipping box.
	 *
	 * @return Pointer to the clipping box.
	 */
	const ccClipBox* box() const
	{
		return m_clipBox;
	}

	/**
	 * @brief Add an entity to be clipped.
	 *
	 * @param[in] anObject Entity to add.
	 * @return true if entity is eligible for clipping.
	 *
	 * @details Entities must be point clouds or meshes.
	 */
	bool addAssociatedEntity(ccHObject* anObject);

	/**
	 * @brief Get the number of associated entities.
	 *
	 * @return Number of entities in the clipping set.
	 */
	unsigned getNumberOfAssociatedEntity() const;

	/**
	 * @brief Extract slices and contours from entities.
	 *
	 * @param[in] clouds Point clouds to process.
	 * @param[in] meshes Meshes to process.
	 * @param[in] clipBox Clipping box to use.
	 * @param[in] singleSliceMode If true, extract single slice instead of repeating.
	 * @param[in] processDimensions Which dimensions to process.
	 * @param[out] outputSlices Extracted slice entities.
	 * @param[in] extractEnvelopes Whether to extract envelopes.
	 * @param[in] maxEdgeLength Maximum envelope edge length.
	 * @param[in] envelopeType Type of envelope to extract.
	 * @param[out] outputEnvelopes Extracted envelopes.
	 * @param[in] extractLevelSet Whether to extract level set.
	 * @param[in] levelSetGridStep Grid step for level set.
	 * @param[in] levelSetMinVertCount Minimum vertices per contour.
	 * @param[out] levelSet Level set polylines.
	 * @param[in] gap Gap between slices.
	 * @param[in] multiPass Use multi-pass envelope extraction.
	 * @param[in] splitEnvelopes Split long envelope segments.
	 * @param[in] projectOnBestFitPlane Project points to best-fit plane.
	 * @param[in] visualDebugMode Show debugging window.
	 * @param[in] generateRandomColors Random colors for slices.
	 * @param[in] progressDialog Optional progress dialog.
	 * @return true on success.
	 *
	 * @details Static method to extract slices and/or contours from
	 * entities using the clipping box. Can extract:
	 * - Slice polylines at box faces
	 * - Alpha-shapes or crust envelopes
	 * - Level set contours
	 */
	static bool ExtractSlicesAndContours(
	    const std::vector<ccGenericPointCloud*>& clouds,
	    const std::vector<ccGenericMesh*>&       meshes,
	    ccClipBox&                               clipBox,
	    bool                                     singleSliceMode,
	    bool                                     processDimensions[3],
	    std::vector<ccHObject*>&                 outputSlices,

	    bool                              extractEnvelopes,
	    PointCoordinateType               maxEdgeLength,
	    ccEnvelopeExtractor::EnvelopeType envelopeType,
	    std::vector<ccPolyline*>&         outputEnvelopes,

	    bool                      extractLevelSet,
	    double                    levelSetGridStep,
	    int                       levelSetMinVertCount,
	    std::vector<ccPolyline*>& levelSet,

	    PointCoordinateType gap                   = 0,
	    bool                multiPass             = false,
	    bool                splitEnvelopes        = false,
	    bool                projectOnBestFitPlane = false,
	    bool                visualDebugMode       = false,
	    bool                generateRandomColors  = false,
	    ccProgressDialog*   progressDialog        = nullptr);

  protected slots:
	/**
	 * @brief Toggle interactor visibility.
	 *
	 * @param[in] state Enable/disable state.
	 */
	void toggleInteractors(bool state);

	/**
	 * @brief Toggle clipping box visibility.
	 *
	 * @param[in] state Enable/disable state.
	 */
	void toggleBox(bool);

	/**
	 * @brief Edit the clipping box interactively.
	 */
	void editBox();

	/**
	 * @brief Restore the previous box state.
	 */
	void restoreLastBox();

	/**
	 * @brief Reset the clipping box to default.
	 */
	void reset();

	/**
	 * @brief Close the dialog.
	 */
	void closeDialog();

	/**
	 * @brief Extract contours from current slice.
	 */
	void extractContour();

	/**
	 * @brief Remove the last extracted contour.
	 */
	void removeLastContour();

	/**
	 * @brief Export the current slice.
	 */
	void exportSlice();

	/**
	 * @brief Export all slices.
	 */
	void exportMultSlices();

	/**
	 * @brief Handle box modification.
	 *
	 * @param[in] box Modified box.
	 */
	void onBoxModified(const ccBBox* box);

	/**
	 * @brief Handle thickness change.
	 *
	 * @param[in] value New thickness.
	 */
	void thicknessChanged(double value);

	/**
	 * @brief Shift box in negative X direction.
	 */
	inline void shiftXMinus() { shiftBox(0, true); }

	/**
	 * @brief Shift box in positive X direction.
	 */
	inline void shiftXPlus() { shiftBox(0, false); }

	/**
	 * @brief Shift box in negative Y direction.
	 */
	inline void shiftYMinus() { shiftBox(1, true); }

	/**
	 * @brief Shift box in positive Y direction.
	 */
	inline void shiftYPlus() { shiftBox(1, false); }

	/**
	 * @brief Shift box in negative Z direction.
	 */
	inline void shiftZMinus() { shiftBox(2, true); }

	/**
	 * @brief Shift box in positive Z direction.
	 */
	inline void shiftZPlus() { shiftBox(2, false); }

	/**
	 * @brief Set view to front.
	 */
	void setFrontView();

	/**
	 * @brief Set view to bottom.
	 */
	void setBottomView();

	/**
	 * @brief Set view to top.
	 */
	void setTopView();

	/**
	 * @brief Set view to back.
	 */
	void setBackView();

	/**
	 * @brief Set view to left.
	 */
	void setLeftView();

	/**
	 * @brief Set view to right.
	 */
	void setRightView();

  protected:
	/**
	 * @brief Extract slices and contours.
	 *
	 * @param[in] singleSliceMode Whether to extract single slice.
	 */
	void extractSlicesAndContours(bool singleSliceMode);

	/**
	 * @brief Shift the clipping box.
	 *
	 * @param[in] dim Dimension (0=X, 1=Y, 2=Z).
	 * @param[in] minus Shift negative direction.
	 */
	void shiftBox(unsigned char dim, bool minus);

	/**
	 * @brief Set a predefined view orientation.
	 *
	 * @param[in] orientation View orientation.
	 */
	void setView(CC_VIEW_ORIENTATION orientation);

	//! The clipping box
	ccClipBox* m_clipBox;
};

#endif // CC_CLIPPING_BOX_TOOL_HEADER
