// cpp_qt_gui template — desktop view with 3D OpenGL viewport.
//
// Shows a Qt 6 main window with a 3D point cloud viewport. By default,
// renders a synthetic spiral. Replace with a file loader, a QProcess
// call to a Rust binary, or a CXX-linked Rust staticlib (advanced).
//
// Controls:
//   - Left-mouse drag:   rotate (yaw + pitch)
//   - Right-mouse drag:  pan (camera right + up)
//   - Middle-mouse drag: zoom (alternative to wheel)
//   - Mouse wheel:       zoom in/out
//   - Arrow keys:        rotate by 5° per press
//   - + / -:             zoom
//   - Home:              reset to default view
//   - "Reset View" button: same as Home

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QStatusBar>
#include <QTimer>

#include "pointcloudview.h"
#include "synthetic.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("cpp_qt_gui_template");
    QApplication::setApplicationVersion("0.1.0");

    QMainWindow window;
    window.setWindowTitle("Point Cloud Viewer — cpp_qt_gui template");
    window.resize(900, 700);

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);

    // Toolbar
    auto* toolbar = new QHBoxLayout();
    auto* dataset_combo = new QComboBox();
    dataset_combo->addItem("Spiral (2000 pts)");
    dataset_combo->addItem("Gaussian (5000 pts)");
    dataset_combo->addItem("Helix (2000 pts)");

    auto* reset_btn = new QPushButton("Reset View");
    auto* info_label = new QLabel(
        "Left-drag: rotate. Right-drag: pan. Wheel / middle-drag: zoom. "
        "Arrows / +/-: rotate / zoom. Home: reset view.");
    info_label->setStyleSheet("color: #888;");
    info_label->setWordWrap(true);

    toolbar->addWidget(new QLabel("Dataset:"));
    toolbar->addWidget(dataset_combo);
    toolbar->addSpacing(20);
    toolbar->addWidget(reset_btn);
    toolbar->addStretch();
    toolbar->addWidget(info_label);

    layout->addLayout(toolbar);

    // 3D viewport — give it explicit focus so wheel events work without
    // requiring the user to click into the viewport first.
    auto* view = new PointCloudView();
    view->setFocus();
    layout->addWidget(view, /*stretch=*/1);

    // Status bar (two labels: persistent info on the left, camera state on the right)
    auto* status = new QLabel("Ready. 0 points.");
    auto* camera_status = new QLabel("yaw=0° pitch=0° dist=0.0");
    window.statusBar()->addWidget(status, /*stretch=*/2);
    window.statusBar()->addPermanentWidget(camera_status, /*stretch=*/1);

    // Wire data loading
    auto load_and_show = [view, status](int idx) {
        std::vector<float> data;
        size_t n = 0;
        switch (idx) {
            case 0: data = synthetic::spiral(2000); n = 2000; break;
            case 1: data = synthetic::gaussian_cloud(5000); n = 5000; break;
            case 2: data = synthetic::helix(2000); n = 2000; break;
            default: data = synthetic::spiral(2000); n = 2000; break;
        }
        view->setPoints(data, n);
        view->resetView();
        status->setText(QString("Loaded %1 synthetic points.   Drag inside the viewport to interact.").arg(n));
    };
    load_and_show(dataset_combo->currentIndex());

    QObject::connect(dataset_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [view, status, load_and_show](int idx) {
        load_and_show(idx);
    });

    QObject::connect(reset_btn, &QPushButton::clicked, [view, status]() {
        view->resetView();
        status->setText("View reset.   Drag inside the viewport to interact.");
    });

    // Live camera-state readout. Polls 10×/sec; cheap.
    auto* camera_timer = new QTimer(&window);
    QObject::connect(camera_timer, &QTimer::timeout, [view, camera_status]() {
        float yaw, pitch, dist;
        QVector3D pan;
        view->cameraState(&yaw, &pitch, &dist, &pan);
        camera_status->setText(QString("yaw=%1°  pitch=%2°  dist=%3  pan=(%4, %5, %6)")
            .arg(static_cast<int>(yaw))
            .arg(static_cast<int>(pitch))
            .arg(dist, 0, 'f', 2)
            .arg(pan.x(), 0, 'f', 2)
            .arg(pan.y(), 0, 'f', 2)
            .arg(pan.z(), 0, 'f', 2));
    });
    camera_timer->start(100);

    window.setCentralWidget(central);
    window.show();
    return app.exec();
}
