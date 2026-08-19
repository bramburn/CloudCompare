// Test signals: with ccOctree.h-style include chain
#include <QOpenGLFunctions_2_1>
#include <QObject>

class TestObj : public QObject
{
    Q_OBJECT
  public:
    TestObj(QObject* parent = nullptr) : QObject(parent) {}
  signals:
    void mySignal();
};

int main() {}
