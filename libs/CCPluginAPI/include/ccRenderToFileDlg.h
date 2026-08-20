// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccRenderToFileDlg.h
 *
 * @brief Dialog for exporting 3D views to image files.
 *
 * @details Dialog for rendering the current 3D view to an image file.
 *
 * ## Overview
 *
 * Allows exporting the current 3D view as:
 * - PNG, JPEG, BMP, TIFF images
 * - Custom resolution (zoom factor)
 * - With or without overlay items
 *
 * ## Supported Formats
 *
 * - PNG (lossless)
 * - JPEG (lossy, smaller files)
 * - BMP (uncompressed)
 * - TIFF (high quality)
 *
 * ## Usage
 *
 * @code
 * ccGLWindowInterface* win = MainWindow::GetActiveGLWindow();
 * ccRenderToFileDlg dialog(win, this);
 *
 * if (dialog.exec() == QDialog::Accepted) {
 *     QString filename = dialog.getFilename();
 *     float zoom = dialog.getZoom();
 *     bool overlay = dialog.renderOverlayItems();
 *     // Export...
 * }
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#pragma once

#include "CCPluginAPI.h"

// Qt
#include <QDialog>

namespace Ui
{
	class RenderToFileDialog;
}

class ccGLWindowInterface;

/**
 * @brief Dialog for rendering 3D view to image file.
 *
 * @details Provides options for exporting the current 3D view
 * to various image formats.
 *
 * Options:
 * - Output filename
 * - Zoom factor (for resolution)
 * - Point scaling
 * - Overlay rendering
 *
 * @extends QDialog
 */
class CCPLUGIN_LIB_API ccRenderToFileDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the render dialog.
	 *
	 * @param[in] win Window to render.
	 * @param[in] parent Parent widget.
	 */
	ccRenderToFileDlg(ccGLWindowInterface* win, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccRenderToFileDlg() override;

	/**
	 * @brief Hide scale and overlay options.
	 */
	void hideOptions();

	/**
	 * @brief Get requested zoom factor.
	 *
	 * @return Zoom multiplier for resolution.
	 */
	float getZoom() const;

	/**
	 * @brief Get output filename.
	 *
	 * @return Selected filename.
	 */
	QString getFilename() const;

	/**
	 * @brief Check if points should not be scaled.
	 *
	 * @return true to keep original point sizes.
	 */
	bool dontScalePoints() const;

	/**
	 * @brief Check if overlay items should be rendered.
	 *
	 * @return true to include labels, rulers, etc.
	 */
	bool renderOverlayItems() const;

  private slots:
	/**
	 * @brief Choose output file.
	 */
	void chooseFile();

	/**
	 * @brief Update info display.
	 */
	void updateInfo();

	/**
	 * @brief Save settings.
	 */
	void saveSettings();

	/**
	 * @brief Show output info.
	 */
	void showOutputInfo();

  private:
	//! Window to render
	ccGLWindowInterface* m_associatedWindow;

	//! Selected file filter
	QString m_selectedFilter;

	//! Current path
	QString m_currentPath;

	//! File filters
	QString m_filters;

	//! UI definition
	Ui::RenderToFileDialog* m_ui;
};
