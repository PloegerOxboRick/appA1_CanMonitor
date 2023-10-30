#ifndef APPMANLINK_H
#define APPMANLINK_H

#include <Code/Platform/appManLink.h>
#include <Code/oxTypes.h>
#include <QWidget>
#include <QMainWindow>


    class AppManWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        AppManWindow( QWidget* parent );
        virtual void    onSignalReceived_Sleep() {}
        virtual void    onSignalReceived_WakeUp() {}
        virtual void    onSignalReceived_Close() {}
        virtual void    onSignalReceived_Minimize() {}    // AppMan asks us to minimuze (neighbour maximizes, logged out)
        virtual void    onSignalReceived_Maximize() {}    // AppMan asks us to maximize
        virtual void    onSignalReceived_PrivateMessage(   const str appSignal, const str sender, QStringList arguments ) {}  // Another app specifically sent something to us
        virtual void    onSignalReceived_BroadcastMessage( const str appSignal, const str sender, QStringList arguments ) {}
        virtual void    onSignalReceived_Custom( const str appSignal, QStringList arguments ) {}
    }; // AppManWindow



    class MyAppManLink  : public AppManLink
    {
    private:
        AppManWindow*   _window;
    public:
        MyAppManLink( str appIdName, AppManWindow* window );

        void    onSignalReceived_Sleep();
        void    onSignalReceived_WakeUp();
        void    onSignalReceived_Close();
        void    onSignalReceived_Minimize();    // AppMan asks us to minimuze (neighbour maximizes, logged out)
        void    onSignalReceived_Maximize();    // AppMan asks us to minimuze (neighbour maximizes, logged out)

        void    onSignalReceived_PrivateMessage(   const str appSignal, const str sender, QStringList arguments );  // Another app specifically sent something to us
        void    onSignalReceived_BroadcastMessage( const str appSignal, const str sender, QStringList arguments );
        void    onSignalReceived_Custom( const str appSignal, QStringList arguments );
    }; // MyAppManLink


#endif // APPMANLINK_H
