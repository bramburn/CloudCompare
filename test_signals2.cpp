// Test Q_SIGNALS with MSVC /permissive-
#include <QtCore/qobjectdefs.h>

class TestObj : public QObject
{
    Q_OBJECT
  public:
    TestObj(QObject* parent = nullptr) : QObject(parent) {}
  Q_SIGNALS:
    void mySignal();
};

int main() {}
