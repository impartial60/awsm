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


void * ProcessThreadCommander( void * temp_pt )
{
    (void)temp_pt;
    //qDebug()<<"ProcessThreadCommander 1";
    struct sockaddr_in si_me, si_other;

    int s, slen = sizeof(si_other) , recv_len;
    socklen_t clilen = sizeof(si_other);

    bool connectStatus = true;

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        qDebug()<<"ProcessThreadCommander: socket == -1";
        connectStatus = false;//die("socket");
    }
    fcntl(s, F_SETFL, O_NONBLOCK);

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(settings->port_awsCommander);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if(connectStatus &&  bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        connectStatus = false;//die("bind");

        qDebug()<<"ProcessThreadCommander: bind == -1";
    }

//    int commanderCounterOld = 0;
//    int commanderCounterNew = 0;

    qDebug()<<"ProcessThreadCommander UDP Server :"<<settings->port_awsCommander;

    while(connectStatus)
    {
        usleep(35 * 1000);

        ssize_t totalReadBytes = 0;
        ssize_t nextReadBytes;
        int readCirclCounter = 0;

        while (totalReadBytes < abs(sizeof(KOMANDI_KOMANDIRA)) && readCirclCounter++ < 10)
        {
            nextReadBytes = recvfrom(s, &komKomM + totalReadBytes, sizeof(KOMANDI_KOMANDIRA) - totalReadBytes, 0, (struct sockaddr *) &si_other, &clilen);
            if(nextReadBytes < 0)
                nextReadBytes = 0;

            totalReadBytes += nextReadBytes;
        }

        if(totalReadBytes == sizeof(KOMANDI_KOMANDIRA))
        {
            if(komKomM.rejimRabotiZRK != apyInfoM.rejimRabotiZRK)   //  2018_08_07
                apyInfoM.totalSimTargetNum = 0;

            apyInfoM.rejimRabotiZRK = komKomM.rejimRabotiZRK;
            apyInfoM.remoteControlZRK = komKomM.remoteControlZRK;

            if(komKomM.targetIndexCY > -1)
                apyInfoM.targetIndexCY = komKomM.targetIndexCY;            

            //  2018-06-20  qDebug()<<"ProcessThreadCommander: komKomM.targetIndexCY:"<<komKomM.targetIndexCY;

            memcpy(&apy2unv.komKom, &komKomM, sizeof(KOMANDI_KOMANDIRA));                 //   2018_06_14
            //qDebug()<<"ProcessThreadCommander: komKomM.rejimRabotiZRK:"<<komKomM.rejimRabotiZRK;

            //  К О М А Н Д И Р   В Ы К Л Ю Ч И Л   Р Е Ж И М   Т Р Е Н А Ж А
            if((apyInfoM.rejimRabotiZRK == RejimRabotiZRK::TR_K || apyInfoM.rejimRabotiZRK == RejimRabotiZRK::TR_A) &&
                 komKomM.rejimRabotiZRK != RejimRabotiZRK::TR_K &&  komKomM.rejimRabotiZRK != RejimRabotiZRK::TR_A)
            {
                //qDebug()<<"ProcessThreadCommander К О М А Н Д И Р   В Ы К Л Ю Ч И Л   Р Е Ж И М   Т Р Е Н А Ж А";

                for(int j = 0; j < MAXPY; j++)
                {
                    for(int i = 0; i < MAXMISSILE; i++)
                    {
                            startInfoM.launcherInfo[j].missileInfo[i].missileStatus = MissileStatus::RAKETA_OTSYTSTVIET;
                    }
                }
            }

            memcpy(&mServer2UserC.apyInfo, &apyInfoM, sizeof(APY_INFO));                 //   2018_06_16
            memcpy(&mServer2UserC.srvDrvInfo, &srvDrvInfoM, sizeof(SERVO_DRIVE_INFO));                 //   2018_06_16
            memcpy(&mServer2UserC.vkp2zrk, &vkp2zrkM, sizeof(VKP2ZRK));                  //   2018_06_16

            memcpy(&mServer2UserC.primaryPlots, &unv2apy.primaryPlots, sizeof(MDO1_Plot) * MAX_MDO1_PRIMARY_PLOTS_COUNT);              //  2018_07_28
            memcpy(&mServer2UserC.joinedTrgtData, &unv2apy.joinedTrgtData, sizeof(MDO1_joinedTargetData) * MAX_MDO1_TARGETS_COUNT);    //  2018_07_28


            sendto(s, &mServer2UserC, sizeof(MServer2UserC), 0, (struct sockaddr*) &si_other, slen);
            //qDebug()<<"ProcessThreadCommander: sizeof(Console2Komandir):"<<sizeof(Console2Komandir)<<"console2UserC.srvDrvInfo.currentAzimuth:"<<console2UserC.srvDrvInfo.currentAzimuth;
        }
    }

    close(s);
    return temp_pt;
}
