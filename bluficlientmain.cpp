#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QTimer>
#include <QCommandLineParser>
#include "bluficlientapplication.h"

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

    QCommandLineOption nameOption(QStringList() << "n" << "name", QObject::tr("Blufi device name regular expression filter (Case sensitive)"), "nameFilter");
    QCommandLineOption addressOption(QStringList() << "a" << "address", QObject::tr("Bluetooth LE public address regular expression filter (Case insensitive)", "address"), "addressFilter");
    QCommandLineOption ssidOption(QStringList() << "s" << "ssid", QObject::tr("SSID of Wi-Fi hotspot"), "ssid");
    QCommandLineOption pskOption(QStringList() << "p" << "psk", QObject::tr("Pre-shared key of Wi-Fi hotspot"), "password");

    QCommandLineParser parser;
    parser.addOptions({ nameOption, addressOption, ssidOption, pskOption });
    parser.setApplicationDescription(QObject::tr("Configure Wi-Fi network for ESP32/ESP8266 device via Bluetooth channel"));
    parser.addHelpOption();
    parser.process(a);

    const QString &ssid = parser.value(ssidOption);
    if (!ssid.isEmpty()) {
        BlufiClientApplication *application = new BlufiClientApplication(&a);

        application->setSsid(ssid);
 
        const QString &psk = parser.value(pskOption);
        if (!psk.isEmpty()) {
            application->setPsk(psk);
        }

        const QString &name = parser.value(nameOption);
        if (!name.isEmpty()) {
            application->setNameFilter(name);
        }

        const QString &address = parser.value(addressOption);
        if (!address.isEmpty()) {
            application->setAddressFilter(address);
        }

        QObject::connect(application, &BlufiClientApplication::destroyed, []() {
            QCoreApplication::exit();
        });

        QTimer::singleShot(0, application, &BlufiClientApplication::start);
    }
    else {
        parser.showHelp();
    }

    return a.exec();
}
