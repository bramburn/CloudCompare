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
 * @brief 2.5D data editor
 *
 * Generic interface for 2.5D raster data editing.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
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
 * @brief 2.5D editor interface
 *
 * Generic interface for 2.5D raster data editing.
 */
class cc2Point5DimEditor
{
  public:
	/// Default constructor
	cc2Point5DimEditor();

	/// Destructor
	virtual ~cc2Point5DimEditor();

  protected: // standard methods
	/// Get projection grid step
	virtual double getGridStep() const = 0;

	/// Get projection dimension (0=X, 1=Y, 2=Z)
	virtual unsigned char getProjectionDimension() const = 0;

	/// Get projection type
	virtual ccRasterGrid::ProjectionType getTypeOfProjection() const = 0;

	/// Get custom bbox
	virtual ccBBox getCustomBBox() const;

	/// Grid is up-to-date
	virtual void gridIsUpToDate(bool state) = 0;

	/// Update 2D display zoom
	virtual void update2DDisplayZoom(ccBBox& box);

  protected: // raster grid related stuff
	/// Show grid box editor
	virtual bool showGridBoxEditor();

	/// Get grid size as string
	virtual QString getGridSizeAsString() const;

	/// Get grid size
	virtual bool getGridSize(unsigned& width, unsigned& height) const;

	/// Create bounding box editor
	void createBoundingBoxEditor(const ccBBox& gridBBox, QWidget* parent);

	/// Create 2D view
	void create2DView(QFrame* parentFrame);

	/// Get fill empty cells strategy
	ccRasterGrid::EmptyCellFillOption getFillEmptyCellsStrategy(QComboBox* comboBox) const;

  public:
	/// Convert raster grid to point cloud
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
	// Members
	ccBoundingBoxEditorDlg* m_bbEditorDlg = nullptr;
	ccGLWindowInterface*     m_glWindow    = nullptr;
	ccPointCloud*            m_rasterCloud = nullptr;
	ccRasterGrid             m_grid;
};

#endif // CC_2_5D_EDITOR_HEADER
