// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file sfEditDlg.h
 *
 * @brief Scalar field display parameters editor widget.
 *
 * @details Widget for configuring scalar field visualization parameters
 * including display range, saturation, and color mapping options.
 *
 * Parameters:
 * - **Display range**: Min/max values shown in view
 * - **Saturation range**: Values mapped to full color
 * - **NaN handling**: Show NaN points in gray
 * - **Scale**: Linear or logarithmic
 * - **Symmetrical**: Center on zero
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccScalarField for scalar field implementation
 * @see ccHistogramWindow for histogram display
 */

#ifndef CC_SF_EDIT_DIALOG_HEADER
#define CC_SF_EDIT_DIALOG_HEADER

// Qt
#include <QWidget>

class ccScalarField;
class ccHistogramWindow;

namespace Ui
{
	class SFEditDlg;
}

/**
 * @brief Scalar field display parameters editor.
 *
 * @details Widget for configuring how scalar field values are
 * displayed in the 3D view.
 *
 * Features:
 * - Display range sliders and spinboxes
 * - Saturation range control
 * - Logarithmic scale option
 * - Symmetrical scale (centered on zero)
 * - NaN handling
 *
 * @extends QWidget
 */
class sfEditDlg : public QWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the SF editor.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit sfEditDlg(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~sfEditDlg();

	/**
	 * @brief Fill dialog with scalar field.
	 *
	 * @param[in] sf Scalar field to configure.
	 */
	void fillDialogWith(ccScalarField* sf);

  public slots:
	/**
	 * @brief Handle min value spinbox change.
	 *
	 * @param[in] val New minimum.
	 */
	void minValSBChanged(double val);

	/**
	 * @brief Handle max value spinbox change.
	 *
	 * @param[in] val New maximum.
	 */
	void maxValSBChanged(double val);

	/**
	 * @brief Handle min saturation spinbox change.
	 *
	 * @param[in] val New min saturation.
	 */
	void minSatSBChanged(double val);

	/**
	 * @brief Handle max saturation spinbox change.
	 *
	 * @param[in] val New max saturation.
	 */
	void maxSatSBChanged(double val);

	/**
	 * @brief Handle min value from histogram change.
	 *
	 * @param[in] val New minimum.
	 */
	void minValHistoChanged(double val);

	/**
	 * @brief Handle max value from histogram change.
	 *
	 * @param[in] val New maximum.
	 */
	void maxValHistoChanged(double val);

	/**
	 * @brief Handle min saturation from histogram change.
	 *
	 * @param[in] val New min saturation.
	 */
	void minSatHistoChanged(double val);

	/**
	 * @brief Handle max saturation from histogram change.
	 *
	 * @param[in] val New max saturation.
	 */
	void maxSatHistoChanged(double val);

	/**
	 * @brief Handle NaN in gray toggle.
	 *
	 * @param[in] state Show NaN in gray.
	 */
	void nanInGrayChanged(bool state);

	/**
	 * @brief Handle always show 0 toggle.
	 *
	 * @param[in] state Always show zero.
	 */
	void alwaysShow0Changed(bool state);

	/**
	 * @brief Handle symmetrical scale toggle.
	 *
	 * @param[in] state Symmetrical scale.
	 */
	void symmetricalScaleChanged(bool state);

	/**
	 * @brief Handle log scale toggle.
	 *
	 * @param[in] state Logarithmic scale.
	 */
	void logScaleChanged(bool state);

  signals:
	/**
	 * @brief Emitted when SF parameters change.
	 */
	void entitySFHasChanged();

  protected:
	/**
	 * @brief Convert display spinbox to slider.
	 *
	 * @param[in] val Spinbox value.
	 *
	 * @return Slider position.
	 */
	double dispSpin2slider(double val) const;

	/**
	 * @brief Convert saturation spinbox to slider.
	 *
	 * @param[in] val Spinbox value.
	 *
	 * @return Slider position.
	 */
	double satSpin2slider(double val) const;

	/**
	 * @brief Convert slider to display spinbox.
	 *
	 * @param[in] pos Slider position.
	 *
	 * @return Spinbox value.
	 */
	double dispSlider2spin(int pos) const;

	/**
	 * @brief Convert saturation slider to spinbox.
	 *
	 * @param[in] pos Slider position.
	 *
	 * @return Spinbox value.
	 */
	double satSlider2spin(int pos) const;

  private:
	//! Associated scalar field
	ccScalarField* m_associatedSF;

	//! Associated histogram
	ccHistogramWindow* m_associatedSFHisto;

	//! UI definition
	Ui::SFEditDlg* m_ui;
};

#endif // CC_SF_EDIT_DIALOG_HEADER
