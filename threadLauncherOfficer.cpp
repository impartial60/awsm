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

//int cntr23 = 0;
MissileStatus::Status *nextMissileStatus;

void launchersSimulator()
{
//    cntr23++;

//    qDebug()<<"launchersSimulator cntr23:"<<cntr23<<"bortConnectorsPressedCounter:"<<launcherUser2Console.launcherUserCommands[1].bortConnectorsPressedCounter[0]
//                <<"missileStatus:"<<launcherConsole2User.launcherInfo[1].missileInfo[0].missileStatus;

    for(int j = 0; j < MAXPY; j++)
    {
        for(int i = 0; i < MAXMISSILE; i++)
        {
            if(startInfoM.launcherInfo[j].missileInfo[i].missileStatus == MissileStatus::RAKETA_OTSYTSTVIET && oficerPyska2Console.komandiPY[j].simulatorBortConctrsIsOn[i])
                startInfoM.launcherInfo[j].missileInfo[i].missileStatus = MissileStatus::ISKHODNOE;
            else
                if(startInfoM.launcherInfo[j].missileInfo[i].missileStatus != MissileStatus::NAVEDENIE &&
                   startInfoM.launcherInfo[j].missileInfo[i].missileStatus != MissileStatus::VSTRECHA  &&
                        oficerPyska2Console.komandiPY[j].simulatorBortConctrsIsOn[i] == false)
                    startInfoM.launcherInfo[j].missileInfo[i].missileStatus = MissileStatus::RAKETA_OTSYTSTVIET;
        }
    }


    for(int j = 0; j < MAXPY; j++)  //for(int j = 1; j < 2; j++)//
    {
        for(int i = 0; i < MAXMISSILE; i++)   //for(int i = 0; i < 1; i++)
        {
            nextMissileStatus = &startInfoM.launcherInfo[j].missileInfo[i].missileStatus;

            if(oficerPyska2Console.komandiPY[j].ishdn_ButtonPressedCounter[i] >= 10)
            {
                *nextMissileStatus = MissileStatus::ISKHODNOE;
            }

            switch((int)*nextMissileStatus)
            {
            case (MissileStatus::ISKHODNOE):
                startInfoM.launcherInfo[j].missileInfo[i].targetNoKLM = 0;

                startInfoM.launcherInfo[j].missileInfo[i].t_obratnogo_otcheta_ostivanie_v_iskhodnom_sec =
                oficerPyska2Console.timeZoomFactor * 0.001 * (programmStartTime.elapsed() - startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_obratnogo_otscheta_ostivanie_v_iskhodnom_milisec);

                if(oficerPyska2Console.komandiPY[j].nakal_BtnPressedCounter[i] >= 10) // начала или продолжает поступать команда включить накал
                {
                    *nextMissileStatus = MissileStatus::PEREHOD_ISKHODNOE_NAKAL;
                    startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_nakala_milisec = programmStartTime.elapsed();

                    //qDebug()<<"launchersSimulator metka 2 cntr23:"<<cntr23<<"bortConnectorsPressedCounter:"<<oficerPyska2Console.komandiPY[1].nakal_BtnPressedCounter[0];
                }

                break;
            case (MissileStatus::PEREHOD_ISKHODNOE_NAKAL):                
                    startInfoM.launcherInfo[j].missileInfo[i].t_skolko_ostalos_perehod_iskhodnoe_nakal_sec = TMAXPEREHOD_ISKHODNOE_NAKAL -
                    oficerPyska2Console.timeZoomFactor * 0.001 * (programmStartTime.elapsed() - startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_nakala_milisec);

                //qDebug()<<"launchersSimulator metka 3 cntr23:"<<cntr23<<"case (MissileStatus::PEREHOD_ISKHODNOE_NAKAL)"<<startInfoG.launcherInfo[j].missileInfo[i].t_skolko_ostalos_perehod_iskhodnoe_nakal_sec;

                if(startInfoM.launcherInfo[j].missileInfo[i].t_skolko_ostalos_perehod_iskhodnoe_nakal_sec <= 0)  // переход в состояние "НАКАЛ" завершен
                {
                    *nextMissileStatus = MissileStatus::NAKAL;
                    startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_obratnogo_otscheta_nakala_milisec = programmStartTime.elapsed();
                }
                break;
            case (MissileStatus::NAKAL):
                startInfoM.launcherInfo[j].missileInfo[i].t_skolko_ostalos_perehod_iskhodnoe_nakal_sec = TMAXPEREHOD_ISKHODNOE_NAKAL;
                startInfoM.launcherInfo[j].missileInfo[i].t_obratnogo_otcheta_nahojdenia_v_nakale_sec = TMAXNAKAL -
                oficerPyska2Console.timeZoomFactor * 0.001 * (programmStartTime.elapsed() - startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_obratnogo_otscheta_nakala_milisec);

                //qDebug()<<"launchersSimulator metka 4 cntr23:"<<cntr23<<"case (MissileStatus::NAKAL)"<<startInfoG.launcherInfo[j].missileInfo[i].t_obratnogo_otcheta_nahojdenia_v_nakale_sec;

                if(startInfoM.launcherInfo[j].missileInfo[i].t_obratnogo_otcheta_nahojdenia_v_nakale_sec <= 0)
                {
                    *nextMissileStatus = MissileStatus::ISKHODNOE;
                    startInfoM.launcherInfo[j].missileInfo[i].counter_ostivanie_v_iskhodnom = 0;
                    startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_obratnogo_otscheta_ostivanie_v_iskhodnom_milisec = programmStartTime.elapsed();
                }

                if(oficerPyska2Console.komandiPY[j].podgt_BtnPressedCounter[i] >= 10) // начала или продолжает поступать команда "ПОДГОТОВКА"
                {
                    *nextMissileStatus = MissileStatus::PEREHOD_NAKAL_GOTOVA;
                    startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_podgotovka_milisec = programmStartTime.elapsed();

                    //qDebug()<<"launchersSimulator metka 5 cntr23:"<<cntr23<<"missileStatus:"<<*nextMissileStatus;
                }

                break;
            case (MissileStatus::PEREHOD_NAKAL_GOTOVA):

                startInfoM.launcherInfo[j].missileInfo[i].t_skolko_ostalos_perehod_nakal_podgotovka_sec =     //  запуск "условного" прогресс-индикатора обратного отсчёта перехода НАКАЛ-ПОДГОТОВКА
                TMAXPEREHOD_NAKAL_GOTOVA - oficerPyska2Console.timeZoomFactor * 0.001 * (programmStartTime.elapsed() - startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_podgotovka_milisec);

                //qDebug()<<"launchersSimulator metka 3 cntr23:"<<cntr23<<"case (MissileStatus::PEREHOD_NAKAL_GOTOVA)"<<startInfoG.launcherInfo[j].missileInfo[i].t_perehod_nakal_podgotovka_sec;

                if(startInfoM.launcherInfo[j].missileInfo[i].t_skolko_ostalos_perehod_nakal_podgotovka_sec <= 0)
                {
                    *nextMissileStatus = MissileStatus::GOTOVA;
                    startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_obratnogo_otscheta_gotova_milisec = programmStartTime.elapsed();
                }

                break;
            case (MissileStatus::GOTOVA):

                startInfoM.launcherInfo[j].missileInfo[i].t_skolko_ostalos_perehod_nakal_podgotovka_sec = TMAXPEREHOD_NAKAL_GOTOVA;

                startInfoM.launcherInfo[j].missileInfo[i].t_obratnogo_otcheta_nahojdenia_v_gotova_sec = TMAXGOTOVA -
                oficerPyska2Console.timeZoomFactor * 0.001 * (programmStartTime.elapsed() - startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_obratnogo_otscheta_gotova_milisec);

                if(startInfoM.launcherInfo[j].missileInfo[i].t_obratnogo_otcheta_nahojdenia_v_gotova_sec <= 0)
                {
                    *nextMissileStatus = MissileStatus::ISKHODNOE;
                    startInfoM.launcherInfo[j].missileInfo[i].counter_ostivanie_v_iskhodnom = 0;
                    startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_obratnogo_otscheta_ostivanie_v_iskhodnom_milisec = programmStartTime.elapsed();
                }
                else
                {
                    if(oficerPyska2Console.komandiPY[j].pysk_BtnPressedCounter[i] >= 80)    // начала или продолжает поступать команда "ПУСК"
                    {                                                                       // 80 - потому что долго отрабатывается
                        *nextMissileStatus = MissileStatus::NAVEDENIE;
                        startInfoM.launcherInfo[j].missileInfo[i].targetNoKLM = apyInfoM.nextTockaRS[0].targetNo;
                        startInfoM.launcherInfo[j].missileInfo[i].time_start_navedenie_milisec = programmStartTime.elapsed();

                        //qDebug()<<"launchersSimulator: case (MissileStatus::GOTOVA): time_start_navedenie_milisec:"<<startInfoM.launcherInfo[j].missileInfo[i].time_start_navedenie_milisec;
                    }
                }
                break;
            case (MissileStatus::NAVEDENIE): case (MissileStatus::VSTRECHA):
                startInfoM.launcherInfo[j].missileInfo[i].t_navedenie_sec =
                        0.001 * (programmStartTime.elapsed() - startInfoM.launcherInfo[j].missileInfo[i].time_start_navedenie_milisec);

                if((int)*nextMissileStatus == MissileStatus::NAVEDENIE)
                {                    
                    float raketaVparam = apyInfoM.nextTockaRS[0].rangeParametr_km / sqrt(apyInfoM.nextTockaRS[0].rangeParametr_km * apyInfoM.nextTockaRS[0].rangeParametr_km +
                            apyInfoM.nextTockaRS[0].kyrsovojParameter_km * apyInfoM.nextTockaRS[0].kyrsovojParameter_km) * VRAKETI;

                    startInfoM.launcherInfo[j].missileInfo[i].xMissileR = raketaVparam * mServer2UserL.startInfo.launcherInfo[j].missileInfo[i].t_navedenie_sec;

                    //qDebug()<<"drawGradysnik: i:"<<i<<"raketaVparam:"<<raketaVparam<<"launcher[j]->t_navedenie_sec[i]:"<<launcher[j]->t_navedenie_sec[i]<<"xMissileR:"<<xMissileR;//<<"trgtRx:"<<trgtRx;

                    //  П Р И   П Е Р В О М   С Р А Б А Т Ы В А Н И И   Э Т О Г О   У С Л О В И Я  -  П О Д Р Ы В   Б Ч
                    if(startInfoM.launcherInfo[j].missileInfo[i].xMissileR + 0.2 > apyInfoM.nextTockaRS[0].rangeParametr_km)
                        *nextMissileStatus = MissileStatus::VSTRECHA;
                }

                if(startInfoM.launcherInfo[j].missileInfo[i].t_navedenie_sec >= TMAXNAVEDENIE)
                {                    
                    *nextMissileStatus = MissileStatus::RAKETA_OTSYTSTVIET;
                    startInfoM.launcherInfo[j].missileInfo[i].t_navedenie_sec = 0.0;
                }
            }
        }
    }
}


void * ProcessThreadLauncherOfficer( void * temp_pt )
{
    struct sockaddr_in si_me, si_other;

    int s, slen = sizeof(si_other);
    socklen_t clilen = sizeof(si_other);

    bool connectStatus = true;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        qDebug()<<"ProcessThreadLauncherOfficer: socket == -1";
        connectStatus = false;//die("socket");
    }

    fcntl(s, F_SETFL, O_NONBLOCK);
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(settings->port_awsLauncherOfficer);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if(connectStatus &&  bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        connectStatus = false;//die("bind");
        qDebug()<<"ProcessThreadLauncherOfficer: bind == -1";
    }

    //int launcherOfficerCounterOld = 0;
    //int launcherOfficerCounterNew = 0;

    qDebug()<<"ProcessThreadLauncherOfficer UDP Server :"<<settings->port_awsLauncherOfficer;

    while(connectStatus)
    {
        usleep(35 * 1000);

        ssize_t totalReadBytes = 0;
        ssize_t nextReadBytes;
        int readCirclCounter = 0;

        while (totalReadBytes < abs(sizeof(KOMANDI_OFICERA_PYSKA)) && readCirclCounter++ < 10)
        {
            nextReadBytes = recvfrom(s, &oficerPyska2Console + totalReadBytes, sizeof(KOMANDI_OFICERA_PYSKA) - totalReadBytes, 0, (struct sockaddr *) &si_other, &clilen);
            if(nextReadBytes < 0)
                nextReadBytes = 0;

            totalReadBytes += nextReadBytes;
        }

        if(apyInfoM.rejimRabotiZRK == RejimRabotiZRK::TR_A || apyInfoM.rejimRabotiZRK == RejimRabotiZRK::TR_K)
        {
            launchersSimulator();
        }
        else
        {
            if(startInfoM.previos_rejimRabotiZRK == RejimRabotiZRK::TR_A || startInfoM.previos_rejimRabotiZRK == RejimRabotiZRK::TR_K)
            {
                for(int j = 0; j < MAXPY; j++)
                {
                    for(int i = 0; i < MAXMISSILE; i++)
                    {
                        startInfoM.launcherInfo[j].missileInfo[i].missileStatus = MissileStatus::RAKETA_OTSYTSTVIET;
                        startInfoM.launcherInfo[j].missileInfo[i].targetNoKLM = 0;
                        startInfoM.launcherInfo[j].missileInfo[i].time_vklychenie_obratnogo_otscheta_ostivanie_v_iskhodnom_milisec = -1000000;
                    }
                }
            }
        }

        startInfoM.previos_rejimRabotiZRK = apyInfoM.rejimRabotiZRK;

        if(totalReadBytes == sizeof(KOMANDI_OFICERA_PYSKA))
        {
            memcpy(&mServer2UserL.startInfo, &startInfoM, sizeof(START_INFO));
            memcpy(&mServer2UserL.apyInfo, &apyInfoM, sizeof(APY_INFO));                 //   2018_06_15
            memcpy(&mServer2UserL.srvDrvInfo, &srvDrvInfoM, sizeof(SERVO_DRIVE_INFO));

            //qDebug()<<"ProcessThreadLauncherOfficer cnsl2UserL.apyInfo.targetIndexCY:"<<cnsl2UserL.apyInfo.targetIndexCY;

            //pthread_mutex_lock(&localConsole_memory_Mutex);
                sendto(s, &mServer2UserL, sizeof( MServer2UserL), 0, (struct sockaddr*) &si_other, slen);
            //pthread_mutex_unlock(&localConsole_memory_Mutex);

            //if(launcherOfficerCounterOld != launcherOfficerCounterNew) launcherOfficerCounterOld = launcherOfficerCounterNew;
        }

    }

    close(s);
    return temp_pt;
}
