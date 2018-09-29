#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_connecting.h"
#include "ui_initialwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QBitArray>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include "initdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), uiConnect(new Ui::Connecting), uiInitial(new Ui::InitialWindow),
    status(Status::initial)
    //m_mode(dia.getMode()), ip(dia.IP()), port(dia.port())
{
    //qDebug() << m_mode << " " << ip << " " << port;
    uiInitial->setupUi(this);
    connect(uiInitial->actionConnect, SIGNAL(triggered(bool)), this, SLOT(tryConnect()));
    jiangPlayer = new QMediaPlayer(this);
    jiangPlayer->setMedia(QMediaContent(QUrl("qrc:/music/jiang")));
    beiPlayer = new QMediaPlayer(this);
    beiPlayer->setMedia(QMediaContent(QUrl("qrc:/music/bei")));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initServer() // for server
{
    qDebug() << "init";
    this->listenSocket =new QTcpServer;
    this->listenSocket->listen(QHostAddress::Any,port);
    connect(this->listenSocket, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void MainWindow::acceptConnection() // for server
{
    qDebug() <<"accept";
    socket = this->listenSocket->nextPendingConnection();
    setStatus(beforeStart);
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(recvMessage()));
}

void MainWindow::connectHost() // for client
{
    qDebug() << "connect";
    this->socket = new QTcpSocket;
    this->socket->connectToHost(ip, port);
    connect(this->socket, SIGNAL(connected()), this, SLOT(setConnected()));
    connect(this->socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(failConnect()));
}

void MainWindow::setConnected() //for client
{
    qDebug() << "success";
    setStatus(beforeStart);
    QObject::connect(this->socket,SIGNAL(readyRead()),this,SLOT(recvMessage()));
}

void MainWindow::failConnect()
{
    qDebug() << "fail";
    setStatus(initial);
}

void MainWindow::send(QString st)
{
    qDebug() << "send:" << st;
    QByteArray *array =new QByteArray;
    array->clear();
    array->append(st);
    socket->write(array->data());
}

void MainWindow::show()
{
    return QMainWindow::show();
}

void MainWindow::move(QString st)
{
    ui->label->setText(QString::number(ui->widget->getCurPlayer()));
    send(st);
}

void MainWindow::failByTimeOut()
{
    setStatus(end);
    send("timeout");
    //lose();
    QMessageBox::information(this, "lose", "超时，你输了");

}

void MainWindow::startGame()
{
    setStatus(running);
    send("start");
}

void MainWindow::exportGame()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save File"),
                                     QDir::currentPath(),
                                     tr("*.txt"));
    if (path.isEmpty())
            return;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "can not open " << path;
        return;
    }
    QTextStream os(&file);
    os << ui->widget->getGame();
    file.close();

}

void MainWindow::exportGame2()
{
    exportGame();
    //send("export");
}

void MainWindow::giveIn()
{
    setStatus(end);
    send("givein");
    QMessageBox::information(this, "lose", "弃权，你输了");
}

void MainWindow::win()
{
    setStatus(end);
    QMessageBox::information(this, "win", "你赢了");
}

void MainWindow::lose()
{
    setStatus(end);
    QMessageBox::information(this, "lose", "你输了");
}

void MainWindow::jiangjun()
{
    qDebug() << "jiangjun " << playerID;
    jiangPlayer->play();

   //send("jiangjun");
}

void MainWindow::beijiang()
{
    qDebug() << "beijiang " << playerID;
    beiPlayer->play();
}
/*
void MainWindow::win()
{
    ui->widget->setEnd();
}

void MainWindow::lose()
{
    ui->widget->setEnd();
}
*/


void MainWindow::setStatus(MainWindow::Status s)
{//initial, connecting, beforeStart, running, end
    switch (s) {
    case initial:
        uiInitial->setupUi(this);
        connect(uiInitial->actionConnect, SIGNAL(triggered(bool)), this, SLOT(tryConnect()));
        break;
    case connecting:
        break;
    case beforeStart:
        if (status != beforeStart && status != running && status != end)
        {
            ui->setupUi(this);
            ui->widget->setPlayerID(playerID);
            ui->widget->setLCD(ui->lcdNumber);
            ui->lcdNumber->display(GameControler::INTERVAL);
            connect(ui->widget, SIGNAL(move(QString)), this, SLOT(move(QString)));
            connect(ui->widget, SIGNAL(win()), this, SLOT(win()));
            connect(ui->widget, SIGNAL(lose()), this, SLOT(lose()));
            connect(ui->widget, SIGNAL(jiangjun()), this, SLOT(jiangjun()));
            connect(ui->widget, SIGNAL(beijiang()), this, SLOT(beijiang()));
            connect(ui->actionImport, SIGNAL(triggered(bool)), this, SLOT(load()));
            connect(ui->widget, SIGNAL(timeout()), this, SLOT(failByTimeOut()));
            connect(ui->pushButtonStart, SIGNAL(clicked(bool)), this, SLOT(startGame()));
            connect(ui->pushButtonSave, SIGNAL(clicked(bool)), this, SLOT(exportGame()));
            connect(ui->pushButtonGiveIn, SIGNAL(clicked(bool)), this, SLOT(giveIn()));
            ui->label->setText("0");
        }
        ui->pushButtonGiveIn->setDisabled(true);
        ui->pushButtonStart->setEnabled(true);
        ui->actionImport->setEnabled(true);
        break;
    case running:
        ui->label->setText(QString::number(ui->widget->getCurPlayer()));
        ui->widget->setStart();
        ui->pushButtonGiveIn->setEnabled(true);
        ui->pushButtonStart->setEnabled(false);
        ui->actionImport->setDisabled(true);
        break;
    case end:
        ui->widget->setEnd();
        ui->pushButtonGiveIn->setDisabled(true);
        ui->actionImport->setEnabled(true);
        ui->pushButtonStart->setDisabled(true);
        break;
    default:
        break;
    }
    status = s;
}

void MainWindow::load()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"),
                                 QDir::currentPath(),
                                 tr("*.txt"));
    if (path.isEmpty())
        return;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "can not open " << path;
        return;
    }
    qDebug() << path;
    QTextStream in(&file);
    QString txt = in.readAll();
    qDebug() <<"txt\n"<<  txt;
    ui->widget->setGame(txt);
    setStatus(beforeStart);
    send("load_"+txt);
    file.close();
    ui->label->setText(QString::number(ui->widget->getCurPlayer()));
}

void MainWindow::recvMessage()
{
    QString info;
    info += this->socket->readAll();
    qDebug() << "receive" <<playerID << " : " << info;
    if (info.left(4) == "move")
    {
        ui->widget->setMove(info);
        ui->label->setText(QString::number(ui->widget->getCurPlayer()));
    }
    else if (info.left(4) == "load")
    {
        ui->widget->setGame(info.mid(5));
        ui->label->setText(QString::number(ui->widget->getCurPlayer()));
        setStatus(beforeStart);
    }
    else if (info.left(7) == "timeout")
    {
        //win();
        setStatus(end);
        QMessageBox::information(this, "win", "对方超时,你赢了");
    }
    else if (info.left(5) == "start")
        setStatus(running);
    else if (info.left(6) == "givein")
    {
        setStatus(end);
        QMessageBox::information(this, "win", "对方弃权,你赢了");
    }
}

void MainWindow::tryConnect()
{
    InitDialog dia(this);
    if (dia.exec() == QDialog::Accepted)
    {
        status = connecting;
        m_mode = dia.getMode();
        ip = dia.IP();
        port = dia.port();
        uiInitial->menubar->hide();
        uiInitial->statusbar->hide();
        qDebug() << m_mode << " " << ip << " " << port;
        if (m_mode == InitDialog::server)
        {
            qDebug() <<"server";
            playerID = 0;
            uiConnect->setupUi(this);
            uiConnect->label_2->setText(ip);
            uiConnect->label_4->setText(QString::number(port));
            connect(uiConnect->exitButton, SIGNAL(clicked(bool)), this, SLOT(stopConnect()));
            initServer();
        }
        if (m_mode == InitDialog::client)
        {
            qDebug() <<"client";
            playerID = 1;
            uiConnect->setupUi(this);
            uiConnect->label_2->setText(ip);
            uiConnect->label_4->setText(QString::number(port));
            connect(uiConnect->exitButton, SIGNAL(clicked(bool)), this, SLOT(stopConnect()));
            connectHost();
        }
    }
}

void MainWindow::stopConnect()
{
    setStatus(initial);
    if(playerID == 0)
        listenSocket->close();
}
