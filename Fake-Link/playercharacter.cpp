#include "playercharacter.h"
#include <QtMath>

playerCharacter::playerCharacter(QObject *parent)
    : QObject(parent),
    pixPos(-1, -1), // 初始位置（可改）
    dir(Down)
{
    // 资源路径按你的资源文件调整
    imgUp    = QImage(":/images/Images/Player/up.png");
    imgDown  = QImage(":/images/Images/Player/down.png");
    imgLeft  = QImage(":/images/Images/Player/left.png");
    imgRight = QImage(":/images/Images/Player/right.png");
}

void playerCharacter::Move(double dx, double dy, double leftLimit, double rightLimit,
                           double topLimit, double bottomLimit, const QVector<QVector<int>> &board)
{
    double newX = x + dx;
    double newY = y + dy;
        // 更新朝向：根据 dx,dy 的主方向决定贴图
    if(dx == 0 && dy == 0){
        return;
    }else if (qAbs(dx) > qAbs(dy)) {
        dir = (dx > 0) ? Right : Left;
    } else {
        dir = (dy >= 0) ? Down : Up;
    }

    // 这里检查是否会碰到边界
    if (x + dx < leftLimit || x + dx + pixwid > rightLimit ||
        y + dy < topLimit || y + dy + pixhei > bottomLimit) {
        //缺陷：可能存在小缝隙
        return ;  // 如果移动后会超出边界，返回 false
    }

    int left = static_cast<int> (newX);
    int top = static_cast<int> (newY);
    int right = static_cast<int> (newX + pixwid);
    int bottom = static_cast<int> (newY + pixhei);

    int allRow = board.size();
    int allCol = board[0].size();

    if(right < 0 || left > allCol || bottom < 0 || top > allRow){
        //不在地图范围内可自由移动
        x = newX;
        y = newY;
        pixPos = QPointF(x, y);
        return;
    }

    //辅助判断是否越界
    auto inMap = [&](int row, int col) -> bool {
        if (row < 0 || row >= allRow || col < 0 || col >= allCol) {
            return false; // 在地图外
        }
        return true; // 在地图内，只有空地能进入
    };

    bool xMove1 = true;
    bool xMove2 = true;
    bool yMove1 = true;
    bool yMove2 = true;
    // 实现斜向移动
    if(dx > 0){
        if(inMap(top, right)) xMove1 = (board[top][right] == 0) ? true : false;
        if(inMap(bottom, right)) xMove2 = (board[bottom][right] == 0) ? true : false;
    }else{
        if(inMap(top, left)) xMove1 = (board[top][left] == 0) ? true : false;
        if(inMap(bottom, left)) xMove2 = (board[bottom][left] == 0) ? true : false;
    }

    if(dy > 0){
        if(inMap(bottom, right)) yMove1 = (board[bottom][right] == 0) ? true : false;
        if(inMap(bottom, left)) yMove2 = (board[bottom][left] == 0) ? true : false;
    }else{
        if(inMap(top, right)) yMove1 = (board[top][right] == 0) ? true : false;
        if(inMap(top, left)) yMove2 = (board[top][left] == 0) ? true : false;
    }

    if(xMove1 && xMove2) x = newX;
    if(yMove1 && yMove2) y = newY;
    pixPos = QPointF(x, y);

}

void playerCharacter::draw(QPainter *painter, const QTransform &logicToScene)
{
    //qDebug() << "draw Player";
    QImage *current = nullptr;
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

    QRectF targetRect(pixPos.x(), pixPos.y(), 0.8, 0.8); // 浮点矩形
    painter->drawImage(targetRect, *current);
}


QPoint playerCharacter::selectTips(const QVector<QVector<int>> &board){
    double midX = x + pixwid/2;
    double midY = y + pixhei/2;

    switch(dir){
    case Up:
        midY -= 1;
        break;
    case Down:
        midY += 1;
        break;
    case Left:
        midX -= 1;
        break;
    case Right:
        midX += 1;
        break;
    }

    int tmpX = static_cast<int>(midX);
    int tmpY = static_cast<int>(midY);

    int allRow = board.size();
    int allCol = board[0].size();

    QPoint fake(-1, -1);//返回无法选择的点；
    if(tmpX < 0 || tmpX > allCol - 1 || tmpY < 0 || tmpY > allRow - 1){
        return fake;
    }

    if(board[tmpY][tmpX] == 0){
        return fake;
    }else{
        QPoint p(tmpX, tmpY);
        return p;
    }
}
