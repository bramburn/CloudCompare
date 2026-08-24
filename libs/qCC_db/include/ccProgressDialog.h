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

#ifndef CC_PROGRESS_DIALOG_HEADER
#define CC_PROGRESS_DIALOG_HEADER

// Local
#include "qCC_db.h"

// Qt
#include <QAtomicInt>
#include <QProgressDialog>
#include <QTimer>

/**
 * @file ccProgressDialog.h
 *
 * @brief Progress dialog for long-running operations
 *
 * Thread-safe progress dialog that implements GenericProgressCallback
 * for use with CCCoreLib algorithms. Displays progress bars and
 * optional cancel buttons for interrupting operations.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// CCCoreLib
#include <GenericProgressCallback.h>

/**
 * @brief Graphical progress indicator (thread-safe)
 *
 * Implements GenericProgressCallback for integration with
 * CCCoreLib algorithms. Provides a progress bar with optional
 * cancel button.
 */
class QCC_DB_LIB_API ccProgressDialog : public QProgressDialog
    , public CCCoreLib::GenericProgressCallback
{
  public:
	/**
	 * @brief Create a progress dialog
	 * @param[in] cancelButton Enable/disable cancel button
	 * @param[in] parent Parent widget
	 *
	 * @note Cancel button can be enabled but operation may not support cancellation
	 */
	ccProgressDialog(bool cancelButton = false,
	                 QWidget* parent = nullptr);

	//! Destructor (virtual)
	virtual ~ccProgressDialog()
	{
	}

	// inherited method
	virtual void update(float percent) override;
	inline virtual void setMethodTitle(const char* methodTitle) override
	{
		setMethodTitle(QString(methodTitle));
	}
	inline virtual void setInfo(const char* infoStr) override
	{
		setInfo(QString(infoStr));
	}
	inline virtual bool isCancelRequested() override
	{
		return wasCanceled();
	}
	virtual void start() override;
	virtual void stop() override;

	//! setMethodTitle with a QString as argument
	virtual void setMethodTitle(QString methodTitle);
	//! setInfo with a QString as argument
	virtual void setInfo(QString infoStr);

  protected:
	//! Refreshes the progress
	/** Should only be called in the main Qt thread!
	    This method is automatically called by 'update' when necessary.
	**/
	void refresh();

  protected:
	//! Current progress value (percent)
	QAtomicInt m_currentValue;

	//! Last displayed progress value (percent)
	QAtomicInt m_lastRefreshValue;
};

#endif // CC_PROGRESS_DIALOG_HEADER
