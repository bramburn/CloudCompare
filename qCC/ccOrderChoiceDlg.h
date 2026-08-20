// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is you can redistribute it and/or modify  #
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
 * @brief Dialog for assigning roles to two entities.
 *
 * @details A dialog that allows the user to assign roles to two
 * entities. This is useful when an operation requires distinguishing
 * between two similar entities (e.g., compared vs. reference,
 * source vs. target).
 *
 * The dialog shows both entities and allows the user to:
 * - See the default role assignment
 * - Swap the roles between entities
 * - Confirm or cancel the assignment
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccComparisonDlg
 */

// Qt
#include <QDialog>

class ccHObject;
class ccMainAppInterface;
class Ui_RoleChoiceDialog;

/**
 * @brief Dialog for assigning roles to two entities.
 *
 * @details Provides a UI for assigning roles to two entities
 * when an operation requires distinguishing between them.
 *
 * Example use cases:
 * - Cloud comparison (compared entity vs. reference entity)
 * - Registration (moving entity vs. fixed entity)
 * - Merge operations (source vs. destination)
 *
 * The dialog shows the entity names and their roles,
 * with a button to swap the assignment if needed.
 *
 * @extends QDialog
 */
class ccOrderChoiceDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the order choice dialog.
	 *
	 * @param[in] firstEntity First entity.
	 * @param[in] firstRole Role description for first entity.
	 * @param[in] secondEntity Second entity.
	 * @param[in] secondRole Role description for second entity.
	 * @param[in] app Application interface.
	 */
	ccOrderChoiceDlg(ccHObject*          firstEntity,
	                 QString             firstRole,
	                 ccHObject*          secondEntity,
	                 QString             secondRole,
	                 ccMainAppInterface* app = nullptr);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccOrderChoiceDlg();

	/**
	 * @brief Get the entity assigned to the first role.
	 * @return First entity.
	 */
	ccHObject* getFirstEntity();

	/**
	 * @brief Get the entity assigned to the second role.
	 * @return Second entity.
	 */
	ccHObject* getSecondEntity();

  protected slots:
	/**
	 * @brief Swap the entity assignments.
	 */
	void swap();

  protected:
	/**
	 * @brief Set colors and labels for entities.
	 */
	void setColorsAndLabels();

	//! UI definition
	Ui_RoleChoiceDialog* m_gui;

	//! Application interface
	ccMainAppInterface* m_app;

	//! First entity
	ccHObject* m_firstEnt;

	//! Second entity
	ccHObject* m_secondEnt;

	//! Whether to use input order
	bool m_useInputOrder;
};

#endif // CC_ORDER_CHOICE_DIALOG_HEADER
