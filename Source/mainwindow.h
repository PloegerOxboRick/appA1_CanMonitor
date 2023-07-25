#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>

#include "appManLink.h"
#include "dataPool.h"
#include <Code/oxCommon.h>
#include <Code/Platform/fileSystem.h>

namespace Ui {
class MainWindow;
}

const int MAXROWS       = 128;
const str CSS_RED       = "background-color: rgb(204,   0,   0); color: white;";
const str CSS_GREY      = "background-color: rgb(128, 128, 128); color: white;";
const str CSS_GREEN     = "background-color: rgb(  0, 220,   0); color: white;";

const str CSS_LBLOK     = "";
const str CSS_LBLWARN   = "background-color: rgb( 250, 248, 100 ); color: black;";



    struct  RecordedMsg {
        int         deltaMSec;
        uint64      count;
        PCanMessage msg;
        str         info;
    }; // RecordedMsg



    class CanMessageLogger : public pdpCanMessageSubscriptionBase {
    public:
        CanMessageLogger( const byte busIndex );
        void    receiveMessageCompressed( PCanMessage msg, const int count, const int deltaMSec, const str info );
    }; // CanMessageLogger



    class MainWindow : public AppManWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        QTimer  tmr;

        void    declareVersion();
        void    addMessage( PCanMessage* msg, const int count, const int deltaMSec, const str info );
        void    setLineAddress( const int index, RecordedMsg *logMsg );
        void    setLineData(    const int index, RecordedMsg *logMsg );

        bool    eventFilter(QObject *obj, QEvent *event);
        void    onMaximize();
        void    onShrink();

        void    onSignalReceived_Sleep();
        void    onSignalReceived_WakeUp();
        void    onSignalReceived_Minimize();    // AppMan asks us to minimuze (neighbour maximizes, logged out)
        void    onSignalReceived_Maximize();    // AppMan asks us to maximize
    public slots:
        void    timerTimeOut();

        void    btnClick_Pause();
        void    btnClick_FilterSrc();
        void    btnClick_FilterMsg();
        void    btnClick_ToggleHex();
        void    btnClick_ToggleJ1939();
        void    btnClick_ToggleBus();
        void    btnClick_Clear();
        void    btnClick_TraceStart();
        void    btnClick_TraceEnd();
        void    btnClick_Minimize();

        void    btnClick_ScrollUp();
        void    btnClick_ScrollDn();

    private:
        Ui::MainWindow*     ui;

        MyAppManLink*       appManLink;
        DataPool*           dataPool;                           // To Get & Subscribe on the Entities we need in this APP
        CanMessageLogger*   subscriptions[4];                   // For each BUS, 1 subscription (but only 1 is active at a time)
        CanMessageLogger*   cSubscription;                      // Current selected BUS

        // Recorded
        RecordedMsg     loggedMsg[ MAXROWS ];
        int             loggedCnt;                              // Unique messages

        // State
        bool            recording;
        bool            showAsJ1939;
        bool            showAsHex;
        bool            showAsBin;
        int             fltrSource;
        int             fltrMsgId;
        int             fltrBusIndex;

    }; // MainWindow


#endif // MAINWINDOW_H
