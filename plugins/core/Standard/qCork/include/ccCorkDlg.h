//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qCork                       #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                  COPYRIGHT: Daniel Girardeau-Montaut                   #
//#                                                                        #
//##########################################################################

#ifndef CC_CORK_DLG_HEADER
#define CC_CORK_DLG_HEADER

/**
 * @file ccCorkDlg.h
 *
 * @brief Cork dialog
 *
 * Dialog for mesh boolean operations.
 */

#include "ui_corkDlg.h"

/**
 * @class ccCorkDlg
 *
 * @brief Cork dialog
 *
 * Dialog for configuring mesh boolean operations.
 */
class ccCorkDlg : public QDialog, public Ui::CorkDialog
{
	Q_OBJECT

public:

	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccCorkDlg(QWidget* parent/*=nullptr*/);

	/**
	 * @enum CSG_OPERATION
	 *
	 * @brief CSG operation types
	 */
	enum CSG_OPERATION { UNION, INTERSECT, DIFF, SYM_DIFF };

	/**
	 * @brief Set mesh names
	 * @param[in] A First mesh name
	 * @param[in] B Second mesh name
	 */
	void setNames(QString A, QString B);

	/**
	 * @brief Get selected operation
	 * @return Selected CSG operation
	 */
	CSG_OPERATION getSelectedOperation() const { return m_selectedOperation; }

	/**
	 * @brief Check if meshes are swapped
	 * @return True if swapped
	 */
	bool isSwapped() const { return m_isSwapped; }

protected Q_SLOTS:

	/// Union selected
	void unionSelected();
	/// Intersect selected
	void intersectSelected();
	/// Difference selected
	void diffSelected();
	/// Symmetric difference selected
	void symDiffSelected();
	/// Swap mesh order
	void swap();

protected:

	CSG_OPERATION m_selectedOperation;
	bool m_isSwapped;
};

#endif //CC_CORK_DLG_HEADER
