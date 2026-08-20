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
 * @brief Points sampling dialog
 *
 * Dialog for sampling points on a mesh.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_ptsSamplingDlg.h>

/**
 * @brief Points sampling dialog
 *
 * Configure point sampling parameters on mesh.
 */
class ccPtsSamplingDlg : public QDialog
    , public Ui::PointsSamplingDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccPtsSamplingDlg(QWidget* parent = nullptr);

	/// Get generate normals flag
	bool generateNormals() const;
	/// Get interpolate RGB flag
	bool interpolateRGB() const;
	/// Get interpolate texture flag
	bool interpolateTexture() const;

	/// Get use density flag
	bool useDensity() const;
	/// Get density value
	double getDensityValue() const;
	/// Get points number
	unsigned getPointsNumber() const;

	/// Set points number
	void setPointsNumber(int count);
	/// Set density value
	void setDensityValue(double density);
	/// Set generate normals
	void setGenerateNormals(bool state);
	/// Set use density
	void setUseDensity(bool state);
};

#endif // CC_POINTS_SAMPLING_DLG_HEADER
