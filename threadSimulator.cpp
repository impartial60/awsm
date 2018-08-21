#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "externM.h"


UV11ANGLES  operatorPovorota(float b, float e)
{
    UV11ANGLES angls;

    angls.deltaF[1] = b * SIN45COS45 - e * SIN45COS45;
    angls.deltaF[0] = b * SIN45COS45 + e * SIN45COS45;

    return angls;
}

//int cntr8 = 0;

void assignGradysnikParams(int ii)
{
    // G R A D Y S N I K   G R A D Y S N I K   G R A D Y S N I K   G R A D Y S N I K   G R A D Y S N I K   G R A D Y S N I K   G R A D Y S N I K
    //azimuth_grad = prm.azimuth360;//ar[4];
    //height_km = prm.radarRelativeAltitude;//ar[6];  //временно
    /////////////////////////////////////////////////////////////////////radialSpeed_ms = prm.radialSpeed;//ar[7];
    //rangeHorizont_km = prm.groundRange_km;// ar[8];
    //kyrs_grad = prm.kyrs;// ar[10];
    //rakyrs_grad = prm.rakyrs;// ar[11];
    //////////////////////////////////////////////////////////////////////groundSpeed_km_s = 0.001 * prm.groundSpeed;//0.001 * ar[12];
    //elevationAngle_grad = prm.elevationAngle;// ar[13];

    // Т Е К У Щ И Й   К У Р С О В О Й   П А Р А М Е Т Р
    float deltaPrm = apyInfoM.nextTocka[ii].rakyrs / Rad2Grad;
    apyInfoM.nextTocka[ii].kyrsovojParameter_km = apyInfoM.nextTocka[ii].groundRange_km * sin(deltaPrm);
    apyInfoM.nextTocka[ii].rangeParametr_km = sqrt(apyInfoM.nextTocka[ii].groundRange_km * apyInfoM.nextTocka[ii].groundRange_km
                                                           - apyInfoM.nextTocka[ii].kyrsovojParameter_km * apyInfoM.nextTocka[ii].kyrsovojParameter_km);
    ZonaPorazhRanges zpr = ZP::glybina_ZP(apyInfoM.nextTocka[ii].kyrsovojParameter_km, apyInfoM.nextTocka[ii].radarRelativeAltitude);
    apyInfoM.nextTocka[ii].bGZoniPorParam_km = zpr.minValue_km;
    apyInfoM.nextTocka[ii].dGZoniPorParam_km = zpr.maxValue_km;
    apyInfoM.nextTocka[ii].maxKyrsovojParameter_km = zpr.maxParams;

    float dgzPor_abs = sqrt(zpr.maxValue_km * zpr.maxValue_km + apyInfoM.nextTocka[ii].kyrsovojParameter_km * apyInfoM.nextTocka[ii].kyrsovojParameter_km);
    float tPoliotaRaketiDGZPor = dgzPor_abs / VRAKETI;
    float groundSpeed_km_s = apyInfoM.nextTocka[ii].groundSpeed_ms * 1.0e-3;

    apyInfoM.nextTocka[ii].dGZoniPyskaParametr_km = zpr.maxValue_km + fabs(groundSpeed_km_s) * tPoliotaRaketiDGZPor;

//    qDebug()<<"assignGradysnikParams: zpr->maxValue_km:"<<zpr->maxValue_km<<"kyrsovojParameter_km:"<<apyInfoM.nextTocka[ii].kyrsovojParameter_km
//           <<"tPoliotaRaketiDGZPor:"<<tPoliotaRaketiDGZPor<<"dGZoniPyskaParametr_km:"<<apyInfoM.nextTocka[ii].dGZoniPyskaParametr_km;

    float a = groundSpeed_km_s * groundSpeed_km_s - VRAKETI * VRAKETI;

    float b =  - 2.0 * fabs(groundSpeed_km_s) * apyInfoM.nextTocka[ii].rangeParametr_km;

    float c = apyInfoM.nextTocka[ii].groundRange_km * apyInfoM.nextTocka[ii].groundRange_km;

    float discr = sqrt(b * b - 4 * a * c);

    //float x1 = (- b + discr)  / 2.0 / a;
    float x2 = (- b - discr)  / 2.0 / a;

    apyInfoM.nextTocka[ii].rangeParametrTV_km = apyInfoM.nextTocka[ii].rangeParametr_km - x2 * groundSpeed_km_s;

    if(groundSpeed_km_s != 0.0)
    {
        //apyInfoM.nextTocka[ii].podletnoeVrema = (apyInfoM.nextTocka[ii].rangeParametr_km / groundSpeed_km_s;
        apyInfoM.nextTocka[ii].podletnoeVrema = (apyInfoM.nextTocka[ii].rangeParametr_km - apyInfoM.nextTocka[ii].dGZoniPyskaParametr_km) / groundSpeed_km_s;
    }
    //qDebug()<<"assignGradysnikParams ii:"<<ii<<"rangeParametr_km"<<apyInfoM.nextTocka[ii].rangeParametr_km<<"groundSpeed_km_s:"<<groundSpeed_km_s<<"podletnoeVrema:"<<apyInfoM.nextTocka[ii].podletnoeVrema;

    //delete zpr;
}

void assignKlmSimulationTargetParams(KlmSimulatorData klmSimulatorData)
{
    int tempTotalSimTargetNum = klmSimulatorData.targetTotalNumber;    

    if(klmSimulatorData.targetTotalNumber > MAXTOCKANUM)
    {
        apyInfoM.totalSimTargetNum = MAXTOCKANUM;
        tempTotalSimTargetNum = MAXTOCKANUM;
    }
    else
    {
        apyInfoM.totalSimTargetNum = klmSimulatorData.targetTotalNumber;
    }

    //apyInfoM.klmControlByte = klmSimulatorData.controlByte;
    apyInfoM.showSimulationTrack = ((klmSimulatorData.controlByte & 0b1) == 0b1);
    //apyInfoM.showSimulationTarget = ((klmSimulatorData.controlByte & 0b01) == 0b01);
    apyInfoM.showSimulationTarget = ((klmSimulatorData.controlByte & 0b10) == 0b10);

    apyInfoM.klmTransactionId = klmSimulatorData.transactionId;    

    for(int j = 0; j < tempTotalSimTargetNum; j++)                                          //  перебор нового списка плотов от АРМа рук.трен.
    {
        bool nextTochkaIsMatched = false;
        int matchedIndex = -1;

        for(int i = 0; i < MAXTOCKANUM; i++)
        {
            if(apyInfoM.nextTocka[i].sourceId == SOURCEID::ARMTREN)                         //  совпадает источник информации
            {
                if(apyInfoM.nextTocka[i].targetNo == klmSimulatorData.trgts[j].targetN)     //  совпадает номер цели в системе нумерации источника
                {
                    matchedIndex = i;
                    nextTochkaIsMatched = true;
                    break;
                }
            }
        }

        if(nextTochkaIsMatched == false)                //  не был найден совпадающий номер цели совпадающего источника
        {
            for(int i = 0; i < MAXTOCKANUM; i++)        //  ищем первый свободный
            {
                if(apyInfoM.nextTocka[i].sourceId == SOURCEID::NOSOURCE)
                {
                    matchedIndex = i;
                    nextTochkaIsMatched = true;
                    break;
                }
            }
        }

        if(nextTochkaIsMatched == false)                //  в списке nextTocka нет свободного места
        {
            int minElapsed = 1000000;

            for(int i = 0; i < MAXTOCKANUM; i++)        //  ищем самое старое сообщение
            {
                if(apyInfoM.nextTocka[i].secsSinceStartOfDay < minElapsed)
                {
                    minElapsed = apyInfoM.nextTocka[i].secsSinceStartOfDay;// monoTrackElapsedTime[i];
                    matchedIndex = i;
                }
            }
        }

        if(matchedIndex >= 0)
        {
            apyInfoM.nextTocka[matchedIndex].secsSinceStartOfDay = 0.001 * QTime::currentTime().msecsSinceStartOfDay();
            apyInfoM.nextTocka[matchedIndex].sourceId = SOURCEID::ARMTREN;
            apyInfoM.nextTocka[matchedIndex].targetNo = klmSimulatorData.trgts[j].targetN;

            float az180 = klmSimulatorData.trgts[j].azimuth;
            if(az180 > 180.0)
                az180 -= 360.0;
            apyInfoM.nextTocka[matchedIndex].kyrs = (float)klmSimulatorData.trgts[j].kyrs;

            apyInfoM.nextTocka[matchedIndex].rakyrs = 180.0 - klmSimulatorData.trgts[j].kyrs + az180;
            apyInfoM.nextTocka[matchedIndex].groundSpeed_ms = klmSimulatorData.trgts[j].speed;
            apyInfoM.nextTocka[matchedIndex].radialSpeed_ms = apyInfoM.nextTocka[j].groundSpeed_ms * cos(apyInfoM.nextTocka[j].rakyrs / Rad2Grad);
            apyInfoM.nextTocka[matchedIndex].azimuth360 = klmSimulatorData.trgts[j].azimuth;

            apyInfoM.nextTocka[matchedIndex].seaLevelAltitude = klmSimulatorData.trgts[j].height;
            apyInfoM.nextTocka[matchedIndex].radarRelativeAltitude = klmSimulatorData.trgts[j].height;

            apyInfoM.nextTocka[matchedIndex].groundRange_km = klmSimulatorData.trgts[j].distance;
            apyInfoM.nextTocka[matchedIndex].slantRange_km = sqrt(apyInfoM.nextTocka[j].groundRange_km * apyInfoM.nextTocka[j].groundRange_km +
                apyInfoM.nextTocka[matchedIndex].radarRelativeAltitude * apyInfoM.nextTocka[j].radarRelativeAltitude);   //  временно для компиляции !!!

            apyInfoM.nextTocka[matchedIndex].groundX = apyInfoM.nextTocka[j].groundRange_km * sin(az180 / Rad2Grad);
            apyInfoM.nextTocka[matchedIndex].groundY = apyInfoM.nextTocka[j].groundRange_km * cos(az180 / Rad2Grad);

        //        qDebug()<<"targetSimulation: i:"<<i<<"apyInfoM.nextTocka.groundX:"<<apyInfoM.nextTocka[i].groundX<<"Y:"<<apyInfoM.nextTocka[i].groundY
        //                   <<"apyInfoM.nextTocka.azimuth360:"<<apyInfoM.nextTocka[i].azimuth360<<"az180:"<<az180;
    //        qDebug()<<"assignKlmSimulationTargetParams: i:"<<i
    //                   //<<"apyInfoM.nextTocka.groundX:"<<apyInfoM.nextTocka[i].groundX<<"Y:"<<apyInfoM.nextTocka[i].groundY
    //                  <<"apyInfoM.nextTocka.groundRange_km:"<<apyInfoM.nextTocka[i].groundRange_km
    //                   <<"apyInfoM.nextTocka.azimuth360:"<<apyInfoM.nextTocka[i].azimuth360;

            //  м е т о д   В О Р О Н И Н А
            float deltaH = (- R_ZemliEquiv + sqrt(R_ZemliEquiv * R_ZemliEquiv + apyInfoM.nextTocka[j].slantRange_km * apyInfoM.nextTocka[j].slantRange_km));

            float srednijUgol = 5.0;
            if(apyInfoM.nextTocka[matchedIndex].slantRange_km != 0)
            {
                float asv = (apyInfoM.nextTocka[matchedIndex].seaLevelAltitude - deltaH) / apyInfoM.nextTocka[j].slantRange_km;
                    srednijUgol = asin(asv) * Rad2Grad;
            }

            apyInfoM.nextTocka[matchedIndex].elevationAngle = srednijUgol;

            apyInfoM.nextTocka[matchedIndex].uv11Angles = operatorPovorota(apyInfoM.nextTocka[j].azimuth360 - srvDrvInfoM.currentAzimuth, apyInfoM.nextTocka[j].elevationAngle - srvDrvInfoM.currentDirectrisaElevationAngle);

            //qDebug()<<"assignArtSimulationTargetParams: apyInfoM.nextTocka.azimuth360:"<<apyInfoM.nextTocka.azimuth360<<"srvDrvInfoM.currentAzimuth:"<<srvDrvInfoM.currentAzimuth;
            //apyInfoM.nextTocka.izmerTime = 0.001 * programmStartTime.elapsed();
            //srvDrvInfoM.targetSimulationOn = shMemU2C.userCmds.startPointParams.targetSimulationOn;

            assignGradysnikParams(j);
        }
    }
}

//float dAz = 0.0;
//int trd = 0;

bool simulatarSimulatar = true;

void * ProcessSimulator(void * temp_pt )
{
    (void) temp_pt;
    struct sockaddr_in udpSockAddr;
    int udpClientSocketFD;

    KlmSimulatorData klmSimulatorData;

    float groundX[2];       //  на восток
    float groundY[2];       //  на север

    if(simulatarSimulatar)//  симулятор симулятора
    {
        klmSimulatorData.controlByte = 0b10;    //  0b10 - только текущее положение цели, 0b11 - трек полностью
        klmSimulatorData.transactionId = 0;
        klmSimulatorData.messageLength = 60;
        klmSimulatorData.targetTotalNumber = 2;

        klmSimulatorData.trgts[0].kyrs = 291;
        klmSimulatorData.trgts[0].speed = 222;
        klmSimulatorData.trgts[0].targetN = 1;
        klmSimulatorData.trgts[0].azimuth = 131;
        klmSimulatorData.trgts[0].height = 3.21;
        klmSimulatorData.trgts[0].distance = 72;

        klmSimulatorData.trgts[1].kyrs = 291;
        klmSimulatorData.trgts[1].speed = 222;
        klmSimulatorData.trgts[1].targetN = 2;
        klmSimulatorData.trgts[1].azimuth = 135;
        klmSimulatorData.trgts[1].height = 5.43;
        klmSimulatorData.trgts[1].distance = 70;

        float az180 = klmSimulatorData.trgts[0].azimuth;
        if(az180 > 180.0)
            az180 -= 360.0;

        groundX[0] = klmSimulatorData.trgts[0].distance * sin(az180 / Rad2Grad);
        groundY[0] = klmSimulatorData.trgts[0].distance * cos(az180 / Rad2Grad);

        az180 = klmSimulatorData.trgts[1].azimuth;
        if(az180 > 180.0)
            az180 -= 360.0;

        groundX[1] = klmSimulatorData.trgts[1].distance * sin(az180 / Rad2Grad);
        groundY[1] = klmSimulatorData.trgts[1].distance * cos(az180 / Rad2Grad);
    }

//    struct sockaddr_in from;
//    socklen_t fromlen = sizeof(struct sockaddr_in);

    //int counter = 0;

    // ИНИЦИАЛИЗАЦИЯ КЛИЕНТСКОГО UDP-СОКЕТА
    socklen_t udpSockAddrLength = sizeof(udpSockAddr);
    if ((udpClientSocketFD=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        qDebug()<<"ProcessSimulator UDP Client Socket failed "<<settings->ipAddressSimulator<<":"<<settings->port_Simulator;//<<"connectStatus:"<<connectStatus;
    }

    udpSockAddr.sin_family = AF_INET;
    udpSockAddr.sin_port = htons(settings->port_Simulator);
    udpSockAddr.sin_addr.s_addr = inet_addr(settings->ipAddressSimulator);  //"127.0.0.1");//

//    bind(udpClientSocketFD,(struct sockaddr *)&udpSockAddr, udpSockAddrLength);
//    static int so_broadcast = 1;
//    setsockopt(udpClientSocketFD, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof so_broadcast);
//    struct timeval tv;
//    tv.tv_sec = 1;
//    setsockopt(udpClientSocketFD, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));

    fcntl(udpClientSocketFD, F_SETFL, O_NONBLOCK);

    qDebug()<<"ProcessSimulator UDP Client "<<settings->ipAddressSimulator<<":"<<settings->port_Simulator;//<<"connectStatus:"<<connectStatus;

    ZRK2SimulatorData zrk2SimulatorData;
    //enum ZRKOPERATIONS {NET_DEISTVIJ, POISK, SOPROVOJDENIE_V_OBZORE, SOPROVOJDENIE, NAVEDENIE, VSTRECHA};

    while (threadStatus->thrStatus_Simulator != cancel)
    {
        usleep(35 * 1000);//usleep(80 * 1000);

        if(simulatarSimulatar)//  симулятор симулятора
        {
            klmSimulatorData.transactionId++;            

            //klmSimulatorData.trgts[0].azimuth += dAz0;
            //klmSimulatorData.trgts[0].distance -= 0.035 * 0.001 * klmSimulatorData.trgts[0].speed;

            //klmSimulatorData.trgts[1].azimuth += dAz1;
            //klmSimulatorData.trgts[1].distance -=  0.035 * 0.001 * klmSimulatorData.trgts[1].speed;//dR1;

            for(int i = 0; i < 2; i++)
            {
                klmSimulatorData.trgts[i].kyrs += 0.005;

                float kyrs180 = klmSimulatorData.trgts[i].kyrs;
                if(kyrs180 > 180.0)
                    kyrs180 -= 360.0;

                groundX[i] += 0.04 * 0.001 * klmSimulatorData.trgts[i].speed * sin(kyrs180 / Rad2Grad);
                groundY[i] += 0.04 * 0.001 * klmSimulatorData.trgts[i].speed * cos(kyrs180 / Rad2Grad);

                klmSimulatorData.trgts[i].distance = sqrt(groundX[i] * groundX[i] + groundY[i] * groundY[i]);

                float az180 = atan2(groundX[i], groundY[i]) * Rad2Grad;

                if(az180 < 0)
                    az180 += 360.0;

                klmSimulatorData.trgts[i].azimuth = az180;
            }

            pthread_mutex_lock(&nextTockaMutex);
                assignKlmSimulationTargetParams(klmSimulatorData);
            pthread_mutex_unlock(&nextTockaMutex);

            //qDebug()<<"ProcessSimulator: transactionId:"<<klmSimulatorData.transactionId<<"distance:"<<klmSimulatorData.trgts[0].distance<<klmSimulatorData.trgts[1].distance;

            ///////////////////////////////////////continue;
        }// конец симулятор симулятора

        // Ф О Р М И Р О В А Н И Е   С О О Б Щ Е Н И Й   О   Д Е Й С Т В И Я Х   П О   Ц Е Л Я М
        zrk2SimulatorData.transactionId++;

        // это просто контрольное сообщение
        zrk2SimulatorData.messageLength = 10;
        zrk2SimulatorData.targetTotalNumber = 1;
        zrk2SimulatorData.zrkTrgtOptions[0].targetN = 0;
        zrk2SimulatorData.zrkTrgtOptions[0].options = ZRKOPERATIONS::NET_DEISTVIJ;

        int vsegoObstrelCelej = 0;
        uint8_t spisokNomerovObstrelCelej[16];

        memset(&spisokNomerovObstrelCelej, 0, 16);

        for(int j = 0; j < MAXPY; j++)
        {
            for(int i = 0; i < MAXMISSILE; i++)
            {
                bool targetIsNew = true;
                int uIndex = -1;
                for(int u = 0; u < vsegoObstrelCelej; u++)
                {
                    if(spisokNomerovObstrelCelej[u] != 0 && startInfoM.launcherInfo[j].missileInfo[i].targetNoKLM != 0 &&
                       spisokNomerovObstrelCelej[u] == startInfoM.launcherInfo[j].missileInfo[i].targetNoKLM)
                    {
                        targetIsNew = false;
                        uIndex = u;
                        break;
                    }
                }

                if(targetIsNew && startInfoM.launcherInfo[j].missileInfo[i].targetNoKLM != 0)
                {
                    uIndex = vsegoObstrelCelej;
                    zrk2SimulatorData.zrkTrgtOptions[uIndex].targetN = startInfoM.launcherInfo[j].missileInfo[i].targetNoKLM;
                    spisokNomerovObstrelCelej[uIndex] = zrk2SimulatorData.zrkTrgtOptions[uIndex].targetN;//startInfoG.launcherInfo[j].missileInfo[i].targetNoKLM;
                    vsegoObstrelCelej++;
                }

                if(uIndex > -1)
                {
                    if(startInfoM.launcherInfo[j].missileInfo[i].missileStatus == MissileStatus::VSTRECHA)
                    {
                        zrk2SimulatorData.zrkTrgtOptions[uIndex].options = ZRKOPERATIONS::VSTRECHA;
                        break;
                    }
                    else
                        if(startInfoM.launcherInfo[j].missileInfo[i].missileStatus == MissileStatus::NAVEDENIE)
                            zrk2SimulatorData.zrkTrgtOptions[uIndex].options = ZRKOPERATIONS::NAVEDENIE;
                        else
                            zrk2SimulatorData.zrkTrgtOptions[uIndex].options = ZRKOPERATIONS::NET_DEISTVIJ;
                }
            }
        }

        if(vsegoObstrelCelej > 0)
        {
            zrk2SimulatorData.messageLength = 8 + 2 * vsegoObstrelCelej;
            zrk2SimulatorData.targetTotalNumber = vsegoObstrelCelej;
        }

        //  2018-06-20  if(zrk2SimulatorData.transactionId % 10 == 0)
        //  2018-06-20      qDebug()<<"ProcessSimulator: transactionId:"<<zrk2SimulatorData.transactionId<<"messageLength:"<<zrk2SimulatorData.messageLength
        //  2018-06-20              <<"targetTotalNumber:"<<zrk2SimulatorData.targetTotalNumber
        //  2018-06-20              <<"targetN"<<zrk2SimulatorData.zrkTrgtOptions[0].targetN<<zrk2SimulatorData.zrkTrgtOptions[0].options
        //  2018-06-20              <<"targetN"<<zrk2SimulatorData.zrkTrgtOptions[1].targetN<<zrk2SimulatorData.zrkTrgtOptions[1].options;

        // О Т П Р А В К А   Н А   А Р М   Р Т   С О О Б Щ Е Н И Й   О   Д Е Й С Т В И Я Х   П О   Ц Е Л Я М
        sendto(udpClientSocketFD, &zrk2SimulatorData, zrk2SimulatorData.messageLength, 0, (struct sockaddr *) &udpSockAddr, udpSockAddrLength);

        ssize_t totalReadBytes = 0;
        ssize_t nextReadBytes;
        int readCirclCounter = 0;
        uint16_t messageLength = 0;

        if(!simulatarSimulatar)
            memset(&klmSimulatorData, 0, sizeof(KlmSimulatorData));

        char readByteArray[sizeof(KlmSimulatorData)];        

        while (totalReadBytes < abs(sizeof(KlmSimulatorData)) && readCirclCounter++ < 10)        
        {
            nextReadBytes = recvfrom(udpClientSocketFD, readByteArray + totalReadBytes, sizeof(KlmSimulatorData) - totalReadBytes, 0, (struct sockaddr *) &udpSockAddr, &udpSockAddrLength);            

            if(nextReadBytes < 0)
                nextReadBytes = 0;

            totalReadBytes += nextReadBytes;

            if(totalReadBytes >= 8)
            {
                if(simulatarSimulatar)              //  если работает настоящий АРМ руководителя тренировки
                {                                   //  симулятор симулятора больше не нужен,
                    simulatarSimulatar = false;     //  а чтобы вернуть симулятор симулятора
                }                                   //  надо выключить-включить режим тренажа на АРМе командира

                memcpy(&messageLength, readByteArray + sizeof(uint32_t), sizeof(uint16_t));

                if(totalReadBytes == messageLength)
                    break;
            }
        }

        if(messageLength >= 8 && totalReadBytes == messageLength)
        {
            memcpy(&klmSimulatorData, readByteArray, messageLength);

            pthread_mutex_lock(&nextTockaMutex);
                assignKlmSimulationTargetParams(klmSimulatorData);
            pthread_mutex_unlock(&nextTockaMutex);

//            qDebug()<<"Simulator Bytes:"<<totalReadBytes<<"mesLen:"<<messageLength
//                    <<"trId:"<<klmSimulatorData.transactionId<<"controlByte:"<<klmSimulatorData.controlByte
//                    <<"всего Ц:"<<klmSimulatorData.targetTotalNumber
//                    <<"R:"<<klmSimulatorData.trgts[0].distance<<klmSimulatorData.trgts[1].distance<<klmSimulatorData.trgts[2].distance<<klmSimulatorData.trgts[254].distance
//                    <<"Nц:"<<klmSimulatorData.trgts[0].targetN
//                    <<"az:"<<klmSimulatorData.trgts[0].azimuth
//                    <<"h:"<<klmSimulatorData.trgts[0].height
//                    <<"k:"<<klmSimulatorData.trgts[0].kyrs
//                    <<"V:"<<klmSimulatorData.trgts[0].speed
//                    //<<"jammingPower:"<<klmSimulatorData.trgts[0].jammingPower
//                    //<<"tip:"<<klmSimulatorData.trgts[0].tip
//                      ;
        }

//<<<<<<<<<<<<<<<<<<<<<<<   тестовая цель летающая по кругу для проверки РС   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//            srvDrvInfoM.maxTockaCurrentNum = 1;
//            srvDrvInfoM.trackPointsNumber = trd++;
//            apyInfoM.nextTocka[0].targetNo = 234;
//            apyInfoM.nextTocka[0].elevationAngle = 3.0;
//            dAz += 0.01;
//            apyInfoM.nextTocka[0].azimuth360 = 130.0 + dAz;
//            apyInfoM.nextTocka[0].uv11Angles = operatorPovorota(apyInfoM.nextTocka[0].azimuth360 - srvDrvInfoM.currentAzimuth,
//                                                                apyInfoM.nextTocka[0].elevationAngle - srvDrvInfoM.currentDirectrisaElevationAngle);
//            apyInfoM.nextTocka[0].slantRange_km = 54.3;
//            qDebug()<<"apyInfoM.nextTocka[0].uv11Angles:"<<apyInfoM.nextTocka[0].uv11Angles.deltaF[0]<<apyInfoM.nextTocka[0].uv11Angles.deltaF[1];

    }

    qDebug()<<"ProcessSimulator canceled";

    return NULL;
}

