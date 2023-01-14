#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QTimer>
#include <QCommandLineParser>
#include "blufiserverapplication.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "blufi_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    BlufiServerApplication *application = new BlufiServerApplication(&a);

    QObject::connect(application, &BlufiServerApplication::destroyed, []() {
        QCoreApplication::exit();
    });

    QTimer::singleShot(0, application, &BlufiServerApplication::start);

    return a.exec();
}
