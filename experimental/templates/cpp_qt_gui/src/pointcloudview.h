#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QPoint>
#include <memory>
#include <vector>

/// A 3D point cloud viewport.
///
/// Renders a vector of (x, y, z) points as coloured GL_POINTS. Supports
/// mouse rotation, pan, and zoom. The colour is mapped from z-coordinate
/// using a simple blue→red gradient (viridis-like).
///
/// Replace the point source with whatever the experiment needs:
///   - Synthetic (see synthetic.h)
///   - File loader (.las, .laz, .obj)
///   - QProcess-launched Rust binary
class PointCloudView : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit PointCloudView(QWidget* parent = nullptr);
    ~PointCloudView() override;

    /// Replace the rendered point cloud.
    void setPoints(const std::vector<float>& interleaved_xyz,
                   size_t point_count);

    /// Set the bounding box for normalization. If unset, computed from points.
    void setBounds(const QVector3D& min, const QVector3D& max);

    /// Reset view to default.
    void resetView();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

private:
    void uploadPointsToGPU();
    void computeBoundsFromPoints();
    QMatrix4x4 buildModelViewProjection() const;
    void drawAxes(const QMatrix4x4& mvp);
    void drawPoints(const QMatrix4x4& mvp);

    std::vector<float> points_;     // interleaved xyz
    size_t point_count_ = 0;

    // GL handles
    GLuint vbo_ = 0;
    GLuint axes_vbo_ = 0;
    GLuint shader_program_ = 0;
    GLuint axes_shader_ = 0;
    size_t vbo_size_ = 0;

    // Camera
    float yaw_ = 30.0f;     // degrees around Y
    float pitch_ = 20.0f;   // degrees around X
    float distance_ = 5.0f;
    QPoint last_mouse_pos_;
    QVector3D pan_offset_ = QVector3D(0, 0, 0);

    // Bounds (set by caller or computed from points)
    QVector3D bounds_min_ = QVector3D(-1, -1, -1);
    QVector3D bounds_max_ = QVector3D(1, 1, 1);
    bool bounds_set_ = false;
};
