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
 * @file ccSelectChildrenDlg.cpp
 *
 * @brief Select children dialog implementation
 *
 * Modal dialog for filtering and selecting child entities from a parent.
 * Supports filtering by name (plain or regex) and by entity type.
 *
 * ## Filter Modes
 *
 * - **By name**: matches entity display names, supports regex
 * - **By type**: point cloud, mesh, sensor, etc. (CC_TYPES enum)
 * - **Strict type**: match exactly or include subclasses
 *
 * ## Selection
 *
 * Returns selected children via getSelectedChildren().
 * State persists across invocations (static variables).
 *
 * @see ccSelectChildrenDlg.h
 */

#include "ccSelectChildrenDlg.h"

#include "ui_selectChildrenDlg.h"

static QString s_lastName;
static bool s_lastNameState = false;
static CC_CLASS_ENUM s_lastType = CC_TYPES::POINT_CLOUD;
static bool s_lastTypeState = true;
static bool s_lastTypeStrictState = true;
static bool s_lastUseRegex = true;

/**
 * @brief Construct the select children dialog
 *
 * @param[in] parent Parent widget
 */
ccSelectChildrenDlg::ccSelectChildrenDlg(QWidget* parent /*=nullptr*/)
    : QDialog(parent, Qt::Tool)
    , mUI(new Ui::SelectChildrenDialog)
{
	mUI->setupUi(this);

	mUI->typeCheckBox->setChecked(s_lastTypeState);
	mUI->typeStrictCheckBox->setChecked(s_lastTypeStrictState);
	mUI->nameCheckBox->setChecked(s_lastNameState);
	mUI->nameLineEdit->setText(s_lastName);
	mUI->checkBoxRegex->setChecked(s_lastUseRegex);

	connect(mUI->buttonBox, &QDialogButtonBox::accepted, this, &ccSelectChildrenDlg::onAccept);
}

ccSelectChildrenDlg::~ccSelectChildrenDlg()
{
	delete mUI;
	mUI = nullptr;
}

void ccSelectChildrenDlg::addType(QString typeName, CC_CLASS_ENUM type)
{
	mUI->typeComboBox->addItem(typeName, QVariant::fromValue<qint64>(type));

	// auto select last selected type
	if (type == s_lastType)
	{
		mUI->typeComboBox->setCurrentIndex(mUI->typeComboBox->count() - 1);
	}
}

void ccSelectChildrenDlg::onAccept()
{
	s_lastNameState = mUI->nameCheckBox->isChecked();
	s_lastName = mUI->nameLineEdit->text();
	s_lastTypeState = mUI->typeCheckBox->isChecked();
	s_lastTypeStrictState = mUI->typeCheckBox->isChecked();
	s_lastType = getSelectedType();
	s_lastUseRegex = getNameIsRegex();
}

CC_CLASS_ENUM ccSelectChildrenDlg::getSelectedType()
{
	if (!mUI->typeCheckBox->isChecked())
	{
		return CC_TYPES::HIERARCHY_OBJECT;
	}

	int currentIndex = mUI->typeComboBox->currentIndex();
	return static_cast<CC_CLASS_ENUM>(mUI->typeComboBox->itemData(currentIndex).value<qint64>());
}

QString ccSelectChildrenDlg::getSelectedName()
{
	if (!mUI->nameCheckBox->isChecked())
	{
		return QString();
	}

	return mUI->nameLineEdit->text();
}

bool ccSelectChildrenDlg::getStrictMatchState() const
{
	return mUI->typeStrictCheckBox->isChecked();
}

bool ccSelectChildrenDlg::getTypeIsUsed() const
{
	return mUI->typeCheckBox->isChecked();
}

bool ccSelectChildrenDlg::getNameIsRegex() const
{
	return mUI->checkBoxRegex->isChecked();
}

bool ccSelectChildrenDlg::getNameMatchIsUsed() const
{
	return mUI->nameCheckBox->isChecked();
}
