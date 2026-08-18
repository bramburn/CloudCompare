#pragma once
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
 * @file ccRenderToFileDlg.h
 *
 * @brief Render to file dialog
 *
 * Dialog for exporting 3D view to image file.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "CCPluginAPI.h"

// Qt
#include <QDialog>

namespace Ui
{
	class RenderToFileDialog;
}

class ccGLWindowInterface;

/**
 * @brief Render to file dialog
 *
 * Export 3D view to image file.
 */
class CCPLUGIN_LIB_API ccRenderToFileDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create render dialog
	 * @param[in] win Window to render
	 * @param[in] parent Parent widget
	 */
	ccRenderToFileDlg(ccGLWindowInterface* win, QWidget* parent = nullptr);

	/**
	 * @brief Destructor
	 */
	~ccRenderToFileDlg() override;

	/**
	 * @brief Hide scale and overlay options
	 */
	void hideOptions();

	/**
	 * @brief Get requested zoom
	 * @return Zoom factor
	 */
	float getZoom() const;
	
	/**
	 * @brief Get output filename
	 * @return Filename
	 */
	QString getFilename() const;
	
	/**
	 * @brief Get scale setting
	 * @return true if points should not be scaled
	 */
	bool dontScalePoints() const;
	
	/**
	 * @brief Get overlay setting
	 * @return true if overlay items should be rendered
	 */
	bool renderOverlayItems() const;

  private:
	/// Choose output file
	void chooseFile();
	
	/// Update info display
	void updateInfo();
	
	/// Save settings
	void saveSettings();
	
	/// Show output info
	void showOutputInfo();

  private:
	/// Window to render
	ccGLWindowInterface* m_associatedWindow;

	/// Selected file filter
	QString m_selectedFilter;
	
	/// Current path
	QString m_currentPath;
	
	/// File filters
	QString m_filters;

	/// UI
	Ui::RenderToFileDialog* m_ui;
};
