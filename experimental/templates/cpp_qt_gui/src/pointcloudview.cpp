// PointCloudView — 3D viewport for point cloud rendering.

/**
 * @file pointcloudview.cpp
 *
 * @brief Point cloud viewport widget implementation
 *
 * Qt widget wrapping a ccGLWindow for point cloud display:
 * - Inherits from ccGLWindowInterface
 * - Provides OpenGL context and event handling
 * - Manages entity rendering
 * - Handles mouse/keyboard interaction
 *
 * @see pointcloudview.h
 */
#include "pointcloudview.h"

#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QtMath>
#include <algorithm>
#include <cmath>

// ----------------------------------------------------------------------------
// GLSL 330 Core shaders.
// ----------------------------------------------------------------------------

static const char* kPointVertexShader = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in float aColor;  // 0..1 mapped from z

uniform mat4 uMVP;
uniform float uPointSize;

out float vColor;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    gl_PointSize = uPointSize;
    vColor = aColor;
}
)GLSL";

static const char* kPointFragmentShader = R"GLSL(
#version 330 core
in float vColor;
out vec4 FragColor;

// Simple viridis-like gradient: blue (low) -> green (mid) -> yellow (high)
vec3 viridis(float t) {
    t = clamp(t, 0.0, 1.0);
    if (t < 0.5) {
        return mix(vec3(0.27, 0.0, 0.33), vec3(0.13, 0.57, 0.55), t * 2.0);
    }
    return mix(vec3(0.13, 0.57, 0.55), vec3(0.99, 0.79, 0.15), (t - 0.5) * 2.0);
}

void main() {
    FragColor = vec4(viridis(vColor), 1.0);
}
)GLSL";

static const char* kAxesVertexShader = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
uniform mat4 uMVP;
out vec3 vColor;
void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vColor = aColor;
}
)GLSL";

static const char* kAxesFragmentShader = R"GLSL(
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() { FragColor = vec4(vColor, 1.0); }
)GLSL";

// ----------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------

static QVector3D vecMin(const QVector3D& a, const QVector3D& b) {
    return QVector3D(std::min(a.x(), b.x()),
                     std::min(a.y(), b.y()),
                     std::min(a.z(), b.z()));
}
static QVector3D vecMax(const QVector3D& a, const QVector3D& b) {
    return QVector3D(std::max(a.x(), b.x()),
                     std::max(a.y(), b.y()),
                     std::max(a.z(), b.z()));
}

// ----------------------------------------------------------------------------
// PointCloudView implementation
// ----------------------------------------------------------------------------

PointCloudView::PointCloudView(QWidget* parent)
    : QOpenGLWidget(parent) {
    // Critical: enable mouse tracking + focus so the widget actually
    // receives mouse press/move/wheel events from the OS.
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    // Give the widget an initial focus so it captures keyboard too.
    setAttribute(Qt::WA_AcceptTouchEvents, false);

    // Larger default cursor — a crosshair is conventional for 3D viewports
    setCursor(Qt::CrossCursor);
}

PointCloudView::~PointCloudView() {
    if (context()) {
        makeCurrent();
        if (vbo_) glDeleteBuffers(1, &vbo_);
        if (axes_vbo_) glDeleteBuffers(1, &axes_vbo_);
        if (shader_program_) glDeleteProgram(shader_program_);
        if (axes_shader_) glDeleteProgram(axes_shader_);
        doneCurrent();
    }
}

void PointCloudView::setPoints(const std::vector<float>& interleaved_xyz,
                               size_t point_count) {
    points_ = interleaved_xyz;
    point_count_ = point_count;
    bounds_set_ = false;
    if (isValid() && context() != nullptr) {
        makeCurrent();
        uploadPointsToGPU();
        doneCurrent();
        update();
    }
}

void PointCloudView::setBounds(const QVector3D& min, const QVector3D& max) {
    bounds_min_ = min;
    bounds_max_ = max;
    bounds_set_ = true;
    if (isValid() && context() != nullptr) {
        update();
    }
}

void PointCloudView::resetView() {
    yaw_ = 30.0f;
    pitch_ = 20.0f;
    distance_ = 5.0f;
    pan_offset_ = QVector3D(0, 0, 0);
    update();
}

void PointCloudView::computeBoundsFromPoints() {
    if (points_.empty()) {
        bounds_min_ = QVector3D(-1, -1, -1);
        bounds_max_ = QVector3D(1, 1, 1);
        return;
    }
    bounds_min_ = QVector3D(std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max());
    bounds_max_ = QVector3D(-std::numeric_limits<float>::max(),
                            -std::numeric_limits<float>::max(),
                            -std::numeric_limits<float>::max());
    for (size_t i = 0; i < point_count_; ++i) {
        QVector3D p(points_[i * 3], points_[i * 3 + 1], points_[i * 3 + 2]);
        bounds_min_ = vecMin(bounds_min_, p);
        bounds_max_ = vecMax(bounds_max_, p);
    }
    bounds_set_ = true;
}

void PointCloudView::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    // Required in OpenGL Core profile for `gl_PointSize` in the vertex
    // shader to actually take effect. Without this, points render at the
    // hardware's minimum (typically 1.0 pixel) regardless of what the
    // shader says.
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Compile point shader
    {
        auto prog = std::make_unique<QOpenGLShaderProgram>(this);
        prog->addShaderFromSourceCode(QOpenGLShader::Vertex, kPointVertexShader);
        prog->addShaderFromSourceCode(QOpenGLShader::Fragment, kPointFragmentShader);
        prog->link();
        if (!prog->isLinked()) {
            qWarning() << "Point shader link failed:" << prog->log();
        }
        shader_program_ = prog->programId();
        // Keep `prog` alive by attaching it as a child of the widget.
        prog->setParent(this);
    }

    // Compile axes shader
    {
        auto prog = std::make_unique<QOpenGLShaderProgram>(this);
        prog->addShaderFromSourceCode(QOpenGLShader::Vertex, kAxesVertexShader);
        prog->addShaderFromSourceCode(QOpenGLShader::Fragment, kAxesFragmentShader);
        prog->link();
        if (!prog->isLinked()) {
            qWarning() << "Axes shader link failed:" << prog->log();
        }
        axes_shader_ = prog->programId();
        prog->setParent(this);
    }

    // Axes: 6 endpoints, each with (pos.xyz, color.rgb)
    const float axes[] = {
        // X axis (red)
        0.0f, 0.0f, 0.0f,  1.0f, 0.3f, 0.3f,
        1.0f, 0.0f, 0.0f,  1.0f, 0.3f, 0.3f,
        // Y axis (green)
        0.0f, 0.0f, 0.0f,  0.3f, 1.0f, 0.3f,
        0.0f, 1.0f, 0.0f,  0.3f, 1.0f, 0.3f,
        // Z axis (blue)
        0.0f, 0.0f, 0.0f,  0.3f, 0.3f, 1.0f,
        0.0f, 0.0f, 1.0f,  0.3f, 0.3f, 1.0f,
    };
    glGenBuffers(1, &axes_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, axes_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    uploadPointsToGPU();
}

void PointCloudView::uploadPointsToGPU() {
    if (!bounds_set_) computeBoundsFromPoints();
    if (points_.empty()) return;

    // Build interleaved buffer: [x, y, z, color01] per point
    std::vector<float> buffer;
    buffer.reserve(point_count_ * 4);
    const float z_range = std::max(1e-6f, bounds_max_.z() - bounds_min_.z());
    for (size_t i = 0; i < point_count_; ++i) {
        const float x = points_[i * 3];
        const float y = points_[i * 3 + 1];
        const float z = points_[i * 3 + 2];
        const float t = (z - bounds_min_.z()) / z_range;
        buffer.push_back(x);
        buffer.push_back(y);
        buffer.push_back(z);
        buffer.push_back(t);
    }

    if (vbo_ == 0) glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(buffer.size() * sizeof(float)),
                 buffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vbo_size_ = point_count_;
}

void PointCloudView::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

QMatrix4x4 PointCloudView::buildModelViewProjection() const {
    const float aspect = static_cast<float>(width()) / std::max(1, height());

    QMatrix4x4 projection;
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);

    QMatrix4x4 view;
    view.translate(0, 0, -distance_);
    view.rotate(pitch_, 1.0f, 0.0f, 0.0f);
    view.rotate(yaw_, 0.0f, 1.0f, 0.0f);
    view.translate(pan_offset_);

    // Center the cloud: translate so the center of bounds is at origin
    QVector3D center = (bounds_min_ + bounds_max_) * 0.5f;
    float scale = 1.0f / std::max({bounds_max_.x() - bounds_min_.x(),
                                   bounds_max_.y() - bounds_min_.y(),
                                   bounds_max_.z() - bounds_min_.z(),
                                   1e-6f});
    QMatrix4x4 model;
    model.translate(-center);
    model.scale(scale);

    return projection * view * model;
}

void PointCloudView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 mvp = buildModelViewProjection();
    drawAxes(mvp);
    drawPoints(mvp);
}

void PointCloudView::drawAxes(const QMatrix4x4& mvp) {
    glUseProgram(axes_shader_);
    GLuint mvp_loc = glGetUniformLocation(axes_shader_, "uMVP");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp.constData());

    glBindBuffer(GL_ARRAY_BUFFER, axes_vbo_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));

    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 6);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void PointCloudView::drawPoints(const QMatrix4x4& mvp) {
    if (point_count_ == 0 || vbo_ == 0) return;

    glUseProgram(shader_program_);
    GLuint mvp_loc = glGetUniformLocation(shader_program_, "uMVP");
    GLuint size_loc = glGetUniformLocation(shader_program_, "uPointSize");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp.constData());
    // Pick a sensible point size in pixels. We scale with 1/distance so
    // the points don't bloat when zoomed in or shrink to nothing when
    // zoomed out. Clamp to keep within a reasonable range.
    const float point_size = std::clamp(80.0f / std::max(0.5f, distance_), 2.0f, 12.0f);
    glUniform1f(size_loc, point_size);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(point_count_));
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void PointCloudView::mousePressEvent(QMouseEvent* e) {
    // Ensure we hold focus so subsequent wheel / keyboard events reach us
    // even if the user clicked on the widget without first tabbing into it.
    setFocus(Qt::MouseFocusReason);
    last_mouse_pos_ = e->pos();
    // Force a redraw so the user sees the press is being received.
    // Without this, a press-then-release with no movement gives no visual
    // feedback and the user thinks the input is broken.
    update();
}

void PointCloudView::mouseMoveEvent(QMouseEvent* e) {
    // QMouseEvent::pos() can be -1,-1 for synthesised events. Guard.
    if (last_mouse_pos_.x() < 0 || last_mouse_pos_.y() < 0) {
        last_mouse_pos_ = e->pos();
        return;
    }
    const QPoint delta = e->pos() - last_mouse_pos_;
    if (delta.isNull()) return;
    last_mouse_pos_ = e->pos();

    // Rotation speed in degrees per pixel. 0.5° per pixel feels natural
    // for a desktop mouse on a ~900px viewport.
    constexpr float kRotSpeed = 0.5f;
    // Pan speed as a fraction of the current camera distance per pixel.
    constexpr float kPanSpeed = 0.002f;

    if (e->buttons() & Qt::LeftButton) {
        yaw_ += delta.x() * kRotSpeed;
        pitch_ += delta.y() * kRotSpeed;
        pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    } else if (e->buttons() & Qt::RightButton) {
        // Pan in the camera's local right/up plane. Derive the right and
        // up vectors from the view matrix's rotation block — this is
        // far simpler and more reliable than inverting the full MVP.
        const float yaw_rad = qDegreesToRadians(yaw_);
        const float pitch_rad = qDegreesToRadians(pitch_);
        // Forward vector in world space (looking from camera into scene).
        const QVector3D forward(std::cos(pitch_rad) * std::sin(yaw_rad),
                                -std::sin(pitch_rad),
                                -std::cos(pitch_rad) * std::cos(yaw_rad));
        // World-up is (0, 1, 0); right = forward × up (right-handed).
        const QVector3D world_up(0, 1, 0);
        const QVector3D right = QVector3D::crossProduct(forward, world_up).normalized();
        const QVector3D up = QVector3D::crossProduct(right, forward).normalized();

        const float pan_amount = distance_ * kPanSpeed;
        // delta.x() > 0 means mouse moved right → pan world left
        // (camera follows mouse) — but the more intuitive convention is
        // "grab and drag": mouse right drags the scene right. Use the
        // intuitive sign.
        pan_offset_ += right * static_cast<float>(delta.x()) * pan_amount;
        pan_offset_ -= up * static_cast<float>(delta.y()) * pan_amount;
    } else if (e->buttons() & Qt::MiddleButton) {
        // Bonus: middle-button drag zooms (alternative to wheel).
        constexpr float kZoomSpeed = 0.02f;
        distance_ = std::clamp(distance_ * (1.0f + delta.y() * kZoomSpeed), 0.5f, 50.0f);
    }
    update();
}

void PointCloudView::wheelEvent(QWheelEvent* e) {
    // angleDelta is in 1/8 of a degree. Positive = wheel up = zoom in.
    // Divide by 120 (one notch = 15° = 120 units) to get notches.
    const float notches = static_cast<float>(e->angleDelta().y()) / 120.0f;
    if (notches == 0.0f) return;
    const float factor = std::pow(0.9f, notches);  // 0.9 per notch
    distance_ = std::clamp(distance_ * factor, 0.5f, 50.0f);
    update();
}

void PointCloudView::keyPressEvent(QKeyEvent* e) {
    // Arrow keys rotate, +/- zooms, Home resets.
    constexpr float kKeyRotStep = 5.0f;
    switch (e->key()) {
        case Qt::Key_Left:  yaw_ -= kKeyRotStep; break;
        case Qt::Key_Right: yaw_ += kKeyRotStep; break;
        case Qt::Key_Up:    pitch_ += kKeyRotStep; break;
        case Qt::Key_Down:  pitch_ -= kKeyRotStep; break;
        case Qt::Key_Plus:
        case Qt::Key_Equal: distance_ = std::clamp(distance_ * 0.9f, 0.5f, 50.0f); break;
        case Qt::Key_Minus: distance_ = std::clamp(distance_ * 1.1f, 0.5f, 50.0f); break;
        case Qt::Key_Home:  resetView(); return;
        default: QOpenGLWidget::keyPressEvent(e); return;
    }
    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    update();
}
