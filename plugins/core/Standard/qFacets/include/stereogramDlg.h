#pragma once

//##########################################################################
//#                                                                        #
//#                     CLOUDCOMPARE PLUGIN: qFacets                       #
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
//#                      COPYRIGHT: Thomas Dewez, BRGM                     #
//#                                                                        #
//##########################################################################

/**
 * @file stereogramDlg.h
 *
 * @brief Stereogram dialog
 *
 * Dialog for stereogram parameters and visualization.
 */

#include "ui_stereogramDlg.h"
#include "ui_stereogramParamsDlg.h"

//qCC_db
#include <ccColorScale.h>
#include <ccPlane.h>

// Qt
#include <QDialog>

//system
#include <utility>

class ccHObject;
class FacetDensityGrid;
class ccColorScaleSelector;
class ccMainAppInterface;

/**
 * @class StereogramParamsDlg
 *
 * @brief Stereogram parameters dialog
 *
 * Dialog for stereogram parameters.
 */
class StereogramParamsDlg : public QDialog, public Ui::StereogramParamsDlg
{
public:

	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	StereogramParamsDlg(QWidget* parent = nullptr)
		: QDialog(parent, Qt::Tool)
		, Ui::StereogramParamsDlg()
	{
		setupUi(this);
	}
};

/**
 * @class StereogramWidget
 *
 * @brief Stereogram widget
 *
 * Orientation-based classification widget.
 */
class StereogramWidget : public QLabel
{
	Q_OBJECT

public:

	/// Constructor
	StereogramWidget(QWidget* parent = nullptr);

	/// Destructor
	~StereogramWidget() override;

	/**
	 * @brief Initialize widget
	 * @param[in] angularStep_deg Angular step in degrees
	 * @param[in] facetGroup Facet group
	 * @param[in] resolution_deg Resolution in degrees
	 * @return Success
	 */
	bool init(	double angularStep_deg,
				ccHObject* facetGroup,
				double resolution_deg = 2.0);

	/**
	 * @brief Get mean direction
	 * @param[out] meanDip_deg Mean dip
	 * @param[out] meanDipDir_deg Mean dip direction
	 */
	void getMeanDir(double& meanDip_deg, double& meanDipDir_deg) { meanDip_deg = m_meanDip_deg; meanDipDir_deg = m_meanDipDir_deg; }

	/// Height for width
	int	heightForWidth (int w) const override { return w; }

	/**
	 * @brief Set density color scale
	 * @param[in] colorScale Color scale
	 */
	void setDensityColorScale(ccColorScale::Shared colorScale) { m_densityColorScale = colorScale; }
	
	/// Get density color scale
	ccColorScale::Shared getDensityColorScale() const { return m_densityColorScale; }
