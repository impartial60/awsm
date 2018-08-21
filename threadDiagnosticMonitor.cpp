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

int cntr12 = 0;

void * ProcessThreadDiagnMonitor( void * temp_pt )
{
    (void)temp_pt;
    struct sockaddr_in si_me, si_other;

    int s, slen = sizeof(si_other) , recv_len;
    socklen_t clilen = sizeof(si_other);

    bool connectStatus = true;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        qDebug()<<"ProcessThreadDiagnosticMonitor: socket == -1";
        connectStatus = false;//die("socket");
    }
    fcntl(s, F_SETFL, O_NONBLOCK);

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(settings->port_awsDiagnMonitor);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if(connectStatus &&  bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        connectStatus = false;//die("bind");

        qDebug()<<"ProcessThreadDiagnosticMonitor: bind == -1";
    }

    int diagnosticMonitorCounterOld = 0;
    int diagnosticMonitorCounterNew = 0;

    qDebug()<<"ProcessThreadDiagnosticMonitor UDP Server :"<<settings->port_awsDiagnMonitor;

    while(connectStatus)
    {
        usleep(35 * 1000);

        if ((recv_len = recvfrom(s, &diagnosticMonitorCounterNew, sizeof(int), 0, (struct sockaddr *) &si_other, &clilen)) == -1)
        {
            //connectStatus = false;
        }

        if (recv_len > 0)//sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            pthread_mutex_lock(&localConsole_memory_Mutex);

//            for(int i = 0; i < 256; i++)
//                console2DiagnosticMonitor.mdo1Echo.echo[0][i] = i % 255;

            //ssize_t totalSentBytes =
            sendto(s, &mServer2DM, sizeof(MServer2DiagnosticMonitor), 0, (struct sockaddr*) &si_other, slen); //  2017-12-29

                //qDebug()<<"ProcessThreadDiagnMonitor: totalSentBytes:"<<totalSentBytes<<"sizeof(AWSG_Console2User):"<<sizeof(AWSG_Console2User);

            pthread_mutex_unlock(&localConsole_memory_Mutex);
        }

        if(diagnosticMonitorCounterOld != diagnosticMonitorCounterNew)
        {
            //qDebug()<<"ProcessThreadDiagnosticMonitor: diagnosticMonitorCounterNew"<<diagnosticMonitorCounterNew;
            diagnosticMonitorCounterOld = diagnosticMonitorCounterNew;
        }
    }

    close(s);
    return temp_pt;
}


void operatorF_common_operation(int roleNo)
{
    struct sockaddr_in si_me, si_other;

    int s, slen = sizeof(si_other) , recv_len;
    socklen_t clilen = sizeof(si_other);

    bool connectStatus = true;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        qDebug()<<"ProcessThreadOperF roleNo"<<roleNo<<"socket == -1";
        connectStatus = false;//die("socket");
    }
    fcntl(s, F_SETFL, O_NONBLOCK);

    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(settings->port_OperatorF[roleNo]);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if(connectStatus &&  bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        connectStatus = false;

        qDebug()<<"ProcessThreadOperF roleNo"<<roleNo<<"bind == -1";
    }

    OperF_to_AWSG operF2AWSG;
    operF2AWSG.transActId = 0;

    int counterOld = 0;

    qDebug()<<"ProcessThreadOperF roleNo"<<roleNo<<"UDP Server :"<<settings->port_OperatorF[roleNo];

    while(connectStatus)
    {
        usleep(35 * 1000);

        if ((recv_len = recvfrom(s, &operF2AWSG, sizeof(OperF_to_AWSG), 0, (struct sockaddr *) &si_other, &clilen)) == -1)
        {
            //srvDrvInfoM.handWheelFSpeed[1] = settings->handWheelSpeedFactor * operF2AWSG.multiTurnValue;   //  for work
            //connectStatus = false;//die("recvfrom()");
        }
        else
        {
//            qDebug()<<"ProcessThreadOperF1: recv_len:"<<recv_len<<"operF2AWSG: transactionId:"<<operF2AWSG.transActId<<"multiTurnValue:"<<operF2AWSG.multiTurnValue;

            //////////////////////////////////////if(settings->useVirtualHandWheel == 1)
                apyInfoM.handWheelFSpeed[roleNo] = settings->handWheelSpeedFactor * operF2AWSG.multiTurnValue;   //  for work

            //qDebug()<<"ProcessThreadOperF1: srvDrvInfoM.handWheelFSpeed[0]:"<<srvDrvInfoM.handWheelFSpeed[0];
        }

        //now reply the client with the same data
        if (recv_len > 0)
        {            
            memcpy(&mServer2UserO.apyInfo, &apyInfoM, sizeof(APY_INFO));    //   2018_06_26

            pthread_mutex_lock(&localConsole_memory_Mutex);

                sendto(s, &mServer2UserO, sizeof(MServer2UserO), 0, (struct sockaddr*) &si_other, slen);

            pthread_mutex_unlock(&localConsole_memory_Mutex);
        }

        if(counterOld != operF2AWSG.transActId)//if(counterOld != counterNew)
        {
            //qDebug()<<"ProcessLauncher: diagnosticMonitorCounterNew"<<diagnosticMonitorCounterNew;
            counterOld = operF2AWSG.transActId;//counterOld = counterNew;
        }
    }
    close(s);
}


void * ProcessThreadOperF1( void * temp_pt )
{
    //qDebug()<<"ProcessThreadOperF1 1";

    operatorF_common_operation(0);

    return temp_pt;
}

void * ProcessThreadOperF2( void * temp_pt )
{
    //qDebug()<<"ProcessThreadOperF2 1";

    operatorF_common_operation(1);

    return temp_pt;
}
