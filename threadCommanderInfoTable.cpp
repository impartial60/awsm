#include <math.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "externM.h"

int oldInfoTableCounter = 0;

void * ProcessThreadInfoTable( void * temp_pt )
{
    (void)temp_pt;
    //qDebug()<<"ProcessInfoTable 1";

    struct sockaddr_in si_me, si_other;

    int s, slen = sizeof(si_other) , recv_len;
    socklen_t clilen = sizeof(si_other);

    bool connectStatus = true;

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        qDebug()<<"ProcessThreadInfoTable: socket == -1";
        connectStatus = false;//die("socket");
    }
    fcntl(s, F_SETFL, O_NONBLOCK);

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(settings->port_awsInfoTable);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if(connectStatus &&  bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        connectStatus = false;//die("bind");

        qDebug()<<"ProcessInfoTable: bind == -1";
    }

    int diagnosticMonitorCounterOld = 0;
    int diagnosticMonitorCounterNew = 0;

    //keep listening for data

    qDebug()<<"ProcessInfoTable UDP Server :"<<settings->port_awsInfoTable;

    int newInfoTableCounter = -1;

    while(connectStatus)
    {
        usleep(35 * 1000);
        ////////////printf("Waiting for data...");
        ////////////fflush(stdout);

        // П О Д Г О Т О В К А   О Т Ч Е Т А   П О   С О С Т О Я Н И Ю   В С Е Х   Р А К Е Т

        //////////////////memset(&commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo, 0, sizeof(sAsterix126_040_RocketInfo));
        //memcpy(&commander2Guidance.vkp2ZRK, &shMemC2U.vkp2zrk, sizeof(SNR_INFO));

        memcpy(&mServer2IT.startInfo, &startInfoM, sizeof(START_INFO));
        memcpy(&mServer2IT.apyInfo, &apyInfoM, sizeof(APY_INFO));
        memcpy(&mServer2IT.komKom, &komKomM, sizeof(KOMANDI_KOMANDIRA));

/*  перемещено в threadVKP.cpp
        for(int i = 0; i < MAXPY; i++)
        {
            for(int j = 0; j < MAXMISSILE; j++)
            {
                //shMemU2C.launcher2Commander.missileStatuses[i * MAXPY + j] = launcher[i]->missileStatus[j];

                switch (launcherConsole2User.startInfo.launcherInfo[i].missileInfo[j].missileStatus)//switch (launcher[i]->missileStatus[j])
                {
                    case (int) MissileStatus::GOTOVA: commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRG1 += 1;
                        //shMemU2C.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRG1 += 1;
                        break;
                    case (int) MissileStatus::NAKAL: commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRG2 += 1; break;
                    // //case (int) MissileStatus::PEREHOD_NAKAL_GOTOVA: shMemU2C.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRP += 1;  break;
                    case (int) MissileStatus::PEREHOD_ISKHODNOE_NAKAL: commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KRP += 1; break;

                    default:    break;
                }

//                if(launcher[i]->missileStatus[j] != MissileStatus::RAKETA_OTSYTSTVIET && launcher[i]->missileStatus[j] != MissileStatus::NAVEDENIE
//                        && launcher[i]->missileStatus[j] != MissileStatus::NEISPRAVNA)
//                shMemU2C.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KR += 1;

                if(launcherConsole2User.startInfo.launcherInfo[i].missileInfo[j].missileStatus != MissileStatus::RAKETA_OTSYTSTVIET &&
                        launcherConsole2User.startInfo.launcherInfo[i].missileInfo[j].missileStatus != MissileStatus::NAVEDENIE
                        && launcherConsole2User.startInfo.launcherInfo[i].missileInfo[j].missileStatus != MissileStatus::NEISPRAVNA)

                commanderConsole2InfoTable.launcher2Commander.asterix126_040_RocketInfo.Rocket27DM1.KR += 1;
            }
        }
*/
        for(int j = 0; j < 4; j++)
        {
            for(int i = 0; i < 4; i++)
            {
                //shMemU2C.launcher2Commander.t_poleta_raket[j * 4 + i] = launcher[j]->t_navedenie_sec[i];

                mServer2UserL.startInfo.launcherInfo[i].missileInfo[j].t_navedenie_sec; // ??????????????????????????????????

            }
        }

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, &newInfoTableCounter, sizeof(int), 0, (struct sockaddr *) &si_other, &clilen)) == -1)
        {
            //connectStatus = false;//die("recvfrom()");
        }

        //qDebug()<<"ProcessInfoTable newInfoTableCounter:"<<newInfoTableCounter<<"awsc_Console2InfoTable.launcher2Commander.missileStatuses[4]:"<<awsc_Console2InfoTable.launcher2Commander.missileStatuses[4];


        //now reply the client with the same data
        if (recv_len > 0)//sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            //////////////////die("sendto()");
            memcpy(&mServer2IT.srvDrvInfo, &srvDrvInfoM, sizeof(SERVO_DRIVE_INFO));
            //qDebug()<<"ProcessInfoTable commanderConsole2InfoTable.srvDrvInfo.unvGeoPosition.geoY.integerDegr:"<<commanderConsole2InfoTable.srvDrvInfo.unvGeoPosition.geoY.integerDegr;

            pthread_mutex_lock(&localConsole_memory_Mutex);
                //sendto(s, &awsc_Console2InfoTable.launcher2Commander, sizeof(data_33), 0, (struct sockaddr*) &si_other, slen);
                sendto(s, &mServer2IT, sizeof(MServer2InfoTableKomandira), 0, (struct sockaddr*) &si_other, slen);
            pthread_mutex_unlock(&localConsole_memory_Mutex);
        }

        if(diagnosticMonitorCounterOld != diagnosticMonitorCounterNew)
        {
            //qDebug()<<"ProcessLauncher: diagnosticMonitorCounterNew"<<diagnosticMonitorCounterNew;
            diagnosticMonitorCounterOld = diagnosticMonitorCounterNew;
        }
    }

    close(s);
    return temp_pt;
}
