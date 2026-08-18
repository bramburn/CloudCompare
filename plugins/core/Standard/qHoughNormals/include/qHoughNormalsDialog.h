//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qHoughNormals               #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                  COPYRIGHT: Daniel Girardeau-Montaut                   #
//#                                                                        #
//##########################################################################

#ifndef QHOUGH_NORMALS_HEADER
#define QHOUGH_NORMALS_HEADER

/**
 * @file qHoughNormalsDialog.h
 *
 * @brief Hough normals dialog
 *
 * Dialog for Hough normals parameters.
 */

#include <QDialog>

namespace Ui {
	class HoughNormalsDialog;
}

/**
 * @class qHoughNormalsDialog
 *
 * @brief Hough normals dialog
 *
 * Dialog for configuring Hough normals parameters.
 */
class qHoughNormalsDialog : public QDialog
{
public:

	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit qHoughNormalsDialog( QWidget* parent = nullptr );
	
	/// Destructor
	~qHoughNormalsDialog();

	/**
	 * @struct Parameters
	 *
	 * @brief Hough normals parameters
	 */
	struct Parameters
	{
		int K = 100;
		int T = 1000;
		int n_phi = 15;
		int n_rot = 5;
		bool use_density = false;
		float tol_angle_rad = 0.79f;
		int k_density = 5;
	};
	
	/**
	 * @brief Set parameters
	 * @param[in] params Parameters
	 */
	void setParameters( const Parameters& params );
	
	/**
	 * @brief Get parameters
	 * @param[out] params Parameters
	 */
	void getParameters( Parameters& params );
	
private:
	/// UI
	Ui::HoughNormalsDialog* m_ui;
};

#endif //QHOUGH_NORMALS_HEADER
