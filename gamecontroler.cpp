#include "gamecontroler.h"

#include <regex>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QPen>

GameControler::GameControler(QWidget *parent) : QWidget(parent)
{
    for (int i=0; i<2; i++)
        for (int j=0; j<16; j++)
            p[i][j] = QPoint(-1,-1);
    setMouseTracking(true);
    choosed_p = -1;
    curPlayer = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(decLCD()));
}

GameControler::Type GameControler::typeByTID(int t_id) const
{
    if (t_id == 0)
        return marshal;
    if (t_id == 1)
        return guardian;
    if (t_id == 2)
        return elephant;
    if (t_id == 3)
        return horse;
    if (t_id == 4)
        return car;
    if (t_id == 5)
        return cannon;
    if (t_id == 6)
        return soldier;
    return all;
}

int GameControler::X(int x) const
{
    return (x+30)/MUL-1;
}

int GameControler::Y(int y) const
{
    return (y+35)/(MUL-4)-1;
}

bool GameControler::validPos(int x, int y) const
{
    return x>=0 && x<=8 && y>=0 && y<=9;
}

int GameControler::getCurPlayer() const
{
    return curPlayer;
}

QString GameControler::getGame() const
{
    QString ret = "";
    if (curPlayer == 0)
        ret.append("red\r\n");
    else
        ret.append("black\r\n");
    for (int i=0; i<7; i++)
    {
        int l = this->typeByTID(i);
        int r = this->typeByTID(i+1);
        int cnt = 0;
        QString s = "";
        for (int j=l; j<r; j++)
            if (p[curPlayer][j].x() != -1)
            {
                cnt++;
                s.append(" <"+QString::number(p[curPlayer][j].x())+","+QString::number(9-p[curPlayer][j].y())+">");
            }
        ret.append(QString::number(cnt));
        ret.append(s);
        ret.append("\r\n");
    }
    qDebug() << ret;
    if (curPlayer == 0)
        ret.append("black\r\n");
    else
        ret.append("red\r\n");
    for (int i=0; i<7; i++)
    {
        int l = this->typeByTID(i);
        int r = this->typeByTID(i+1);
        int cnt = 0;
        QString s = "";
        for (int j=l; j<r; j++)
            if (p[curPlayer^1][j].x() != -1)
            {
                cnt++;
                s.append(" <"+QString::number(p[curPlayer^1][j].x())+","+QString::number(9-p[curPlayer^1][j].y())+">");
            }
        ret.append(QString::number(cnt));
        ret.append(s);
        ret.append("\r\n");
    }
    return ret;
}

void GameControler::setMove(QString st)
{
    std::smatch m;
    std::regex e ("(\\d+)");
    int cnt = 0;
    std::string s = st.toStdString();
    int a[4];
    while (std::regex_search(s, m, e))
    {
        a[cnt++] = QString::fromStdString(m[1].str()).toInt();
        qDebug("%d",a[cnt-1]);
        s = m.suffix().str();
    }
    move(a[0], a[1], a[2], a[3]);
    update();
}

void GameControler::setGame(const QString &st)
{
    if (st == "")
    {
        p[1][0] = QPoint(4,0);
        p[1][1] = QPoint(3,0);
        p[1][2] = QPoint(5,0);
        p[1][3] = QPoint(2,0);
        p[1][4] = QPoint(6,0);
        p[1][5] = QPoint(1,0);
        p[1][6] = QPoint(7,0);
        p[1][7] = QPoint(0,0);
        p[1][8] = QPoint(8,0);
        p[1][9] = QPoint(1,2);
        p[1][10] = QPoint(7,2);
        p[1][11] = QPoint(0,3);
        p[1][12] = QPoint(2,3);
        p[1][13] = QPoint(4,3);
        p[1][14] = QPoint(6,3);
        p[1][15] = QPoint(8,3);
        for (int i=0; i<16; i++)
        {
            p[0][i] = p[1][i];
            p[0][i].setY(9-p[1][i].y());
        }
    }
    else
    {
        qDebug() << "set game\n" << st;
        QVector<std::string> stt;
        std::smatch m1;
        std::regex e1 ("(.*)\r\n");
        std::string s1 = st.toStdString();
        while (std::regex_search(s1, m1, e1))
        {
             stt.push_back(m1[1].str());
             s1 = m1.suffix().str();
        }
        //qDebug() << stt;
        qDebug() <<"stt:";
        for (std::string st: stt)
        {
            qDebug() <<QString::fromStdString(st);
        }

        bool cur = (stt[0].substr(0,5) == "black");
        qDebug() << "cur:" << cur;
        curPlayer = cur;
        for (int i=1; i<=7; i++)
        {
            std::smatch m;
            std::regex e ("<(\\d),(\\d)>");
            int cnt = typeByTID(i-1);
            std::string s = stt[i];
            while (std::regex_search(s, m, e))
            {
                p[cur][cnt].setX(QString::fromStdString(m[1].str()).toInt());
                p[cur][cnt].setY(9-QString::fromStdString(m[2].str()).toInt());
                //qDebug() << QString::fromStdString(m[1].str()) << " " << QString::fromStdString(m[2].str());
                cnt++;
                s = m.suffix().str();
            }
            while (cnt < typeByTID(i))
            {
                p[cur][cnt].setX(-1);
                p[cur][cnt].setY(-1);
                cnt++;
            }
        }
        cur ^= 1;
        for (int i=9; i<=15; i++)
        {
            std::smatch m;
            std::regex e ("<(\\d),(\\d)>");
            int cnt = typeByTID(i-9);
            std::string s = stt[i];
            while (std::regex_search(s, m, e))
            {
                p[cur][cnt].setX(QString::fromStdString(m[1].str()).toInt());
                p[cur][cnt].setY(9-QString::fromStdString(m[2].str()).toInt());
                //qDebug() << QString::fromStdString(m[1].str()) << " " << QString::fromStdString(m[2].str());
                cnt++;
                s = m.suffix().str();
            }
            while (cnt < typeByTID(i-8))
            {
                p[cur][cnt].setX(-1);
                p[cur][cnt].setY(-1);
                cnt++;
            }
        }
    }
    update();
}

void GameControler::setStart()
{
    isStart = 1;
    int has = 0;
    for (int i=0; i<2; i++)
        for (int j=0; j<16; j++)
        {
            has += (p[i][j].x()!=-1);
            has += (p[i][j].y()!=-1);
        }
    if (has == 0)
        setGame();
    if (curPlayer == playerID)
    {
        remain->display(INTERVAL);
        timer->start(1000);
    }
}

void GameControler::setEnd()
{
    timer->stop();
    isStart = 0;
    choosed_id = choosed_p = -1;
}

void GameControler::setPlayerID(int id)
{
    playerID = id;
}

void GameControler::setLCD(QLCDNumber *lcd)
{
    remain = lcd;
}

void GameControler::decLCD()
{
    int t = remain->value();
    t--;
    remain->display(t);
    if (t==0)
    {
        choosed_p = choosed_id = -1;
        emit timeout();
    }
}

bool GameControler::noChess(int x, int y) const
{
    for (int i=0; i<2; i++)
        for (int j=0; j<16; j++)
            if (p[i][j].x() == x && p[i][j].y() == y)
                return 0;
    return 1;
}

void GameControler::move(int per, int id, int x, int y)
{
    timer->stop();
    for (int i=0; i<2; i++)
        for (int j=0; j<16; j++)
            if (p[i][j].x() == x && p[i][j].y() == y)
            {
                p[i][j].setX(-1);
                p[i][j].setY(-1);
            }
    p[per][id].setX(x);
    p[per][id].setY(y);
    update();

    bool jiang=0;
    for (int j=0; j<16; j++)
        if (check(playerID, j, p[playerID^1][0].x(), p[playerID^1][0].y()))
            jiang = 1;
    if (jiang)
        emit jiangjun();

    jiang = 0;
    for (int j=0; j<16; j++)
        if (check(playerID^1, j, p[playerID][0].x(), p[playerID][0].y()))
            jiang = 1;
    if (jiang)
        emit beijiang();

    curPlayer ^= 1;
    if (p[playerID][0].x() == -1)
    {
        choosed_p = choosed_id = -1;
        emit lose();
        return;
    }
    if (p[playerID^1][0].x() == -1)
    {
        choosed_p = choosed_id = -1;
        emit win();
        return;
    }
    if (playerID == curPlayer)
    {
        remain->display(INTERVAL);
        timer->start(1000);
    }
}

void GameControler::paintEvent(QPaintEvent *event)
{
    QPixmap pix = QPixmap(":/chess/b").scaled(this->rect().size(), Qt::KeepAspectRatio);
    QPainter painter(this);
    painter.drawPixmap(this->rect(), pix);
    for (int i=0; i<2; i++)
        for (int j=0; j<16; j++)
        {
            if (p[i][j].x() == -1) continue;
            QString st = QString::number(i)+QString::number(type(j));
            QPixmap pix = QPixmap(":/chess/"+st).scaled(QSize(MUL,MUL-4), Qt::KeepAspectRatio);
            painter.drawPixmap((p[i][j].x()+1)*MUL-30, (p[i][j].y()+1)*(MUL-4)-35, MUL, MUL-4, pix);
        }
    if (choosed_p != -1)
    {
        QPen pen("#9B30FF");
        pen.setWidth(3);
        painter.setPen(pen);
        painter.setBrush(Qt::transparent);
        painter.drawEllipse((p[choosed_p][choosed_id].x()+1)*MUL-30, (p[choosed_p][choosed_id].y()+1)*(MUL-4)-35, MUL, MUL-4);

        QString st = QString::number(choosed_p)+QString::number(type(choosed_id));
        QPixmap pix = QPixmap(":/chess/"+st).scaled(QSize(MUL,MUL-4), Qt::KeepAspectRatio);
        painter.drawPixmap(mouseX-MUL/2, mouseY-(MUL-4)/2, MUL, MUL-4, pix);
/*
        for (int i=0; i<=8; i++)
            for (int j=0; j<=9; j++)
                if (check(choosed_p, choosed_id, i, j))
                    painter.drawEllipse((i+1)*MUL-30, (j+1)*(MUL-4)-35, MUL, MUL-4);*/
    }
}

void GameControler::mousePressEvent(QMouseEvent *event)
{
    if (!isStart)
        return;
    if (playerID != curPlayer)
        return;
    if (choosed_p == -1)
    {
        int x = X(event->pos().x()), y = Y(event->pos().y());
        if (!validPos(x,y)) return;
        for (int j=0; j<16; j++)
            if (p[playerID][j].x() == x && p[playerID][j].y() == y)
            {
                choosed_p = playerID;
                choosed_id = j;
            }
    }
    else
    {
        int x = X(event->pos().x()), y = Y(event->pos().y());
        if (!validPos(x,y)) return;
        if (!check(choosed_p, choosed_id, x, y))
            return;
        move(choosed_p, choosed_id, x, y);
        emit move("move_"+QString::number(choosed_p)+"_"+QString::number(choosed_id)+
                  "_"+QString::number(x)+"_"+QString::number(y)); //must after move()
        choosed_p = -1;
    }
}

void GameControler::mouseMoveEvent(QMouseEvent *event)
{
    if (choosed_id!=-1)
    {
        mouseX = event->pos().x();
        mouseY = event->pos().y();
        update();
    }
}

bool GameControler::check(int per, int id, int x, int y)
{
    if (!validPos(x, y))
        return 0;
    if (p[per][id].x() == -1)
        return 0;
    if (per == 1)
    {
        for (int i=0; i<2; i++)
            for (int j=0; j<16; j++)
                p[i][j].setY(9-p[i][j].y());
        y = 9-y;
    }
    int curX = p[per][id].x(), curY = p[per][id].y();
    int dx = x-curX, dy = y-curY;
    bool ok = 1;
    //不能吃自己
    for (int i=0; i<2; i++)
        for (int j=0; j<16; j++)
            if (p[i][j].x() == x && p[i][j].y() == y && i == per)
            {
                ok = 0;
            }
    //移动方式
    switch (type(id))
    {
    case marshal:
    {
        if (x<3 || x>5)
            ok = 0;
        if (y<7)
            ok = 0;
        if ((dx==0) == (dy==0))
            ok = 0;
        if (abs(dx)>1 || abs(dy)>1)
            ok = 0;
        break;
    }
    case guardian:
    {
        if (x<3 || x>5)
            ok = 0;
        if (y<7)
            ok = 0;
        if (abs(dx)!=1 || abs(dy)!=1)
            ok = 0;
        break;
    }
    case elephant:
    {
        if (y<5)
            ok = 0;
        if (abs(dx)!=2 || abs(dy)!=2)
            ok = 0;
        else if (!noChess(curX+dx/2, curY+dy/2))
            ok = 0;
        break;
    }
    case horse:
    {
        if (abs(dx)<1 || abs(dx)>2)
            ok = 0;
        else if (abs(dy)<1 || abs(dy)>2)
            ok = 0;
        else if ((abs(dx)==1) == (abs(dy)==1))
            ok = 0;
        else if (!noChess(curX+dx/2, curY+dy/2))
            ok = 0;
        break;
    }
    case car:
    {
        if (dx == 0)
        {
            for (int j = std::min(y, curY)+1; j <= std::max(y, curY)-1; j++)
                if (!noChess(x,j))
                    ok = 0;
        }
        else if (dy == 0)
        {
            for (int i = std::min(x, curX)+1; i <= std::max(x, curX)-1; i++)
                if (!noChess(i,y))
                    ok = 0;
        }
        else ok = 0;
        break;
    }
    case cannon:
    {
        int cnt = 0;
        if (dx == 0)
        {
            for (int j = std::min(y, curY)+1; j <= std::max(y, curY)-1; j++)
                if (!noChess(x,j))
                    cnt++;
        }
        else if (dy == 0)
        {
            for (int i = std::min(x, curX)+1; i <= std::max(x, curX)-1; i++)
                if (!noChess(i,y))
                    cnt++;
        }
        else ok = 0;
        if (noChess(x,y))
        {
            if (cnt != 0)
                ok = 0;
        }
        else
        {
            if (cnt != 1)
                ok = 0;
        }
        break;
    }
    case soldier:
    {
        if ((dx==0) == (dy==0))
            ok = 0;
        if (abs(dx)>1 || abs(dy)>1)
            ok = 0;
        if (curY>=5)
        {
            if (dy != -1)
                ok = 0;
        }
        else
        {
            if (dy == 1)
                ok = 0;
        }
        break;
    }
    default:
        ok = 0;
    }
    //对将
    //吃将不会导致对将
    if (QPoint(x,y) != p[0][0] && QPoint(x,y) != p[1][0])
    {
        int px = p[per][id].x(), py = p[per][id].y();
        p[per][id].setX(x);
        p[per][id].setY(y);
        if (p[0][0].x() == p[1][0].x())
        {
            //qDebug() << "duijiang";
            int cnt = 0;
            for (int j = std::min(p[0][0].y(), p[1][0].y())+1; j <= std::max(p[0][0].y(), p[1][0].y())-1; j++)
                if (!noChess(p[0][0].x(), j))
                    cnt++;
            if (cnt == 0)
                ok = 0;
        }
        p[per][id].setX(px);
        p[per][id].setY(py);
    }
    if (per == 1)
    {
        y = 9-y;
        for (int i=0; i<2; i++)
            for (int j=0; j<16; j++)
                p[i][j].setY(9-p[i][j].y());
    }
    return ok;
}

GameControler::Type GameControler::type(int id) const
{
    if (id >= soldier)
        return soldier;
    if (id >= cannon)
        return cannon;
    if (id >= car)
        return car;
    if (id >= horse)
        return horse;
    if (id >=elephant)
        return elephant;
    if (id >= guardian)
        return guardian;
    if (id >= marshal)
        return marshal;
    return all;
}
