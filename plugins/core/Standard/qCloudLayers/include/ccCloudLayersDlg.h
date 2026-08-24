#pragma once

// ##########################################################################
// #                                                                        #
// #                   CLOUDCOMPARE PLUGIN: qCloudLayers                    #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                     COPYRIGHT: WigginsTech 2022                        #
// #                                                                        #
// ##########################################################################

/**
 * @file ccCloudLayersDlg.h
 *
 * @brief Cloud layers dialog
 *
 * Dialog for managing point cloud layers (ASPRS classification).
 *
 * @author WigginsTech 2022
 */

// local
#include "ccAsprsModel.h"
#include "ccCloudLayersHelper.h"

#include <ui_ccCloudLayersDlg.h>

// CC
#include <ccOverlayDialog.h>

class ccPointCloud;
class ccMouseCircle;

class QString;
class QModelIndex;

/**
 * @class ccCloudLayersDlg
 *
 * @brief Cloud layers dialog
 *
 * Manage point cloud layers with ASPRS classification.
 */
class ccCloudLayersDlg : public ccOverlayDialog
    , public Ui::ccCloudLayersDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] app Main application interface
	 * @param[in] parent Parent widget
	 */
	explicit ccCloudLayersDlg(ccMainAppInterface* app, QWidget* parent = nullptr);

	/// Destructor
	virtual ~ccCloudLayersDlg();

	/// Start dialog
	bool start() override;

	/// Stop dialog
	void stop(bool accepted) override;

	/**
	 * @brief Set point cloud
	 * @param[in] cloud Point cloud
	 * @return Success
	 */
	bool setPointCloud(ccPointCloud* cloud);

  private:
	/// Reset UI state
	void resetUI();

	/// Initialize table view
	void initTableView();

	/// Save settings
	void saveSettings();

	/// Load settings
	void loadSettings();

	/// Event filter
	bool eventFilter(QObject* obj, QEvent* event) override;

	/// Reject dialog
	void reject() override;

  private Q_SLOTS:

	/// Add new ASPRS item
	void addClicked();

	/// Delete selected ASPRS items
	void deleteClicked();

	/// Start drawing mouse circle
	void startClicked();

	/// Pause drawing mouse circle
	void pauseClicked();

	/// Apply changes and close
	void applyClicked();

	/// Close dialog
	void closeClicked()
	{
		reject();
	}

	/// Scalar field index changed
	void scalarFieldIndexChanged(int index);

	/// Input class index changed
	void inputClassIndexChanged(int index);

	/// Output class index changed
	void outputClassIndexChanged(int index);

	/// ASPRS model signals
	void codeChanged(ccAsprsModel::AsprsItem item, int oldCode);
	void colorChanged(ccAsprsModel::AsprsItem item);
	void classNameChanged(int row, QString newName);

	/// Show color picker dialog
	void tableViewDoubleClicked(const QModelIndex& index);

	/// Update input and output comboboxes
	void updateInputOutput();
	void swapInputOutput();

	void mouseMoved(int x, int y, Qt::MouseButtons buttons);

  private:
	ccMainAppInterface* m_app;
	ccAsprsModel m_asprsModel;
	ccCloudLayersHelper* m_helper;
	ccMouseCircle* m_mouseCircle;
};
