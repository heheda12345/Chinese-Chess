#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMediaPlayer>
#include "initdialog.h"

namespace Ui {
class MainWindow;
class Connecting;
class InitialWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //explicit MainWindow(InitDialog::Mode mode, QString ip_, int port_, QWidget *parent = 0);
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum Status { initial, connecting, beforeStart, running, end };
    void setStatus(Status s);

public slots:
    void load();
    void show();
    void move(QString st);
    void send(QString st);
    void failByTimeOut();
    void startGame();
    //void startGame2();
    void exportGame();
    void exportGame2();
    void giveIn();
    void win();
    void lose();
    void jiangjun();
    void beijiang();

private slots:
    void tryConnect(); // for both
    void stopConnect(); // for both
    void acceptConnection();
    void setConnected();
    void failConnect();
    void recvMessage();

private:
    void initServer();
    void connectHost();
    Ui::MainWindow *ui;
    Ui::Connecting *uiConnect;
    Ui::InitialWindow *uiInitial;
    InitDialog::Mode m_mode;
    QString ip;
    int port;
    QTcpServer *listenSocket;
    QTcpSocket *socket;
    QMediaPlayer *jiangPlayer;
    QMediaPlayer *beiPlayer;
    InitDialog* dia;
    Status status;
    int playerID;
};

#endif // MAINWINDOW_H
