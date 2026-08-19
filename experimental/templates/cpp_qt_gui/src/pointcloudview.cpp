// PointCloudView — 3D viewport for point cloud rendering.

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

out float vColor;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    gl_PointSize = 2.0;
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
    setMouseTracking(true);
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
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp.constData());

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
    last_mouse_pos_ = e->pos();
}

void PointCloudView::mouseMoveEvent(QMouseEvent* e) {
    QPoint delta = e->pos() - last_mouse_pos_;
    last_mouse_pos_ = e->pos();

    if (e->buttons() & Qt::LeftButton) {
        yaw_ += delta.x() * 0.5f;
        pitch_ += delta.y() * 0.5f;
        pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    } else if (e->buttons() & Qt::RightButton) {
        // Pan in screen space
        QMatrix4x4 inv = buildModelViewProjection().inverted();
        QVector4D a = inv * QVector4D(0, 0, 0, 1);
        QVector4D b = inv * QVector4D(delta.x() * 0.005f, -delta.y() * 0.005f, 0, 1);
        pan_offset_ += (b - a).toVector3D();
    }
    update();
}

void PointCloudView::wheelEvent(QWheelEvent* e) {
    const float factor = (e->angleDelta().y() > 0) ? 0.9f : 1.1f;
    distance_ = std::clamp(distance_ * factor, 0.5f, 50.0f);
    update();
}
