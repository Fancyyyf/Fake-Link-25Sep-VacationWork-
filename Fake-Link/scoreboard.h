#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <QWidget>
#include <QPainter>
#include <QFont>

class scoreBoard : public QWidget {
    Q_OBJECT

public:
    explicit scoreBoard(QWidget *parent = nullptr);

    void addScore(int delta);
    void resetScore();
    int getScore() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int score;
};

#endif // SCOREBOARD_H
