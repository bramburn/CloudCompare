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
 * @brief Implementation of the zoom adjustment dialog
 * @details Allows users to adjust camera focal distance and pixel size
 * for precise zoom control in 3D views.
 * @see ccAdjustZoomDlg
 */

#include "ccAdjustZoomDlg.h"

// local
#include "ccGLWindowInterface.h"

/**
 * @brief Constructor
 * @param win GL window to adjust zoom for
 * @param parent Parent widget
 * @details Initializes the dialog with current viewport parameters
 * including window dimensions, focal distance, and pixel size.
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
		windowLabel->setText(QString("%1 [%2 x %3]").arg(win->getWindowTitle()).arg(win->glWidth()).arg(win->glHeight()));

		const ccViewportParameters& params = win->getViewportParameters();
		assert(!params.perspectiveView);

		m_windowWidth_pix = win->glWidth();
		if (m_windowWidth_pix < 1)
		{
			assert(false);
			m_windowWidth_pix = 1;
		}

		m_distanceToWidthRatio = params.computeDistanceToWidthRatio(win->glWidth(), win->glHeight());
		if (m_distanceToWidthRatio <= 0.0)
		{
			assert(false);
			m_distanceToWidthRatio = 1.0;
		}

		double focalDist = params.getFocalDistance();
		focalDoubleSpinBox->setValue(focalDist);

		pixelCountSpinBox->setValue(1);
		pixelSizeDoubleSpinBox->setValue(focalDist * m_distanceToWidthRatio / m_windowWidth_pix);
	}
	else
	{
		windowLabel->setText("Error");
	}

	connect(focalDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &ccAdjustZoomDlg::onFocalChanged);
	connect(pixelSizeDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &ccAdjustZoomDlg::onPixelSizeChanged);
	connect(pixelCountSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &ccAdjustZoomDlg::onPixelCountChanged);
}

/**
 * @brief Gets the currently configured focal distance
 * @return Focal distance value from the spin box
 */
double ccAdjustZoomDlg::getFocalDistance() const
{
	return focalDoubleSpinBox->value();
}

/**
 * @brief Handles focal distance spin box changes
 * @param focalDist New focal distance value
 * @details Recalculates pixel size based on the new focal distance,
 * maintaining consistency with the distance-to-width ratio.
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
 * @brief Handles pixel size spin box changes
 * @param pixelSizeNPixels New pixel size value
 * @details Recalculates focal distance based on the new pixel size,
 * maintaining consistency with the viewport parameters.
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
 * @brief Handles pixel count spin box changes
 * @param pixelCount New pixel count value
 * @details Recalculates pixel size based on the new pixel count,
 * which represents how many screen pixels correspond to one unit.
 */
void ccAdjustZoomDlg::onPixelCountChanged(int pixelCount)
{
	assert(pixelCount > 0);

	pixelSizeDoubleSpinBox->blockSignals(true);
	double pixelSizeNPixels = (focalDoubleSpinBox->value() * m_distanceToWidthRatio * pixelCount) / m_windowWidth_pix;
	pixelSizeDoubleSpinBox->setValue(pixelSizeNPixels);
	pixelSizeDoubleSpinBox->blockSignals(false);
}
