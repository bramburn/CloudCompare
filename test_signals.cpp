// Test: does Qt6 signals: compile with MSVC /permissive-?
#include <QtCore/qobjectdefs.h>
#include <QtCore/qobject.h>

class TestQtSignals : public QObject {
    Q_OBJECT
public:
    explicit TestQtSignals(QObject* parent = nullptr) : QObject(parent) {}
    ~TestQtSignals() = default;

public slots:
    void doSomething() {}

signals:
    void changed();
    void updated(int value);

public:
    void trigger() {
        emit changed();
        emit updated(42);
    }
};

int main() {
    return 0;
}
