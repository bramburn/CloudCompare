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
 * @file ccRasterizeTool.h
 *
 * @brief Rasterize tool for converting point clouds to raster grids.
 *
 * @details Tool for converting 3D point clouds to 2.5D raster grids
 * (gridded elevation models / DEMs).
 *
 * Features:
 * - Multiple projection directions (X, Y, Z)
 * - Multiple interpolation methods (average, min, max, etc.)
 * - Empty cell filling strategies
 * - Output as cloud, mesh, image, or GeoTIFF
 * - Contour line generation
 * - Hillshade computation
 *
 * @extends QDialog
 * @extends cc2Point5DimEditor
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see cc2Point5DimEditor
 * @see ccRasterGrid
 */

// Local
#include "cc2.5DimEditor.h"

// Qt
#include <QDialog>
#include <QString>

class ccGenericPointCloud;
class ccPointCloud;
class ccPolyline;

namespace Ui
{
	class RasterizeToolDialog;
}

/**
 * @brief Tool for converting point clouds to raster grids.
 *
 * @details Provides a complete interface for rasterizing point clouds
 * into 2.5D grids (Digital Elevation Models).
 *
 * Key features:
 * - **Projection**: X, Y, or Z direction
 * - **Interpolation**: MIN, MAX, AVG, etc.
 * - **Output formats**: Cloud, mesh, image, GeoTIFF, ASCII
 * - **Derived products**: Contours, hillshade, X-ray
 *
 * @extends QDialog
 * @extends cc2Point5DimEditor
 */
class ccRasterizeTool : public QDialog
    , public cc2Point5DimEditor
{
	Q_OBJECT

  public:
	/**
	 * @brief Bands to export.
	 */
	struct ExportBands
	{
		bool height    = true;    //!< Export height values
		bool rgb       = false;   //!< Export RGB colors
		bool density   = false;   //!< Export point density
		bool visibleSF = false;   //!< Export visible scalar field
		bool allSFs    = false;   //!< Export all scalar fields
	};

	/**
	 * @brief Construct the rasterize tool.
	 *
	 * @param[in] cloud Point cloud to rasterize.
	 * @param[in] parent Parent widget.
	 */
	ccRasterizeTool(ccGenericPointCloud* cloud, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccRasterizeTool() override;

	/**
	 * @brief Export as GeoTIFF file.
	 *
	 * @param[in] outputFilename Output file path.
	 * @param[in] exportBands Bands to export.
	 * @param[in] fillEmptyCellsStrategy Strategy for empty cells.
	 * @param[in] grid Raster grid to export.
	 * @param[in] gridBBox Grid bounding box.
	 * @param[in] Z Projection dimension.
	 * @param[in] customHeightForEmptyCells Custom height for empty cells.
	 * @param[in] originCloud Original cloud (for coordinate system).
	 * @param[in] visibleSfIndex Visible scalar field index.
	 *
	 * @return true on success.
	 */
	static bool ExportGeoTiff(const QString&                    outputFilename,
	                          const ExportBands&                exportBands,
	                          ccRasterGrid::EmptyCellFillOption fillEmptyCellsStrategy,
	                          const ccRasterGrid&               grid,
	                          const ccBBox&                    gridBBox,
	                          unsigned char                     Z,
	                          double                            customHeightForEmptyCells = std::numeric_limits<double>::quiet_NaN(),
	                          ccGenericPointCloud*              originCloud               = nullptr,
	                          int                               visibleSfIndex            = -1);

  private slots:
	/**
	 * @brief Export grid as point cloud.
	 *
	 * @param[in] autoExport Auto-export mode.
	 * @return Generated cloud.
	 */
	ccPointCloud* generateCloud(bool autoExport = true);

	/**
	 * @brief Export grid as raster image.
	 */
	void generateRaster() const;

	/**
	 * @brief Export grid as mesh.
	 */
	void generateMesh();

	/**
	 * @brief Export contour lines.
	 */
	void exportContourLines();

	/**
	 * @brief Generate contour plot.
	 */
	void generateContours();

	/**
	 * @brief Generate hillshade.
	 */
	void generateHillshade();

	/**
	 * @brief Generate X-ray scalar field.
	 */
	void generateXRaySF();

	/**
	 * @brief Remove all contour lines.
	 */
	void removeContourLines();

	/**
	 * @brief Test conditions and accept.
	 */
	void testAndAccept();

	/**
	 * @brief Test conditions and reject.
	 */
	void testAndReject();

	/**
	 * @brief Save settings and close.
	 */
	void saveSettings();

	/**
	 * @brief Handle active layer change.
	 *
	 * @param[in] index New layer index.
	 * @param[in] autoRedraw Auto-redraw display.
	 */
	void activeLayerChanged(int index, bool autoRedraw = true);

	/**
	 * @brief Handle projection direction change.
	 *
	 * @param[in] index New direction index.
	 */
	void projectionDirChanged(int index);

	/**
	 * @brief Handle std dev layer change.
	 *
	 * @param[in] index New layer index.
	 */
	void stdDevLayerChanged(int index);

	/**
	 * @brief Handle projection type change.
	 *
	 * @param[in] index New type index.
	 */
	void projectionTypeChanged(int index);

	/**
	 * @brief Handle resample option toggle.
	 *
	 * @param[in] state Resample state.
	 */
	void resampleOptionToggled(bool state);

	/**
	 * @brief Handle SF projection type change.
	 *
	 * @param[in] index New type index.
	 */
	void sfProjectionTypeChanged(int index);

	/**
	 * @brief Handle empty cell filling strategy change.
	 *
	 * @param[in] index New strategy index.
	 */
	void fillEmptyCellStrategyChanged(int index);

	/**
	 * @brief Handle grid option change.
	 */
	void gridOptionChanged();

	/**
	 * @brief Update grid info display.
	 *
	 * @param[in] withNonEmptyCells Include non-empty cell count.
	 */
	void updateGridInfo(bool withNonEmptyCells = false);

	/**
	 * @brief Update grid and 2D display.
	 */
	void updateGridAndDisplay();

	/**
	 * @brief Update cloud name.
	 *
	 * @param[in] withNonEmptyCellNumber Include cell count.
	 */
	void updateCloudName(bool withNonEmptyCellNumber);

	/**
	 * @brief Export grid as image.
	 */
	void generateImage() const;

	/**
	 * @brief Export grid as ASCII matrix.
	 */
	void generateASCIIMatrix() const;

	/**
	 * @brief Handle statistics export target change.
	 *
	 * @param[in] state Export state.
	 */
	void onStatExportTargetChanged(bool state);

	/**
	 * @brief Show interpolation parameters dialog.
	 */
	void showInterpolationParamsDialog();

  private: // standard methods (inherited from cc2Point5DimEditor)
	/**
	 * @brief Get grid step.
	 */
	double getGridStep() const override;

	/**
	 * @brief Get projection dimension.
	 */
	unsigned char getProjectionDimension() const override;

	/**
	 * @brief Get projection type.
	 */
	ccRasterGrid::ProjectionType getTypeOfProjection() const override;

	/**
	 * @brief Show grid box editor.
	 */
	bool showGridBoxEditor() override;

	/**
	 * @brief Get std dev layer index.
	 */
	int getStdDevLayerIndex() const;

	/**
	 * @brief Get custom height for empty cells.
	 */
	double getCustomHeightForEmptyCells() const;

	/**
	 * @brief Get statistics percentile value.
	 */
	double getStatisticsPercentileValue() const;

	/**
	 * @brief Get extended fill strategy.
	 */
	ccRasterGrid::EmptyCellFillOption getFillEmptyCellsStrategyExt(double& emptyCellsHeight,
	                                                               double& minHeight,
	                                                               double& maxHeight) const;

	/**
	 * @brief Get exported statistics.
	 */
	void getExportedStats(std::vector<ccRasterGrid::ExportableFields>& stats) const;

	/**
	 * @brief Check if resampling original cloud.
	 */
	bool resampleOriginalCloud() const;

	/**
	 * @brief Get SF projection type.
	 */
	ccRasterGrid::ProjectionType getTypeOfSFProjection() const;

	/**
	 * @brief Update std dev layer combo box.
	 */
	void updateStdDevLayerComboBox();

	/**
	 * @brief Mark grid as up-to-date.
	 */
	void gridIsUpToDate(bool state) override;

	/**
	 * @brief Load persistent settings.
	 */
	void loadSettings();

	/**
	 * @brief Update the grid.
	 *
	 * @param[in] projectSFs Project scalar fields.
	 * @return true on success.
	 */
	bool updateGrid(bool projectSFs = false);

	/**
	 * @brief Check if dialog can close.
	 */
	bool canClose();

	/**
	 * @brief Add a new contour line.
	 *
	 * @param[in] poly Contour polyline.
	 * @param[in] height Contour height.
	 */
	void addNewContour(ccPolyline* poly, double height);

  protected: // raster grid related
	/**
	 * @brief Convert grid to cloud.
	 */
	ccPointCloud* convertGridToCloud(bool                                               exportHeightStats,
	                                 bool                                               exportSFStats,
	                                 const std::vector<ccRasterGrid::ExportableFields>& exportedStatistics,
	                                 bool                                               projectSFs,
	                                 bool                                               projectColors,
	                                 bool                                               copyHillshadeSF,
	                                 bool                                               copyXRaySF,
	                                 const QString&                                     activeSFName,
	                                 double                                             percentileValue,
	                                 bool                                               exportToOriginalCS,
	                                 bool                                               appendGridSizeToSFNames,
	                                 ccProgressDialog*                                  progressDialog = nullptr) const override;

  private:
	/**
	 * @brief Layer types.
	 */
	enum LayerType
	{
		LAYER_HEIGHT = 0, //!< Height/elevation layer
		LAYER_RGB    = 1, //!< RGB color layer
		LAYER_SF     = 2  //!< Scalar field layer
	};

	//! UI definition
	Ui::RasterizeToolDialog* m_UI;

	//! Cloud to rasterize
	ccGenericPointCloud* m_cloud;

	//! Whether cloud has scalar fields
	bool m_cloudHasScalarFields;

	//! Contour lines
	std::vector<ccPolyline*> m_contourLines;

	//! Delaunay interpolation parameters
	ccRasterGrid::DelaunayInterpolationParams m_delaunayInterpParams;

	//! Kriging parameters
	ccRasterGrid::KrigingParams m_krigingParams;
};
