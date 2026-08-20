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

#ifndef CC_ADJUST_ZOOM_DIALOG_HEADER
#define CC_ADJUST_ZOOM_DIALOG_HEADER

/**
 * @file ccAdjustZoomDlg.h
 *
 * @brief Adjust zoom dialog for precise camera focal distance control.
 *
 * @details Dialog that allows users to precisely set the camera focal distance
 * in a 3D view when operating in orthographic mode. Provides synchronized
 * controls for focal distance, pixel size, and pixel count.
 *
 * This dialog is particularly useful for:
 * - Calibrating the visual scale of orthographic views
 * - Matching zoom levels between different windows
 * - Precise positioning of the camera for measurements
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccGLWindowInterface
 * @see ccViewportParameters
 */

#include <QDialog>
#include <ui_adjustZoomDlg.h>

class ccGLWindowInterface;

/**
 * @brief Dialog for adjusting camera focal distance in orthographic mode.
 *
 * @details Provides precise control over the camera's focal distance through
 * three synchronized parameters:
 * - Focal distance: the camera's focal length
 * - Pixel size: the size of one pixel in world units
 * - Pixel count: how many pixels represent one world unit
 *
 * Changes to any of these parameters automatically recalculate the others
 * to maintain consistency with the viewport's geometry.
 *
 * @extends QDialog
 * @extends Ui::AdjustZoomDialog
 *
 * @par Usage
 * @code
 * ccAdjustZoomDlg* dlg = new ccAdjustZoomDlg(glWindow, this);
 * if (dlg->exec() == QDialog::Accepted) {
 *     double focal = dlg->getFocalDistance();
 *     // Apply to window...
 * }
 * @endcode
 */
class ccAdjustZoomDlg : public QDialog
    , public Ui::AdjustZoomDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the adjust zoom dialog.
	 *
	 * @param[in] win The 3D window to adjust zoom for.
	 * @param[in] parent Parent widget (optional).
	 *
	 * @details Initializes the dialog with the current viewport parameters
	 * from the specified GL window. The dialog reads the window's current
	 * focal distance, dimensions, and viewport parameters.
	 *
	 * @note The window must be in orthographic mode for this dialog
	 *       to function correctly.
	 */
	ccAdjustZoomDlg(ccGLWindowInterface* win, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccAdjustZoomDlg() = default;

	/**
	 * @brief Get the requested focal distance.
	 *
	 * @return The focal distance value configured by the user.
	 *
	 * @details Returns the focal distance that should be applied to the
	 * viewport after the dialog is accepted.
	 */
	double getFocalDistance() const;

  protected slots:
	/**
	 * @brief Handle focal distance spin box changes.
	 *
	 * @param[in] focalDist New focal distance value.
	 *
	 * @details Recalculates pixel size based on the new focal distance,
	 * maintaining consistency with the distance-to-width ratio.
	 */
	void onFocalChanged(double focalDist);

	/**
	 * @brief Handle pixel size spin box changes.
	 *
	 * @param[in] pixelSizeNPixels New pixel size value.
	 *
	 * @details Recalculates focal distance based on the new pixel size,
	 * maintaining consistency with the viewport parameters.
	 */
	void onPixelSizeChanged(double pixelSizeNPixels);

	/**
	 * @brief Handle pixel count spin box changes.
	 *
	 * @param[in] pixelCount New pixel count value.
	 *
	 * @details Recalculates pixel size based on the new pixel count,
	 * which represents how many screen pixels correspond to one world unit.
	 */
	void onPixelCountChanged(int pixelCount);

  protected:
	/**
	 * @brief Width of the GL window in pixels.
	 */
	int m_windowWidth_pix;

	/**
	 * @brief Ratio of distance to viewport width.
	 *
	 * Used for consistent unit conversion between focal distance
	 * and pixel size.
	 */
	double m_distanceToWidthRatio;
};

#endif // CC_ADJUST_ZOOM_DIALOG_HEADER
