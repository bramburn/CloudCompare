// cpp_qt_console template — minimal Qt 6 console app.
//
// Replace this with your experiment. The skeleton just demonstrates that
// QCoreApplication, Qt's command-line parser, and Qt logging all work
// in a console context (no GUI).

/**
 * @file main.cpp
 *
 * @brief Console Qt application template
 *
 * Minimal Qt console application template demonstrating
 * CloudCompare library integration from a standalone app.
 *
 * ## Usage
 *
 * This template shows how to:
 * - Initialize the CCCoreLib library
 * - Load a point cloud from a file
 * - Process it using CCCoreLib algorithms
 * - Print results to stdout
 *
 * Compile against the CloudCompare build artifacts.
 */
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcMain, "template.qt.console")

// Trivial example: count lines in a file. Replace with your experiment.
static int countLines(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(lcMain) << "could not open" << path << ":" << f.errorString();
        return -1;
    }
    int n = 0;
    while (!f.atEnd()) {
        f.readLine();
        ++n;
    }
    return n;
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("cpp_qt_console_template");
    QCoreApplication::setApplicationVersion("0.1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Demo CLI: count lines in a text file.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Path to text file (use - for stdin).", "[file]");
    QCommandLineOption verboseOpt({"v", "verbose"}, "Print extra info.");
    parser.addOption(verboseOpt);
    parser.process(app);

    if (parser.isSet(verboseOpt)) {
        QLoggingCategory::setFilterRules("template.qt.console.debug=true");
    }

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        qCInfo(lcMain) << "no file given — printing hello world instead";
        QTextStream(stdout) << "Hello, Qt 6 console template!\n";
        return 0;
    }

    const QString path = args.first();
    if (path == "-") {
        QTextStream(stdout) << "stdin mode not implemented; pass a file path\n";
        return 1;
    }

    QFileInfo fi(path);
    if (!fi.exists()) {
        qCCritical(lcMain) << "file not found:" << path;
        return 2;
    }

    const int n = countLines(path);
    QTextStream(stdout) << path << " : " << n << " line(s)\n";
    return (n < 0) ? 3 : 0;
}
