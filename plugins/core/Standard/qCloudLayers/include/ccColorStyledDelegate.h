#pragma once

//##########################################################################
//#                                                                        #
//#                   CLOUDCOMPARE PLUGIN: qCloudLayers                    #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                     COPYRIGHT: WigginsTech 2022                        #
//#                                                                        #
//##########################################################################

/**
 * @file ccColorStyledDelegate.h
 *
 * @brief Color styled delegate
 *
 * Custom item delegate for color display in table.
 */

#include <QStyledItemDelegate>

class QPainter;

/**
 * @class ccColorStyledDelegate
 *
 * @brief Color styled delegate
 *
 * Custom delegate for rendering color cells.
 */
class ccColorStyledDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
	/**
	 * @brief Create delegate
	 * @param[in] parent Parent object
	 */
	ccColorStyledDelegate(QObject* parent = nullptr);
	
	/// Paint color cell
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};
