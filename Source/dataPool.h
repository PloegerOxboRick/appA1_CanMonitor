#ifndef DATAPOOL_H
#define DATAPOOL_H

#include <Code/SocketAPI_Client/pdpSocketAPI_CanSystem.h>

//****************************************************************
// RR.VV.BB    01.02.13
const str   APPNAME     = "appA1_CanMonitor";
const int   REVISION    = 00;   // Major
const int   VERSION     = 01;   //
const int   BUILD       = 03;


//****************************************************************


    // This class will Subscribe on the DataPool to get the entities we need for this program
    class DataPool
    {
    private:
        float                    lastBusUpdate;

        pdpSocketAPI_CanSystem*  canSystem;

    public:
        CanBusState              canBusState[4];

        DataPool();
        ~DataPool();

        void        update( const float deltaSecs );
    }; // DataPool


    DataPool*       getDataPool();
#define _DATAPOOL   getDataPool()

#endif // DATAPOOL_H
