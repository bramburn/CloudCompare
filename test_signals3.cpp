// Test signals: (Qt6 style) with MSVC /permissive-
#include <QtCore/qobjectdefs.h>

class TestObj : public QObject
{
    Q_OBJECT
  public:
    TestObj(QObject* parent = nullptr) : QObject(parent) {}
  signals:
    void mySignal();
};

int main() {}
