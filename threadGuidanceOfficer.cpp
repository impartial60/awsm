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

void * ProcessThreadGuidanceOfficer( void * temp_pt )
{
    (void)temp_pt;
    //qDebug()<<"ProcessThreadGuidanceOfficer 1";

    struct sockaddr_in si_me, si_other;

    int s, slen = sizeof(si_other) , recv_len;
    socklen_t clilen = sizeof(si_other);

    bool connectStatus = true;

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        qDebug()<<"ProcessThreadGuidanceOfficer: socket == -1";
        connectStatus = false;//die("socket");
    }
    fcntl(s, F_SETFL, O_NONBLOCK);

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(settings->port_awsGuidanceOfficer);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if(connectStatus &&  bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        connectStatus = false;//die("bind");

        qDebug()<<"ProcessThreadGuidanceOfficer: bind == -1";
    }

//    int oficerNavedCounterOld = 0;
//    int oficerNavedCounterNew = 0;

    qDebug()<<"ProcessThreadGuidanceOfficer UDP Server :"<<settings->port_awsGuidanceOfficer;

    while(connectStatus)
    {
        usleep(35 * 1000);

        ssize_t totalReadBytes = 0;
        ssize_t nextReadBytes;
        int readCirclCounter = 0;

        while (totalReadBytes < abs(sizeof(KOMANDI_OFICERA_NAVED)) && readCirclCounter++ < 10)
        {
            nextReadBytes = recvfrom(s, &apy2unv.komandi_NAVED + totalReadBytes, sizeof(KOMANDI_OFICERA_NAVED) - totalReadBytes, 0, (struct sockaddr *) &si_other, &clilen);
            if(nextReadBytes < 0)
                nextReadBytes = 0;

            totalReadBytes += nextReadBytes;
        }

        if(totalReadBytes == sizeof(KOMANDI_OFICERA_NAVED))
        {
            //qDebug()<<"ProcessThreadGuidanceOfficer: apy2unv.komandi_NAVED.guidanceOfficerCmds.rejimPoiskaSNR:"<<apy2unv.komandi_NAVED.guidanceOfficerCmds.rejimPoiskaSNR;

            memcpy(&mServer2UserG.apyInfo, &apyInfoM, sizeof(APY_INFO));                 //   2018_06_15
            memcpy(&mServer2UserG.srvDrvInfo, &srvDrvInfoM, sizeof(SERVO_DRIVE_INFO));                 //   2018_06_14
            memcpy(&mServer2UserG.komKom, &komKomM, sizeof(KOMANDI_KOMANDIRA));          //   2018_06_14


            //memcpy(&cnsl2UserG.usmInfo, &unv2apy.usmInfo, sizeof(USM_Info));        //  2018_07_07

            //qDebug()<<"*ProcessThreadGuidanceOfficer: cnsl2UserG.usmInfo.sync[7]:"<< cnsl2UserG.usmInfo.sync[7]<<cnsl2UserG.usmInfo.usm02[9];


            //  2018-06-20  qDebug()<<"ProcessThreadGuidanceOfficer: komKomM.targetIndexCY:"<<komKomM.targetIndexCY;

            //pthread_mutex_lock(&localConsole_memory_Mutex);
                sendto(s, &mServer2UserG, sizeof(MServer2UserG), 0, (struct sockaddr*) &si_other, slen);
            //pthread_mutex_unlock(&localConsole_memory_Mutex);
            //if(oficerNavedCounterOld != oficerNavedCounterNew)            oficerNavedCounterOld = oficerNavedCounterNew;

            if(apy2unv.komandi_NAVED.guidanceOfficerCmds.targetIndexCY > -1)
            {
                apyInfoM.targetIndexCY = apy2unv.komandi_NAVED.guidanceOfficerCmds.targetIndexCY;
            }

            apyInfoM.rejimSoprovojdeniaSNR = apy2unv.komandi_NAVED.guidanceOfficerCmds.rejimSoprovojdeniaSNR;
        }

        //qDebug()<<"ProcessSharedMemoryReadWrite: shMemC2U.commander2Guidance.komKom.executeNewAzManualCounter:"<<shMemC2U.commander2Guidance.komKom.executeNewAzManualCounter;

        /////////////////////   2018_06_14   servoDriveExecuter();

        /////////////////////   2018_06_14   if(srvDrvInfoM.rejimPoiskaSNR == POISK_PEREKLYCHEN_V_SOPROVOJDENIE)
            /////////////////////   2018_06_14   rsTrgtPrmsCalculator();
    }

    close(s);
    return temp_pt;
}
