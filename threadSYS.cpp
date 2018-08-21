#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "externM.h"

#pragma pack(push,1)
typedef  struct {
    u_int16_t   sid;
    u_int8_t    dlc;
    u_int8_t    data[8];
}MY_SID_CAN_MSG;

//typedef union
//{
//    u_int8_t all[352];
//    struct {
//        MY_SID_CAN_MSG txmsg1_ID044h;  // Mode LCS
//        MY_SID_CAN_MSG txmsg2_ID050h;  //Command START for reel 0
//        MY_SID_CAN_MSG txmsg3_ID051h;  //Command START for reel 1
//        MY_SID_CAN_MSG txmsg4_ID052h;  //Command START for reel 2
//        MY_SID_CAN_MSG txmsg5_ID053h;  //Command START for reel 3
//        MY_SID_CAN_MSG txmsg6_ID180h;  //Prepare for START for reel 0
//        MY_SID_CAN_MSG txmsg7_ID181h;  //Prepare for START for reel 1
//        MY_SID_CAN_MSG txmsg8_ID182h;  //Prepare for START for reel 2
//        MY_SID_CAN_MSG txmsg9_ID183h;  //Prepare for START for reel 3
//        MY_SID_CAN_MSG txmsg10_ID190h;  //Aimpoint Options reel 0
//        MY_SID_CAN_MSG txmsg11_ID191h;  //Aimpoint Options reel 1
//        MY_SID_CAN_MSG txmsg12_ID192h;  //Aimpoint Options reel 2
//        MY_SID_CAN_MSG txmsg13_ID193h;  //Aimpoint Options reel 3
//        MY_SID_CAN_MSG txmsg14_ID194h;  //Target speed for reel 0
//        MY_SID_CAN_MSG txmsg15_ID195h;  //Target speed for reel 1
//        MY_SID_CAN_MSG txmsg16_ID196h;  //Target speed for reel 2
//        MY_SID_CAN_MSG txmsg17_ID197h;  //Target speed for reel 3
//        MY_SID_CAN_MSG txmsg18_ID198h;  //Coordinates of the target reel 0
//        MY_SID_CAN_MSG txmsg19_ID199h;  //Coordinates of the target reel 1
//        MY_SID_CAN_MSG txmsg20_ID200h;  //Coordinates of the target reel 2
//        MY_SID_CAN_MSG txmsg21_ID201h;  //Coordinates of the target reel 4
//        MY_SID_CAN_MSG txmsg22_ID288h;  //Reels Angles
//        MY_SID_CAN_MSG txmsg23_ID290h;  //Coordinates of MLS
//        MY_SID_CAN_MSG txmsg24_ID291h;  //Angles for START not Allowed
//        MY_SID_CAN_MSG txmsg25_ID292h;  //Base Angle
//        MY_SID_CAN_MSG txmsg26;          //Reserv
//        MY_SID_CAN_MSG txmsg27;          //Reserv
//        MY_SID_CAN_MSG txmsg28;          //Reserv
//        MY_SID_CAN_MSG txmsg29;          //Reserv
//        MY_SID_CAN_MSG txmsg30;          //Reserv
//        MY_SID_CAN_MSG txmsg31;          //Reserv
//        MY_SID_CAN_MSG txmsg32;          //Reserv
//    };
//} SNR2SYS;//FROM_TCPIP_DATA;

//typedef union
//{
//    u_int8_t all[352];
//    struct {
//        MY_SID_CAN_MSG m044modeLCS;             // Mode LCS
//        MY_SID_CAN_MSG m05xStart[4];            // Command START for reel 0...3
//        MY_SID_CAN_MSG m18xPrep[4];             // Prepare for START for reel 0...3
//        MY_SID_CAN_MSG m19xOptn[4];             // Aimpoint Options reel 0...3
//        MY_SID_CAN_MSG m19xSpeed[4];            // Target speed for reel 0...3
//        MY_SID_CAN_MSG m19xCoord[4];            // Coordinates of the target reel 0
//        MY_SID_CAN_MSG m288angles;              // Reels Angles
//        MY_SID_CAN_MSG m290coordMLS;            // Coordinates of MLS
//        MY_SID_CAN_MSG m291restrictedAngles;    // Angles for START not Allowed
//        MY_SID_CAN_MSG m292baseAngle;           // Base Angle
//        MY_SID_CAN_MSG mRes[7];                 // Reserv
//    };
//} SNR2SYS;//FROM_TCPIP_DATA;

//typedef union
//{
//    u_int8_t all[88];
//    struct {
//        MY_SID_CAN_MSG rxmsg1_ID040h;  //Common Data
//        MY_SID_CAN_MSG rxmsg2_ID100h;  //M0 Data
//        MY_SID_CAN_MSG rxmsg3_ID101h;  //M1 Data
//        MY_SID_CAN_MSG rxmsg4_ID102h;  //M2 Data
//        MY_SID_CAN_MSG rxmsg5_ID103h;  //M3 Data
//        MY_SID_CAN_MSG rxmsg6;         //Reserv
//        MY_SID_CAN_MSG rxmsg7;         //Reserv
//        MY_SID_CAN_MSG rxmsg8;         //Reserv

//};
//} SYS2SNR;//TO_TCPIP_DATA;


union SNR2SYS
{
    u_int8_t all[352];
    struct {
        MY_SID_CAN_MSG m044modeLCS;             // Mode LCS
        MY_SID_CAN_MSG m05xStart[4];            // Command START for reel 0...3
        MY_SID_CAN_MSG m18xPrep[4];             // Prepare for START for reel 0...3
        MY_SID_CAN_MSG m19xOptn[4];             // Aimpoint Options reel 0...3
        MY_SID_CAN_MSG m19xSpeed[4];            // Target speed for reel 0...3
        MY_SID_CAN_MSG m19xCoord[4];            // Coordinates of the target reel 0...3
        MY_SID_CAN_MSG m20xSpherCoord[4];       // Spherical Coordinates of the target reel 0...3
        MY_SID_CAN_MSG m288angles;              // Reels Angles
        MY_SID_CAN_MSG m290coordMLS;            // Coordinates of MLS
        MY_SID_CAN_MSG m291restrictedAngles;    // Angles for START not Allowed
        MY_SID_CAN_MSG m292baseAngle;           // Base Angle
        MY_SID_CAN_MSG mRes[3];                 // Reserv
    };
}
snr2sys;//FROM_TCPIP_DATA;


union  SYS2SNR
{
    u_int8_t all[88];
    struct {
        MY_SID_CAN_MSG m040commonData;      //Common Data
        MY_SID_CAN_MSG m10xMissileInfo[4];  //M0...3 Data
        MY_SID_CAN_MSG mRes[2];             //Reserv
        MY_SID_CAN_MSG m501PowerSupplies;   //2018_06_22 Иванов прислал письмо

};
}sys2snr; //SYS2SNR;//TO_TCPIP_DATA;

#pragma pack(pop)

//SNR2SYS snr2sys;
//SYS2SNR sys2snr;

void snr2sysFixedValuesCoder()  //  П О С Т О Я Н Н А Я   Ч А С Т Ь
{
    ///////////////////////////////////////////////////////////////
    //  В Ы С О К О П Р И О Р И Т Е Т Н Ы Е   С О О Б Щ Е Н И Я  //
    ///////////////////////////////////////////////////////////////

    //  ID 0x040         У П Р А В Л Е Н И Е   П У С К О В О Й   У С Т А Н О В К О Й
    snr2sys.m044modeLCS.sid = 0x44;
    snr2sys.m044modeLCS.dlc = 1;
    snr2sys.m044modeLCS.data[0] = 0b10;   // всегда такое

    for(int i = 0; i < 4; i++)
    {
        //  ID 0x050 + N R   К О М А Н Д А   П У С К
        snr2sys.m05xStart[i].sid = 0x50 + i;
        snr2sys.m05xStart[i].dlc = 0x02;

        ///////////////////////////////////////////////////////
        //  В З А И М О Д Е Й С Т В И Е   С   Р А К Е Т О Й  //
        ///////////////////////////////////////////////////////

        //  ID 0x180 + N R   П О Д Г О Т О В К А   К   П У С К У
        snr2sys.m18xPrep[i].sid = 0x180 + i;
        snr2sys.m18xPrep[i].dlc = 0x01;

        //  ID 0x190 + N R   П О Л Е Т Н О Е   З А Д А Н И Е,   О П Ц И И   Н А В Е Д Е Н И Я
        snr2sys.m19xOptn[i].sid = 0x190 + i;
        snr2sys.m19xOptn[i].dlc = 0x06;

        //  ID 0x194 + N R   П О Л Е Т Н О Е   З А Д А Н И Е,   К О О Р Д И Н А Т Ы   Ц Е Л И   В   С К   А Н П
        snr2sys.m19xCoord[i].sid = 0x194 + i;
        snr2sys.m19xCoord[i].dlc = 0x08;

        //  ID 0x198 + N R   П О Л Е Т Н О Е   З А Д А Н И Е,   С К О Р О С Т И   Ц Е Л И   В   С К   А Н П
        snr2sys.m19xSpeed[i].sid = 0x198 + i;
        snr2sys.m19xSpeed[i].dlc = 0x08;

        //  ID 0x200 + N R   П О Л Е Т Н О Е   З А Д А Н И Е,   С Ф Е Р И Ч Е С К И Е   К О О Р Д И Н А Т Ы   Ц Е Л И   В   С К   А Н П
        snr2sys.m20xSpherCoord[i].sid = 0x200 + i;
        snr2sys.m20xSpherCoord[i].dlc = 0x08;
    }

    ///////////////////////////////////////////////////////////////////////////////
    //  В З А И М О Д Е Й С Т В И Е   С   П У С К О В О Й   У С Т А Н О В К О Й  //
    ///////////////////////////////////////////////////////////////////////////////

    //  ID 0x288         Т Е К У Щ И Е   У Г Л Ы   Н А П Р А В Л Я Ю Щ И Х   П У

    //  ID 0x290         К О О Р Д И Н А Т Ы   П У   В   С К   А Н П

    //  ID 0x291         У Г Л Ы   З А П Р Е Т А   С Т Р Е Л Ь Б Ы

    // ID 0x292     Б А З О В Ы Й   У Г О Л   А  Н  П  (антенного поста)   В   Г Е О Г Р А Ф И Ч Е С К О Й   С К
    snr2sys.m292baseAngle.sid = 0x292;
    snr2sys.m292baseAngle.dlc = 2;
}

//uint8_t komandaPYSK1prinata[4] = {0, 0, 0, 0};

void snr2sysVariableValuesCoder(int launcherIndex)  //  П Е Р Е М Е Н Н А Я   Ч А С Т Ь
{
    float az180 =  srvDrvInfoM.currentAzimuth;
    if(az180 > 180.0)
        az180 -= 360.0;
    int16_t psi = (az180 / 180.0 * 32768);

    for(int i = 0; i < 4; i++)
    {

        ///////////////////////////////////////////////////////////////
        //  В Ы С О К О П Р И О Р И Т Е Т Н Ы Е   С О О Б Щ Е Н И Я  //
        ///////////////////////////////////////////////////////////////

        //  ID 0x050 + N R   К О М А Н Д А   П У С К
        // ............................

        ///////////////////////////////////////////////////////
        //  В З А И М О Д Е Й С Т В И Е   С   Р А К Е Т О Й  //
        ///////////////////////////////////////////////////////

        //  ID 0x180 + N R   П О Д Г О Т О В К А   К   П У С К У

        //if(launcherUser2Console.launcherUserCommands[1].nakal_ButtonPressedCounter[0] >= 10) //


        //    if(oficerNaved2Console.guidanceOfficerCmds.writeDataContinues)
        //        snr2sys.txmsg6_ID180h.data[0] = 0x05;
        //    else
        //        snr2sys.txmsg6_ID180h.data[0] = 0x00;

        //qDebug()<<"launcherIndex:"<<launcherIndex<<"missileStatus:"<<startInfoG.launcherInfo[launcherIndex].missileInfo[i].missileStatus;

        uint8_t bitNakal = 0;
        uint8_t bitPodg = 0;
        uint8_t bitCelVZRP = 0b10000;  //  считаем, что цель всегда в зоне пуска - это проверка должна быть не здесь
        uint16_t kodPYSKA = 0;

        if(oficerPyska2Console.komandiPY[launcherIndex].ishdn_ButtonPressedCounter[i] == 0) // если не нажата кнопка "ИСХОДНОЕ"
        {
            if((oficerPyska2Console.komandiPY[launcherIndex].nakal_BtnPressedCounter[i] >= 10 &&
               startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::ISKHODNOE) ||
               startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::PEREHOD_ISKHODNOE_NAKAL ||
               startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::NAKAL ||
               startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::PEREHOD_NAKAL_GOTOVA ||
               startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::GOTOVA)
                bitNakal = 0b1;

            if((oficerPyska2Console.komandiPY[launcherIndex].podgt_BtnPressedCounter[i] >= 10 &&
               startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::NAKAL) ||
               startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::PEREHOD_NAKAL_GOTOVA ||
               startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::GOTOVA)
            {
                bitPodg = 0b100;

                if(oficerPyska2Console.komandiPY[launcherIndex].podgt_BtnPressedCounter[i] >= 10 &&                                         //  запуск "условного" прогресс-индикатора
                   startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::NAKAL)                             //  обратного отсчёта перехода НАКАЛ-ПОДГОТОВКА
                    startInfoM.launcherInfo[launcherIndex].missileInfo[i].time_vklychenie_podgotovka_milisec = programmStartTime.elapsed();
            }

            if(oficerPyska2Console.komandiPY[launcherIndex].pysk_BtnPressedCounter[i] >= 10 &&
               startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::GOTOVA)
            {
                uint8_t pyskPrinat = (sys2snr.m10xMissileInfo[i].data[1] & 0b1000000) >> 6;

                if(pyskPrinat)//komandaPYSK1prinata[i])         // доделать: если за 2 сек команда не принята - перейти в сотстояние MissileStatus::NEISPRAVNA
                    kodPYSKA = 0xA590 + i;
                else
                    kodPYSKA = 0x5A68 + i;

                //if(i == 2)
                    qDebug()<<"^^^ ^^^ ^^^ ^^^  kodPYSKA:"<<kodPYSKA%256<<kodPYSKA/256;
            }
        }

        memcpy(snr2sys.m05xStart[i].data, &kodPYSKA, sizeof(uint16_t));    //memcpy(snr2sys.txmsg2_ID050h.data, &kodPYSKA, sizeof(uint16_t));

        snr2sys.m18xPrep[i].data[0] = bitNakal + bitPodg + bitCelVZRP;    //snr2sys.txmsg6_ID180h.data[0] = bitNakal + bitPodg + bitCelVZRP;

        //qDebug()<<"oficerPyska2Console.komandiPY[1].ishdn_ButtonPressedCounter[i]:"<<oficerPyska2Console.komandiPY[1].ishdn_ButtonPressedCounter[i]<<snr2sys.m18xPrep[i].data[0];


        //  ID 0x190 + N R   П О Л Е Т Н О Е   З А Д А Н И Е,   О П Ц И И   Н А В Е Д Е Н И Я

        //  ID 0x194 + N R   П О Л Е Т Н О Е   З А Д А Н И Е,   К О О Р Д И Н А Т Ы   Ц Е Л И   В   С К   А Н П

        //  ID 0x198 + N R   П О Л Е Т Н О Е   З А Д А Н И Е,   С К О Р О С Т И   Ц Е Л И   В   С К   А Н П

        //  ID 0x200 + N R   П О Л Е Т Н О Е   З А Д А Н И Е,   С Ф Е Р И Ч Е С К И Е   К О О Р Д И Н А Т Ы   Ц Е Л И   В   С К   А Н П

        uint16_t time16 = 0xEEEE;//programmStartTime.elapsed();  //  только для теста

        snr2sys.m20xSpherCoord[i].data[0] = time16 / 256;
        snr2sys.m20xSpherCoord[i].data[1] = time16 % 256;

        memcpy(snr2sys.m20xSpherCoord[i].data + 2, &psi, sizeof(int16_t));          //  только для теста  - это азимут директрисы УНВ - вращаем УНВ - убеждаемся, что азимут передаётся корректно

        int16_t teta = (srvDrvInfoM.currentDirectrisaElevationAngle / 180.0 * 32768);
        memcpy(snr2sys.m20xSpherCoord[i].data + 4, &teta, sizeof(int16_t));         //  только для теста  - это угол места директрисы УНВ - качаем УНВ - убеждаемся, что угол места передаётся корректно

        uint16_t dalnost = (uint16_t)(131.07 / 0.002 * 65536);                      //  только для теста - 131.07 - максимально возможная дальность цели, км; 0.002 - ЦМР, км
        memcpy(snr2sys.m20xSpherCoord[i].data + 6, &dalnost, sizeof(uint16_t));     //  меняем дальность в коде - убеждаемся, что передаётся корректно
    }

    if(launcherIndex == 22222222)
    qDebug()<<"snr2sysVariableValuesCoder: missileStatus:"
           <<startInfoM.launcherInfo[launcherIndex].missileInfo[0].missileStatus
           <<startInfoM.launcherInfo[launcherIndex].missileInfo[1].missileStatus
           <<startInfoM.launcherInfo[launcherIndex].missileInfo[2].missileStatus
           <<startInfoM.launcherInfo[launcherIndex].missileInfo[3].missileStatus
           <<"pysk_BtnPressedCounter:"
           <<oficerPyska2Console.komandiPY[launcherIndex].pysk_BtnPressedCounter[0]
           <<oficerPyska2Console.komandiPY[launcherIndex].pysk_BtnPressedCounter[1]
           <<oficerPyska2Console.komandiPY[launcherIndex].pysk_BtnPressedCounter[2]
           <<oficerPyska2Console.komandiPY[launcherIndex].pysk_BtnPressedCounter[3]
          <<"kodPYSKA:"<<snr2sys.m05xStart[0].data<<snr2sys.m05xStart[1].data<<snr2sys.m05xStart[2].data<<snr2sys.m05xStart[3].data;

    ///////////////////////////////////////////////////////////////////////////////
    //  В З А И М О Д Е Й С Т В И Е   С   П У С К О В О Й   У С Т А Н О В К О Й  //
    ///////////////////////////////////////////////////////////////////////////////


    //  ID 0x288         Т Е К У Щ И Е   У Г Л Ы   Н А П Р А В Л Я Ю Щ И Х   П У

    //  ID 0x290         К О О Р Д И Н А Т Ы   П У   В   С К   А Н П

    //  ID 0x291         У Г Л Ы   З А П Р Е Т А   С Т Р Е Л Ь Б Ы

    // ID 0x292     Б А З О В Ы Й   У Г О Л   А  Н  П  (антенного поста)   В   Г Е О Г Р А Ф И Ч Е С К О Й   С К
    memcpy(snr2sys.m292baseAngle.data, &psi, sizeof(int16_t));//memcpy(snr2sys.txmsg25_ID292h.data, &psi, sizeof(int16_t));

}


void sys2snrParser(int launcherIndex)
{
    //qDebug()<<"ProcessSYS totalReadBytes sid:"<<totalReadBytes<<sys2snr.rxmsg1_ID040h.sid<<"dlc:"<<sys2snr.rxmsg1_ID040h.dlc<<"data:"<<sys2snr.rxmsg1_ID040h.data[0]<<sys2snr.rxmsg1_ID040h.data[1];

    for(int i = 0; i < 4; i++)
    {
        uint8_t priznakM1 = sys2snr.m10xMissileInfo[i].data[0] & 0b1;
        uint8_t priznakM2 = (sys2snr.m10xMissileInfo[i].data[0] & 0b10) >> 1;

        uint8_t pyIspr = (sys2snr.m10xMissileInfo[i].data[0] & 0b100) >> 2;

        uint8_t rakIspr = (sys2snr.m10xMissileInfo[i].data[0] & 0b1000) >> 3;
        uint8_t pitVkl = (sys2snr.m10xMissileInfo[i].data[0] & 0b10000) >> 4;
        uint8_t cB = (sys2snr.m10xMissileInfo[i].data[0] & 0b100000) >> 5;
        uint8_t shod = (sys2snr.m10xMissileInfo[i].data[0] & 0b1000000) >> 6;
        uint8_t neshod = (sys2snr.m10xMissileInfo[i].data[0] & 0b10000000) >> 7;

        if((priznakM1 == 1 || priznakM2 == 1) && cB == 1)
            startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus = MissileStatus::ISKHODNOE;
        else
        {
            if(startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::GOTOVA &&
                    oficerPyska2Console.komandiPY[launcherIndex].pysk_BtnPressedCounter[i] > 10)  //komandaPYSK1prinata == 1
            {
                if(shod)    //  единственный и окончательный признак успешного схода ракеты
                {
                    startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus = MissileStatus::NAVEDENIE;
                    qDebug()<<"SYS data: MissileStatus::NAVEDENIE";
                    startInfoM.launcherInfo[launcherIndex].missileInfo[i].time_start_navedenie_milisec = programmStartTime.elapsed();
                }
            }
            else
            {
                if(startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus == MissileStatus::NAVEDENIE)
                {
                    startInfoM.launcherInfo[launcherIndex].missileInfo[i].t_navedenie_sec =
                            0.001 * (programmStartTime.elapsed() - startInfoM.launcherInfo[launcherIndex].missileInfo[i].time_start_navedenie_milisec);

                    if(startInfoM.launcherInfo[launcherIndex].missileInfo[i].t_navedenie_sec > 70.0)
                        startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus = MissileStatus::RAKETA_OTSYTSTVIET;
                }
            }
        }

        uint8_t postypilaKomandaNakal = sys2snr.m10xMissileInfo[i].data[1] & 0b1;
        if(postypilaKomandaNakal)
            startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus = MissileStatus::PEREHOD_ISKHODNOE_NAKAL;
        uint8_t nakalOtrabotan = (sys2snr.m10xMissileInfo[i].data[1] & 0b10) >> 1;
        if(nakalOtrabotan)
            startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus = MissileStatus::NAKAL;
        uint8_t postypilaKomandaPodgotovka = (sys2snr.m10xMissileInfo[i].data[1] & 0b100) >> 2;
        uint8_t poliotZadPolycheno = (sys2snr.m10xMissileInfo[i].data[1] & 0b1000) >> 3;
        uint8_t sGP = (sys2snr.m10xMissileInfo[i].data[1] & 0b10000) >> 4;
        if(postypilaKomandaPodgotovka == 1 && sGP == 0)
            startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus = MissileStatus::PEREHOD_NAKAL_GOTOVA;
        else
            if(sGP)
                startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus = MissileStatus::GOTOVA;

        uint8_t sVRprinat = (sys2snr.m10xMissileInfo[i].data[1] & 0b100000) >> 5;
        uint8_t pyskPrinat = (sys2snr.m10xMissileInfo[i].data[1] & 0b1000000) >> 6;
        //komandaPYSK1prinata[i] = pyskPrinat;
        uint8_t pyskOtrabotan = (sys2snr.m10xMissileInfo[i].data[1] & 0b10000000) >> 7;

        //if(pyskPrinat == 1)
        qDebug()<<"SYS data: pyskPrinat:"<<pyskPrinat<<"pyskOtrabotan:"<<pyskOtrabotan<<"neshod:"<<neshod<<"shod:"<<shod;

        //if(pyskOtrabotan == 1)
            //qDebug()<<"SYS data: pyskOtrabotan";

        uint8_t rgsOk = sys2snr.m10xMissileInfo[i].data[2] & 0b1;
        uint8_t rgsP2polychena = (sys2snr.m10xMissileInfo[i].data[2] & 0b10) >> 1;
        uint8_t rgsYO = (sys2snr.m10xMissileInfo[i].data[2] & 0b100) >> 2;
        uint8_t rgs4P = (sys2snr.m10xMissileInfo[i].data[2] & 0b1000) >> 3;
        uint8_t rgsGG = (sys2snr.m10xMissileInfo[i].data[2] & 0b10000) >> 4;
        uint8_t as = (sys2snr.m10xMissileInfo[i].data[2] & 0b100000) >> 5;
        // дальше 2 резервных байта

        /////////////////qDebug()<<"SYS data: РГС нач.тест Ok:"<<rgsOk<<"РГС Ком П2 получена:"<<rgsP2polychena<<"РГС углы отработаны:"<<rgsYO
                ////////<<"РГС частота подстроена:"<<rgs4P<<"РГС головка готова:"<<rgsGG<<"АС  автосопр:"<<as;

        uint16_t t_nakala;
        memcpy(&t_nakala, sys2snr.m10xMissileInfo[i].data + 4, sizeof(int16_t));
        uint16_t t_podg;
        memcpy(&t_podg, sys2snr.m10xMissileInfo[i].data + 6, sizeof(int16_t));

        switch((int)startInfoM.launcherInfo[launcherIndex].missileInfo[i].missileStatus)
        {
            case MissileStatus::PEREHOD_ISKHODNOE_NAKAL: startInfoM.launcherInfo[launcherIndex].missileInfo[i].t_skolko_ostalos_perehod_iskhodnoe_nakal_sec = t_nakala; break;
            case MissileStatus::NAKAL: startInfoM.launcherInfo[launcherIndex].missileInfo[i].t_obratnogo_otcheta_nahojdenia_v_nakale_sec = t_nakala;

            break;
            case MissileStatus::PEREHOD_NAKAL_GOTOVA:
                {
                    float koef = 0.6;
                    float virtualElapsedTime = 0.001 * (programmStartTime.elapsed() - startInfoM.launcherInfo[launcherIndex].missileInfo[i].time_vklychenie_podgotovka_milisec);

                    if(virtualElapsedTime < koef * TMAXPEREHOD_NAKAL_GOTOVA)
                    {
                        startInfoM.launcherInfo[launcherIndex].missileInfo[i].t_skolko_ostalos_perehod_nakal_podgotovka_sec = TMAXPEREHOD_NAKAL_GOTOVA - virtualElapsedTime;
//                        qDebug()<<"sys2snrParser: if virtualElapsedTime"<<virtualElapsedTime<<0.001 * programmStartTime.elapsed()
//                               <<0.001 * startInfoM.launcherInfo[launcherIndex].missileInfo[i].time_vklychenie_podgotovka_milisec
//                              <<"res:"<<startInfoM.launcherInfo[launcherIndex].missileInfo[i].t_skolko_ostalos_perehod_nakal_podgotovka_sec;
                    }
                    else
                    {
                        startInfoM.launcherInfo[launcherIndex].missileInfo[i].t_skolko_ostalos_perehod_nakal_podgotovka_sec =
                                (1.0 - koef) * TMAXPEREHOD_NAKAL_GOTOVA + 1.0 - sqrt(virtualElapsedTime - koef * TMAXPEREHOD_NAKAL_GOTOVA + 1.0);

//                        qDebug()<<"sys2snrParser: else virtualElapsedTime"<<virtualElapsedTime<<0.001 * programmStartTime.elapsed()
//                               <<0.001 * startInfoM.launcherInfo[launcherIndex].missileInfo[i].time_vklychenie_podgotovka_milisec
//                              <<"res:"<<startInfoM.launcherInfo[launcherIndex].missileInfo[i].t_skolko_ostalos_perehod_nakal_podgotovka_sec;
                    }


                }
            break;
            case MissileStatus::GOTOVA: startInfoM.launcherInfo[launcherIndex].missileInfo[i].t_obratnogo_otcheta_nahojdenia_v_gotova_sec = t_podg;
        }

        //  Б Л О К И   В Т О Р И Ч Н О Г О   П И Т А Н И Я         // 2018_06_23
        memcpy(&startInfoM.powerSuppliesBytes, &sys2snr.m501PowerSupplies, 8);

        //uint16_t increment = sys2snr.m501PowerSupplies.data[1] * 256 + sys2snr.m501PowerSupplies.data[0];


                    //Беззнаковая 16-ти разрядная переменная  – счетчик отправленных слов.
                    //Значение счетчика увеличивается на единицу с каждой  следующей отправкой информации.
                    //Интервал значений (циклический) – 0…65535d.
                    //Byte 0 – младшие разряды
                    //Byte 1 – старшие разряды
                    //Byte 2
                    //Бит 0 – неисправность выхода напряжения цепи  +27В-1(A)/XS7:A*
                    //Бит  1 – неисправность выхода напряжения цепи  +27В-1(B)/XS7:B*
                    //Бит 2 – неисправность выхода напряжения цепи  +27В-1(C)/XS7:C*

                    //Бит 3 – неисправность выхода напряжения цепи  +27В-2(A)/XS8:A*
                    //Бит 4 – неисправность выхода напряжения цепи  +27В-2(B)/XS8:B*
                    //Бит 5 – неисправность выхода напряжения цепи  +27В-2(C)/XS8:C*

                    //Бит 6 – неисправность выхода напряжения цепи  +27В-3(A)/XS9:A*
                    //Бит 7 – неисправность выхода напряжения цепи  +27В-3(B)/XS9:B*
                    //Byte 3
                    //Бит 0 – неисправность выхода напряжения цепи +27В-3(С)/XS9:С*

                    //Бит 1 – неисправность выхода напряжения цепи +27В-4(A)/XS10:A*
                    //Бит 2 – неисправность выхода напряжения цепи +27В-4(B)/XS10:B*
                    //Бит 3 – неисправность выхода напряжения цепи +27В-4(C)/XS10:C*

                    //Бит 4 – низкое общее напряжение  источников питания 27В

                    //Бит 5 – превышение допустимой рабочей температуры изделия

                    //Бит 6 – внутренняя неисправность системы мониторинга/диагностики

                    //Бит 7 – не используется (Резерв)- всегда «0»
                    //Byte 4
                    //Бит 0 – неисправность/отсутствие входного электропитания  Источника Питания 27В №1
                    //Бит 1 – неисправность/отсутствие входного электропитания  Источника Питания 27В №2
                    //Бит 2 – неисправность/отсутствие входного электропитания  Источника Питания 27В №3

                    //Бит 3 – неисправность/отсутствие выходного напряжения  Источника Питания 27В №1
                    //Бит 4 – неисправность/отсутствие выходного напряжения  Источника Питания 27В №2
                    //Бит 5 – неисправность/отсутствие выходного напряжения  Источника Питания 27В №3

                    //Бит 6 – неисправность/отсутствие напряжения цепи +24VLow/XS11:A,B,C*
                    //Бит 7 – не используется (Резерв)- всегда «0»

                    //Беззнаковая 16-ти разрядная переменная  – текущее значение силового напряжения 27В на общей шине, выраженная в милливольтах
                    //Byte 5 – младшие разряды
                    //Byte 6 – старшие разряды
                    //Интервал значений– 0…65535d.

                    //Byte 7 – Знаковая 8-ти разрядная переменная  – текущее значение температуры внутри изделия, выраженное в градусах Цельсия
                    //Интервал значений   от минус 40d до плюс 75d.



        //qDebug()<<"sys2snr.m10xMissileInfo[2].data[1]:"<<sys2snr.m10xMissileInfo[2].data[1];

       //////////////// qDebug()<<"SYS data: M1:"<<priznakM1<<"M2:"<<priznakM2<<"ПУиспр:"<<pyIspr<<"РакИспр:"<<rakIspr
       ////////////////        <<"ПитВкл:"<<pitVkl<<"ЦБ:"<<cB<<"Резерв:"<<res<<"Несход:"<<neshod<<"t_nakala:"<<t_nakala<<"t_podg:"<<t_podg;

       if(i == 222222222222)
        qDebug()<<"SYS data: поступНАКАЛ:"<<postypilaKomandaNakal<<"НАКАЛ отраб:"<<nakalOtrabotan
               <<"поступПОДГ:"<<postypilaKomandaPodgotovka<<"полётЗад:"<<poliotZadPolycheno<<"СГП:"<<sGP<<"СВР:"<<sVRprinat
               <<"ПУСКпринят:"<<pyskPrinat<<"ПУСКотраб:"<<pyskOtrabotan<<"t_nakala:"<<t_nakala<<"t_podg:"<<t_podg;
    }
}


void * ProcessThreadSYS(void * temp_pt )
{
    int tcpSocketFileDescriptor = -1;

    unsigned char readBytes[sizeof(SYS2SNR)];

    qDebug()<<"ProcessSYS: 1";

    clientConnect(&tcpSocketFileDescriptor, 9761, "10.5.0.31", "ProcessThreadSYS initial connect");
    qDebug()<<"ProcessSYS: 2";
    fcntl(tcpSocketFileDescriptor, F_SETFL, O_NONBLOCK);
    qDebug()<<"ProcessSYS: 3";
    snr2sysFixedValuesCoder();  //  П О С Т О Я Н Н А Я   Ч А С Т Ь
    qDebug()<<"ProcessSYS: 4";

    while(1)
    {
        usleep(35 * 1000);          //qDebug()<<"********************************************************      ProcessSYS";

        if(apyInfoM.rejimRabotiZRK == RejimRabotiZRK::TR_A || apyInfoM.rejimRabotiZRK == RejimRabotiZRK::TR_K)  //  2018_07_07
            continue;

        snr2sysVariableValuesCoder(1);    //  П Е Р Е М Е Н Н А Я   Ч А С Т Ь           только для теста 1 - ПУ N 2

        qDebug()<<"ProcessSYS: 11";

        //ssize_t totalSentBytes =
        send(tcpSocketFileDescriptor, &snr2sys, sizeof(SNR2SYS), 0);

        ssize_t nextReadBytes;

        ssize_t totalReadBytes = 0;

        int vsego_propyschenih_periodov = 0;

        qDebug()<<"ProcessSYS: vsego_propyschenih_periodov:"<<vsego_propyschenih_periodov;

        while (totalReadBytes < sizeof(SYS2SNR) && vsego_propyschenih_periodov <= 10)
        {
            nextReadBytes = recv(tcpSocketFileDescriptor, readBytes + totalReadBytes, sizeof(SYS2SNR) - totalReadBytes, 0);

            //qDebug()<<"ProcessSYS nextReadBytes:"<<nextReadBytes;

            totalReadBytes += nextReadBytes;

            if(nextReadBytes <= 0)
            {
                vsego_propyschenih_periodov++;

                qDebug()<<"ProcessSYS: vsego_propyschenih_periodov:"<<vsego_propyschenih_periodov;
            }
            else
            {
                vsego_propyschenih_periodov = 0;
            }
        }

        if(vsego_propyschenih_periodov >= 10)
        {
//            for(int j = 0; j < MAXPY; j++)                                                                          //   временно      временно      временно
//                for(int i = 0; i < MAXMISSILE; i++)                                                                 //   временно      временно      временно
//                    startInfoG.launcherInfo[j].missileInfo[i].missileStatus = MissileStatus::RAKETA_OTSYTSTVIET;    //   временно      временно      временно

//            qDebug()<<"ProcessSYS : vsego_propyschenih_periodov >= 10  "<<startInfoG.launcherInfo[1].missileInfo[0].missileStatus;

            break;
        }

        if(totalReadBytes == sizeof(SYS2SNR))
        {
            memcpy(&sys2snr, readBytes, sizeof(SYS2SNR));
            memset(readBytes, 0, sizeof(SYS2SNR));

            ////  2018_07_07    //if(apyInfoM.rejimRabotiZRK != RejimRabotiZRK::TR_A && apyInfoM.rejimRabotiZRK != RejimRabotiZRK::TR_K)
            sys2snrParser(1); // раскодикорвка информации с ПУ N 2 (1 - индекс считаем от нуля)
        }

    };

    close(tcpSocketFileDescriptor);

    threadStatus->thrStatus_SYS = cancel;

    return temp_pt;
}
