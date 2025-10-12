#ifndef PLAYERCHARACTER_H
#define PLAYERCHARACTER_H

#include <QObject>
#include <QWidget>
#include <QPointF>
#include <QPixmap>
#include <QPainter>
#include <QVector>
#include <QTransform>

class playerCharacter:public QObject
{
    Q_OBJECT
public:
    enum Direction { Up, Down, Left, Right };

    explicit playerCharacter(QObject *parent = nullptr, int num = 1);

    // 尝试移动（由 MainWindow 传入 dx,dy（逻辑坐标单位）和地图）
    // 返回是否实际移动成功
    void Move(double dx, double dy, double leftLimit, double rightLimit,
              double topLimit, double bottomLimit, const QVector<QVector<int>> &board);

    // 绘制（使用逻辑->场景变换）
    void draw(QPainter *painter, const QTransform &logicToScen, bool freeze);

    QPointF position() const { return pixPos; }

    double getSpeed() const {return speed;}

    void setPosition(const QPointF &p)
    {
        pixPos = p;
        x = pixPos.x();
        y = pixPos.y();
    }

    void setSpeed(double s) { speed = s; }

    Direction getDir(){return dir;}

    QPoint selectTips(const QVector<QVector<int>> &board);

signals:
    void positionChanged(QPointF newPos);

private:
    // 碰撞检测（以 AABB 方式检测玩家矩形是否与非0格子重叠）
    void canMove(double dx, double dy, const QVector<QVector<int>> &board) const;

    QPointF pixPos;
    double x = 0, y = 0;
    Direction dir = Down;

    // 玩家在逻辑坐标系中的占用尺寸（以格子为单位，默认小于1），稍微小一点，不然容易卡住
    double pixwid = 0.4;
    double pixhei = 0.4;

    double speed; // 单位：逻辑单位/帧（可由 MainWindow 调整）

    // 方向图片
    QImage imgUp, imgDown, imgLeft, imgRight;

    //玩家序号
    int number = 1;

};

#endif // PLAYERCHARACTER_H
