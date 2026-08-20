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
 * @brief Color gradient dialog for configuring color ramps.
 *
 * @details Dialog for setting up color gradient parameters when
 * applying color gradients to entities. Supports:
 * - Default gradient (smooth color transition)
 * - Two-color gradient (simple ramp)
 * - Banding mode (stepped colors)
 *
 * Used by various color-related operations in CloudCompare
 * such as coloring by height, intensity, or scalar fields.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccEntityAction::setColorGradient()
 */

// Qt
#include <QColor>
#include <ui_colorGradientDlg.h>

/**
 * @brief Dialog for configuring color gradient settings.
 *
 * @details Provides a UI for setting up color gradients used
 * when coloring entities. The user can choose:
 *
 * 1. **Default**: Smooth color transition between two colors
 *
 * 2. **Two Colors**: Simple gradient from color 1 to color 2
 *
 * 3. **Banding**: Stepped colors at regular intervals
 *    (useful for creating contour-like effects)
 *
 * @extends QDialog
 * @extends Ui::ColorGradientDialog
 */
class ccColorGradientDlg : public QDialog
    , public Ui::ColorGradientDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Gradient types.
	 */
	enum GradientType
	{
		Default,    //!< Smooth gradient transition
		TwoColors,  //!< Simple two-color ramp
		Banding     //!< Stepped/banded gradient
	};

	/**
	 * @brief Construct the color gradient dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccColorGradientDlg(QWidget* parent);

	/**
	 * @brief Get the selected gradient type.
	 *
	 * @return Gradient type.
	 */
	GradientType getType() const;

	/**
	 * @brief Set the gradient type.
	 *
	 * @param[in] type Gradient type to select.
	 */
	void setType(GradientType type);

	/**
	 * @brief Get the selected gradient colors.
	 *
	 * @param[out] first First color (start of gradient).
	 * @param[out] second Second color (end of gradient).
	 */
	void getColors(QColor& first, QColor& second) const;

	/**
	 * @brief Get the banding frequency.
	 *
	 * @return Number of bands.
	 *
	 * @details Only meaningful when type is Banding.
	 */
	double getBandingFrequency() const;

	/**
	 * @brief Get the ramp dimension.
	 *
	 * @return Dimension (1, 2, or 3).
	 *
	 * @details Specifies whether the gradient applies to
	 * 1D (height), 2D (XY), or 3D (XYZ) coordinates.
	 */
	unsigned char getDimension() const;

  protected slots:
	/**
	 * @brief Handle first color selection.
	 */
	void changeFirstColor();

	/**
	 * @brief Handle second color selection.
	 */
	void changeSecondColor();
};

#endif // CC_COLOR_GRADIENT_DLG_HEADER
