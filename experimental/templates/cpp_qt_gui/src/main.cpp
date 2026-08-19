// cpp_qt_gui template — desktop view with 3D OpenGL viewport.
//
// Shows a Qt 6 main window with a 3D point cloud viewport. By default,
// renders a synthetic spiral. Replace with a file loader, a QProcess
// call to a Rust binary, or a CXX-linked Rust staticlib (advanced).
//
// Controls:
//   - Left-mouse drag: rotate
//   - Right-mouse drag: pan
//   - Mouse wheel: zoom
//   - "Reset View" button: return to default

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QStatusBar>

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
    auto* info_label = new QLabel("Drag to rotate. Wheel to zoom. Right-drag to pan.");
    info_label->setStyleSheet("color: #888;");

    toolbar->addWidget(new QLabel("Dataset:"));
    toolbar->addWidget(dataset_combo);
    toolbar->addSpacing(20);
    toolbar->addWidget(reset_btn);
    toolbar->addStretch();
    toolbar->addWidget(info_label);

    layout->addLayout(toolbar);

    // 3D viewport
    auto* view = new PointCloudView();
    layout->addWidget(view, /*stretch=*/1);

    // Status bar
    auto* status = new QLabel("Ready. 0 points.");
    window.statusBar()->addWidget(status);

    // Wire data loading
    auto load_and_show = [view, status]() {
        // For now, hardcoded synthetic data. Replace with file load / QProcess.
        std::vector<float> data;
        size_t n = 0;
        // Default to spiral; selection handler will overwrite.
        data = synthetic::spiral(2000);
        n = data.size() / 3;
        view->setPoints(data, n);
        status->setText(QString("Loaded %1 synthetic points.").arg(n));
    };
    load_and_show();

    QObject::connect(dataset_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [view, status](int idx) {
        std::vector<float> data;
        size_t n = 0;
        switch (idx) {
            case 0: data = synthetic::spiral(2000); n = 2000; break;
            case 1: data = synthetic::gaussian_cloud(5000); n = 5000; break;
            case 2: data = synthetic::helix(2000); n = 2000; break;
        }
        view->setPoints(data, n);
        view->resetView();
        status->setText(QString("Loaded %1 synthetic points.").arg(n));
    });

    QObject::connect(reset_btn, &QPushButton::clicked, [view, status]() {
        view->resetView();
        status->setText("View reset.");
    });

    window.setCentralWidget(central);
    window.show();
    return app.exec();
}
