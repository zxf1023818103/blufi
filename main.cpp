#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QTimer>
#include <QCommandLineParser>
#include "blufiapplication.h"

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

    QCommandLineOption nameOption(QStringList() << "n" << "name", QObject::tr("Blufi device name filter (Could use regular expression)"));
    QCommandLineOption addressOption(QStringList() << "a" << "address", QObject::tr("Bluetooth LE MAC address filter (Could use regular expression)"));
    QCommandLineOption ssidOption(QStringList() << "s" << "ssid", QObject::tr("SSID of Wi-Fi hotspot"));
    QCommandLineOption pskOption(QStringList() << "p" << "psk", QObject::tr("Pre-shared key of Wi-Fi hotspot"));

    QCommandLineParser parser;
    parser.addOptions({ nameOption, addressOption, ssidOption, pskOption });
    parser.setApplicationDescription(QObject::tr("Configure Wi-Fi network for ESP32/ESP8266 device via Bluetooth channel"));
    parser.addHelpOption();
    parser.setSingleDashWordOptionMode(QCommandLineParser::SingleDashWordOptionMode::ParseAsCompactedShortOptions);
    parser.process(a);

    if (parser.isSet(ssidOption)) {
        BlufiApplication *application = new BlufiApplication(&a);

        application->setSsid(parser.value(ssidOption));

        if (parser.isSet(pskOption)) {
            application->setPsk(parser.value(pskOption));
        }

        if (parser.isSet(nameOption)) {
            application->setNameFilter(parser.value(nameOption));
        }

        if (parser.isSet(addressOption)) {
            application->setAddressFilter(parser.value(addressOption));
        }

        QObject::connect(application, &BlufiApplication::finished, &QCoreApplication::quit);

        QTimer::singleShot(0, application, &BlufiApplication::start);
    }
    else {
        parser.showHelp();
    }

    return a.exec();
}
