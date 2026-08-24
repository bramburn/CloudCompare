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

// qCC
/**
 * @file ccGLWindowStereo.cpp
 *
 * @brief Stereo GL window implementation
 *
 * Implements stereo (3D) rendering mode for ccGLWindow.
 *
 * ## Stereo Modes
 *
 * - **Anaglyph**: red/cyan for cheap 3D glasses
 * - **Page flip**: NVIDIA 3D Vision dual-buffer stereo
 * - **HDMI 3D**: frame-packing via HDMI
 *
 * ## Stereo Rendering
 *
 * Renders the scene twice with eye-separated cameras,
 * compositing the two views according to the active mode.
 *
 * @see ccGLWindowStereo.h, ccGLWindow
 */

#include "ccGLWindowStereo.h"

// qCC_db
#include <ccHObject.h>

// CCFbo
#include <ccFrameBufferObject.h>

// Qt
#include <QMessageBox>
#include <QResizeEvent>

ccGLWindowStereo::ccGLWindowStereo(QSurfaceFormat* format /*=nullptr*/,
                                   QWidget* parent /*=nullptr*/,
                                   bool silentInitialization /*=false*/)
    : QOpenGLWidget(parent)
    , ccGLWindowInterface(this, silentInitialization)
    , m_parentWidget(nullptr)
{
	// Store format for preInitialize (must be set before context creation)
	m_format = format ? *format : QSurfaceFormat::defaultFormat();

	if (format)
	{
		setFormat(*format);
	}

	// default picking mode
	setPickingMode(DEFAULT_PICKING);

	// default interaction mode
	setInteractionMode(MODE_TRANSFORM_CAMERA);

	// signal/slot connections
	connect(m_signalEmitter, &ccGLWindowSignalEmitter::itemPickedFast, this, &ccGLWindowStereo::onItemPickedFastSlot, Qt::DirectConnection);
	connect(&m_scheduleTimer, &QTimer::timeout, [&]()
	        { checkScheduledRedraw(); });
	connect(&m_autoRefreshTimer, &QTimer::timeout, this, [&]()
	        { update(); });
	connect(&m_deferredPickingTimer, &QTimer::timeout, this, [&]()
	        { doPicking(); });

	QString windowTitle = QString("3D View Stereo %1").arg(m_uniqueID);
	setWindowTitle(windowTitle);
	setObjectName(windowTitle);
}

ccGLWindowStereo::~ccGLWindowStereo()
{
	// disable the stereo mode (mainly to release the FBO of stereo glasses ;)
	disableStereoMode();

	uninitializeGL();
	// QOpenGLWidget handles context cleanup automatically
}

void ccGLWindowStereo::grabMouse()
{
	QWidget::grabMouse();
}

void ccGLWindowStereo::releaseMouse()
{
	QWidget::releaseMouse();
}

void ccGLWindowStereo::setParentWidget(QWidget* widget)
{
	m_parentWidget = widget;

	if (widget)
	{
		// drag & drop handling
		widget->setAcceptDrops(true);
		widget->setAttribute(Qt::WA_AcceptTouchEvents, true);
		widget->setAttribute(Qt::WA_OpaquePaintEvent, true);

		m_parentWidget->setObjectName(windowTitle());
	}
}

void ccGLWindowStereo::doMakeCurrent()
{
	// QOpenGLWidget manages the context automatically
	makeCurrent();

	if (m_activeFbo)
	{
		m_activeFbo->start();
	}
}

bool ccGLWindowStereo::preInitialize(bool& firstTime)
{
	firstTime = false;

	// QOpenGLWidget::makeCurrent() creates the context lazily on first call
	makeCurrent();

	if (!context() || !context()->isValid())
	{
		return false;
	}

	firstTime = true;
	return true;
}

bool ccGLWindowStereo::postInitialize(bool firstTime)
{
	if (firstTime)
	{
		resizeGL(width(), height());
	}

	return true;
}

bool ccGLWindowStereo::event(QEvent* evt)
{
	// process generic events
	if (processEvents(evt))
	{
		return true;
	}

	switch (evt->type())
	{
	case QEvent::Resize:
	{
		QSize newSize = static_cast<QResizeEvent*>(evt)->size();
		resizeGL(newSize.width(), newSize.height());
		evt->accept();
	}
		return true;

	case QEvent::UpdateRequest:
	case QEvent::Show:
	case QEvent::Paint:
	{
		requestUpdate();
		evt->accept();
	}
		return true;

	default:
		// nothing to do
		break;
	}

	return QOpenGLWidget::event(evt);
}

void ccGLWindowStereo::resizeGL(int w, int h)
{
	onResizeGL(w, h);

	requestUpdate();
}

GLuint ccGLWindowStereo::defaultQtFBO() const
{
	return 0;
}

void ccGLWindowStereo::requestUpdate()
{
	if (!m_autoRefresh)
	{
		update();
	}
}

bool ccGLWindowStereo::initPaintGL()
{
	// Ensure GL is initialized (QOpenGLWidget handles context automatically)
	if (!m_initialized && !initialize())
	{
		return false;
	}

	doMakeCurrent();

	ccQOpenGLFunctions* glFunc = functions();
	assert(glFunc);

	glFunc->glViewport(m_glViewport.x(), m_glViewport.y(), m_glViewport.width(), m_glViewport.height());

	return true;
}

void ccGLWindowStereo::swapGLBuffers()
{
	// QOpenGLWidget swaps buffers automatically after each frame paintGL() callback.
	// For non-stereo mode there is nothing extra to do.
	// For stereo mode, ccGLWindowInterface::enableStereoMode manages FBO swapping manually.
}

bool ccGLWindowStereo::enableStereoMode(const StereoParams& params)
{
	return ccGLWindowInterface::enableStereoMode(params);
}

void ccGLWindowStereo::disableStereoMode()
{
	ccGLWindowInterface::disableStereoMode();
}

void ccGLWindowStereo::Create(ccGLWindowStereo*& window, QWidget*& widget, bool silentInitialization /*=false*/)
{
	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setStereo(true);

	window = new ccGLWindowStereo(&format, nullptr, silentInitialization);
	// ccGLWindowStereo is itself a QWidget (QOpenGLWidget subclass)
	widget = window;
}

ccGLWindowStereo* ccGLWindowStereo::FromWidget(QWidget* widget)
{
	// Direct cast for the case where ccGLWindowStereo is used directly as a widget
	ccGLWindowStereo* stereoWindow = qobject_cast<ccGLWindowStereo*>(widget);
	if (stereoWindow)
	{
		return stereoWindow;
	}

	// Fallback: check if it's a ccGLStereoWidget container
	ccGLStereoWidget* myWidget = qobject_cast<ccGLStereoWidget*>(widget);
	if (myWidget)
	{
		return myWidget->associatedWindow();
	}

	assert(false);
	return nullptr;
}

void ccGLWindowStereo::doSetMouseTracking(bool enable)
{
	if (m_parentWidget)
	{
		m_parentWidget->setMouseTracking(enable);
	}
}
