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
 * @file ccColorGradientDlg.cpp
 * @brief Implementation of color gradient dialog
 * @details Dialog for configuring color gradient parameters including
 * default ramp, two-color custom ramp, and banding modes.
 * @see ccColorGradientDlg
 */

#include "ccColorGradientDlg.h"

// common
#include <ccQtHelpers.h>

// Qt
#include <QColorDialog>

// system
#include <assert.h>

// persistent parameters
static QColor                           s_firstColor(Qt::black);
static QColor                           s_secondColor(Qt::white);
static ccColorGradientDlg::GradientType s_lastType(ccColorGradientDlg::Default);
static double                           s_lastFreq     = 5.0;
static int                              s_lastDimIndex = 2;

/**
 * @brief Constructor
 * @param parent Parent widget
 * @details Initializes UI and restores previous settings.
 */
ccColorGradientDlg::ccColorGradientDlg(QWidget* parent)
    : QDialog(parent, Qt::Tool)
    , Ui::ColorGradientDialog()
{
	setupUi(this);

	connect(firstColorButton, &QAbstractButton::clicked, this, &ccColorGradientDlg::changeFirstColor);
	connect(secondColorButton, &QAbstractButton::clicked, this, &ccColorGradientDlg::changeSecondColor);

	// restore previous parameters
	ccQtHelpers::SetButtonColor(secondColorButton, s_secondColor);
	ccQtHelpers::SetButtonColor(firstColorButton, s_firstColor);
	setType(s_lastType);
	bandingFreqSpinBox->setValue(s_lastFreq);
	directionComboBox->setCurrentIndex(s_lastDimIndex);
}

/**
 * @brief Gets the gradient direction dimension
 * @return Dimension index (0=X, 1=Y, 2=Z)
 */
unsigned char ccColorGradientDlg::getDimension() const
{
	s_lastDimIndex = directionComboBox->currentIndex();
	return static_cast<unsigned char>(s_lastDimIndex);
}

/**
 * @brief Sets the gradient type
 * @param type Gradient type (Default, TwoColors, or Banding)
 */
void ccColorGradientDlg::setType(ccColorGradientDlg::GradientType type)
{
	switch (type)
	{
	case Default:
		defaultRampRadioButton->setChecked(true);
		break;
	case TwoColors:
		customRampRadioButton->setChecked(true);
		break;
	case Banding:
		bandingRadioButton->setChecked(true);
		break;
	default:
		assert(false);
	}
}

/**
 * @brief Gets the current gradient type
 * @return Current gradient type
 */
ccColorGradientDlg::GradientType ccColorGradientDlg::getType() const
{
	// ugly hack: we use 's_lastType' here as the type is only requested
	// when the dialog is accepted
	if (customRampRadioButton->isChecked())
		s_lastType = TwoColors;
	else if (bandingRadioButton->isChecked())
		s_lastType = Banding;
	else
		s_lastType = Default;

	return s_lastType;
}

/**
 * @brief Gets the custom gradient colors
 * @param[out] first First color
 * @param[out] second Second color
 */
void ccColorGradientDlg::getColors(QColor& first, QColor& second) const
{
	assert(customRampRadioButton->isChecked());
	first  = s_firstColor;
	second = s_secondColor;
}

/**
 * @brief Gets the banding frequency
 * @return Number of bands for banding mode
 */
double ccColorGradientDlg::getBandingFrequency() const
{
	// ugly hack: we use 's_lastFreq' here as the frequency is only requested
	// when the dialog is accepted
	s_lastFreq = bandingFreqSpinBox->value();
	return s_lastFreq;
}

/**
 * @brief Opens color picker for first gradient color
 */
void ccColorGradientDlg::changeFirstColor()
{
	QColor newCol = QColorDialog::getColor(s_firstColor, this);
	if (newCol.isValid())
	{
		s_firstColor = newCol;
		ccQtHelpers::SetButtonColor(firstColorButton, s_firstColor);
	}
}

/**
 * @brief Opens color picker for second gradient color
 */
void ccColorGradientDlg::changeSecondColor()
{
	QColor newCol = QColorDialog::getColor(s_secondColor, this);
	if (newCol.isValid())
	{
		s_secondColor = newCol;
		ccQtHelpers::SetButtonColor(secondColorButton, s_secondColor);
	}
}
