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
 * @file ccStereoModeDlg.h
 *
 * @brief Stereo mode dialog
 *
 * Dialog for configuring 3D stereo display settings.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "CCAppCommon.h"

// qCC_gl
#include <ccGLWindowInterface.h>

// Qt
#include <QDialog>

namespace Ui
{
	class StereoModeDialog;
}

/**
 * @brief Stereo mode dialog
 *
 * Configure stereo/3D display parameters.
 */
class CCAPPCOMMON_LIB_API ccStereoModeDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccStereoModeDlg(QWidget* parent);

	/**
	 * @brief Destructor
	 */
	~ccStereoModeDlg() override;

	/**
	 * @brief Get current stereo parameters
	 * @return Stereo parameters
	 */
	ccGLWindowInterface::StereoParams getParameters() const;

	/**
	 * @brief Set stereo parameters
	 * @param[in] params Parameters to set
	 */
	void setParameters(const ccGLWindowInterface::StereoParams& params);

	/**
	 * @brief Check if FOV should be updated
	 * @return true if FOV update needed
	 */
	bool updateFOV() const;

  protected:
	/// Called when glass type changes
	void glassTypeChanged(int);

  private:
	/// UI
	Ui::StereoModeDialog* m_ui;
};
