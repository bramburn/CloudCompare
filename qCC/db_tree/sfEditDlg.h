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

#ifndef CC_SF_EDIT_DIALOG_HEADER
#define CC_SF_EDIT_DIALOG_HEADER

/**
 * @file sfEditDlg.h
 *
 * @brief Scalar field edit dialog
 *
 * Widget for editing scalar field display parameters.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QWidget>

class ccScalarField;
class ccHistogramWindow;

namespace Ui
{
	class SFEditDlg;
}

/**
 * @brief Scalar field edit dialog
 *
 * Scalar field display parameters editor.
 */
class sfEditDlg : public QWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Create widget
	 * @param[in] parent Parent widget
	 */
	explicit sfEditDlg(QWidget* parent = nullptr);

	/// Destructor
	~sfEditDlg();

	/**
	 * @brief Fill with scalar field
	 * @param[in] sf Scalar field
	 */
	void fillDialogWith(ccScalarField* sf);

  public:
	/// Handle min value spinbox changed
	void minValSBChanged(double);
	/// Handle max value spinbox changed
	void maxValSBChanged(double);
	/// Handle min saturation spinbox changed
	void minSatSBChanged(double);
	/// Handle max saturation spinbox changed
	void maxSatSBChanged(double);

	/// Handle min value from histogram changed
	void minValHistoChanged(double);
	/// Handle max value from histogram changed
	void maxValHistoChanged(double);
	/// Handle min saturation from histogram changed
	void minSatHistoChanged(double);
	/// Handle max saturation from histogram changed
	void maxSatHistoChanged(double);

	/// Handle NaN in gray changed
	void nanInGrayChanged(bool);
	/// Handle always show 0 changed
	void alwaysShow0Changed(bool);
	/// Handle symmetrical scale changed
	void symmetricalScaleChanged(bool);
	/// Handle log scale changed
	void logScaleChanged(bool);

  signals:

	/// Emitted when SF display parameters change
	void entitySFHasChanged();

  protected:
	/// Convert spinbox to slider value
	double dispSpin2slider(double val) const;
	/// Convert saturation spinbox to slider value
	double satSpin2slider(double val) const;
	/// Convert slider to spinbox value
	double dispSlider2spin(int pos) const;
	/// Convert saturation slider to spinbox value
	double satSlider2spin(int pos) const;

	/// Associated scalar field
	ccScalarField* m_associatedSF;
	/// Associated histogram
	ccHistogramWindow* m_associatedSFHisto;

	Ui::SFEditDlg* m_ui;
};

#endif // CC_SF_EDIT_DIALOG_HEADER
