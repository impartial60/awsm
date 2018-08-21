#ifndef EXTERNM_H
#define EXTERNM_H
#include <QTime>
#include <QDebug>

#include "../../aws125CommonInclude/sharedMem.h"
#include "../../aws125CommonInclude/externCommon.h"
//#include "../../aws125CommonInclude/sys.h"
#include "threadstatus.h"
#include "srcKram/tp.h"
#include "settingsM.h"

extern ServerUNV2MainServer unv2apy;
extern MainServer2ServerUNV apy2unv;

extern START_INFO startInfoM;
extern VKP2ZRK vkp2zrkM;
extern P18_2ZRK p18_2ZRKM;
extern SERVO_DRIVE_INFO srvDrvInfoM;
extern APY_INFO apyInfoM;

extern bool komandirPereklychilTrenaj;
extern KOMANDI_KOMANDIRA komKomM;
extern KOMANDI_OFICERA_PYSKA oficerPyska2Console;

extern MServer2UserC mServer2UserC;
extern MServer2UserG mServer2UserG;
extern MServer2DiagnosticMonitor mServer2DM;
extern MServer2UserL mServer2UserL;
extern MServer2UserO mServer2UserO;
extern MServer2InfoTableKomandira mServer2IT;

extern QTime programmStartTime;

extern void* ret_val_All_Threads;
extern ThreadStatus *threadStatus;

extern pthread_mutex_t localConsole_memory_Mutex;
extern pthread_mutex_t nextTockaMutex;

extern int restartCntr;

extern SettingsConsoleM *settings;
extern ZP zonaPorazh;

extern void rsTrgtPrmsCalculator();       
extern uint16_t uint16_t_byte_reorder(uint16_t inputValue);
extern void asterix_I126_050_Coder(uint8_t *sck);   //  I126/050        С О С Т О Я Н И Е   Ц Е Л Е В О Г О   К А Н А Л А   1


extern void * ProcessThreadCommander( void * temp_pt );
extern void * ProcessThreadLauncherOfficer( void * temp_pt );
extern void * ProcessThreadSYS(void * temp_pt );
extern void * ProcessThreadCnvEth485(void * temp_pt );
extern void * ProcessSimulator(void * temp_pt );
extern void * ProcessThreadGuidanceOfficer(void * temp_pt );
extern void * ProcessThreadDiagnMonitor( void * temp_pt );
extern void * ProcessThreadOperF1( void * temp_pt );
extern void * ProcessThreadOperF2( void * temp_pt );
extern void * ProcessThreadMDO1( void * temp_pt );
extern void * ProcessThreadMDO1Echo( void * temp_pt );
extern void * ProcessThreadInfoTable( void * temp_pt );
extern void * ProcessThreadServerUNV( void * temp_pt );
extern void * ProcessThreadVKP(void * temp_pt );
extern void * ProcessThreadP18(void * temp_pt );

extern float convert360angle2PlusMinus180(float dltAz360);

#endif // EXTERNM_H
