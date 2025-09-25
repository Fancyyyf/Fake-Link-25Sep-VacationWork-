#include "playercharacter.h"
#include <QtMath>

playerCharacter::playerCharacter(QObject *parent)
    : QObject(parent),
    pixPos(0.0, 0.0), // 初始位置（可改）
    dir(Down)
{
    // 资源路径按你的资源文件调整
    imgUp    = QPixmap(":/images/Images/Player/up.png");
    imgDown  = QPixmap(":/images/Images/Player/down.png");
    imgLeft  = QPixmap(":/images/Images/Player/left.png");
    imgRight = QPixmap(":/images/Images/Player/right.png");
}

bool playerCharacter::canOccupyRect(double left, double top, double right, double bottom,
                           const QVector<QVector<int>> &board) const
{
    // int rows = board.size();
    // int cols = board[0].size();

    // // 需要检查与该矩形相交的所有格子
    // int cMin = qFloor(left);
    // int cMax = qFloor(qMax(left, right - 1e-9)); // 右边界可能正好是整数
    // int rMin = qFloor(top);
    // int rMax = qFloor(qMax(top, bottom - 1e-9));

    // // 保证索引在范围内
    // cMin = qMax(0, cMin);
    // rMin = qMax(0, rMin);
    // cMax = qMin(cols - 1, cMax);
    // rMax = qMin(rows - 1, rMax);

    // for (int r = rMin; r <= rMax; ++r) {
    //     for (int c = cMin; c <= cMax; ++c) {
    //         if (board[r][c] != 0) return false; // 非零格子为障碍
    //     }
    // }
    return true;
}

void playerCharacter::Move(double dx, double dy, const QVector<QVector<int>> &board)
{
    double newX = x += dx;
    double newY = y += dy;

    x = newX;
    y = newY;
    pixPos = QPointF(newX, newY);


    // // 目标中心
    // QPointF newCenter = pos + QPointF(dx, dy);

    // // 计算矩形（center -> left/top/right/bottom）
    // double halfW = widthLogic / 2.0;
    // double halfH = heightLogic / 2.0;

    // double left   = newCenter.x() - halfW;
    // double top    = newCenter.y() - halfH;
    // double right  = newCenter.x() + halfW;
    // double bottom = newCenter.y() + halfH;

    // // 碰撞检测（在逻辑坐标系）
    // if (canOccupyRect(left, top, right, bottom, board)) {
    //     pos = newCenter;

    //     // 更新朝向：根据 dx,dy 的主方向决定贴图
    //     if (qAbs(dx) >= qAbs(dy)) {
    //         dir = (dx > 0) ? Right : Left;
    //     } else {
    //         dir = (dy > 0) ? Down : Up;
    //     }

    //     emit positionChanged(pos);
    //     return true;
    // }

    // // 如果整步不可行，你可以尝试逐轴移动（允许沿轴滑动）
    // // 先尝试只沿 X：
    // bool moved = false;
    // if (!qFuzzyIsNull(dx)) {
    //     QPointF tryCenterX = pos + QPointF(dx, 0);
    //     double l = tryCenterX.x() - halfW, t = tryCenterX.y() - halfH;
    //     double r = tryCenterX.x() + halfW, b = tryCenterX.y() + halfH;
    //     if (canOccupyRect(l, t, r, b, board)) {
    //         pos = tryCenterX;
    //         dir = (dx > 0) ? Right : Left;
    //         moved = true;
    //     }
    // }
    // if (!moved && !qFuzzyIsNull(dy)) {
    //     QPointF tryCenterY = pos + QPointF(0, dy);
    //     double l = tryCenterY.x() - halfW, t = tryCenterY.y() - halfH;
    //     double r = tryCenterY.x() + halfW, b = tryCenterY.y() + halfH;
    //     if (canOccupyRect(l, t, r, b, board)) {
    //         pos = tryCenterY;
    //         dir = (dy > 0) ? Down : Up;
    //         moved = true;
    //     }
    // }

    // if (moved) {
    //     emit positionChanged(pos);
    //     return true;
    // }

    // // 无法移动
    // return false;
}

void playerCharacter::draw(QPainter *painter, const QTransform &logicToScene)
{
    //qDebug() << "draw Player";
    QPixmap *current = nullptr;
    switch (dir) {
    case Up:    current = &imgUp; break;
    case Down:  current = &imgDown; break;
    case Left:  current = &imgLeft; break;
    case Right: current = &imgRight; break;
    }

    //qDebug() << "Current size:" << current->size();
    if (!current || current->isNull()) {
        qDebug() << "empty Current";
        return;
    }

    //painter->fillRect(QRectF(0,0,5,5), Qt::red);
    painter->setOpacity(1.0);
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->setClipping(false);
    QRect rect(pixPos.x(), pixPos.y(), pixwid, pixhei);
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);

    // painter->drawPixmap(0, 0 , imgDown);

    QImage img(":/images/Images/Player/down.png");
    painter->drawImage(0, 0, img);
}
