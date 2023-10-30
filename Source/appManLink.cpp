#include "appManLink.h"
#include <QDebug>


    AppManWindow::AppManWindow(QWidget *parent) :
            QMainWindow(parent)
    {
    } // createa



//*************************************************************************
//  APP MAN

    MyAppManLink::MyAppManLink( str appIdName, AppManWindow* window )
        : AppManLink( appIdName, NULL )
    {
        this->_window           = window;
    } // create


    void MyAppManLink::onSignalReceived_Sleep()
    {
//        this->_window->hide();`   <-- dont
        AppManLink::onSignalReceived_Sleep();

        this->_window->onSignalReceived_Sleep();
    } // onSignalReceived_Sleep


    void MyAppManLink::onSignalReceived_WakeUp()
    {
//        this->_window->show();
        AppManLink::onSignalReceived_WakeUp();

        this->_window->onSignalReceived_WakeUp();
    } // onSignalReceived_WakeUp


    void MyAppManLink::onSignalReceived_Close()
    {
        AppManLink::onSignalReceived_Close();
        this->_window->onSignalReceived_Close();
    } // onSignalReceived_Close


    void MyAppManLink::onSignalReceived_Minimize()
    {   // AppMan asks us to minimuze (neighbour maximizes, logged out)

        this->_window->onSignalReceived_Minimize();
    } // onSignalReceived_Minimize

    void MyAppManLink::onSignalReceived_Maximize()
    {   // AppMan asks us to minimuze (neighbour maximizes, logged out)

        this->_window->onSignalReceived_Maximize();
    } // onSignalReceived_Maximize


    void MyAppManLink::onSignalReceived_PrivateMessage(   const str appSignal, const str sender, QStringList arguments )
    {   // Another app specifically sent something to us
        this->_window->onSignalReceived_PrivateMessage( appSignal, sender, arguments );
    } // onSignalReceived_PrivateMessage

    void MyAppManLink::onSignalReceived_BroadcastMessage( const str appSignal, const str sender, QStringList arguments )
    {   // Another app specifically sent something to us
        this->_window->onSignalReceived_BroadcastMessage( appSignal, sender, arguments );
    } // onSignalReceived_BroadcastMessage

    void MyAppManLink::onSignalReceived_Custom( const str appSignal, QStringList arguments )
    {   // Another app specifically sent something to us
        AppManLink::onSignalReceived_Custom( appSignal, arguments );
        this->_window->onSignalReceived_Custom( appSignal, arguments );
    } // onSignalReceived_Custom

