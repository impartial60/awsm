#include <unistd.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

#include "externM.h"

extern UV11ANGLES  operatorPovorota(float b, float e);
extern void assignGradysnikParams(int ii);

#define Asterix126Cat           126         // категория Asterix 126

uint8_t array125[1000];
QString str125;
QString str126;
int u;

/* ПП переставляет байты местами для координат WGS, используемых в Asterix
 * Вход:
 *   *Coord - указатель на координаты
 * Выход:
 *        -  ...туда же записывается результат
 */
//void //Asterix::
//swapWGS(sAsterix_WGS_Coordinates *Coord)
//{
//    swapBytes(&Coord->Height);
//    uint8_t tmp = Coord->Lat.Coord[0];
//    Coord->Lat.Coord[2] = Coord->Lat.Coord[0];
//    Coord->Lat.Coord[2] = tmp;
//    tmp = Coord->Lon.Coord[0];
//    Coord->Lon.Coord[2] = Coord->Lon.Coord[0];
//    Coord->Lon.Coord[2] = tmp;
//}

uint16_t uint16_t_byte_reorder(uint16_t inputValue)
{
    uint8_t uint8_t_1 = inputValue >> 8;
    uint16_t uint16_t_returnValue = inputValue << 8;

    uint16_t_returnValue += uint8_t_1;
    //qDebug()<<"uint16_t_byte_reorder: inputValue:"<<inputValue<<"uint8_t_1:"<<uint8_t_1<<"uint16_t_returnValue:"<<uint16_t_returnValue;
    //qDebug()<<"uint16_t_byte_reorder: inputValue:"<<QString("0x%1").arg(inputValue, 4, 16, QChar('0'))<<"uint8_t_1:"<<uint8_t_1<<"uint16_t_returnValue:"<<QString("0x%1").arg(uint16_t_returnValue, 4, 16, QChar('0'));

    return uint16_t_returnValue;
}


QString strByte125(uint8_t inputVal, QString pName8, QString pName7, QString pName6, QString pName5,
                                     QString pName4, QString pName3, QString pName2, QString pName1)
{
    QString parameterName;

    QString retStr = QString(": 0x%1 ").arg(inputVal, 2, 16, QChar('0'));

    for(int i = 8; i >= 1; i--)
    {
        int retVal = 0;

        switch(i)
        {
            case 8: parameterName = pName8; if((inputVal & 0x80) == 0x80) retVal = 1; break;
            case 7: parameterName = pName7; if((inputVal & 0x40) == 0x40) retVal = 1; break;
            case 6: parameterName = pName6; if((inputVal & 0x20) == 0x20) retVal = 1; break;
            case 5: parameterName = pName5; if((inputVal & 0x10) == 0x10) retVal = 1; break;
            case 4: parameterName = pName4; if((inputVal & 0x08) == 0x08) retVal = 1; break;
            case 3: parameterName = pName3; if((inputVal & 0x04) == 0x04) retVal = 1; break;
            case 2: parameterName = pName2; if((inputVal & 0x02) == 0x02) retVal = 1; break;
            case 1: parameterName = pName1; if((inputVal & 0x01) == 0x01) retVal = 1;
        }

        if(parameterName == "-" || parameterName == "#")
            retStr += " " + parameterName;
        else
            retStr += " " + parameterName + ":" + QString::number(retVal);
    }

    return retStr;
}


COORD_YHX coordYHXParser125(int uu)
{
    uint8_t byteCrd[12];

    byteCrd[1] = array125[uu + 0]; byteCrd[0] = array125[uu + 1];
    byteCrd[3] = array125[uu + 2]; byteCrd[2] = array125[uu + 3];
    byteCrd[5] = array125[uu + 4]; byteCrd[4] = array125[uu + 5];
    byteCrd[7] = array125[uu + 6]; byteCrd[6] = array125[uu + 7];
    byteCrd[9] = array125[uu + 8]; byteCrd[8] = array125[uu + 9];
    byteCrd[11]= array125[uu + 10];byteCrd[10]=array125[uu + 11];


    int16_t crdY1;
    int16_t crdH1;
    int16_t crdX1;

    int16_t vY1;
    int16_t vH1;
    int16_t vX1;

    memcpy(&crdY1, byteCrd     , 2);
    memcpy(&crdH1, byteCrd +  2, 2);
    memcpy(&crdX1, byteCrd +  4, 2);

    memcpy(&vY1,   byteCrd +  6, 2);
    memcpy(&vH1,   byteCrd +  8, 2);
    memcpy(&vX1,   byteCrd + 10, 2);

//    qDebug()<<"ProcessVKP: crdY1:"<<50 * crdY1<<"crdH1:"<<50*crdH1<<"crdX1:"<<50*crdX1;
//    qDebug()<<"              vY1:"<<vY1<<"vH1:"<<vH1<<"vX1:"<<vX1;

    COORD_YHX retYHX;       // 0.05 км - цена мл. разряда по Северу, Высоте и Востоку
                            // 1 м/с   - цена мл. разряда всех первых производных

    retYHX.y_km = 0.05 * crdY1;
    retYHX.h_km = 0.05 * crdH1;
    retYHX.x_km = 0.05 * crdX1;

    retYHX.vy_ms =  vY1;
    retYHX.vh_ms =  vH1;
    retYHX.vx_ms =  vX1;

    qDebug()<<"(#(#(#(coordYHXParser125: Север"<<retYHX.y_km<<"Высота:"<<retYHX.h_km<<"Восток:"<<retYHX.x_km<<"VСевер:"<<retYHX.vy_ms<<"VВысота:"<<retYHX.vh_ms<<"VВосток:"<<retYHX.vx_ms;

    return retYHX;
}


void strKomandiCelevojKanal125()
{
    //str125 += " &&&&&&&&&&&есть I125/050 Команды по целевому каналу 1 ";    

    bool targetInfoExist = false;

    /////////////////////////////////////////////if((array125[u + 1] & 0x01) == 0x01)
    {
        targetInfoExist = true;
        str125 += " есть инф.по цели ";
    }
    ///////////////////////////////////////////else
        ///////////////////////////////////////////str125 += " нет инф.по цели ";

    str125 += QString(" НомК:0b%1 ").arg((array125[u+1] & 0b1110) >> 1, 3, 2, QChar('0'));

    str125 += "b1" + strByte125(array125[u  ], "-", "-", "-", "-", "-", "РЗРК", "РИС", "ЗПВД");// Байт 1
    str125 += "b2" + strByte125(array125[u+1], "ЗПВС", "ЗП", "ОЦУ", "ЦУ", "#", "#", "#", "#");// Байт 2

    if(targetInfoExist)
    {
        str125 += QString(" NцКП: 0x%1%2").arg(array125[u+2], 2, 16, QChar('0')).arg(array125[u+3], 2, 16, QChar('0'));

        str125 += QString(" NцЗРК: 0x%1").arg(array125[u+4], 2, 16, QChar('0'));

        str125 += "b6" + strByte125(array125[u+5], "БР", "МР", "ПАП", "МанК", "МанВ", "МанС", "Приор", "-");  //Байт 6  Признаки цели


        //str125 += QString(" NцЗРК: 0x%1").arg(, 2, 16, QChar('0'));

        //Байт 7    Тип цели + ХВО (характеристика воздушного объекта)

        uint8_t targetType = (array125[u+6] & 0b11110000) >> 4;
        QString targetTypeStr;

        switch (targetType)
        {
            case 0b0000: targetTypeStr = "нет инф"; break;
            case 0b0001: targetTypeStr = "кр(бесп)"; break;
            case 0b0010: targetTypeStr = "вертол"; break;
            case 0b0011: targetTypeStr = "аэрод"; break;
            case 0b0101: targetTypeStr = "рез"; break;
            case 0b0110: targetTypeStr = "рез"; break;
            case 0b0111: targetTypeStr = "рез"; break;
            default:     targetTypeStr = "???"; break;
        }

        uint8_t hvoValue = array125[u+6] & 0b00001111;
        QString hvoStr;        

        switch (hvoValue)
        {
            case 0b0000: hvoStr = "нет инф"; break;
            case 0b0001: hvoStr = "свой"; break;
            case 0b0010: hvoStr = "чужой (противник)"; break;
            case 0b0011: hvoStr = "неопознанный"; break;
            case 0b0100: hvoStr = "рез"; break;
            default:     hvoStr = "???"; break;
        }

        str125 += QString(" b7: 0x%1").arg(array125[u+6], 2, 16, QChar('0')) + "ТЦ:" + targetTypeStr + "ХВО:" + hvoStr;

        str125 += QString(" b8: 0x%1").arg(array125[u+7], 2, 16, QChar('0'));

        uint8_t opoznValue = array125[u+8] & 0b00001111;
        QString opoznStr;

        switch (opoznValue)
        {
            case 0b0000: opoznStr = "не пров."; break;
            case 0b0001: opoznStr = "отвечает"; break;
            case 0b0010: opoznStr = "не отв."; break;
            case 0b0011: opoznStr = "рез"; break;
            default:     hvoStr = "???"; break;
        }

        str125 += QString(" b9: 0x%1").arg(array125[u+8], 2, 16, QChar('0')) + "ОП:" + opoznStr;

        QString groupStr = QString(" b10: 0x%1").arg(array125[u+9], 2, 16, QChar('0')) +
                " ПГ:" + QString::number((array125[u+9] & 0x80) >> 7) +
                " КолГ:" + QString::number((array125[u+9] & 0x20) >> 5) +
                " Прав:" + QString::number((array125[u+9] & 0b00001110) >> 1);

        str125 += groupStr;

        QString pomehaStr = QString(" b11: 0x%1").arg(array125[u+10], 2, 16, QChar('0')) +
                " НП:" + QString::number((array125[u+9] & 0b11000000) >> 6) +
                " Ин:" + QString::number((array125[u+9] & 0b00110000) >> 4) +
                " Вид:" + QString::number(array125[u+9] & 0b00001111);

        str125 += pomehaStr;

        //////////////////////////////////////////////////coordYHXParser125(u+11);

//                               qDebug()<<"############coordYHXParser125: Север"<<shMemC2U.vkp2zrk.coordYHX.y_km<<"Высота:"
//                                      <<shMemC2U.vkp2zrk.coordYHX.h_km<<"Восток:"<<shMemC2U.vkp2zrk.coordYHX.x_km<<"VСевер:"<<shMemC2U.vkp2zrk.coordYHX.vy_ms
//                                     <<"VВысота:"<<shMemC2U.vkp2zrk.coordYHX.vh_ms<<"VВосток:"<<shMemC2U.vkp2zrk.coordYHX.vx_ms;


        str125 += QString(" Север b12: 0x%1").arg(array125[u+11], 2, 16, QChar('0'));
        str125 += QString(" b13: 0x%1").arg(array125[u+12], 2, 16, QChar('0'));
        //float coordY = 0.05 * (256 * array125[u+11] + array125[u+12]);
        //str125 += "[" + QString::number(coordY, 'f', 2) + "km]";

        str125 += QString(" Высота b14: 0x%1").arg(array125[u+13], 2, 16, QChar('0'));
        str125 += QString(" b15: 0x%1").arg(array125[u+14], 2, 16, QChar('0'));
        //float coordH = 0.05 * (256 * array125[u+13] + array125[u+14]);
        //str125 += "[" + QString::number(coordH, 'f', 2) + "km]";

        str125 += QString(" Восток b16: 0x%1").arg(array125[u+15], 2, 16, QChar('0'));
        str125 += QString(" b17: 0x%1").arg(array125[u+16], 2, 16, QChar('0'));
        //float coordX = 0.05 * (256 * array125[u+15] + array125[u+16]);
        //str125 += "[" + QString::number(coordX, 'f', 2) + "km]";

        str125 += QString(" VСевер b18: 0x%1").arg(array125[u+17], 2, 16, QChar('0'));
        str125 += QString(" b19: 0x%1").arg(array125[u+18], 2, 16, QChar('0'));
        //float vY = 256 * array125[u+17] + array125[u+18];
        //str125 += "[" + QString::number(vY, 'f', 2) + "m/s]";

        str125 += QString(" VВысота b20: 0x%1").arg(array125[u+19], 2, 16, QChar('0'));
        str125 += QString(" b21: 0x%1").arg(array125[u+20], 2, 16, QChar('0'));
        //float vH = 256 * array125[u+19] + array125[u+20];
        //str125 += "[" + QString::number(vH, 'f', 2) + "m/s]";

        str125 += QString(" VВосток b22: 0x%1").arg(array125[u+21], 2, 16, QChar('0'));
        str125 += QString(" b23: 0x%1").arg(array125[u+22], 2, 16, QChar('0'));
        //float vX = 256 * array125[u+21] + array125[u+22];
        //str125 += "[" + QString::number(vX, 'f', 2) + "m/s]";

        u += 23;
    }   // end targetInfoExist
    else    // если нет информации по цели
    {
        u += 2;
    }
}


void strCelKomplTrenaja125()
{
    str125 += QString(" NцКП: 0x%1%2").arg(array125[u  ], 2, 16, QChar('0')).arg(array125[u+1], 2, 16, QChar('0'));

    str125 += QString(" Север b3: 0x%1").arg(array125[u+2], 2, 16, QChar('0'));
    str125 += QString(" b4: 0x%1").arg(array125[u+3], 2, 16, QChar('0'));

    str125 += QString(" Высота b5: 0x%1").arg(array125[u+4], 2, 16, QChar('0'));
    str125 += QString(" b6: 0x%1").arg(array125[u+5], 2, 16, QChar('0'));

    str125 += QString(" Восток b7: 0x%1").arg(array125[u+6], 2, 16, QChar('0'));
    str125 += QString(" b8: 0x%1").arg(array125[u+7], 2, 16, QChar('0'));


    str125 += QString(" VСевер b9: 0x%1").arg(array125[u+8], 2, 16, QChar('0'));
    str125 += QString(" b10: 0x%1").arg(array125[u+9], 2, 16, QChar('0'));

    str125 += QString(" VВысота b11: 0x%1").arg(array125[u+10], 2, 16, QChar('0'));
    str125 += QString(" b12: 0x%1").arg(array125[u+11], 2, 16, QChar('0'));

    str125 += QString(" VВосток b13: 0x%1").arg(array125[u+12], 2, 16, QChar('0'));
    str125 += QString(" b14: 0x%1").arg(array125[u+13], 2, 16, QChar('0'));    
}


void converterIskra2NextTocka() /////////////////// П Р Е О Б Р А З О В А Н И Е   П А Р А М Е Т Р О В   Ц Е Л И   И З   И С К Р Ы   В   nextTocka
{
    //        typedef struct        {
    //            float azimuth360;

    //            UV11ANGLES uv11Angles;

    //            float slantRange_km;
    //            float groundRange_km;

    //            float kyrs;
    //            float rakyrs;
    //            float newKyrs;
    //            float kyrs_proizv_1;

    //            float radialSpeed;
    //            float groundSpeed;

    //            float elevationAngle;
    //            float radarRelativeAltitude;
    //            float seaLevelAltitude;
    //            float groundX;      //  на восток
    //            float groundY;      //  на север

    //            bool isRealTarget;
    //            bool isNew;

    //            u_int16_t amplitude;

    //            sLPH soprovojdCoord;        // Координаты сопровождаемой СНР цели

    //        } TargetParams; // data_29; // Структура данных №29 от офицера наведения

    // какая цель ближе к ЦУ
    float maxNeviazka = 3000;

    int ind = 0;

    for(int i = 0; i < 3; i++)  // ПОКА ТРИ ЦЕЛИ КОМПЛЕКСНОГО ТРЕНАЖА
    {
        float dx = vkp2zrkM.cmplxYHX[i].x_km - vkp2zrkM.coordYHX.x_km;
        float dy = vkp2zrkM.cmplxYHX[i].y_km - vkp2zrkM.coordYHX.y_km;
        float dh = vkp2zrkM.cmplxYHX[i].h_km - vkp2zrkM.coordYHX.h_km;

        float neviazka = sqrt(dx * dx + dy * dy + dh *dh);
        if(neviazka < maxNeviazka)
        {
            ind = i;
            maxNeviazka = neviazka;
        }
    }

    apyInfoM.targetIndexCY = ind;

    for(int i = 0; i < 3; i++)  // ПОКА ТРИ ЦЕЛИ КОМПЛЕКСНОГО ТРЕНАЖА
    {
        float x_km = vkp2zrkM.cmplxYHX[i].x_km;
        float y_km = vkp2zrkM.cmplxYHX[i].y_km;

        float az360 = atan2(x_km, y_km) * Rad2Grad;
        if(az360 < 0.0)
            az360 += 360.0;

        apyInfoM.nextTocka[i].azimuth360 = az360;

        apyInfoM.nextTocka[i].radarRelativeAltitude = vkp2zrkM.cmplxYHX[ind].h_km;
        apyInfoM.nextTocka[i].seaLevelAltitude = apyInfoM.nextTocka[i].radarRelativeAltitude + 0.001 * apyInfoM.unvGeoPosition.seaLevelH;

        apyInfoM.nextTocka[i].groundRange_km = sqrt(x_km * x_km + y_km * y_km);

        apyInfoM.nextTocka[i].slantRange_km =
           sqrt(apyInfoM.nextTocka[i].groundRange_km * apyInfoM.nextTocka[i].groundRange_km +
                apyInfoM.nextTocka[i].radarRelativeAltitude * apyInfoM.nextTocka[i].radarRelativeAltitude);

        //  м е т о д   В О Р О Н И Н А
        float deltaH = (- R_ZemliEquiv + sqrt(R_ZemliEquiv * R_ZemliEquiv + apyInfoM.nextTocka[i].slantRange_km *
                        apyInfoM.nextTocka[i].slantRange_km));

        float srednijUgol = 5.0;
        if(apyInfoM.nextTocka[i].slantRange_km != 0)
        {
            float asv = (apyInfoM.nextTocka[i].seaLevelAltitude // - 0.001 * settings->seaLevel
                         - deltaH) / apyInfoM.nextTocka[i].slantRange_km;
            if(asv > 1)
                srednijUgol = 22.0;
            if(asv < -1)
                srednijUgol = 12.0;
            if(-1 <= asv && asv <= 1)
                srednijUgol = asin(asv) * Rad2Grad;
        }

        apyInfoM.nextTocka[i].elevationAngle = srednijUgol;// * Rad2Grad;

        apyInfoM.nextTocka[i].uv11Angles =
        operatorPovorota(apyInfoM.nextTocka[i].azimuth360 - srvDrvInfoM.currentAzimuth,
                                                 apyInfoM.nextTocka[i].elevationAngle - srvDrvInfoM.currentDirectrisaElevationAngle);

        float vx = vkp2zrkM.cmplxYHX[i].vx_ms;
        float vy = vkp2zrkM.cmplxYHX[i].vy_ms;

        float kyrs360 = atan2(vx, vy) * Rad2Grad;;
        if(kyrs360 < 0.0)
            kyrs360 += 360.0;

        apyInfoM.nextTocka[i].kyrs = kyrs360;

        float az180 = apyInfoM.nextTocka[i].azimuth360;
        if(az180 > 180.0)
            az180 -= 360.0;
        apyInfoM.nextTocka[i].rakyrs = 180.0 - apyInfoM.nextTocka[i].kyrs + az180;

        apyInfoM.nextTocka[i].groundSpeed_ms = sqrt(vx * vx + vy * vy);
        //apyInfoM.nextTocka[i].radialSpeed =
        //apyInfoM.nextTocka[i].groundSpeed_ms * cos(apyInfoM.nextTocka[i].rakyrs / Rad2Grad);

        apyInfoM.nextTocka[i].groundX = x_km;
        apyInfoM.nextTocka[i].groundY = y_km;

        assignGradysnikParams(i);
    }
}


void * ProcessThreadVKP(void * temp_pt )
{ 
    (void)temp_pt;
    vkp2zrkM.sizeInfo125 = 0;

    float coordFactor = 1024.0 * 1024.0 * 8.0 / 180.0;

    int psockfd = -1;

    PROTOKOL_FULL_126 protokol126;

    // П О С Т О Я Н Н А Я   Ч А С Т Ь   1 2 6   П Р О Т О К О Л А
    protokol126.cat = Asterix126Cat;

    protokol126.FSPEC1 = 0xFD;   // всё есть, кроме поля резерв + ещё один FSPEC        //protokol126.FSPEC = 0xE0;
    protokol126.FSPEC2 = 0x80;   // всегда только один(первый) целевой канал

    protokol126.Id = uint16_t_byte_reorder(0x0001);   // вынести в конфиг
    protokol126.NumZRK = 0x67;  // 103                // вынести в конфиг

    // I126/045         К О О Р Д И Н А Т Ы   З Р К        // координаты хранятся у офицера наведения в aws125/awsG125/awsG125FactorySettings.conf
    // int16_t intH = 1987;   float floatY = 8.704802;   float floatX = 38.997446;
    int16_t intH = apyInfoM.unvGeoPosition.seaLevelH;
    float floatY = (float)apyInfoM.unvGeoPosition.geoY.integerDegr + 1.0 / 60 * (apyInfoM.unvGeoPosition.geoY.integerMin +
              1.0 / 60 * (apyInfoM.unvGeoPosition.geoY.integerSec +  1.0 / 100 * apyInfoM.unvGeoPosition.geoY.integerSec100));
    float floatX = (float)apyInfoM.unvGeoPosition.geoX.integerDegr + 1.0 / 60 * (apyInfoM.unvGeoPosition.geoX.integerMin +
              1.0 / 60 * (apyInfoM.unvGeoPosition.geoX.integerSec +  1.0 / 100 * apyInfoM.unvGeoPosition.geoX.integerSec100));

    protokol126.coordsZRK[0] = (intH &   0xFF00) >>  8;
    protokol126.coordsZRK[1] = (intH &     0xFF);

    int32_t intY = (uint32_t)(coordFactor * floatY);
    protokol126.coordsZRK[2] = (intY & 0xFF0000) >> 16;
    protokol126.coordsZRK[3] = (intY &   0xFF00) >>  8;
    protokol126.coordsZRK[4] = (intY &     0xFF);

    int32_t intX = (uint32_t)(coordFactor * floatX);
    protokol126.coordsZRK[5] = (intX & 0xFF0000) >> 16;
    protokol126.coordsZRK[6] = (intX &   0xFF00) >>  8;
    protokol126.coordsZRK[7] = (intX &     0xFF);

    //qDebug()<<"srvDrvInfoM.currentAzimuth:"<<srvDrvInfoM.currentAzimuth<<"protokol126.BisectorWorkingSector:"<<protokol126.BisectorWorkingSector;

    clientConnect(&psockfd, settings->port_VKP, settings->ipAddressVKP, "ProcessThreadVKP initial connect");

    while (1)
    {
        u = 0;
        memset(array125, 0, sizeof(array125));
        str125 = "";

        usleep(500*1000);

        // П Е Р Е М Е Н Н А Я   Ч А С Т Ь   1 2 6   П Р О Т О К О Л А

        //  I126/020            Б И С С Е К Т Р И С А   Р А Б О Ч Е Г О   С Е К Т О Р А
        protokol126.BisectorWorkingSector = (uint8_t)(srvDrvInfoM.currentAzimuth / 360.0 * 256);

        // I126/030             С О С Т О Я Н И Е   З Р К
//        uint8_t gotovnZRK;
//        switch (shMemU2C.komKom.rejimRabotiZRK) {
//            case RejimRabotiZRK::RR: gotovnZRK = 0b0100; break; // регламентный контроль
//            case RejimRabotiZRK::BR: gotovnZRK = 0b1001; break; // боевая работа
//            case RejimRabotiZRK::TR: gotovnZRK = 0b1010; break; // ЗРК в режиме автономного тренажа
//            case RejimRabotiZRK::FK: gotovnZRK = 0b1100; break; // ЗРК в режиме комплексного контроля функционирования
//            default:                 gotovnZRK = 0b0101; break; // идет включение
//        }
        protokol126.sostZRK[0] = 0x01;  //gotovnZRK;    // для отладки - ЗРК всегда в готовности N 1

        uint8_t sostZRK_byte2 = 0x00;
//        if(shMemU2C.komKom.remoteControlZRK)
//            sostZRK_byte1 += 0x80;      // ЗРК на управлении от вышестоящего КП (1)

        sostZRK_byte2 += 0x08;      //КИЦК – количество исправных целевых каналов, 0x08 - для отладки - всегда исправен 1 канал

        if(apyInfoM.remoteControlZRK)
            sostZRK_byte2 += 0x80;    //УВКП – ЗРК на управлении от вышестоящего КП

        protokol126.sostZRK[1] = sostZRK_byte2;

        uint8_t sostZRK_byte3 = 0x00;

        sostZRK_byte3 += 0x80;  // 0x80 - мощность излучения 100%   0x00 - пониженная
        sostZRK_byte3 += 0x40;  // 0x40 - антенна  0x00 - эквивалент
        if(apyInfoM.remoteControlZRK == false) //if(shMemU2C.komKom.remoteControlZRK == false)
            sostZRK_byte3 += 0x20;  // 0x20 - ЗРК находится в режиме автономных боевых действий..., 0x00 - централиз. упр.

        sostZRK_byte3 += 0x02;  // количество исправных ракетных каналов, 0x02 - для отладки - всегда исправны 2 канала

        protokol126.sostZRK[2] = sostZRK_byte3;

        // I126/040         И Н Ф О Р М А Ц И Я   О   Р А К Е Т А Х

        memset(&protokol126.missileInfo, 0, 6);
        //  КР - общее количество ракет (боезапас) данного типа в ЗРК
        uint8_t missile_KR  = 0;//commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KR;
        //  КРП – количество ракет включенных на подготовку (ракеты, которые переводятся из  «готовность №2» в «готовность №1»).
        uint8_t missile_KRP = 0;//commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRP;
        //  КРГ 1 – количество ракет в готовности №1 (готовые к немедленному пуску);
        uint8_t missile_KRG1 = 0;//commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRG1;
        //  КРГ 2 – количество ракет в готовности №2 (будут готовы к немедленному пуску только после их включения на подготовку в гот.1 через 0,5 мин);
        uint8_t missile_KRG2 = 0;//commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRG2;

        for(int i = 0; i < MAXPY; i++)
        {
            for(int j = 0; j < MAXMISSILE; j++)
            {
                switch (startInfoM.launcherInfo[i].missileInfo[j].missileStatus)
                {
                    case (int) MissileStatus::GOTOVA: missile_KRG1 += 1; //commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRG1 += 1;
                        break;
                    case (int) MissileStatus::NAKAL: missile_KRG2 += 1; //commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRG2 += 1;
                        break;
                    case (int) MissileStatus::PEREHOD_ISKHODNOE_NAKAL: missile_KRP += 1;    //commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRP += 1;
                        break;
                    default:    break;
                }

                if(startInfoM.launcherInfo[i].missileInfo[j].missileStatus != MissileStatus::RAKETA_OTSYTSTVIET &&
                        startInfoM.launcherInfo[i].missileInfo[j].missileStatus != MissileStatus::NAVEDENIE  && startInfoM.launcherInfo[i].missileInfo[j].missileStatus != MissileStatus::NEISPRAVNA)
                        missile_KR += 1;    //commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KR += 1;
            }
        }

        protokol126.missileInfo[2] = (missile_KR << 4) + missile_KRP;
        protokol126.missileInfo[3] = (missile_KRG1 << 4) + missile_KRG2;        

        size_t size126 = 0x1B;// = 27 - по умолчанию цели нет  // 0x38 = 56 - когда есть;

        if(apyInfoM.rejimPoiskaSNR == RejimPoiskaSNR::POISK_PEREKLYCHEN_V_SOPROVOJDENIE)
        {
            size126 = 0x38; // СНР сопровождает цель, информация по ней будет выдаваться на КП
        }
        protokol126.messageLength = uint16_t_byte_reorder(size126);

        //  I126/050        С О С Т О Я Н И Е   Ц Е Л Е В О Г О   К А Н А Л А   1
        asterix_I126_050_Coder(protokol126.targetInfo1);

        //qDebug()<<"ProcessVKP: катег 126 Y:"<<shMemC2U.vkp2zrk.coordYHX.y_km<<"X:"<<shMemC2U.vkp2zrk.coordYHX.x_km;
//        qDebug()<<"ProcessVKP: катег 126 Y:"<<apyInfoM.nextTocka.groundY<<"X:"<<apyInfoM.nextTocka.groundX<<"H:"<<apyInfoM.nextTocka.seaLevelAltitude<<"vY:"<<vY<<"vX:"<<vX
//               <<"targetInfo1: "<< protokol126.targetInfo1[22] << protokol126.targetInfo1[23];

        //protokol126.targetInfo1[1] = 0;  // целевой канал свободен, номер канала 1, инф.о цели есть

        // О Т С Ы Л К А    1 2 6   К А Т Е Г О Р И И
        if(send(psockfd, &protokol126, size126, 0) < 0 )
            clientConnect(&psockfd, settings->port_VKP, settings->ipAddressVKP, "VKP: reconnect");

        uint8_t header125[4];
        memset(header125, 0, 4);

        // П О Д Г О Т О В К А   К   П Р И Ё М У   1 2 5   К А Т Е Г О Р И И
        vkp2zrkM.komandiPoCelevomyKanaly1.commandInfoExist = false;
        vkp2zrkM.komandiPoCelevomyKanaly1.targetInfoExist = false;

        //QString str125 = "str125: ";     //  7E 00 08 70 00 01 67 5E

        // П Р И Ё М   С Н А Ч А Л А   Т О Л Ь К О   З А Г О Л О В К А   1 2 5   К А Т Е Г О Р И И
        read(psockfd, &header125, 4);

        int sizeInfo125 = 256 * header125[1] + header125[2] - 4;

        vkp2zrkM.sizeInfo125 = 256 * header125[1] + header125[2] - 4;

        //qDebug()<<"ProcessVKP: size125:"<<sizeInfo125<<"FSPEC:"<<header125[3];

        if(header125[0] == 0x7D)// && sizeInfo125 == 0x0B)     //  0x7D - 125 категория
        {
            // П Р И Ё М   О С Т А Л Ь Н О Й   Ч А С Т И   1 2 5   К А Т Е Г О Р И И
            size_t numReadByte = read(psockfd, array125, sizeInfo125);

            if((int)numReadByte < 0)
            {
                clientConnect(&psockfd, settings->port_VKP, settings->ipAddressVKP, "VKP: reconnect");
                continue;
            }
            else
            {
                if((int)numReadByte == sizeInfo125)
                {
                    //0000   4c cc 6a 8e 40 df 64 d1 54 09 bd 61 08 00 45 00
                    //0010   00 53 68 a8 40 00 7e 06 40 d5 0a 00 57 b8 c0 a8
                    //0020   30 c7 42 69 af 22 8d b3 aa 2c b0 47 f0 cc 80 18
                    //0030   00 fb 0b e3 00 00 01 01 08 0a 00 23 42 00 00 40
                    //0040   d4 71 7d 00 1f c1 80 00 01 67 fc 13 00 03 00 00
                    //0050   02 e7 40 00 b4 0c 1b ff ff 08 90 ff 07 00 00 ff
                    //0060   70

                    //CAT   LEN   FSPEC1  FSPEC2  SAC SIC Nzrk
                    // 7d  00 1f   c1     80       00  01  67  fc 13 00 03 00 00
                                        //0050   02 e7 40 00 b4 0c 1b ff ff 08 90 ff 07 00 00 ff
                                        //0060   70

                    while(u < sizeInfo125)
                    {                        
                        vkp2zrkM.fspec1 = header125[3];

                        uint8_t fspec1 = //0b11111001;
                                header125[3];   // 1-й FSPEC

                        bool fspec2Exist = false;        // по умолчанию 2-й FSPEC отсутствует
                        //int u = 0;                       // u - индекс для перебора элементов массива 125 категории
                        uint8_t fspec2;

                        if((fspec1 & 0x01) == 0x01)        // 2-й FSPEC есть ?
                        {
                            fspec2Exist = true;           // есть
                            fspec2 = array125[u]; // 2-й FSPEC    //0b11111110;
                            vkp2zrkM.fspec2 = array125[u]; // 2-й FSPEC

                            //0b11001110;///////////////////
                            if((fspec2 & 0x01) == 0x01)  // учитывая, что их может быть сколько угодно, убеждаемся,
                                str125 += "обнаружен 3-й FSPEC, которого не должно быть ";         // что он последний

                            //while(array125[u] & 0x01 = 0x01 && u < sizeInfo125)
                            //    u++;

                            u += 1;
                        }

                        // расшифровка данных, определённая 1-м FSPEC

                        if((fspec1 & 0x80) == 0x80) // если есть SAC SIC (Идентификатор источника данных)
                        {
                           str125 += //"SAC:"<<array125[u] + " SIC:"<<array125[u + 1];

                           QString("SAC:%1[0x%2] SIC:%3[0x%4]")
                                   .arg(array125[u    ], 3, 10, QChar('0')).arg(array125[u    ], 2, 16, QChar('0'))
                                   .arg(array125[u + 1], 3, 10, QChar('0')).arg(array125[u + 1], 2, 16, QChar('0'));
//                                   .arg(array125[u + 2], 3, 10, QChar('0')).arg(array125[u + 2], 2, 16, QChar('0'));                           

                           vkp2zrkM.sac = array125[u    ];
                           vkp2zrkM.sic = array125[u + 1];

                           u += 2;
                        }

                        if((fspec1 & 0x40) == 0x40) //  если есть Номер ЗРК
                        {
                           str125 += QString(" NЗРК:%1[0x%2]").arg(array125[u], 3, 10, QChar('0')).arg(array125[u], 2, 16, QChar('0'));
                           vkp2zrkM.Nzrk = array125[u    ];

                           u += 1;
                        }

                        if((fspec1 & 0x20) == 0x20) //  если есть Сектор ответственности
                        {
                            if((array125[u] & 0b11) == 0b01)
                            {
                                vkp2zrkM.sectorOtvetstvennostiNaznachit = true;
                                vkp2zrkM.sectorOtvetstvennostiStart = 360.0 / 256 * array125[u + 1];
                                vkp2zrkM.sectorOtvetstvennostiEnd = 360.0 / 256 * array125[u + 2];

                                str125 += //<<360.0/ 256 * array125[u + 1]<<"..."<<360.0/ 256 * array125[u + 2];
                                QString(" назн.сектор отв:%1[0x%2]...%3[0x%4]")
                                        .arg(array125[u + 1], 3, 10, QChar('0')).arg(array125[u + 1], 2, 16, QChar('0'))
                                        .arg(array125[u + 2], 3, 10, QChar('0')).arg(array125[u + 2], 2, 16, QChar('0'));
                            }
                            else
                                if((array125[u] & 0b11) == 0b10)
                                {
                                    vkp2zrkM.sectorOtvetstvennostiNaznachit = false;
                                    str125 += " отм. сектор отв. ";
                                }
                                else
                                    str125 += QString(" команда: 0x%1 по сектор отв.не расп;").arg(array125[u], 2, 16, QChar('0'));
                           u += 3;
                        }

                        if((fspec1 & 0x10) == 0x10) //  если есть Команда на ЗРК
                        {
                            vkp2zrkM.komandaYprZRK030[0] = array125[u    ];
                            vkp2zrkM.komandaYprZRK030[1] = array125[u + 1];

                            str125 += "Команда b1" + strByte125(array125[u  ], "запр.изл", "М100%", "упрВКП", "#", "#", "#", "-", "-");// Байт 1

                            str125 += QString(" КК: 0b%1").arg(array125[u] & 0b1111, 4, 2, QChar('0'));

                            if((array125[u + 1] & 0x40) == 0x40)
                                str125 += " запр.изл.сек:1";
                            else
                                str125 += " запр.изл.сек:0";

                            str125 += QString(" РЦ: 0b%1").arg((array125[u] & 0b00111000)>>3, 3, 2, QChar('0'));

                           u += 2;
                        }

                        if((fspec1 & 0x08) == 0x08) //  если есть I125/040 Управление ракетами
                        {
                            memcpy(&vkp2zrkM.yprRaketami040, array125 + u, 6);

                            str125 += QString(" Упр.рак: 0x%1%2 %3%4 %5%6")
                                    .arg(array125[u  ], 2, 16, QChar('0')).arg(array125[u+1], 2, 16, QChar('0'))
                                    .arg(array125[u+2], 2, 16, QChar('0')).arg(array125[u+3], 2, 16, QChar('0'))
                                    .arg(array125[u+4], 2, 16, QChar('0')).arg(array125[u+5], 2, 16, QChar('0'));

                            uint8_t og0 = (array125[u  ] & 0x80) >> 7;
                            uint8_t kn0 = (array125[u  ] & 0b01110000) >> 4;
                            uint8_t kp0 = array125[u  ] & 0b1111;

                            uint8_t og1 = (array125[u + 1] & 0x80) >> 7;
                            uint8_t kn1 = (array125[u + 1] & 0b01110000) >> 4;
                            uint8_t kp1 = array125[u + 1] & 0b1111;

                            uint8_t og2 = (array125[u + 2] & 0x80) >> 7;
                            uint8_t kn2 = (array125[u + 2] & 0b01110000) >> 4;
                            uint8_t kp2 = array125[u + 2] & 0b1111;


                            uint8_t on0 = (array125[u + 3] & 0b01110000) >> 4;
                            uint8_t op0 = array125[u + 3] & 0b1111;

                            uint8_t on1 = (array125[u + 4] & 0b01110000) >> 4;
                            uint8_t op1 = array125[u + 4] & 0b1111;

                            uint8_t on2 = (array125[u + 5] & 0b01110000) >> 4;
                            uint8_t op2 = array125[u + 5] & 0b1111;

                            str125 += QString("27Д ОГ: %1  КРП_Н: %2  КРП_Р: %3  ОП_Н: %4  ОП_Р: %5").arg(og0).arg(kn0).arg(kp0).arg(on0).arg(op0);
                            str125 += QString("27ДМ1 ОГ: %1  КРП_Н: %2  КРП_Р: %3  ОП_Н: %4  ОП_Р: %5").arg(og1).arg(kn1).arg(kp1).arg(on1).arg(op1);
                            str125 += QString("27ДМ2 ОГ: %1  КРП_Н: %2  КРП_Р: %3  ОП_Н: %4  ОП_Р: %5").arg(og2).arg(kn2).arg(kp2).arg(on2).arg(op2);

                            u += 6;
                        }


                        if(fspec2Exist)
                        {
                            //str125 += " ^^^^^^^^^^^^ fspec2Exist = true ";

                            if((fspec2 & 0x80) == 0x80) //  если есть I125/050  Команды по целевому каналу 1
                            {
                                vkp2zrkM.komandiPoCelevomyKanaly1.commandInfoExist = true;

                                //astrx125_050 komandiPoCelevomyKanaly1;

                                if((array125[u + 1] & 0x01) == 0x01)
                                {
                                    vkp2zrkM.komandiPoCelevomyKanaly1.targetInfoExist = true;

                                    if((array125[u + 0] & 0x01) == 0x01)
                                        vkp2zrkM.komandiPoCelevomyKanaly1.ZPVD = true;
                                    else
                                        vkp2zrkM.komandiPoCelevomyKanaly1.ZPVD = false;

                                    if((array125[u + 0] & 0x02) == 0x02)
                                        vkp2zrkM.komandiPoCelevomyKanaly1.R_IS = true;
                                    else
                                        vkp2zrkM.komandiPoCelevomyKanaly1.R_IS = false;

                                    if((array125[u + 0] & 0x04) == 0x04)
                                        vkp2zrkM.komandiPoCelevomyKanaly1.R_ZRK = true;
                                    else
                                        vkp2zrkM.komandiPoCelevomyKanaly1.R_ZRK = false;

                                    if((array125[u + 1] & 0x10) == 0x10)
                                    {
                                        vkp2zrkM.komandiPoCelevomyKanaly1.CU = true;
                                        apyInfoM.rejimPoiskaSNR = RejimPoiskaSNR::POISK_PEREKLYCHEN_V_ZAHVAT;   //  2018-06-15
                                    }
                                    else
                                        vkp2zrkM.komandiPoCelevomyKanaly1.CU = false;

                                    if((array125[u + 1] & 0x20) == 0x20)
                                    {
                                        vkp2zrkM.komandiPoCelevomyKanaly1.OCU = true;
                                        apyInfoM.rejimPoiskaSNR = POISK_VIKLUCHEN;      //  2018-06-15
                                    }
                                    else
                                        vkp2zrkM.komandiPoCelevomyKanaly1.OCU = false;

                                    if((array125[u + 1] & 0x40) == 0x40)
                                        vkp2zrkM.komandiPoCelevomyKanaly1.ZP = true;
                                    else
                                        vkp2zrkM.komandiPoCelevomyKanaly1.ZP = false;

                                    if((array125[u + 1] & 0x80) == 0x80)
                                        vkp2zrkM.komandiPoCelevomyKanaly1.ZPVS = true;
                                    else
                                        vkp2zrkM.komandiPoCelevomyKanaly1.ZPVS = false;

                                    vkp2zrkM.komandiPoCelevomyKanaly1.NceliKP = 256 * array125[u + 3] + array125[u + 2];

                                    memcpy(&vkp2zrkM.header050Chanal1, array125 + u, 11);

                                    COORD_YHX parsYHX = coordYHXParser125(u+11);

                                    memcpy(&vkp2zrkM.coordYHX, &parsYHX, sizeof(COORD_YHX));
                                }
                                else
                                {
                                    memcpy(&vkp2zrkM.header050Chanal1, array125 + u, 2);
                                }

                                //float azCU_grad = atan2(shMemC2U.vkp2zrk.coordYHX.x_km, shMemC2U.vkp2zrk.coordYHX.y_km) * Rad2Grad;

//                                qDebug()<<"*** *** *** ProcessVKP:"<<"X:"<<shMemC2U.vkp2zrk.coordYHX.x_km
//                                       <<"Y:"<<shMemC2U.vkp2zrk.coordYHX.y_km<<"azCU_grad:"<<azCU_grad;


                                strKomandiCelevojKanal125();

                                //qDebug()<<"((((((((((((((((((((((((((((((((((((((coordYHXParser125:"<<shMemC2U.vkp2zrk.coordYHX.y_km<<retYHX.y_km<<;

                                //shMemC2U.vkp2zrk.coordYHX =
                                        //coordYHXParser125(u+11);

//                               qDebug()<<"############coordYHXParser125: Север"<<shMemC2U.vkp2zrk.coordYHX.y_km<<"Высота:"
//                                      <<shMemC2U.vkp2zrk.coordYHX.h_km<<"Восток:"<<shMemC2U.vkp2zrk.coordYHX.x_km<<"VСевер:"<<shMemC2U.vkp2zrk.coordYHX.vy_ms
//                                     <<"VВысота:"<<shMemC2U.vkp2zrk.coordYHX.vh_ms<<"VВосток:"<<shMemC2U.vkp2zrk.coordYHX.vx_ms;


//                                qDebug()<<"*** *** *** ProcessVKP: azCU_grad:"<<azCU_grad<<"currentAzimuth"<<srvDrvInfoM.currentAzimuth<<"deltaAz:"<<deltaAz<<"cyStatus:"
//                                       <<srvDrvInfoM.cyStatus<<"targetInfoExist:"<<shMemC2U.commander2Guidance.vkp2ZRK.komandiPoCelevomyKanaly1.targetInfoExist
//                                      <<"coord:"<<shMemC2U.commander2Guidance.vkp2ZRK.coordYHX.y_km<<shMemC2U.commander2Guidance.vkp2ZRK.coordYHX.x_km;

                            }
                            if((fspec2 & 0x40) == 0x40) //  если есть I125/060  Команды по целевому каналу 2
                                strKomandiCelevojKanal125();
                            if((fspec2 & 0x20) == 0x20) //  если есть I125/070  Команды по целевому каналу 3
                                strKomandiCelevojKanal125();
                            if((fspec2 & 0x10) == 0x10) //  если есть I125/080  Команды по целевому каналу 4
                                strKomandiCelevojKanal125();

                            if((fspec2 & 0x08) == 0x08) //  если есть I125/091   Цель №1 комплексного тренажа
                            {
                                str125 += "*** Цель №1 комплексного тренажа ***   ";

                                COORD_YHX parsYHX = coordYHXParser125(u + 2);
                                memcpy(&vkp2zrkM.cmplxYHX[0], &parsYHX, sizeof(COORD_YHX));
                                vkp2zrkM.cmplxTrgtN[0] = 256 * array125[u + 0] + array125[u + 1];

                                strCelKomplTrenaja125();
                                u += 14;
                            }
                            if((fspec2 & 0x04) == 0x04) //  если есть I125/092   Цель №2 комплексного тренажа
                            {
                                str125 += "*** Цель №2 комплексного тренажа ***   ";

                                COORD_YHX parsYHX = coordYHXParser125(u + 2);
                                memcpy(&vkp2zrkM.cmplxYHX[1], &parsYHX, sizeof(COORD_YHX));
                                vkp2zrkM.cmplxTrgtN[1] = 256 * array125[u + 0] + array125[u + 1];

                                strCelKomplTrenaja125();
                                u += 14;
                            }
                            if((fspec2 & 0x02) == 0x02) //  если есть I125/093   Цель №3 комплексного тренажа
                            {
                                str125 += "*** Цель №3 комплексного тренажа ***   ";

                                COORD_YHX parsYHX = coordYHXParser125(u + 2);
                                memcpy(&vkp2zrkM.cmplxYHX[2], &parsYHX, sizeof(COORD_YHX));
                                vkp2zrkM.cmplxTrgtN[2] = 256 * array125[u + 0] + array125[u + 1];

                                strCelKomplTrenaja125();
                                u += 14;
                            }

                            str125 += "************** u:" + QString::number(u);
                        }
                    }   // end while(u < sizeInfo125)

//                        shMemC2U.vkp2zrk.sectorOtvetstvennostiNaznachit = false;
//                        if(protokol125.sectorOtvetstvennostiNaznachit == 0x01)
//                            shMemC2U.vkp2zrk.sectorOtvetstvennostiNaznachit = true;

//                        shMemC2U.vkp2zrk.sectorOtvetstvennostiStart = 360.0/ 256 * protokol125.sectorOtvetstvennostiStart;
//                        shMemC2U.vkp2zrk.sectorOtvetstvennostiEnd = 360.0/ 256 * protokol125.sectorOtvetstvennostiEnd;

                       //assignArtSimulationTargetParams();

                }   // end if(numReadByte == sizeInfo125)
            }
        }   // end if(header125[0] == 0x7D)   //  0x7D - 125 категория

        /////////////////////////////////////////////qDebug()<<"ProcessVKP: ";
        /////////////////////////////////////////////qDebug()<<str125;

        //float azCU_grad = atan2(vkp2zrkG.coordYHX.x_km, vkp2zrkG.coordYHX.y_km) * Rad2Grad;
        //float deltaAz = azCU_grad - srvDrvInfoM.currentAzimuth;

        // П Р Е О Б Р А З О В А Н И Е   П А Р А М Е Т Р О В   Ц Е Л И   И З   И С К Р Ы   В   nextTocka
        converterIskra2NextTocka();

//        qDebug()<<"*** *** *** ProcessVKP: azCU_grad:"<<azCU_grad<<"currentAzimuth"<<srvDrvInfoM.currentAzimuth<<"deltaAz:"<<deltaAz<<"cyStatus:"
//               <<srvDrvInfoM.cyStatus<<"targetInfoExist:"<<shMemC2U.vkp2zrk.komandiPoCelevomyKanaly1.targetInfoExist
//              <<"coord:"<<shMemC2U.vkp2zrk.coordYHX.y_km<<shMemC2U.vkp2zrk.coordYHX.x_km;

//        qDebug()<<"*** *** *** ProcessVKP: srvDrvInfo.cyStatus:"<<srvDrvInfoM.cyStatus
//               <<"CU"<<shMemC2U.vkp2zrk.komandiPoCelevomyKanaly1.CU<<"OCU"<<shMemC2U.vkp2zrk.komandiPoCelevomyKanaly1.OCU
//               <<"targetInfoExist:"<<shMemC2U.vkp2zrk.komandiPoCelevomyKanaly1.targetInfoExist
//        <<"coord vkp2zrk: ЦУ"<<shMemC2U.vkp2zrk.coordYHX.y_km<<shMemC2U.vkp2zrk.coordYHX.x_km
//        <<"1:"<<shMemC2U.vkp2zrk.cmplxYHX[0].y_km<<shMemC2U.vkp2zrk.cmplxYHX[0].x_km
//        <<"2:"<<shMemC2U.vkp2zrk.cmplxYHX[1].y_km<<shMemC2U.vkp2zrk.cmplxYHX[1].x_km
//        <<"3:"<<shMemC2U.vkp2zrk.cmplxYHX[2].y_km<<shMemC2U.vkp2zrk.cmplxYHX[2].x_km;

    }   // end while (!clear_op)

    CloseSocket(psockfd);
    return NULL;
}
