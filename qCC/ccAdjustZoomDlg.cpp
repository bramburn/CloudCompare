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
 * @file ccAdjustZoomDlg.cpp
 *
 * @brief Implementation of the zoom adjustment dialog.
 *
 * @details Implements the ccAdjustZoomDlg class which provides precise
 * camera focal distance control for orthographic 3D views.
 *
 * The dialog maintains consistency between three interrelated parameters:
 * - Focal distance (camera zoom)
 * - Pixel size (world units per pixel)
 * - Pixel count (pixels per world unit)
 *
 * When the user changes any parameter, the others are automatically
 * recalculated to maintain geometric consistency with the viewport.
 *
 * @see ccAdjustZoomDlg
 */

#include "ccAdjustZoomDlg.h"

// local
#include "ccGLWindowInterface.h"

/**
 * @brief Construct the adjust zoom dialog.
 *
 * @param[in] win The 3D window to adjust zoom for.
 * @param[in] parent Parent widget (optional).
 *
 * @details Initializes the dialog with current viewport parameters from
 * the specified GL window. Sets up the UI and connects signal handlers
 * for synchronized updates between focal distance, pixel size, and
 * pixel count controls.
 */
ccAdjustZoomDlg::ccAdjustZoomDlg(ccGLWindowInterface* win, QWidget* parent /*=nullptr*/)
    : QDialog(parent, Qt::Tool)
    , Ui::AdjustZoomDialog()
    , m_windowWidth_pix(0)
    , m_distanceToWidthRatio(0.0)
{
	setupUi(this);

	if (win)
	{
		// Display window title and dimensions
		windowLabel->setText(QString("%1 [%2 x %3]").arg(win->getWindowTitle()).arg(win->glWidth()).arg(win->glHeight()));

		const ccViewportParameters& params = win->getViewportParameters();
		assert(!params.perspectiveView);

		m_windowWidth_pix = win->glWidth();
		if (m_windowWidth_pix < 1)
		{
			assert(false);
			m_windowWidth_pix = 1;
		}

		// Compute the distance-to-width ratio for unit conversions
		m_distanceToWidthRatio = params.computeDistanceToWidthRatio(win->glWidth(), win->glHeight());
		if (m_distanceToWidthRatio <= 0.0)
		{
			assert(false);
			m_distanceToWidthRatio = 1.0;
		}

		// Initialize spin boxes with current values
		double focalDist = params.getFocalDistance();
		focalDoubleSpinBox->setValue(focalDist);

		pixelCountSpinBox->setValue(1);
		pixelSizeDoubleSpinBox->setValue(focalDist * m_distanceToWidthRatio / m_windowWidth_pix);
	}
	else
	{
		windowLabel->setText("Error");
	}

	// Connect spin box signals for synchronized updates
	connect(focalDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &ccAdjustZoomDlg::onFocalChanged);
	connect(pixelSizeDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &ccAdjustZoomDlg::onPixelSizeChanged);
	connect(pixelCountSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &ccAdjustZoomDlg::onPixelCountChanged);
}

/**
 * @brief Get the currently configured focal distance.
 *
 * @return Focal distance value from the spin box.
 *
 * @details Returns the focal distance that should be applied to the
 * viewport after the dialog is accepted.
 */
double ccAdjustZoomDlg::getFocalDistance() const
{
	return focalDoubleSpinBox->value();
}

/**
 * @brief Handle focal distance spin box changes.
 *
 * @param[in] focalDist New focal distance value.
 *
 * @details Recalculates pixel size based on the new focal distance,
 * maintaining consistency with the distance-to-width ratio.
 *
 * Formula: pixelSize = (focalDist * ratio * pixelCount) / windowWidth
 */
void ccAdjustZoomDlg::onFocalChanged(double focalDist)
{
	assert(pixelCountSpinBox->value() > 0);

	pixelSizeDoubleSpinBox->blockSignals(true);
	double pixelSizeNPixels = (focalDist * m_distanceToWidthRatio * pixelCountSpinBox->value()) / m_windowWidth_pix;
	pixelSizeDoubleSpinBox->setValue(pixelSizeNPixels);
	pixelSizeDoubleSpinBox->blockSignals(false);
}

/**
 * @brief Handle pixel size spin box changes.
 *
 * @param[in] pixelSizeNPixels New pixel size value.
 *
 * @details Recalculates focal distance based on the new pixel size,
 * maintaining consistency with the viewport parameters.
 *
 * Formula: focalDist = (pixelSize * windowWidth) / (pixelCount * ratio)
 */
void ccAdjustZoomDlg::onPixelSizeChanged(double pixelSizeNPixels)
{
	assert(pixelCountSpinBox->value() > 0);

	focalDoubleSpinBox->blockSignals(true);
	double focalDist = (pixelSizeNPixels * m_windowWidth_pix) / (pixelCountSpinBox->value() * m_distanceToWidthRatio);
	focalDoubleSpinBox->setValue(focalDist);
	focalDoubleSpinBox->blockSignals(false);
}

/**
 * @brief Handle pixel count spin box changes.
 *
 * @param[in] pixelCount New pixel count value.
 *
 * @details Recalculates pixel size based on the new pixel count,
 * which represents how many screen pixels correspond to one world unit.
 *
 * Formula: pixelSize = (focalDist * ratio * pixelCount) / windowWidth
 */
void ccAdjustZoomDlg::onPixelCountChanged(int pixelCount)
{
	assert(pixelCount > 0);

	pixelSizeDoubleSpinBox->blockSignals(true);
	double pixelSizeNPixels = (focalDoubleSpinBox->value() * m_distanceToWidthRatio * pixelCount) / m_windowWidth_pix;
	pixelSizeDoubleSpinBox->setValue(pixelSizeNPixels);
	pixelSizeDoubleSpinBox->blockSignals(false);
}
