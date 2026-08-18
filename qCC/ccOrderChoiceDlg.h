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

#ifndef CC_ORDER_CHOICE_DIALOG_HEADER
#define CC_ORDER_CHOICE_DIALOG_HEADER

/**
 * @file ccOrderChoiceDlg.h
 *
 * @brief Order choice dialog
 *
 * Dialog for assigning roles to two entities.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QDialog>

class ccHObject;
class ccMainAppInterface;
class Ui_RoleChoiceDialog;

/**
 * @brief Order choice dialog
 *
 * Assign roles to two entities (e.g. compared/reference).
 */
class ccOrderChoiceDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] firstEntity First entity
	 * @param[in] firstRole Role of first entity
	 * @param[in] secondEntity Second entity
	 * @param[in] secondRole Role of second entity
	 * @param[in] app Application interface
	 */
	ccOrderChoiceDlg(ccHObject*          firstEntity,
	                 QString             firstRole,
	                 ccHObject*          secondEntity,
	                 QString             secondRole,
	                 ccMainAppInterface* app = 0);

	/// Destructor
	virtual ~ccOrderChoiceDlg();

	/// Get first entity
	ccHObject* getFirstEntity();
	/// Get second entity
	ccHObject* getSecondEntity();

  protected:
	/// Swap entities
	void swap();

  protected:
	/// Set colors and labels
	void setColorsAndLabels();

	Ui_RoleChoiceDialog* m_gui;
	ccMainAppInterface*  m_app;
	ccHObject*           m_firstEnt;
	ccHObject*           m_secondEnt;
	bool                 m_useInputOrder;
};

#endif // CC_ORDER_CHOICE_DIALOG_HEADER
