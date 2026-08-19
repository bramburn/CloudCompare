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

#include "qCC_glWindow.h"

// local
#include "ccGLWindowInterface.h"

/**
 * @file ccGLWindowStereo.h
 *
 * @brief Stereo GL window
 *
 * OpenGL window with stereo 3D display support.
 * Migrated to QOpenGLWidget in Qt 6 (previously QWindow with manual context).
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QHBoxLayout>
#include <QOpenGLWidget>
#include <QWidget>

/**
 * @brief Stereo GL window (Qt 6: QOpenGLWidget-based)
 *
 * OpenGL window with stereo 3D support.
 * Inherits from QOpenGLWidget for automatic context management (Qt 6 pattern).
 * The actual stereo rendering uses custom stereo FBOs via ccGLWindowInterface.
 */
class CCGLWINDOW_LIB_API ccGLWindowStereo : public QOpenGLWidget
    , public ccGLWindowInterface
{
	Q_OBJECT

  public:
	/**
	 * @brief Create stereo window
	 * @param[in] format Surface format (stereo must be enabled)
	 * @param[in] parent Parent widget
	 * @param[in] silentInitialization Skip init messages
	 */
	ccGLWindowStereo(QSurfaceFormat* format = nullptr, QWidget* parent = nullptr, bool silentInitialization = false);

	//! Destructor
	~ccGLWindowStereo() override;

	// inherited from ccGLWindowInterface
	inline qreal getDevicePixelRatio() const override
	{
		return devicePixelRatio();
	}
	inline QFont getFont() const override
	{
		return font();
	}
	inline QOpenGLContext* getOpenGLContext() const override
	{
		return context();
	}
	inline void setWindowCursor(const QCursor& cursor) override
	{
		setCursor(cursor);
	}
	void            doMakeCurrent() override;
	inline QObject* asQObject() override
	{
		return this;
	}
	inline const QObject* asQObject() const override
	{
		return this;
	}
	inline QString getWindowTitle() const override
	{
		return windowTitle();
	}
	inline void doGrabMouse() override
	{
		grabMouse();
	}
	inline void doReleaseMouse() override
	{
		releaseMouse();
	}
	inline QPoint doMapFromGlobal(const QPoint& P) const override
	{
		return mapFromGlobal(P);
	}
	inline void doShowMaximized() override
	{
		showMaximized();
	}
	inline void doResize(int w, int h) override
	{
		resize(w, h);
	}
	inline void doResize(const QSize& size) override
	{
		resize(size);
	}
	inline QImage doGrabFramebuffer() override
	{
		return grabFramebuffer();
	}
	inline bool isStereo() const override
	{
		return true;
	}

	//! Returns the parent widget
	QWidget* parentWidget() const
	{
		return m_parentWidget;
	}

	//! Sets 'parent' widget
	void setParentWidget(QWidget* widget);

	//! Returns the font
	inline const QFont& font() const
	{
		return m_font;
	}

	// shortcuts
	inline void setWindowTitle(QString title)
	{
		QWidget::setWindowTitle(title);
	}
	inline QString windowTitle() const
	{
		return QWidget::windowTitle();
	}
	inline QWidget* asWidget() override
	{
		return this;
	}
	inline QSize getScreenSize() const override
	{
		return size();
	}

	// inherited from ccGLWindowInterface
	inline int qtWidth() const override
	{
		return QWidget::width();
	}
	inline int qtHeight() const override
	{
		return QWidget::height();
	}
	inline QSize qtSize() const override
	{
		return QWidget::size();
	}
	bool enableStereoMode(const StereoParams& params) override;
	void disableStereoMode() override;

	// inherited from ccGenericGLDisplay
	void requestUpdate() override;

	//! For compatibility with the QOpenGLWidget version
	inline void update()
	{
		doPaintGL();
	}

	void grabMouse();
	void releaseMouse();

	//! Creates an instance
	static void Create(ccGLWindowStereo*& window, QWidget*& widget, bool silentInitialization = false);

	//! Casts a widget to a ccGLWindowStereo instance (if possible)
	static ccGLWindowStereo* FromWidget(QWidget* widget);

  protected: // rendering
	// inherited from ccGLWindowInterface
	inline ccQOpenGLFunctions* functions() const override
	{
		return context() ? QOpenGLVersionFunctionsFactory::get<ccQOpenGLFunctions>(context()) : nullptr;
	}
	inline QSurfaceFormat getSurfaceFormat() const override
	{
		return format();
	}
	void doSetMouseTracking(bool enable) override;
	void doShowFullScreen() override
	{
		showFullScreen();
	}
	void doShowNormal() override
	{
		showNormal();
	}
	bool initPaintGL() override;
	void swapGLBuffers() override;

	// inherited from QOpenGLWidget — not used (use doMakeCurrent() instead)
	// makeCurrent() is deleted in QOpenGLWidget's public API when context is managed
	// We expose it via doMakeCurrent() in ccGLWindowInterface

	//! use doMakeCurrent() from ccGLWindowInterface instead

  protected: // other methods
	//! Reacts to the itemPickedFast signal (shortcut)
	protected Q_SLOTS: void onItemPickedFastSlot(ccHObject* pickedEntity, int pickedItemIndex, int x, int y)
	{
		onItemPickedFast(pickedEntity, pickedItemIndex, x, y);
	}

	// inherited from ccGLWindowInterface
	int width() const override
	{
		return QWidget::width();
	}
	int height() const override
	{
		return QWidget::height();
	}
	QSize size() const override
	{
		return QWidget::size();
	}
	GLuint defaultQtFBO() const override;

	// events handling
	void mousePressEvent(QMouseEvent* event) override
	{
		processMousePressEvent(event);
	}
	void mouseMoveEvent(QMouseEvent* event) override
	{
		processMouseMoveEvent(event);
	}
	void mouseDoubleClickEvent(QMouseEvent* event) override
	{
		processMouseDoubleClickEvent(event);
	}
	void mouseReleaseEvent(QMouseEvent* event) override
	{
		processMouseReleaseEvent(event);
	}
	void wheelEvent(QWheelEvent* event) override
	{
		processWheelEvent(event);
	}
	bool event(QEvent* evt) override;

	bool preInitialize(bool& firstTime) override;
	bool postInitialize(bool firstTime) override;

	void         resizeGL(int w, int h);
	virtual void dragEnterEvent(QDragEnterEvent* event)
	{
		doDragEnterEvent(event);
	}
	virtual void dropEvent(QDropEvent* event)
	{
		doDropEvent(event);
	}

  protected: // members
	//! Format (set via setFormat() before context creation)
	QSurfaceFormat m_format;

	//! Associated widget
	QWidget* m_parentWidget;
};

//! Container widget for ccGLWindowStereo (thin wrapper; ccGLWindowStereo is itself a QWidget in Qt 6)
class CCGLWINDOW_LIB_API ccGLStereoWidget : public QWidget
{
	Q_OBJECT

  public:
	ccGLStereoWidget(ccGLWindowStereo* window, QWidget* parent = nullptr)
	    : QWidget(parent)
	    , m_associatedWindow(nullptr)
	{
		setLayout(new QHBoxLayout);
		layout()->setContentsMargins(0, 0, 0, 0);

		if (window)
		{
			setAssociatedWindow(window);
		}
	}

	virtual ~ccGLStereoWidget()
	{
		// ccGLWindowStereo is owned by the caller; just clear the association
		m_associatedWindow = nullptr;
	}

	inline ccGLWindowStereo* associatedWindow() const
	{
		return m_associatedWindow;
	}

	void setAssociatedWindow(ccGLWindowStereo* window)
	{
		if (window)
		{
			assert(layout() && layout()->count() == 0);
			// ccGLWindowStereo is already a QWidget — embed it directly
			layout()->addWidget(window);
			m_associatedWindow = window;
			m_associatedWindow->setParentWidget(this);
		}
		else
		{
			assert(false);
		}
	}

  protected:
	ccGLWindowStereo* m_associatedWindow;
};
