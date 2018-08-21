#include <QDebug>
#include <QCoreApplication>
#include <QTime>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>

#include  "srcKram/tp.h"
#include <sharedMem.h>
#include <sys.h>

#include "threadstatus.h"
#include "settingsM.h"

pthread_t pthreadAll;
void* ret_val_All_Threads;
void * Process_All_Threads( void * temp_pt );

pthread_mutex_t localConsole_memory_Mutex;
pthread_mutex_t nextTockaMutex;

ThreadStatus *threadStatus;
int restartCntr = 0;

ServerUNV2MainServer unv2apy;
MainServer2ServerUNV apy2unv;

bool komandirPereklychilTrenaj = false;
KOMANDI_KOMANDIRA komKomM;
KOMANDI_OFICERA_PYSKA oficerPyska2Console;

MServer2UserC mServer2UserC;
MServer2InfoTableKomandira mServer2IT;

MServer2UserG mServer2UserG;
MServer2DiagnosticMonitor mServer2DM;

MServer2UserL mServer2UserL;
MServer2UserO mServer2UserO;

START_INFO startInfoM;
VKP2ZRK vkp2zrkM;
P18_2ZRK p18_2ZRKM;
SERVO_DRIVE_INFO srvDrvInfoM;
APY_INFO apyInfoM;

QTime programmStartTime;
SettingsConsoleM *settings;
ZP zonaPorazh;

extern pid_t proc_find(const char* name, long secondPid);

int main(int argc, char *argv[])
{    
    (void)argc; (void)argv;

    //   П Р О В Е Р К А   Н А   Е Д И Н С Т В Е Н Н О С Т Ь   З А П У Щ Е Н Н О Г О   Э К З Е М П Л Я Р А   П Р О Г Р А М М Ы
    pid_t own_pid = getpid();           //  это пид самой себя
    printf("awsM125Server own pid: % d\n", own_pid);

    pid_t pid = proc_find("awsM125Server", own_pid);
    if(pid >= 0)    // найден старый работающий экземпляр программы
    {
        printf("!!!OLD awsM125Server DETECTED!!! pid: %d - it will be killed\n", pid);
        kill(pid, SIGTERM);
        sleep(1);
    }
    else
        printf("OLD awsM125Server NOT FOUND - it`s Ok...\n");

    settings = new SettingsConsoleM(&apyInfoM.unvGeoPosition);

    if(settings->useP18 > 0)
        apyInfoM.p18TransactionId = 0;  //  предусматривается возможность подключения к П18

    //qDebug()<<"srvDrvInfoM.p18TransactionId:"<<srvDrvInfoM.p18TransactionId;

    //qDebug()<<"***  srvDrvInfoM.unvGeoPosition:"<<srvDrvInfoM.unvGeoPosition.geoY.integerDegr<<srvDrvInfoM.unvGeoPosition.geoY.integerMin<<srvDrvInfoM.unvGeoPosition.geoY.integerSec<<srvDrvInfoM.unvGeoPosition.geoY.integerSec100
    //      <<srvDrvInfoM.unvGeoPosition.geoX.integerDegr<<srvDrvInfoM.unvGeoPosition.geoX.integerMin<<srvDrvInfoM.unvGeoPosition.geoX.integerSec<<srvDrvInfoM.unvGeoPosition.geoX.integerSec100;

    programmStartTime.start();

    //srvDrvInfoM.totalSimTargetNum = 1;

    komKomM.targetIndexCY = -1;
    //srvDrvInfoM.p18GeoPosition.geoY = 99;  //  99 - признак того, что информация о точке стояния отсутствует
    //srvDrvInfoM.trackPointsNumber = 0;

    threadStatus = new ThreadStatus();

    pthread_mutex_init(&localConsole_memory_Mutex, NULL);
    pthread_mutex_init(&nextTockaMutex, NULL);

    pthread_create( &pthreadAll, NULL, Process_All_Threads, NULL );
    qDebug()<<"pthreadAll: pthread_create";

    pthread_join( pthreadAll, &ret_val_All_Threads);
    qDebug()<<"pthreadAll: pthread_join";

    return 123;//a.exec();
}
