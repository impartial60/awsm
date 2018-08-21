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

//int oldDispatcherCounter = 0;

void unv2apyOperations()
{
    if(srvDrvInfoM.cyOtrabotano == 1)
    {
        if(apyInfoM.rejimPoiskaSNR == RejimPoiskaSNR::POISK_PEREKLYCHEN_V_ZAHVAT)
            apyInfoM.rejimPoiskaSNR = RejimPoiskaSNR::POISK_PEREKLYCHEN_V_SOPROVOJDENIE;

        //qDebug()<<"unv2apyOperations: srvDrvInfoM.cyOtrabotano == 1";
    }
}


void * ProcessThreadServerUNV( void * temp_pt )
{
    (void)temp_pt;
    struct sockaddr_in udpSockAddr;
    socklen_t udpSockAddrLength = sizeof(udpSockAddr);

    apy2unv.cntrD2MCA = 1000;

    bool connectStatus = true;

    int udpClientSocketFD;

    if ( (udpClientSocketFD=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        connectStatus = false;//die("socket");
        qDebug()<<"udpSrv: socket failed";
    }

    fcntl(udpClientSocketFD, F_SETFL, O_NONBLOCK);

    udpSockAddr.sin_family = AF_INET;
    udpSockAddr.sin_port = htons(settings->port_serverUNV);
    udpSockAddr.sin_addr.s_addr = inet_addr(settings->ipAddressServerUNV);

    qDebug()<<"ProcessThreadServerUNV: UDP Client:"<<settings->ipAddressServerUNV<<":"<<settings->port_serverUNV;

    while(connectStatus)
    {
        usleep(35 * 1000);

        apy2unv.cntrD2MCA++;

        if(!(apyInfoM.rejimPoiskaSNR == POISK_PEREKLYCHEN_V_SOPROVOJDENIE &&                             //  переключение режима поиска всегда осуществляется офицером наведения или командиром
             apy2unv.komandi_NAVED.guidanceOfficerCmds.rejimPoiskaSNR == POISK_PEREKLYCHEN_V_ZAHVAT))    //  кроме перехода из захвата в сопровождение
            apyInfoM.rejimPoiskaSNR = apy2unv.komandi_NAVED.guidanceOfficerCmds.rejimPoiskaSNR;          //   2018_06_19

        apyInfoM.rejimSoprovojdeniaSNR = apy2unv.komandi_NAVED.guidanceOfficerCmds.rejimSoprovojdeniaSNR;   //   2018_06_18

        rsTrgtPrmsCalculator();     //   2018_06_16

        memcpy(&apy2unv.apyInfo, &apyInfoM, sizeof(APY_INFO));                 //   2018_06_15

        //qDebug()<<"ProcessThreadServerUNV: apy2unv.komKom.rejimRabotiZRK:"<<apy2unv.komKom.rejimRabotiZRK;

//        qDebug()<<"M: ProcessThreadServerUNV:"<<apy2unv.komandi_NAVED.guidanceOfficerCmds.servoDriveExecute[0]                                           <<apy2unv.komandi_NAVED.guidanceOfficerCmds.servoDriveExecute[1]
//                                           <<apy2unv.komandi_NAVED.guidanceOfficerCmds.servoDriveDelta[0]                                           <<apy2unv.komandi_NAVED.guidanceOfficerCmds.servoDriveDelta[1];
        //ssize_t totalSentBytes =
        sendto(udpClientSocketFD, &apy2unv, sizeof(MainServer2ServerUNV), 0, (struct sockaddr *) &udpSockAddr, udpSockAddrLength);

        ssize_t totalReadBytes = 0;
        ssize_t nextReadBytes;
        int readCirclCounter = 0;

        while (totalReadBytes < abs(sizeof(ServerUNV2MainServer)) && readCirclCounter++ < 10)
        {
            nextReadBytes = recvfrom(udpClientSocketFD,  &unv2apy + totalReadBytes, sizeof(ServerUNV2MainServer) - totalReadBytes, 0, (struct sockaddr *) &udpSockAddr, &udpSockAddrLength);
            if(nextReadBytes < 0)
                nextReadBytes = 0;

            totalReadBytes += nextReadBytes;
        }

        if(totalReadBytes == sizeof(ServerUNV2MainServer))
        {
            //////////////////qDebug()<<"ProcessThreadServerUNV  totalReadBytes:"<<totalReadBytes<<0.001 * programmStartTime.currentTime().msecsSinceStartOfDay() - 52000;


            memcpy(&srvDrvInfoM, &unv2apy.srvDrvInfo, sizeof(SERVO_DRIVE_INFO));                       //   2018_06_14

            memcpy(&mServer2UserO.srvDrvInfo, &srvDrvInfoM, sizeof(SERVO_DRIVE_INFO));
            //   2018_06_26     memcpy(&cnsl2Operator.apyInfo, &apyInfoM, sizeof(APY_INFO));

            memcpy(&mServer2UserO.mdo1Echo, &unv2apy.mdo1Echo, sizeof(MDO1_echoData));           //  2018_06_21
            //  2018_07_21  memcpy(&unv2apy.mdo1Info, &unv2apy.mdo1Info, sizeof(MDO1_incomingData) * 3);         //  2018_06_21

            memcpy(&mServer2DM.usmInfo, &unv2apy.usmInfo, sizeof(USM_Info));           //  2018_07_07
            memcpy(&mServer2UserG.usmInfo, &unv2apy.usmInfo, sizeof(USM_Info));        //  2018_07_07
            memcpy(&mServer2UserG.mdo1Info, &unv2apy.mdo1Info, sizeof(MDO1_incomingData) * 3);        //  2018_07_20
            //memcpy(&unv2apy.primaryPlots, &primaryPlots, sizeof(MDO1_Plot) * MAX_MDO1_PRIMARY_PLOTS_COUNT);       //  2018_07_27
            memcpy(&mServer2UserG.primaryPlots, &unv2apy.primaryPlots, sizeof(MDO1_Plot) * MAX_MDO1_PRIMARY_PLOTS_COUNT);       //  2018_07_27

            memcpy(&mServer2UserG.joinedTrgtData, &unv2apy.joinedTrgtData, sizeof(MDO1_joinedTargetData) * MAX_MDO1_TARGETS_COUNT);         //  2018_07_21

            //qDebug()<<"*ProcessThreadServerUNV: unv2apy.usmInfo.sync[7]:"<<unv2apy.usmInfo.sync[7]<<unv2apy.usmInfo.usm02[9];

            //qDebug()<<"diagnosticParser: "<<console2User.mdo1Info[0].header.x55;

            unv2apyOperations();
        }

        //qDebug()<<"ProcessThreadServerUNV: apy2unv.cntrD2MCA:"<<apy2unv.cntrD2MCA<<"totalReadBytes:"<<totalReadBytes<<"unv2apy.cntrMCA2D:"<<unv2apy.cntrMCA2D;

    }

    close(udpClientSocketFD);

    return temp_pt;
}
