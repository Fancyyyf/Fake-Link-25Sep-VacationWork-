#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <QFont>
#include <QWidget>
#include <QPainter>

class scoreBoard : public QWidget {
    Q_OBJECT

public:
    explicit scoreBoard(QWidget *parent = nullptr, int num = 1);

    void addScore(int delta);

    void addScore2(int delta);

    void resetScore();

    int getScore() const;

    int getScore2() const;

    int getBestScore() const;

    void setScore(int s1);

    void setScore2(int s2);

    void changePlayers(int num);

    void setBestScore(int bs);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int score;
    int score2;
    int bestScore;

    int playerNum;
};

#endif // SCOREBOARD_H
