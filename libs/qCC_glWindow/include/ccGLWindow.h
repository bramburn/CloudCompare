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
// ##########################################################################

/**
 * @file ccGLWindow.h
 *
 * @brief OpenGL 3D view widget
 *
 * Concrete OpenGL rendering widget for CloudCompare. Extends QOpenGLWidget
 * and implements ccGLWindowInterface for 3D scene rendering, interaction,
 * and picking.
 *
 * Rendering pipeline:
 * - initializeGL() → OpenGL context setup, shaders, textures
 * - resizeGL() → viewport, projection matrix update
 * - paintGL() → fullRenderingPass() → drawBackground + draw3D + drawForeground
 *
 * Input events:
 * - Mouse: rotation, pan, zoom, picking
 * - Wheel: zoom
 * - Drag/Drop: load files by dropping on the window
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 * @see ccGLWindowInterface for the abstract interface
 * @see QOpenGLWidget for the Qt OpenGL widget base
 */

#include "qCC_glWindow.h"

// local
#include "ccGLWindowInterface.h"

// Qt
#include <QOpenGLWidget>

/**
 * @brief OpenGL 3D rendering widget
 *
 * Implements ccGLWindowInterface using QOpenGLWidget as the underlying
 * Qt OpenGL surface. All OpenGL rendering is delegated to the interface
 * methods (drawBackground, draw3D, drawForeground, etc.).
 *
 * Key Qt overrides:
 * - initializeGL: one-time OpenGL setup
 * - resizeGL: viewport + projection update
 * - paintGL: scene rendering
 * - mouse/wheel events: camera control and picking
 *
 * @see ccGLWindowInterface for the full rendering/interaction API
 */
class CCGLWINDOW_LIB_API ccGLWindow : public QOpenGLWidget
    , public ccGLWindowInterface
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct an OpenGL 3D window
	 *
	 * @param[in] format                 Surface format (nullptr = default format)
	 * @param[in] parent                Parent widget
	 * @param[in] silentInitialization  If true, suppress initialization log messages
	 */
	ccGLWindow(QSurfaceFormat* format = nullptr, QOpenGLWidget* parent = nullptr, bool silentInitialization = false);

	/**
	 * @brief Destructor
	 *
	 * Releases OpenGL resources (textures, FBOs, lists) and
	 * unregisters from ccGLWindowInterface's global registry.
	 */
	~ccGLWindow() override;

	// ccGLWindowInterface — Qt/OpenGL shortcuts

	inline qreal getDevicePixelRatio() const override { return devicePixelRatio(); }
	inline QFont getFont() const override { return font(); }
	inline QOpenGLContext* getOpenGLContext() const override { return context(); }
	inline void setWindowCursor(const QCursor& cursor) override { setCursor(cursor); }
	void doMakeCurrent() override;
	inline QObject* asQObject() override { return this; }
	inline const QObject* asQObject() const override { return this; }
	inline QString getWindowTitle() const override { return windowTitle(); }
	inline void doGrabMouse() override { grabMouse(); }
	inline void doReleaseMouse() override { releaseMouse(); }
	inline QPoint doMapFromGlobal(const QPoint& P) const override { return mapFromGlobal(P); }
	inline void doShowMaximized() override { showMaximized(); }
	inline void doResize(int w, int h) override { resize(w, h); }
	inline void doResize(const QSize& size) override { resize(size); }
	inline QImage doGrabFramebuffer() override { return grabFramebuffer(); }

	/**
	 * @brief Check if stereo mode is enabled
	 * @return false (stereo not supported in this class; see ccGLWindowStereo)
	 */
	inline bool isStereo() const override { return false; }

	inline QWidget* asWidget() override { return this; }
	inline QSize getScreenSize() const override { return size(); }

	// ccGLWindowInterface — size wrappers
	inline int qtWidth() const override { return QOpenGLWidget::width(); }
	inline int qtHeight() const override { return QOpenGLWidget::height(); }
	inline QSize qtSize() const override { return QOpenGLWidget::size(); }

	/**
	 * @brief Enable stereo mode
	 *
	 * Not supported in the base ccGLWindow class.
	 * Use ccGLWindowStereo for stereo rendering.
	 *
	 * @param[in] params Stereo parameters
	 * @return false (stereo not supported)
	 */
	bool enableStereoMode(const StereoParams& params) override;

	// ccGenericGLDisplay
	/**
	 * @brief Request a render update
	 *
	 * Schedules a repaint via QWidget::update().
	 */
	void requestUpdate() override;

	/**
	 * @brief Create a new GL window and its Qt widget
	 *
	 * @param[out] window               Created ccGLWindow (nullptr on failure)
	 * @param[out] widget               Qt widget wrapper for the window
	 * @param[in]  silentInitialization Suppress initialization messages
	 */
	static void Create(ccGLWindow*& window, QWidget*& widget, bool silentInitialization = false);

	/**
	 * @brief Cast a QWidget to a ccGLWindow if possible
	 *
	 * @param[in] widget Widget to cast
	 * @return ccGLWindow pointer if widget is a ccGLWindow, nullptr otherwise
	 */
	static ccGLWindow* FromWidget(QWidget* widget);

  protected: // OpenGL / Qt wrappers

	// ccGLWindowInterface — OpenGL function access
	/**
	 * @brief Get OpenGL 2.1 function table
	 *
	 * Uses QOpenGLVersionFunctionsFactory to obtain a
	 * QOpenGLFunctions_2_1 instance for the current context.
	 *
	 * @return OpenGL function table, or nullptr if no context
	 */
	inline ccQOpenGLFunctions* functions() const override
	{
		return context() ? QOpenGLVersionFunctionsFactory::get<ccQOpenGLFunctions>(context()) : nullptr;
	}

	inline QSurfaceFormat getSurfaceFormat() const override { return format(); }
	inline void doSetMouseTracking(bool) override { setMouseTracking(true); }
	inline void doShowFullScreen() override { showFullScreen(); }
	inline void doShowNormal() override { showNormal(); }

	// makeCurrent deleted to prevent accidental use — use doMakeCurrent() instead
	void makeCurrent() = delete;

	/**
	 * @brief Handle fast item picking signal
	 *
	 * Connected to ccGLWindowInterface::itemPickedFast signal.
	 * Forwards to onItemPickedFast().
	 */
	protected Q_SLOTS: void onItemPickedFastSlot(ccHObject* pickedEntity, int pickedItemIndex, int x, int y)
	{
		onItemPickedFast(pickedEntity, pickedItemIndex, x, y);
	}

	// ccGLWindowInterface — size overrides (HIDPI aware)
	int width() const override { return QOpenGLWidget::width(); }
	int height() const override { return QOpenGLWidget::height(); }
	QSize size() const override { return QOpenGLWidget::size(); }

	/**
	 * @brief Get the default Qt FBO ID
	 *
	 * Returns the framebuffer object used by QOpenGLWidget
	 * internally.
	 *
	 * @return OpenGL FBO handle
	 */
	GLuint defaultQtFBO() const override;

	// Qt event overrides → ccGLWindowInterface handlers
	void mousePressEvent(QMouseEvent* event) override { processMousePressEvent(event); }
	void mouseMoveEvent(QMouseEvent* event) override { processMouseMoveEvent(event); }
	void mouseDoubleClickEvent(QMouseEvent* event) override { processMouseDoubleClickEvent(event); }
	void mouseReleaseEvent(QMouseEvent* event) override { processMouseReleaseEvent(event); }
	void wheelEvent(QWheelEvent* event) override { processWheelEvent(event); }

	/**
	 * @brief Handle generic Qt events
	 *
	 * Handles QEvent::TouchBegin and other non-mouse events.
	 *
	 * @param[in] evt Qt event
	 * @return true if event was handled
	 */
	bool event(QEvent* evt) override;

	// QOpenGLWidget lifecycle → ccGLWindowInterface
	void initializeGL() override { initialize(); }
	void resizeGL(int w, int h) override { onResizeGL(w, h); }
	void paintGL() override { doPaintGL(); }

	// Drag-and-drop
	void dragEnterEvent(QDragEnterEvent* event) override { doDragEnterEvent(event); }
	void dropEvent(QDropEvent* event) override { doDropEvent(event); }
};
