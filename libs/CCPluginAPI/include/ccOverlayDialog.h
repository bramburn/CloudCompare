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

#include "CCPluginAPI.h"

/**
 * @file ccOverlayDialog.h
 *
 * @brief Overlay dialog interface
 *
 * Base class for floating dialogs that appear above
 * 3D views during interactive operations.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Qt
#include <QDialog>
#include <QList>

class ccGLWindowInterface;

/**
 * @brief Overlay dialog interface
 *
 * Floating dialogs that appear above 3D views.
 */
class CCPLUGIN_LIB_API ccOverlayDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create an overlay dialog
	 * @param[in] parent Parent widget
	 * @param[in] flags Window flags
	 */
	explicit ccOverlayDialog(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Tool);

	/**
	 * @brief Destructor
	 */
	~ccOverlayDialog() override;

	/**
	 * @brief Link with a 3D window
	 * @param[in] win Window to link with
	 * @return true on success
	 */
	virtual bool linkWith(ccGLWindowInterface* win);

	/**
	 * @brief Start the process/dialog
	 * @return true on success
	 */
	virtual bool start();

	/**
	 * @brief Stop the process/dialog
	 * @param[in] accepted Process result
	 */
	virtual void stop(bool accepted);

	// reimplemented from QDialog
	void reject() override;

	/**
	 * @brief Add overridden keyboard shortcut
	 * @param[in] key Key to override
	 */
	void addOverriddenShortcut(Qt::Key key);

	/**
	 * @brief Check if started
	 * @return true if process is active
	 */
	bool started() const
	{
		return m_processing;
	}

  Q_SIGNALS:

	/**
	 * @brief Process finished signal
	 * @param[in] accepted Result state
	 */
	void processFinished(bool accepted);

	/**
	 * @brief Shortcut triggered signal
	 * @param[in] key Key that was triggered
	 */
	void shortcutTriggered(int key);

	//! Signal emitted when a 'show' event is detected
	void shown();

  protected:
	//! Slot called when the linked window is deleted (calls 'onClose')
	virtual void onLinkedWindowDeletion(ccGLWindowInterface* object = nullptr);

  protected:
	// inherited from QObject
	bool eventFilter(QObject* obj, QEvent* e) override;

	//! Associated (MDI) window
	ccGLWindowInterface* m_associatedWin;

	//! Running/processing state
	bool m_processing;

	//! Overridden keys
	QList<int> m_overriddenKeys;
};
