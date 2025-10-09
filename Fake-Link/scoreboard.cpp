#include "scoreboard.h"

scoreBoard::scoreBoard(QWidget *parent, int num):
    QWidget(parent),score(0), score2(0), playerNum(num){
    this->resize(240, 160);
}

void scoreBoard::addScore(int delta) {
    score += delta;
    if (score < 0) score = 0; // 防止负分
    update();
}
void scoreBoard::addScore2(int delta) {
    score2 += delta;
    if (score2 < 0) score2 = 0; // 防止负分
    update();
}

void scoreBoard::resetScore() {
    score = 0;
    score2 = 0;
    update();
}

int scoreBoard::getScore() const {
    return score;
}

int scoreBoard::getScore2() const {
    return score2;
}

void scoreBoard::changePlayers(int num){
    playerNum = num;
}

void scoreBoard::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景（半透明黑色框）
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.setPen(QColor(255, 215, 0));
    painter.drawRoundedRect(rect(), 10, 10);

    // 文字（RPG 金色风格）
    QFont font("Consolas", 18, QFont::Bold);
    painter.setFont(font);
    painter.setPen(QColor(255, 215, 0)); // 金色

    //分数列表
    QFontMetrics fm(font);
    int lineHeight = fm.height();
    //qDebug() << lineHeight;
    //行0
    QRect rect0(0, 10, width(), lineHeight);
    painter.drawText(rect0, Qt::AlignHCenter | Qt::AlignVCenter, "Score List");

    // 行 1
    QRect rect1(0, 10 + lineHeight, width(), lineHeight);
    painter.drawText(rect1, Qt::AlignHCenter | Qt::AlignVCenter, QString("Player_1: %1").arg(score));

    // 行 2
    if (playerNum == 1){
        QRect rect2(0, 10 + 2 * lineHeight, width(), lineHeight);
        painter.drawText(rect2, Qt::AlignHCenter | Qt::AlignVCenter, "Player_2: default");
    }else{
        QRect rect2(0, 10 + 2 * lineHeight, width(), lineHeight);
        painter.drawText(rect2, Qt::AlignHCenter | Qt::AlignVCenter, QString("Player_2: %1").arg(score2));
    }

    // 行 3
    QRect rect3(0, 10 + 3 * lineHeight, width(), lineHeight);
    painter.drawText(rect3, Qt::AlignHCenter | Qt::AlignVCenter, "BEST: 1200");
}


