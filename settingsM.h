#ifndef SETTINGSM_G
#define SETTINGSM_G

#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QDir>
#include <QDebug>
//#include <arpa/inet.h>
#include <pwd.h>

#include <zrkServiceInfo.h>

class SettingsConsoleM : QObject
{
    Q_OBJECT
    public:

//    int SAC,
//        SIC;

    float seaLevel;

    int useP18;
    //  2018_06_25  int useVirtualHandWheel;
    float handWheelSpeedFactor;
    float handWheelPositionBFactor;
    float handWheelPositionEFactor;

    char ipAddressServerUNV[255];
    char ipAddressSimulator[255];
    char ipAddressVKP[255];
    char ipAddressP18[255];

    ushort port_OperatorF[2];
    ushort port_awsCommander;
    ushort port_awsGuidanceOfficer;
    ushort port_awsLauncherOfficer;
    ushort port_Simulator;
    ushort port_awsDiagnMonitor;
    ushort port_awsInfoTable;
    ushort port_serverUNV;
    ushort port_VKP;
    ushort port_P18;

    int servoDriveDiagnostic;
    QString programAbsolutePath;

    SettingsConsoleM(geoPosition *geoPos)
    {
        QDir dir = QDir::currentPath();
        programAbsolutePath = dir.absolutePath();

        QSettings *qSetting = new QSettings(dir.absolutePath() + "/awsM125FactorySettings.conf", QSettings::NativeFormat);

        useP18 = qSetting->value("Application/useP18", 0).toInt();
        //  2018_06_25  useVirtualHandWheel = qSetting->value("Application/useVirtualHandWheel", 0).toInt();

        handWheelSpeedFactor = qSetting->value("Application/handWheelSpeedFactor", 0.000001).toFloat();

        //SAC = qSetting->value("SACSIC/SAC", 0x11).toInt();
        //SIC = qSetting->value("SACSIC/SIC", 0x96).toInt();

        //seaLevel = qSetting->value("Application/seaLevel_meters", 170).toFloat();
        //isHDLC = qSetting->value("Application/isHDLCFactory", false).toBool();

        //refrActiveIndex = qSetting->value("Application/refrActiveIndex", -4000).toFloat();        

        QString strIpAddressAndPortMainConsoleApp = qSetting->value("IPAddressesAndPorts/ipAddressAndPort_serverUNV", "127.0.0.1:30024").toString();
        QStringList strListIpAddrPortMainConsoleApp = strIpAddressAndPortMainConsoleApp.split(":");

        if(strListIpAddrPortMainConsoleApp.size() == 2)
        {
            strcpy(ipAddressServerUNV,  strListIpAddrPortMainConsoleApp[0].toStdString().c_str());
            bool ok;
            port_serverUNV = strListIpAddrPortMainConsoleApp[1].toUShort(&ok, 10);
        }

        QString strIpAddressAndPortSimulator = qSetting->value("IPAddressesAndPorts/ipAddressAndPort_awsSimulator", "192.168.48.152:21222").toString();
        QStringList strListIpAddrPortSimulator = strIpAddressAndPortSimulator.split(":");

        if(strListIpAddrPortSimulator.size() == 2)
        {
            strcpy(ipAddressSimulator,  strListIpAddrPortSimulator[0].toStdString().c_str());
            bool ok;
            port_Simulator = strListIpAddrPortSimulator[1].toUShort(&ok, 10);
        }

        QString strIpAddressAndPortVKP = qSetting->value("IPAddressesAndPorts/ipAddressAndPort_VKP", "10.0.87.184:17001").toString();
        QStringList strListIpAddrPortVKP = strIpAddressAndPortVKP.split(":");

        //qDebug()<<"SettingsConsoleC strIpAddressAndPortSimulator:"<<strIpAddressAndPortVKP;

        if(strListIpAddrPortVKP.size() == 2)
        {
            strcpy(ipAddressVKP,  strListIpAddrPortVKP[0].toStdString().c_str());
            bool ok;
            port_VKP = strListIpAddrPortVKP[1].toUShort(&ok, 10);
        }

        QString strIpAddressAndPortP18 = qSetting->value("IPAddressesAndPorts/ipAddressAndPort_P18", "192.168.48.109:17001").toString();
        QStringList strListIpAddrPortP18 = strIpAddressAndPortP18.split(":");

        //qDebug()<<"SettingsConsoleC strIpAddressAndPortSimulator:"<<strIpAddressAndPortVKP;

        if(strListIpAddrPortP18.size() == 2)
        {
            strcpy(ipAddressP18,  strListIpAddrPortP18[0].toStdString().c_str());
            bool ok;
            port_P18 = strListIpAddrPortP18[1].toUShort(&ok, 10);
        }

        port_awsCommander = qSetting->value("IPAddressesAndPorts/port_awsCommander", 30016).toUInt();
        port_awsDiagnMonitor = qSetting->value("IPAddressesAndPorts/portDiagnMonitor", 30018).toUInt();

        port_awsGuidanceOfficer = qSetting->value("IPAddressesAndPorts/port_awsGuidanceOfficer", 30017).toUInt();
        port_awsLauncherOfficer = qSetting->value("IPAddressesAndPorts/port_awsLauncherOfficer", 30019).toUInt();

        port_OperatorF[0] = qSetting->value("IPAddressesAndPorts/portOperatorF1", 30028).toUInt();
        port_OperatorF[1] = qSetting->value("IPAddressesAndPorts/portOperatorF2", 30029).toUInt();

        port_awsInfoTable = qSetting->value("IPAddressesAndPorts/portInfoTable", 30014).toUInt();        

        // К О О Р Д И Н А Т Ы   Т О Ч К И   С Т О Я Н И Я
        geoPos->geoY.integerDegr = qSetting->value("Coordinates/integerDegreeY", 0).toInt();

        geoPos->geoY.integerMin = qSetting->value("Coordinates/integerMinuteY", 0).toInt();
        geoPos->geoY.integerSec = qSetting->value("Coordinates/integerSecondY", 0).toInt();
        geoPos->geoY.integerSec100 = qSetting->value("Coordinates/integer100SecY", 0).toInt();

        geoPos->geoX.integerDegr = qSetting->value("Coordinates/integerDegreeX", 0).toInt();
        geoPos->geoX.integerMin = qSetting->value("Coordinates/integerMinuteX", 0).toInt();
        geoPos->geoX.integerSec = qSetting->value("Coordinates/integerSecondX", 0).toInt();
        geoPos->geoX.integerSec100 = qSetting->value("Coordinates/integer100SecX", 0).toInt();

        geoPos->seaLevelH = qSetting->value("Coordinates/integerFullHeight", 0).toInt();         

//        qDebug()<<"SettingsConsoleG: ipAddressG:"<<ipAddressG<<"canPortG:"<<canPortG<<"ipAddressF1:"<<ipAddressF1<<"canPortF1:"<<canPortF1
//                  <<"ipAddressF2:"<<ipAddressF2<<"canPortF2:"<<canPortF2<<"ipAddressSimulator:"
//                 <<ipAddressSimulator<<"portSimulator:"<<portSimulator<<"portF[0]:"<<portOperatorF[0]<<"portF[1]"<<portOperatorF[1];
    }
};

#endif // SETTINGSM_G
