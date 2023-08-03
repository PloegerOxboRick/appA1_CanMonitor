#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLocale>
#include <QKeyEvent>

//**********************************************************************************
//  GLOBAL VAR

MainWindow* _mainWindow = NULL;

const str   APPNAME     = "appA1_CanMonitor";
// RR.VV.BB    01.02.13
const int   REVISION    = 01;   // Major
const int   VERSION     = 01;   //
const int   BUILD       = 02;

      str   appBuildDate= "<not set>";
      str   appVersion  = "<not set>";



//************************************************************************
//  Callbacks

    void    onAppManSignal_Minimize( str appSignal, QStringList args )
    {
        _mainWindow->onShrink();
    }
    void    onAppManSignal_ScrollUp( str appSignal, QStringList args )
    {
        _mainWindow->btnClick_ScrollUp();
    }
    void    onAppManSignal_ScrollDn( str appSignal, QStringList args )
    {
        _mainWindow->btnClick_ScrollDn();
    }

    void    onAppManSignal_Record( str appSignal, QStringList args )
    {
        _mainWindow->btnClick_Pause();
    }
    void    onAppManSignal_Pause( str appSignal, QStringList args )
    {
        _mainWindow->btnClick_Pause();
    }

    void    onAppManSignal_Clear( str appSignal, QStringList args )
    {
        _mainWindow->btnClick_Clear();
    }



//**********************************************************************************
//


    CanMessageLogger::CanMessageLogger( const byte busIndex )
        : pdpCanMessageSubscriptionBase( "CanMonitor_Bus"+QString::number(busIndex),
                                         busIndex, 32, true )
    {
    } // create


    void    CanMessageLogger::receiveMessageCompressed(PCanMessage msg,
                                                       const int count, const int deltaMSec, const QString info)
    {
        _mainWindow->addMessage( &msg, count, deltaMSec, info );
    } // receiveMessageCompressed



//**********************************************************************************
//  Main Window

    MainWindow::MainWindow(QWidget *parent) :
        AppManWindow(parent),
        ui(new Ui::MainWindow)
    {
        _mainWindow = this;
        ui->setupUi(this);

        this->recording     = false;
        this->showAsJ1939   = true;
        this->showAsHex     = true;
        this->showAsBin     = false;
        this->fltrSource    = -1;
        this->fltrMsgId     = -1;
        this->fltrBusIndex  = 1;

        this->loggedCnt     = 0;
        this->ui->btnRecording->hide();
        this->ui->btnStopTrace->hide();

        connect( &this->tmr, SIGNAL(timeout()), this, SLOT(timerTimeOut()) );
        connect( this->ui->btnRecording     , SIGNAL(pressed()) , this, SLOT(btnClick_Pause()) );
        connect( this->ui->btnPaused        , SIGNAL(pressed()) , this, SLOT(btnClick_Pause()) );
        connect( this->ui->btnFltrSrc       , SIGNAL(pressed()) , this, SLOT(btnClick_FilterSrc()) );
        connect( this->ui->btnFltrMsgId     , SIGNAL(pressed()) , this, SLOT(btnClick_FilterMsg ()) );
        connect( this->ui->btnToggleHex     , SIGNAL(pressed()) , this, SLOT(btnClick_ToggleHex()) );
        connect( this->ui->btnToggleProtocol, SIGNAL(pressed()) , this, SLOT(btnClick_ToggleJ1939()) );
        connect( this->ui->btnToggleBus     , SIGNAL(pressed()) , this, SLOT(btnClick_ToggleBus()) );
        connect( this->ui->btnStartTrace    , SIGNAL(pressed()) , this, SLOT(btnClick_TraceStart()) );
        connect( this->ui->btnStopTrace     , SIGNAL(pressed()) , this, SLOT(btnClick_TraceEnd()) );
        connect( this->ui->btnStopTrace     , SIGNAL(pressed()) , this, SLOT(btnClick_TraceStop()) );
        connect( this->ui->btnClear         , SIGNAL(pressed()) , this, SLOT(btnClick_Clear()) );
        connect( this->ui->btnMinimize      , SIGNAL(pressed()) , this, SLOT(btnClick_Minimize()) );

        connect( this->ui->btnScrollUp      , SIGNAL(pressed()) , this, SLOT(btnClick_ScrollUp()) );
        connect( this->ui->btnScrollDn      , SIGNAL(pressed()) , this, SLOT(btnClick_ScrollDn()) );


        this->ui->tableWidget->setColumnWidth( 0,  50   );  // SRC
        this->ui->tableWidget->setColumnWidth( 1,  80   );  // PGN
        this->ui->tableWidget->setColumnWidth( 2, 150   );  // Descr
        this->ui->tableWidget->setColumnWidth( 3,  50   );  // Delta
        this->ui->tableWidget->setColumnWidth( 4,  55   );  // #

        this->ui->tableWidget->setColumnWidth( 5,  46   );  // B1
        this->ui->tableWidget->setColumnWidth( 6,  46   );  // B2
        this->ui->tableWidget->setColumnWidth( 7,  46   );  // B3
        this->ui->tableWidget->setColumnWidth( 8,  46   );  // B4
        this->ui->tableWidget->setColumnWidth( 9,  46   );  // B5
        this->ui->tableWidget->setColumnWidth(10,  46   );  // B6
        this->ui->tableWidget->setColumnWidth(11,  46   );  // B7
        this->ui->tableWidget->setColumnWidth(12,  46   );  // B8

        this->ui->tableWidget->horizontalHeader()->show();
        this->ui->tableWidget->verticalHeader()->show();

        this->ui->tableWidget->setRowCount( MAXROWS );
        for (int i=0; i<MAXROWS; i++) {
            // Make cells
            for (int j=0; j<13; j++)
                this->ui->tableWidget->setItem( i, j, new QTableWidgetItem("") );

            this->ui->tableWidget->item( i, 0 )->setTextAlignment( Qt::AlignCenter );   // src
            this->ui->tableWidget->item( i, 1 )->setTextAlignment( Qt::AlignCenter );   // pgn
            this->ui->tableWidget->item( i, 2 )->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );     // info

            this->ui->tableWidget->item( i, 3 )->setTextAlignment( Qt::AlignCenter );   // delta
            this->ui->tableWidget->item( i, 4 )->setTextAlignment( Qt::AlignCenter );   // count

            this->ui->tableWidget->item( i, 5 )->setTextAlignment( Qt::AlignCenter );   // b1
            this->ui->tableWidget->item( i, 6 )->setTextAlignment( Qt::AlignCenter );
            this->ui->tableWidget->item( i, 7 )->setTextAlignment( Qt::AlignCenter );
            this->ui->tableWidget->item( i, 8 )->setTextAlignment( Qt::AlignCenter );
            this->ui->tableWidget->item( i, 9 )->setTextAlignment( Qt::AlignCenter );
            this->ui->tableWidget->item( i,10 )->setTextAlignment( Qt::AlignCenter );
            this->ui->tableWidget->item( i,11 )->setTextAlignment( Qt::AlignCenter );
            this->ui->tableWidget->item( i,12 )->setTextAlignment( Qt::AlignCenter );
        } // for i

        tmr.setInterval( 500 );
        tmr.start();


        this->dataPool      = new DataPool();   // Subscribe on the Entities we need in this APP
        // Listen to AppManager
        this->appManLink    = new MyAppManLink( "CanMonitor", this );
        this->declareVersion();

        // Make CAN message subscriptions
        for (int i=0; i<4; i++) {
            this->subscriptions[i]  = new CanMessageLogger( i+1 );
            // No filters, we want them all

            //CanMessageFilter fltr = CanMessageFilter( -1, 0xF000, 0xFFFF );
            //this->subscriptions[i]->addFilter( fltr );
        }
        this->cSubscription   = this->subscriptions[0];

        this->ui->frameMinimized->show();
        this->ui->frameMaximized->hide();
        this->ui->frameMaximized->setGeometry( 0,0, SCREENW_LARGE, SCREENH_LARGE );
        this->setGeometry( 0,0, SCREENW_SMALL, SCREENH_SMALL );

        this->installEventFilter( this );

        // Unpause
        this->btnClick_Pause();
    } // create


    MainWindow::~MainWindow()
    {
        for (int i=0; i<4; i++) {
            delete this->subscriptions[i];
        }

        this->tmr.stop();
        delete ui;
        delete dataPool;
        delete appManLink;
    } // destroy



    void    MainWindow::declareVersion()
    {
        // RR.VV.BB    01.02.13
        const int REVISION  = 01;   // Major
        const int VERSION   = 01;   //
        const int BUILD     = 02;

        QString buildDate   = QStringLiteral(__DATE__);
        QString buildTime   = QStringLiteral(__TIME__);

        appVersion          = oxStrings::lead( REVISION, 2 ) +"."+ oxStrings::lead( VERSION, 2 ) +"."+ oxStrings::lead( BUILD, 2 );
        appBuildDate        = buildDate;

        qDebug()<<APPNAME+" VERSION: "+ appVersion;
        qDebug()<<APPNAME+" BUILDDATE: "+ buildDate+"   "+buildTime;

        if ( _ES ) {
            _ES->setModuleVersion( APPNAME, appVersion, appBuildDate );
        }
    } // declareVersion



    void    MainWindow::addMessage( PCanMessage* msg,
                                    const int count, const int deltaMSec, const str info )
    {
        if ( !this->recording ) return;
        // Pass filters?
        if ( this->fltrSource >= 0 ) {
            if ( this->fltrSource != msg->getSource() )
                return;
        }
        if ( this->fltrMsgId >= 0 ) {
            if ( this->showAsJ1939 ) {
                if ( this->fltrMsgId != msg->getPGN() )
                    return;
            } else
                if ( this->fltrMsgId != msg->absMsgId )
                    return;
        }

        // Check if already in the list
        int index = -1;
        for (int i=0; i<this->loggedCnt; i++ ) {
            if ( this->loggedMsg[i].msg.absMsgId == msg->absMsgId )
                if ( this->loggedMsg[i].info == info ) {
                    // Use Existing line
                    index = i;
                    this->loggedMsg[index].deltaMSec    = deltaMSec;
                    this->loggedMsg[index].count        = count;
                    break;
                }
        } // for i


        // Add new line?
        if ( index <= -1 ) {
            if ( this->loggedCnt == MAXROWS ) {
                // MaxRows reached, too bad
                return;
            }
            // Add new
            index   = this->loggedCnt++;
            this->loggedMsg[index].deltaMSec= deltaMSec;
            this->loggedMsg[index].count    = count;
            this->loggedMsg[index].info     = info;
            this->loggedMsg[index].msg      = *msg;

            this->setLineAddress( index, &this->loggedMsg[index] );

            // Clear
            this->ui->tableWidget->item( index, 3 )->setText( "" ); // Delta unknown yet
            for (int i=0; i<8; i++)
                this->ui->tableWidget->item( index, 5+i )->setText( "" );

        } else {
            this->loggedMsg[index].msg.data = msg->data;    // update data
        } //

        // Update line
        this->setLineData( index, &this->loggedMsg[index] );
    } // addMessage



    void    MainWindow::setLineAddress( const int index, RecordedMsg *logMsg )
    {
        if ( this->showAsJ1939  &&  logMsg->msg.extended ) {
            PCanMessage msg;
            msg.setAddress( logMsg->msg.absMsgId, true );

            int src = msg.getSource();
            int pgn = msg.getPGN();
            if ( this->showAsHex ) {
                this->ui->tableWidget->item( index, 0 )->setText( "x" + oxStrings::byteToHex( src )  );
                this->ui->tableWidget->item( index, 1 )->setText( "x" + oxStrings::u16ToHex( pgn ) );
            } else
            if ( 1 ) {
                this->ui->tableWidget->item( index, 0 )->setText( QString::number( src )  );
                this->ui->tableWidget->item( index, 1 )->setText( QString::number( pgn )  );
            }

        } else {
            // Raw, no J1939 address encoding
            this->ui->tableWidget->item( index, 0 )->setText( "" );
            if ( this->showAsHex ) {
                this->ui->tableWidget->item( index, 1 )->setText( "x" + oxStrings::u32ToHex( (uint32)logMsg->msg.absMsgId )  );
            } else {
                this->ui->tableWidget->item( index, 1 )->setText( QString::number( logMsg->msg.absMsgId )  );
            }
        }

        this->ui->tableWidget->item( index, 2 )->setText( logMsg->info );

    } // setLineAddress



    void    MainWindow::setLineData(   const int index, RecordedMsg *logMsg )
    {
        if ( this->loggedMsg[index].deltaMSec > 0 ) {
            this->ui->tableWidget->item( index, 3 )->setText( QString::number( logMsg->deltaMSec )  );
        }
        this->ui->tableWidget->item( index, 4 )->setText( QString::number( logMsg->count )  );

        if ( this->showAsHex ) {
            for (int i=0; i<logMsg->msg.dlc; i++) {
                this->ui->tableWidget->item( index, 5+i )->setText( oxStrings::byteToHex( logMsg->msg.data.bytes[i] ) );
            }
        } else {
            for (int i=0; i<logMsg->msg.dlc; i++) {
                this->ui->tableWidget->item( index, 5+i )->setText( QString::number( logMsg->msg.data.bytes[i] ) );
            }
        }
    } // setLineData



    void    MainWindow::timerTimeOut()
    {
        // Bus State
        this->dataPool->update( this->tmr.interval() * 0.001f );
        CanBusState state = this->dataPool->canBusState[ this->fltrBusIndex-1 ];

        if ( state.lineOpened ) {
            this->ui->lblLineOpened->setStyleSheet( CSS_GREEN );
            this->ui->lblLineOpened->setText( "yes" );
            this->ui->lblLineOpened_2->setStyleSheet( CSS_GREEN );
            this->ui->lblLineOpened_2->setText( "yes" );
        } else {
            this->ui->lblLineOpened->setStyleSheet( CSS_GREY );
            this->ui->lblLineOpened->setText( "no" );
            this->ui->lblLineOpened_2->setStyleSheet( CSS_GREY );
            this->ui->lblLineOpened_2->setText( "no" );
        }

        if ( state.lineError ) {
            this->ui->lblLineError->setStyleSheet( CSS_RED );
            this->ui->lblLineError->setText( "yes" );
            this->ui->lblLineError_2->setStyleSheet( CSS_RED );
            this->ui->lblLineError_2->setText( "yes" );
        } else {
            this->ui->lblLineError->setStyleSheet( CSS_GREY );
            this->ui->lblLineError->setText( "no" );
            this->ui->lblLineError_2->setStyleSheet( CSS_GREY );
            this->ui->lblLineError_2->setText( "no" );
        }

        QLocale loc;
        this->ui->lblResetCnt->setText(     QString::number( state.resetCount ) );
        this->ui->lblLastErrCode->setText(  canBusErrorCodeToString( state.lastErrCode ) );
        this->ui->lblOverflowCnt->setText(  QString::number( state.txOverFlowCnt ) );
        this->ui->lblBitrate->setText(      QString::number( round( state.bitrate * 0.001 ) ) );
        this->ui->lblRxCount->setText(      loc.toString( state.rxCount ) );    // With thousands seperator
        this->ui->lblTxCount->setText(      loc.toString( state.txCount ) );
        this->ui->lblErrFrames->setText(    loc.toString( state.errFrameCount ) );

        this->ui->lblResetCnt_2->setText(     QString::number( state.resetCount ) );
        this->ui->lblLastErrCode_2->setText(  canBusErrorCodeToString( state.lastErrCode ) );
        this->ui->lblOverflowCnt_2->setText(  QString::number( state.txOverFlowCnt ) );
        this->ui->lblRxCount_2->setText(      loc.toString( state.rxCount ) );    // With thousands seperator
        this->ui->lblTxCount_2->setText(      loc.toString( state.txCount ) );
        this->ui->lblErrFrames_2->setText(    loc.toString( state.errFrameCount ) );

        if ( state.lastErrCode != CANERR_NOERROR ) {
            this->ui->lblLastErrCode->setStyleSheet( CSS_LBLWARN );
            this->ui->lblLastErrCode_2->setStyleSheet( CSS_LBLWARN );
        } else {
            this->ui->lblLastErrCode->setStyleSheet( CSS_LBLOK );
            this->ui->lblLastErrCode_2->setStyleSheet( CSS_LBLOK );
        }

        if ( state.resetCount > 0 ) {
            this->ui->lblResetCnt->setStyleSheet( CSS_LBLWARN );
            this->ui->lblResetCnt_2->setStyleSheet( CSS_LBLWARN );
        }
        if ( state.txOverFlowCnt > 0 ) {
            this->ui->lblOverflowCnt->setStyleSheet( CSS_LBLWARN );
            this->ui->lblOverflowCnt_2->setStyleSheet( CSS_LBLWARN );
        }
        if ( state.errFrameCount > 0 ) {
            this->ui->lblErrFrames->setStyleSheet( CSS_LBLWARN );
            this->ui->lblErrFrames_2->setStyleSheet( CSS_LBLWARN );
        }

/*  SIMULATE SOME RANDOM MESSAGES
*/
#ifdef __VIRTUALMACHINE
/*        PCanMessage msg;
        msg.setAddress( 0xFF00 + mRandI(0,254), 0xA0 + mRandI(0,10), 6 );
        msg.dlc = 5;
        msg.data.bytes[0]   = mRandI( 0, 255 );
        msg.data.bytes[1]   = mRandI( 0, 255 );
        msg.data.bytes[2]   = mRandI( 0, 255 );
        msg.data.bytes[3]   = mRandI( 0, 255 );
        msg.data.bytes[4]   = mRandI( 0, 255 );
        msg.data.bytes[5]   = mRandI( 0, 255 );
        msg.data.bytes[6]   = mRandI( 0, 255 );
        msg.data.bytes[7]   = mRandI( 0, 255 );

        this->addMessage( &msg, 1, 0, "TestMessage" );
        */
#endif
// */
    } // timerTimeOut



    void    MainWindow::btnClick_Pause()
    {
        this->recording = !this->recording;
        if ( this->recording ) {
            this->cSubscription->start();
            this->ui->btnRecording->show();
            this->ui->btnPaused->hide();
        } else {
            this->cSubscription->pause();
            this->ui->btnRecording->hide();
            this->ui->btnPaused->show();
        }
    } // btnClick_Pause


    void    MainWindow::btnClick_FilterSrc()
    {
        if ( this->fltrSource == -1 ) {
            // Get Source from selected row
            int fltrSrc = 0;
            QItemSelectionModel* sel = this->ui->tableWidget->selectionModel();

            if ( sel->selectedIndexes().count() > 0 ) {
                int row = sel->selectedIndexes().at(0).row();
                if ( row >= this->loggedCnt ) return;
                fltrSrc = this->loggedMsg[row].msg.getSource();
            }

            // Apply filter
            this->fltrSource    = fltrSrc;
            this->ui->btnFltrSrc->setText( "Source:\n" + QString::number( this->fltrSource ) );
        } else {
            this->fltrSource    = -1;
            this->ui->btnFltrSrc->setText( "Source:\nAll" );
        }
        this->btnClick_Clear();
    } // btnClick_FilterSrc


    void    MainWindow::btnClick_FilterMsg()
    {
        if ( this->fltrMsgId == -1 ) {
            // Get msgId from selected row
            int fltrId = 0;
            QItemSelectionModel* sel = this->ui->tableWidget->selectionModel();

            if ( sel->selectedIndexes().count() > 0 ) {
                int row = sel->selectedIndexes().at(0).row();
                if ( row >= this->loggedCnt ) return;
                if ( this->showAsJ1939 )
                    fltrId = this->loggedMsg[row].msg.getPGN(); else
                    fltrId = this->loggedMsg[row].msg.absMsgId;
            }

            // Apply filter
            this->fltrMsgId    = fltrId;
            this->ui->btnFltrMsgId->setText( "Msg ID:\n" + QString::number( this->fltrMsgId ) );
        } else {
            this->fltrMsgId    = -1;
            this->ui->btnFltrMsgId->setText( "Msg ID:\nAll" );
        }
        this->btnClick_Clear();
    } // btnClick_FilterMsg


    void    MainWindow::btnClick_ToggleHex()
    {
        if ( this->showAsHex ) {        // Toggle to DEC
            this->showAsHex = false;
            this->showAsBin = false;
            this->ui->btnToggleHex->setText( "DEC" );
        //} else
        //if ( !this->showAsHex  &&  !this->showAsBin ) { // Toggle to BIN
        //    this->showAsHex = false;
        //    this->showAsBin = true;
        //    this->ui->btnToggleHex->setText( "BIN" );
        } else {                        // Toggle to HEX
            this->showAsHex = true;
            this->showAsBin = false;
            this->ui->btnToggleHex->setText( "HEX" );
        }

        // Refresh
        for (int i=0; i<this->loggedCnt; i++) {
            this->setLineAddress( i, &this->loggedMsg[i] );
            this->setLineData( i, &this->loggedMsg[i] );
        } // for i

    } // btnClick_ToggleHex


    void    MainWindow::btnClick_ToggleJ1939()
    {
        this->showAsJ1939 = !this->showAsJ1939;
        if ( this->showAsJ1939 ) {
            this->ui->btnFltrSrc->show();
            this->ui->btnToggleProtocol->setText( "J1939" );

            this->ui->tableWidget->horizontalHeaderItem(1)->setText( "PGN" );
            this->ui->tableWidget->setColumnWidth( 0,  50    );  // SRC
            this->ui->tableWidget->setColumnWidth( 1,  80    );  // PGN
        } else {
            this->ui->btnFltrSrc->hide();
            this->ui->btnToggleProtocol->setText( "Raw" );

            this->ui->tableWidget->horizontalHeaderItem(1)->setText( "Message ID" );
            this->ui->tableWidget->setColumnWidth( 0,   0   );  // SRC
            this->ui->tableWidget->setColumnWidth( 1, 130   );  // PGN
        }

        // Refresh
        for (int i=0; i<this->loggedCnt; i++) {
            this->setLineAddress( i, &this->loggedMsg[i] );
            this->setLineData( i, &this->loggedMsg[i] );
        } // for i

    } // btnClick_ToggleJ1939


    void    MainWindow::btnClick_ToggleBus()
    {
        // Hold current subscription
        this->cSubscription->pause();       // Pause the previous bus
        this->cSubscription->clearLog();

        // Clear table cells
        for (int i=0; i<MAXROWS; i++) {
            for (int j=0; j<12; j++)
                this->ui->tableWidget->item( i, j )->setText( "" );
        } // for i
        this->loggedCnt = 0;

        // Next bus
        ++this->fltrBusIndex;
        if ( this->fltrBusIndex > 3 )
            this->fltrBusIndex = 1;

        this->cSubscription = this->subscriptions[ this->fltrBusIndex-1 ];
        if ( this->recording )
            this->cSubscription->start();   // Resume current bus

        this->ui->btnToggleBus->setText( "BUS "+ QString::number( this->fltrBusIndex ) );
        this->ui->lblCanBus->setText( "CAN-BUS "+ QString::number( this->fltrBusIndex ) );
    } // btnClick_ToggleBus


    void    MainWindow::btnClick_Clear()
    {
        this->cSubscription->pause();
        this->cSubscription->clearLog();

        // Clear table cells
        for (int i=0; i<MAXROWS; i++) {
            for (int j=0; j<12; j++)
                this->ui->tableWidget->item( i, j )->setText( "" );
        } // for i

        if ( this->recording )
            this->cSubscription->start();
        this->loggedCnt = 0;
    } // btnClick_Clear


    void    MainWindow::btnClick_TraceStart()
    {
        bool usbPresent;
        str targetPath  = oxFileSystem::getUsbFolder( &usbPresent );

#ifdef  __VIRTUALMACHINE
        usbPresent  = true;
        targetPath  = "/home/ccs/Desktop/POG/Framework/V4/Apps/Common/Service/CanMonitor/_testUSB/";
#endif

        if ( usbPresent ) {
            str filename= "canTrace_" + oxClock::getUTC().toString( "yyyy-MM-dd__hh-mm-ss") +".txt";

            _CAN->canTraceCreate( this->fltrBusIndex );
            if ( this->fltrSource >= 0 ) {
                CanMessageFilter fltr = CanMessageFilter( this->fltrSource );
                _CAN->canTraceAddFilter( this->fltrBusIndex, fltr );
            }
            if ( _CAN->canTraceStart( this->fltrBusIndex, targetPath + filename ) ) {
                this->ui->btnStartTrace->hide();
                this->ui->btnStopTrace->show();
                this->ui->btnToggleBus->hide(); // Cant switch buses during measurement
            }
        }
    } // btnClick_TraceStart


    void    MainWindow::btnClick_TraceEnd()
    {
        if ( _CAN->canTraceEnd( this->fltrBusIndex ) ) {

        } else {
            // Failed to save

        }

        this->ui->btnStartTrace->show();
        this->ui->btnStopTrace->hide();
        this->ui->btnToggleBus->show(); // Cant switch buses during measurement

    } // btnClick_TraceEnd


    void    MainWindow::btnClick_Minimize()
    {
        _mainWindow->onShrink();
    } // btnClick_Minimize



    void    MainWindow::btnClick_ScrollUp()
    {
//        QKeyEvent* k = new QKeyEvent( QEvent::KeyPress, Qt::Key_PageUp   , Qt::NoModifier );
        for (int i=0; i<10; i++) {
            QKeyEvent* k = new QKeyEvent( QEvent::KeyPress, Qt::Key_Up   , Qt::NoModifier );
            QApplication::postEvent( this->ui->tableWidget , k );
        }
    } // btnClick_ScrollUp


    void    MainWindow::btnClick_ScrollDn()
    {
//        QKeyEvent* k = new QKeyEvent( QEvent::KeyPress, Qt::Key_PageDown , Qt::NoModifier );
        for (int i=0; i<10; i++) {
            QKeyEvent* k = new QKeyEvent( QEvent::KeyPress, Qt::Key_Down , Qt::NoModifier );
            QApplication::postEvent( this->ui->tableWidget , k );
        }
    } // btnClick_ScrollDn


    bool    MainWindow::eventFilter(QObject *obj, QEvent *event)
    {
        if (event->type() == QEvent::MouseButtonRelease ) {
            if ( this->ui->frameMinimized->isVisible() ) {
                this->onMaximize();
                return true;
            } else {

            }
            return false;
        }

        return QWidget::eventFilter(obj, event);
    } // eventFilter



    void    MainWindow::onMaximize()
    {
        this->ui->frameMinimized->hide();
        this->ui->frameMaximized->show();

        _APPMANLINK->requestMaximize();
        this->setGeometry( 0,0, SCREENW_LARGE, SCREENH_LARGE );
    } // onMaximize


    void    MainWindow::onShrink()
    {
        this->ui->frameMinimized->show();
        this->ui->frameMaximized->hide();

        _APPMANLINK->requestMinimize();

        this->setGeometry( 0,0, SCREENW_SMALL, SCREENH_SMALL );
        _APPMANLINK->adjustPanel_Hide();
    } // onShrink



    void    MainWindow::onSignalReceived_Sleep()
    {

    } //


    void    MainWindow::onSignalReceived_WakeUp()
    {

    } //



    void    MainWindow::onSignalReceived_Minimize()
    {   // AppMan asks us to minimuze (neighbour maximizes, logged out)
        this->ui->frameMinimized->show();
        this->ui->frameMaximized->hide();

        this->setGeometry( 0,0, SCREENW_SMALL, SCREENH_SMALL );
    } // onSignalReceived_Minimize


    void    MainWindow::onSignalReceived_Maximize()
    {   // AppMan asks us to maximize
        this->ui->frameMinimized->hide();
        this->ui->frameMaximized->show();

        this->setGeometry( 0,0, SCREENW_LARGE, SCREENH_LARGE );
    } // onSignalReceived_Maximize
