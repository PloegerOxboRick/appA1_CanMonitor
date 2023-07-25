#ifndef DATAPOOL_H
#define DATAPOOL_H

#include <Code/SocketAPI_Client/pdpSocketAPI_CanSystem.h>


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
