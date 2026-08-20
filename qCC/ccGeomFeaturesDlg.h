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
// #                   COPYRIGHT: Daniel Girardeau-Montaut                   #
// #                                                                        #
// ##########################################################################

#ifndef CC_GEOM_FEATURES_DIALOG_HEADER
#define CC_GEOM_FEATURES_DIALOG_HEADER

/**
 * @file ccGeomFeaturesDlg.h
 *
 * @brief Geometry features dialog for selecting geometric computations.
 *
 * @details Dialog for selecting which geometric features to compute
 * on point clouds.
 *
 * Geometric features include statistical measures of local surface geometry:
 * - **Curvature**: How curved the surface is at each point
 * - **Roughness**: Local deviation from a fitted plane
 * - **Density**: Local point density
 * - **Moment of inertia**: Surface anisotropy measures
 * - **Moment of inertia along normals**: Normal-based anisotropy
 *
 * These features are computed using neighborhood analysis
 * with a configurable radius.
 *
 * @author Daniel Girardeau-Montaut
 *
 * @see ccLibAlgorithms
 * @see CCCoreLib::GeometricalAnalysisTools
 */

// Local
#include "ccLibAlgorithms.h"

// Qt
#include <QDialog>
#include <ui_geomFeaturesDlg.h>

/**
 * @brief Dialog for selecting geometric features to compute.
 *
 * @details Provides a UI for choosing which geometric features
 * to compute on point clouds.
 *
 * Features are computed locally around each point using a
 * specified neighborhood radius.
 *
 * @extends QDialog
 * @extends Ui::GeomFeaturesDialog
 */
class ccGeomFeaturesDlg : public QDialog
    , public Ui::GeomFeaturesDialog
{
  public:
	/**
	 * @brief Construct the geometry features dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccGeomFeaturesDlg(QWidget* parent = nullptr);

	/**
	 * @brief Set the initially selected features.
	 *
	 * @param[in] features Features to pre-select.
	 */
	void setSelectedFeatures(const ccLibAlgorithms::GeomCharacteristicSet& features);

	/**
	 * @brief Get the selected features.
	 *
	 * @param[out] features Selected feature set.
	 * @return true if at least one feature is selected.
	 */
	bool getSelectedFeatures(ccLibAlgorithms::GeomCharacteristicSet& features) const;

	/**
	 * @brief Set the neighborhood radius.
	 *
	 * @param[in] r Radius value.
	 */
	void setRadius(double r);

	/**
	 * @brief Get the neighborhood radius.
	 *
	 * @return Radius value.
	 */
	double getRadius() const;

	/**
	 * @brief Set the up direction for 2.5D features.
	 *
	 * @param[in] upDir Up direction vector.
	 *
	 * @details Used for features that require a reference direction
	 * (e.g., vertical roughness).
	 */
	void setUpDirection(const CCVector3& upDir);

	/**
	 * @brief Get the up direction.
	 *
	 * @return Up direction, or nullptr if not set.
	 */
	CCVector3* getUpDirection() const;

	/**
	 * @brief Reset the dialog to defaults.
	 */
	void reset();

  protected:
	/**
	 * @brief Feature option with checkbox.
	 */
	struct Option : ccLibAlgorithms::GeomCharacteristic
	{
		/**
		 * @brief Construct a feature option.
		 *
		 * @param[in] cb Checkbox for this feature.
		 * @param[in] c Feature type.
		 * @param[in] option Sub-option.
		 */
		Option(QCheckBox* cb, CCCoreLib::GeometricalAnalysisTools::GeomCharacteristic c, int option = 0)
		    : ccLibAlgorithms::GeomCharacteristic(c, option)
		    , checkBox(cb)
		{
		}

		//! Checkbox for this option
		QCheckBox* checkBox = nullptr;
	};

	//! Feature options
	std::vector<Option> m_options;
};

#endif // CC_GEOM_FEATURES_DIALOG_HEADER
