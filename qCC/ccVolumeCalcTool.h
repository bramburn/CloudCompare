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

#ifndef CC_VOLUME_CALC_TOOL_HEADER
#define CC_VOLUME_CALC_TOOL_HEADER

// Local
#include "cc2.5DimEditor.h"

/**
 * @file ccVolumeCalcTool.h
 *
 * @brief Volume calculation tool for computing volumes between surfaces.
 *
 * @details Tool for calculating volume between two point clouds or surfaces.
 *
 * This computes:
 * - Total volume between ground and ceiling surfaces
 * - Added volume (above ground)
 * - Removed volume (below ground)
 * - Surface area
 * - Matching percentage
 *
 * Used for:
 * - Stockpile volume calculations
 * - Cut/fill analysis
 * - Terrain difference analysis
 * - Mining/construction surveys
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see cc2Point5DimEditor
 */

#include <QDialog>

class ccGenericPointCloud;
class ccPointCloud;
class ccPolyline;

namespace Ui
{
	class VolumeCalcDialog;
}

/**
 * @brief Tool for calculating volume between two surfaces.
 *
 * @details Computes volume between two point clouds (ground and ceiling).
 *
 * Can compare:
 * - Two point clouds
 * - Point cloud vs. constant height plane
 * - Two constant height planes
 *
 * Features:
 * - 2.5D raster-based computation
 * - Configurable grid resolution
 * - Empty cell filling strategies
 * - Volume report with statistics
 * - Export results
 *
 * @extends QDialog
 * @extends cc2Point5DimEditor
 */
class ccVolumeCalcTool : public QDialog
    , public cc2Point5DimEditor
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the volume calculation tool.
	 *
	 * @param[in] cloud1 First cloud (ground).
	 * @param[in] cloud2 Second cloud (ceiling).
	 * @param[in] parent Parent widget.
	 */
	ccVolumeCalcTool(ccGenericPointCloud* cloud1, ccGenericPointCloud* cloud2, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccVolumeCalcTool();

	// Inherited from cc2Point5DimEditor
	/**
	 * @brief Get grid step.
	 */
	virtual double getGridStep() const override;

	/**
	 * @brief Get projection dimension.
	 */
	virtual unsigned char getProjectionDimension() const override;

	/**
	 * @brief Get projection type.
	 */
	virtual ccRasterGrid::ProjectionType getTypeOfProjection() const override;

	/**
	 * @brief Volume calculation report.
	 */
	struct ReportInfo
	{
		ReportInfo()
		    : volume(0)
		    , addedVolume(0)
		    , removedVolume(0)
		    , surface(0)
		    , matchingPrecent(0)
		    , ceilNonMatchingPercent(0)
		    , groundNonMatchingPercent(0)
		    , averageNeighborsPerCell(0)
		{
		}

		/**
		 * @brief Convert to text.
		 *
		 * @param[in] precision Decimal precision.
		 * @return Formatted text.
		 */
		QString toText(int precision = 6) const;

		double volume;                    //!< Total volume
		double addedVolume;              //!< Volume above ground
		double removedVolume;           //!< Volume below ground
		double surface;                 //!< Surface area
		float  matchingPrecent;         //!< Matching percentage
		float  ceilNonMatchingPercent;  //!< Ceil non-matching %
		float  groundNonMatchingPercent; //!< Ground non-matching %
		double averageNeighborsPerCell;  //!< Avg neighbors per cell
	};

	/**
	 * @brief Compute volume between two surfaces.
	 *
	 * @param[in] grid Raster grid.
	 * @param[in] ground Ground cloud.
	 * @param[in] ceil Ceiling cloud.
	 * @param[in] gridBox Grid bounding box.
	 * @param[in] vertDim Vertical dimension.
	 * @param[in] gridStep Grid cell size.
	 * @param[in] gridWidth Grid width.
	 * @param[in] gridHeight Grid height.
	 * @param[in] projectionType Projection type.
	 * @param[in] groundEmptyCellFillStrategy Ground fill strategy.
	 * @param[in] groundMaxEdgeLength Ground max edge.
	 * @param[in] ceilEmptyCellFillStrategy Ceil fill strategy.
	 * @param[in] ceilMaxEdgeLength Ceil max edge.
	 * @param[out] reportInfo Volume report.
	 * @param[in] groundHeight Ground constant height.
	 * @param[in] ceilHeight Ceil constant height.
	 * @param[in] parentWidget Parent widget.
	 *
	 * @return true on success.
	 */
	static bool ComputeVolume(ccRasterGrid&                     grid,
	                          ccGenericPointCloud*              ground,
	                          ccGenericPointCloud*              ceil,
	                          const ccBBox&                     gridBox,
	                          unsigned char                     vertDim,
	                          double                            gridStep,
	                          unsigned                          gridWidth,
	                          unsigned                          gridHeight,
	                          ccRasterGrid::ProjectionType      projectionType,
	                          ccRasterGrid::EmptyCellFillOption groundEmptyCellFillStrategy,
	                          double                            groundMaxEdgeLength,
	                          ccRasterGrid::EmptyCellFillOption ceilEmptyCellFillStrategy,
	                          double                            ceilMaxEdgeLength,
	                          ccVolumeCalcTool::ReportInfo&     reportInfo,
	                          double                            groundHeight,
	                          double                            ceilHeight,
	                          QWidget*                          parentWidget = nullptr);

	/**
	 * @brief Convert volume grid to point cloud.
	 *
	 * @param[in] grid Raster grid.
	 * @param[in] gridBox Grid bounding box.
	 * @param[in] vertDim Vertical dimension.
	 * @param[in] exportToOriginalCS Export in original CS.
	 *
	 * @return Point cloud representation.
	 *
	 * @note The returned cloud must be deleted by caller.
	 */
	static ccPointCloud* ConvertGridToCloud(ccRasterGrid& grid,
	                                        const ccBBox& gridBox,
	                                        unsigned char vertDim,
	                                        bool          exportToOriginalCS);

  protected slots:
	/**
	 * @brief Save settings and accept.
	 */
	void saveSettingsAndAccept();

	/**
	 * @brief Save persistent settings.
	 */
	void saveSettings();

	/**
	 * @brief Handle projection direction change.
	 * @param[in] index New direction.
	 */
	void projectionDirChanged(int index);

	/**
	 * @brief Handle SF projection type change.
	 * @param[in] index New type.
	 */
	void sfProjectionTypeChanged(int index);

	/**
	 * @brief Handle ground fill strategy change.
	 * @param[in] index New strategy.
	 */
	void groundFillEmptyCellStrategyChanged(int index);

	/**
	 * @brief Handle ceil fill strategy change.
	 * @param[in] index New strategy.
	 */
	void ceilFillEmptyCellStrategyChanged(int index);

	/**
	 * @brief Handle grid option change.
	 */
	void gridOptionChanged();

	/**
	 * @brief Update grid info display.
	 */
	void updateGridInfo();

	/**
	 * @brief Update grid and display.
	 */
	void updateGridAndDisplay();

	/**
	 * @brief Swap ground and ceil roles.
	 */
	void swapRoles();

	/**
	 * @brief Handle ground source change.
	 * @param[in] index New source.
	 */
	void groundSourceChanged(int index);

	/**
	 * @brief Handle ceil source change.
	 * @param[in] index New source.
	 */
	void ceilSourceChanged(int index);

	/**
	 * @brief Export to clipboard.
	 */
	void exportToClipboard() const;

	/**
	 * @brief Export grid as cloud.
	 */
	void exportGridAsCloud() const;

	/**
	 * @brief Set number precision.
	 * @param[in] precision Decimal places.
	 */
	void setDisplayedNumberPrecision(int precision);

  protected: // standard methods
	// Inherited from cc2Point5DimEditor
	/**
	 * @brief Show grid box editor.
	 */
	virtual bool showGridBoxEditor() override;

	/**
	 * @brief Mark grid as up-to-date.
	 */
	virtual void gridIsUpToDate(bool state) override;

	/**
	 * @brief Load settings.
	 */
	void loadSettings();

	/**
	 * @brief Update the grid.
	 * @return true on success.
	 */
	bool updateGrid();

	/**
	 * @brief Convert grid to cloud.
	 *
	 * @param[in] exportToOriginalCS Use original CS.
	 * @return Cloud representation.
	 */
	ccPointCloud* convertGridToCloud(bool exportToOriginalCS) const;

	/**
	 * @brief Output the report.
	 * @param[in] info Report to display.
	 */
	void outputReport(const ReportInfo& info);

	/**
	 * @brief Get ground cloud or height.
	 * @return Cloud or constant height.
	 */
	std::pair<ccGenericPointCloud*, double> getGroundCloud() const;

	/**
	 * @brief Get ceil cloud or height.
	 * @return Cloud or constant height.
	 */
	std::pair<ccGenericPointCloud*, double> getCeilCloud() const;

  protected: // members
	//! First cloud
	ccGenericPointCloud* m_cloud1;

	//! Second cloud
	ccGenericPointCloud* m_cloud2;

	//! Last report
	ReportInfo m_lastReport;

	//! UI definition
	Ui::VolumeCalcDialog* m_ui;
};

#endif // CC_VOLUME_CALC_TOOL_HEADER
