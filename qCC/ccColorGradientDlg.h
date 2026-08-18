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

#ifndef CC_COLOR_GRADIENT_DLG_HEADER
#define CC_COLOR_GRADIENT_DLG_HEADER

/**
 * @file ccColorGradientDlg.h
 *
 * @brief Color gradient dialog
 *
 * Dialog for configuring color gradients.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QColor>
#include <ui_colorGradientDlg.h>

/**
 * @brief Color gradient dialog
 *
 * Configure color gradient settings.
 */
class ccColorGradientDlg : public QDialog
    , public Ui::ColorGradientDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccColorGradientDlg(QWidget* parent);

	/// Gradient type
	enum GradientType
	{
		Default,   //!< Default gradient
		TwoColors, //!< Two colors
		Banding    //!< Banding mode
	};

	/**
	 * @brief Get gradient type
	 * @return Gradient type
	 */
	GradientType getType() const;
	
	/**
	 * @brief Set gradient type
	 * @param[in] type Gradient type
	 */
	void setType(GradientType type);

	/**
	 * @brief Get gradient colors
	 * @param[out] first First color
	 * @param[out] second Second color
	 */
	void getColors(QColor& first, QColor& second) const;

	/**
	 * @brief Get banding frequency
	 * @return Banding frequency
	 */
	double getBandingFrequency() const;

	/**
	 * @brief Get dimension
	 * @return Ramp dimension
	 */
	unsigned char getDimension() const;

  protected:
	void changeFirstColor();
	void changeSecondColor();
};

#endif // CC_COLOR_GRADIENT_DLG_HEADER
