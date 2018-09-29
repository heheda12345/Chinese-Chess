#include "initdialog.h"
#include "ui_initdialog.h"

#include <QRegExpValidator>
#include <QMessageBox>
#include <QHostAddress>
#include <QNetworkInterface>

InitDialog::InitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InitDialog)
{
    ui->setupUi(this);
    QString value = "([0-9]|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])";
    IP_reg = QRegExp(value+"\\."+value+"\\."+value+"\\."+value);
    ui->lineEdit->setValidator(new QRegExpValidator(IP_reg, ui->lineEdit));
    ui->lineEdit->setText(getIPAddress());
    ui->lineEdit->setReadOnly(true);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMode(int)));
    m_mode = server;
}

InitDialog::~InitDialog()
{
    delete ui;
}

QString InitDialog::IP() const
{
    return ui->lineEdit->text();
}

int InitDialog::port() const
{
    return ui->spinBox->value();
}

InitDialog::Mode InitDialog::getMode() const
{
    return m_mode;
}

QString InitDialog::getIPAddress() const
{
    QString ipAddress;
//    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
//    for (int i = 0; i < ipAddressesList.size(); ++i)
//    {
//        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&  ipAddressesList.at(i).toIPv4Address())
//        {
//            ipAddress = ipAddressesList.at(i).toString();
//            break;
//        }
//    }
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddress;
}

void InitDialog::changeMode(int mode)
{
    if (mode == server)
    {
        ui->lineEdit->setText(getIPAddress());
        ui->lineEdit->setReadOnly(true);
    }
    if (mode == client)
    {
        ui->lineEdit->setText(getIPAddress());
        ui->lineEdit->setReadOnly(false);
    }
    m_mode = Mode(mode);
    qDebug() <<"mode:" << m_mode;
}


void InitDialog::accept()
{
    QString st = ui->lineEdit->text();
    if (!IP_reg.exactMatch(st))
    {
        QMessageBox::warning(this, "Error", "Host IP is incomplete!");
        return;
    }
    qDebug() << getMode() << " " << IP() << " " << port();
    return QDialog::accept();
}

