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

#ifndef CC_POINTS_SAMPLING_DLG_HEADER
#define CC_POINTS_SAMPLING_DLG_HEADER

/**
 * @file ccPtsSamplingDlg.h
 *
 * @brief Points sampling dialog for mesh point sampling.
 *
 * @details Dialog for configuring parameters when sampling points
 * on mesh surfaces. Sampling generates a point cloud from a mesh
 * by placing points on the mesh triangles.
 *
 * Sampling modes:
 * - **Density-based**: Specify points per unit area
 * - **Total points**: Specify total number of points to generate
 *
 * Interpolation options:
 * - Generate normals at sampled points
 * - Interpolate RGB colors from mesh
 * - Interpolate texture coordinates
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccEntityAction::samplePoints()
 */

#include <ui_ptsSamplingDlg.h>

/**
 * @brief Dialog for configuring mesh point sampling.
 *
 * @details Provides a UI for setting up point sampling parameters
 * when converting a mesh to a point cloud.
 *
 * Two sampling modes:
 * 1. **Density mode**: Points per surface unit (e.g., points/m²)
 * 2. **Total points mode**: Exact number of points to generate
 *
 * Interpolation options preserve mesh properties on sampled points:
 * - Normals for surface orientation
 * - RGB colors for visualization
 * - Texture coordinates for mapping
 *
 * @extends QDialog
 * @extends Ui::PointsSamplingDialog
 */
class ccPtsSamplingDlg : public QDialog
    , public Ui::PointsSamplingDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the points sampling dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccPtsSamplingDlg(QWidget* parent = nullptr);

	/**
	 * @brief Check if normals should be generated.
	 * @return true if normals generation is enabled.
	 */
	bool generateNormals() const;

	/**
	 * @brief Check if RGB colors should be interpolated.
	 * @return true if RGB interpolation is enabled.
	 */
	bool interpolateRGB() const;

	/**
	 * @brief Check if texture should be interpolated.
	 * @return true if texture interpolation is enabled.
	 */
	bool interpolateTexture() const;

	/**
	 * @brief Check if density mode is used.
	 * @return true if density-based sampling is enabled.
	 */
	bool useDensity() const;

	/**
	 * @brief Get the density value.
	 * @return Points per unit area.
	 */
	double getDensityValue() const;

	/**
	 * @brief Get the number of points to generate.
	 * @return Number of points.
	 */
	unsigned getPointsNumber() const;

	/**
	 * @brief Set the number of points.
	 * @param[in] count Number of points.
	 */
	void setPointsNumber(int count);

	/**
	 * @brief Set the density value.
	 * @param[in] density Points per unit area.
	 */
	void setDensityValue(double density);

	/**
	 * @brief Set whether to generate normals.
	 * @param[in] state Generation state.
	 */
	void setGenerateNormals(bool state);

	/**
	 * @brief Set whether to use density mode.
	 * @param[in] state Density mode state.
	 */
	void setUseDensity(bool state);
};

#endif // CC_POINTS_SAMPLING_DLG_HEADER
