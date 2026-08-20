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

#ifndef CC_2_5D_EDITOR_HEADER
#define CC_2_5D_EDITOR_HEADER

/**
 * @file cc2.5DimEditor.h
 *
 * @brief Generic interface for 2.5D raster data editing.
 *
 * @details Provides a base interface for working with 2.5D raster
 * representations of point cloud data.
 *
 * 2.5D refers to data that is represented as a 2D grid (raster)
 * where each cell contains a height value (Z). This is commonly
 * used for:
 * - Digital Elevation Models (DEMs)
 * - Terrain models
 * - Depth maps
 * - Rasterized point clouds
 *
 * The editor provides:
 * - Grid creation and management
 * - Bounding box editing
 * - 2D view visualization
 * - Grid-to-point-cloud conversion
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccRasterGrid
 * @see cc2.5DimEditor
 */

// qCC_db
#include <ccRasterGrid.h>

class ccBoundingBoxEditorDlg;
class ccGLWindowInterface;
class ccPointCloud;
class ccGenericPointCloud;
class ccProgressDialog;
class QWidget;
class QFrame;
class QComboBox;

/**
 * @brief Generic interface for 2.5D raster data editing.
 *
 * @details Provides common functionality for 2.5D raster operations
 * including grid management, visualization, and export.
 *
 * Subclasses must implement:
 * - getGridStep(): Grid cell size
 * - getProjectionDimension(): Which dimension is the height (0=X, 1=Y, 2=Z)
 * - getTypeOfProjection(): How to project onto the grid
 * - gridIsUpToDate(): Grid validity state
 *
 * @note This is an abstract base class meant to be subclassed
 * by dialogs or tools that work with raster grids.
 */
class cc2Point5DimEditor
{
  public:
	/**
	 * @brief Default constructor.
	 */
	cc2Point5DimEditor();

	/**
	 * @brief Destructor.
	 */
	virtual ~cc2Point5DimEditor();

  protected: // standard methods
	/**
	 * @brief Get the projection grid step.
	 * @return Grid cell size.
	 */
	virtual double getGridStep() const = 0;

	/**
	 * @brief Get the projection dimension.
	 * @return 0=X, 1=Y, 2=Z.
	 *
	 * @details Specifies which axis is treated as the height
	 * dimension when creating the 2.5D grid.
	 */
	virtual unsigned char getProjectionDimension() const = 0;

	/**
	 * @brief Get the projection type.
	 * @return Projection type enum.
	 */
	virtual ccRasterGrid::ProjectionType getTypeOfProjection() const = 0;

	/**
	 * @brief Get custom bounding box.
	 * @return Custom bbox or default if none set.
	 */
	virtual ccBBox getCustomBBox() const;

	/**
	 * @brief Mark grid as up-to-date.
	 * @param[in] state Whether grid is current.
	 */
	virtual void gridIsUpToDate(bool state) = 0;

	/**
	 * @brief Update 2D display zoom.
	 * @param[in,out] box Bounding box to adjust.
	 */
	virtual void update2DDisplayZoom(ccBBox& box);

  protected: // raster grid related stuff
	/**
	 * @brief Show grid bounding box editor.
	 * @return true if editor was shown and accepted.
	 */
	virtual bool showGridBoxEditor();

	/**
	 * @brief Get grid size as string.
	 * @return String like "W x H".
	 */
	virtual QString getGridSizeAsString() const;

	/**
	 * @brief Get grid dimensions.
	 * @param[out] width Grid width.
	 * @param[out] height Grid height.
	 * @return true if grid size is known.
	 */
	virtual bool getGridSize(unsigned& width, unsigned& height) const;

	/**
	 * @brief Create bounding box editor.
	 * @param[in] gridBBox Grid bounding box.
	 * @param[in] parent Parent widget.
	 */
	void createBoundingBoxEditor(const ccBBox& gridBBox, QWidget* parent);

	/**
	 * @brief Create 2D visualization view.
	 * @param[in] parentFrame Parent frame widget.
	 */
	void create2DView(QFrame* parentFrame);

	/**
	 * @brief Get empty cell fill strategy.
	 * @param[in] comboBox Combo box with selected option.
	 * @return Fill strategy enum.
	 */
	ccRasterGrid::EmptyCellFillOption getFillEmptyCellsStrategy(QComboBox* comboBox) const;

  public:
	/**
	 * @brief Convert raster grid to point cloud.
	 *
	 * @param[in] exportHeightStats Export height statistics.
	 * @param[in] exportSFStats Export scalar field statistics.
	 * @param[in] exportedStatistics List of statistics to export.
	 * @param[in] projectSFs Project scalar fields to grid.
	 * @param[in] projectColors Project colors to grid.
	 * @param[in] resampleInputCloudXY Resample in XY.
	 * @param[in] resampleInputCloudZ Resample in Z.
	 * @param[in] inputCloud Input cloud for resampling.
	 * @param[in] percentileValue Percentile for outlier removal.
	 * @param[in] exportToOriginalCS Export in original coordinate system.
	 * @param[in] appendGridSizeToSFNames Add grid size to SF names.
	 * @param[in] progressDialog Optional progress dialog.
	 *
	 * @return Newly created point cloud, or nullptr on error.
	 *
	 * @note The caller owns the returned cloud.
	 */
	ccPointCloud* convertGridToCloud(bool                                               exportHeightStats,
	                                 bool                                               exportSFStats,
	                                 const std::vector<ccRasterGrid::ExportableFields>& exportedStatistics,
	                                 bool                                               projectSFs,
	                                 bool                                               projectColors,
	                                 bool                                               resampleInputCloudXY,
	                                 bool                                               resampleInputCloudZ,
	                                 ccGenericPointCloud*                               inputCloud,
	                                 double                                             percentileValue,
	                                 bool                                               exportToOriginalCS,
	                                 bool                                               appendGridSizeToSFNames,
	                                 ccProgressDialog*                                  progressDialog = nullptr) const;

  protected:
	//! Bounding box editor
	ccBoundingBoxEditorDlg* m_bbEditorDlg = nullptr;

	//! 2D OpenGL window
	ccGLWindowInterface* m_glWindow = nullptr;

	//! Raster cloud (for display)
	ccPointCloud* m_rasterCloud = nullptr;

	//! The raster grid
	ccRasterGrid m_grid;
};

#endif // CC_2_5D_EDITOR_HEADER
