#include "dataPool.h"


//***********************************************************************
//  GLOBAL VAR

DataPool*   _dataPool   = NULL;


    DataPool*       getDataPool()
    {
        return  _dataPool;
    } // getDataPool



//***********************************************************************
//  DATAPOOL

    DataPool::DataPool()
    {
        _dataPool           = this;
        this->lastBusUpdate = 2.f;

        // Connect with the EntitySystem API
        this->entitySystem = new pdpSocketAPI_EntitySystem( );
        _ES->connectToDataPool( "127.0.0.1", PDPSOCKETAPI_ENTITYSYSTEM_PORT, "CanMonitor" );

        if ( _ES->isConnected() ) {
            this->vDisplayNightMode             = _ES->findValue( "vDisplayNightMode"           );

            this->subMachineState               = _ES->subscribe( "subMachineState", 5000, true );
            this->subMachineState->addEntity( this->vDisplayNightMode );

            this->subMachineState->start();
        } else {
            this->subMachineState               = NULL;
            this->vDisplayNightMode             = NULL;
        }

        // Connect with the CanSystem API
        this->canSystem = new pdpSocketAPI_CanSystem( );
        if ( !_CAN->connectToDataPool( "127.0.0.1", PDPSOCKETAPI_CANSYSTEM_PORT, "CanMonitor" ) ) {
            qDebug()<<"Cannot connect with SocketAPI CanSystem on port "<<PDPSOCKETAPI_CANSYSTEM_PORT;
        }

        for (int i=0; i<4; i++) {
            this->canBusState[i].bitrate        = 0;
            this->canBusState[i].busIndex       = i+1;
            this->canBusState[i].busLoadAvgPerc = 0;
            this->canBusState[i].busLoadPeakPerc= 0;
            this->canBusState[i].lastErrCode    = CANERR_NOERROR;
            this->canBusState[i].lineError      = false;
            this->canBusState[i].lineOpened     = false;
            this->canBusState[i].resetCount     = 0;
            this->canBusState[i].txOverFlowCnt  = 0;
            this->canBusState[i].txCount        = 0;
            this->canBusState[i].rxCount        = 0;
            this->canBusState[i].errFrameCount  = 0;
        } // for i

    } // create


    DataPool::~DataPool()
    {
        if ( this->subMachineState )
            delete this->subMachineState;

        delete this->entitySystem;
        delete this->canSystem;
        _dataPool   = NULL;
    } // destroy



    void        DataPool::update( const float deltaSecs)
    {
        if ( this->lastBusUpdate >= 2.f ) {
            _CAN->refreshBusStates( true ); // Refresh aSync, which means the actual results take some time to arrive
            this->lastBusUpdate = 0.f;
        } else {
            if ( this->lastBusUpdate < 1.f && this->lastBusUpdate + deltaSecs >= 1.f ) {
                this->canBusState[0]    = _CAN->getBusState( 1 );
                this->canBusState[1]    = _CAN->getBusState( 2 );
                this->canBusState[2]    = _CAN->getBusState( 3 );
                this->canBusState[3]    = _CAN->getBusState( 4 );
            }
            this->lastBusUpdate += deltaSecs;
        }
    } // update




    bool        DataPool::nightMode()
    {
#ifdef __VIRTUALMACHINE
#endif
        if ( this->vDisplayNightMode )
            return this->vDisplayNightMode->getValueB(); else
            return false;
    } // nightMode


