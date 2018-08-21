#ifndef THREADSTATUS_H
#define THREADSTATUS_H

enum threedStatus {cReate, inuse, cancel, notuse};

class ThreadStatus
{    
    public:

    threedStatus thrStatus_VKP;
    threedStatus thrStatus_P18;
    threedStatus thrStatus_SYS;
    threedStatus thrStatus_GuidanceOf;
    threedStatus thrStatus_LauncherOf;
    threedStatus thrStatus_Commander;

    threedStatus thrStatus_Simulator;
    threedStatus thrStatus_DiagnMonitor;    
    threedStatus thrStatus_OperF1;
    threedStatus thrStatus_OperF2;
    threedStatus thrStatus_InfoTable;
    threedStatus thrStatus_serverUNV;

    ThreadStatus()//int semaphorId)
    {
//        if(semaphorId == -1)
//            thrStatus_GuidanceOf = notuse;
//        else
        thrStatus_GuidanceOf = cReate;

        thrStatus_VKP = notuse;//cReate;
        thrStatus_P18 = notuse;//cReate;
        thrStatus_LauncherOf = cReate;
        thrStatus_SYS = cReate; //notuse;//
        thrStatus_Commander = cReate;//

        thrStatus_Simulator = notuse;//cReate;//
        thrStatus_DiagnMonitor = cReate; //notuse;//
        thrStatus_OperF1 = cReate;
        thrStatus_OperF2 = cReate;
        thrStatus_InfoTable = cReate; //notuse;//
        thrStatus_serverUNV = cReate; //notuse;//
    }
};

#endif // THREADSTATUS_H
