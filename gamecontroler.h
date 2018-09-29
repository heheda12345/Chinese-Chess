#ifndef GAMECONTROLER_H
#define GAMECONTROLER_H

#include <QWidget>
#include <QPoint>
#include <QTimer>
#include <QLCDNumber>

class GameControler : public QWidget
{
    Q_OBJECT
public:
    explicit GameControler(QWidget *parent = nullptr);
    enum Type
    {
        marshal = 0,
        guardian = 1,
        elephant = 3,
        horse = 5,
        car = 7,
        cannon = 9,
        soldier = 11,
        all = 16
    };
    static const int MUL = 60;
    static const int INTERVAL = 50;
    Type type(int id) const;
    Type typeByTID(int t_id) const;
    int X(int x) const;
    int Y(int y) const;
    bool validPos(int x, int y) const;
    int getCurPlayer() const;
    QString getGame() const;
    void setMove(QString st);
    void setGame(const QString &st = "");
    void setStart();
    void setEnd();
    void setPlayerID(int id);
    void setLCD(QLCDNumber* lcd);
    bool noChess(int x, int y) const;
    QTimer* timer;
signals:
    void move(QString st);
    void win();
    void lose();
    void jiangjun();
    void beijiang();
    void timeout();
public slots:
    void move(int per, int id, int x, int y);
    void decLCD();
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    bool check(int p, int id, int x, int y);
    int a[9][9];
    int curPlayer;
    bool isStart;
    int playerID;
    int choosed_p;
    int choosed_id;
    int mouseX, mouseY;
    QPoint p[2][16];
    QLCDNumber *remain;
};

#endif // GAMECONTROLER_H
