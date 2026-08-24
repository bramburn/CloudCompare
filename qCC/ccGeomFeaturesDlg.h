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
// #                   COPYRIGHT: Daniel Girardeau-Montaut                  #
// #                                                                        #
// ##########################################################################

#ifndef CC_GEOM_FEATURES_DIALOG_HEADER
#define CC_GEOM_FEATURES_DIALOG_HEADER

/**
 * @file ccGeomFeaturesDlg.h
 *
 * @brief Geometry features dialog
 *
 * Dialog for selecting geometric features to compute.
 *
 * @author Daniel Girardeau-Montaut
 */

// Local
#include "ccLibAlgorithms.h"

// Qt
#include <QDialog>
#include <ui_geomFeaturesDlg.h>

/**
 * @brief Geometry features dialog
 *
 * Select geometric features to compute.
 */
class ccGeomFeaturesDlg : public QDialog
    , public Ui::GeomFeaturesDialog
{
  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccGeomFeaturesDlg(QWidget* parent = nullptr);

	/**
	 * @brief Set selected features
	 * @param[in] features Features to select
	 */
	void setSelectedFeatures(const ccLibAlgorithms::GeomCharacteristicSet& features);

	/**
	 * @brief Get selected features
	 * @param[out] features Selected features
	 * @return true if selection is valid
	 */
	bool getSelectedFeatures(ccLibAlgorithms::GeomCharacteristicSet& features) const;

	/**
	 * @brief Set radius
	 * @param[in] r Radius
	 */
	void setRadius(double r);

	/**
	 * @brief Get radius
	 * @return Radius
	 */
	double getRadius() const;

	/**
	 * @brief Set up direction
	 * @param[in] upDir Up direction
	 */
	void setUpDirection(const CCVector3& upDir);

	/**
	 * @brief Get up direction
	 * @return Up direction, or nullptr
	 */
	CCVector3* getUpDirection() const;

	/// Reset dialog
	void reset();

  protected:
	struct Option : ccLibAlgorithms::GeomCharacteristic
	{
		Option(QCheckBox* cb, CCCoreLib::GeometricalAnalysisTools::GeomCharacteristic c, int option = 0)
		    : ccLibAlgorithms::GeomCharacteristic(c, option)
		    , checkBox(cb)
		{
		}

		QCheckBox* checkBox = nullptr;
	};

	std::vector<Option> m_options;
};

#endif // CC_GEOM_FEATURES_DIALOG_HEADER
