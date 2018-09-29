#ifndef INITDIALOG_H
#define INITDIALOG_H

#include <QDialog>
#include <QString>
#include <QRegExp>

namespace Ui {
class InitDialog;
}

class InitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InitDialog(QWidget *parent = 0);
    enum Mode { server, client };
    ~InitDialog();
    QString IP() const;
    int port() const;
    Mode getMode() const;

public slots:
    void changeMode(int mode);
protected:
    void accept();

private:
    QString getIPAddress() const;
    Ui::InitDialog *ui;
    QRegExp IP_reg, port_reg;
    Mode m_mode;
};

#endif // INITDIALOG_H
