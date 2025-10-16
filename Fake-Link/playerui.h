#ifndef PLAYERUI_H
#define PLAYERUI_H

#include <QKeyEvent>
#include <QSettings>
#include <QString>
#include <QPoint>
#include <QPaintEvent>
#include <QPainter>
#include <QWidget>

#include "settings.h"

namespace Ui {
class playerUI;
}

class playerUI : public QWidget {
    Q_OBJECT

public:
    gameSettings preSets();

    explicit playerUI(QWidget *parent = nullptr);

    ~playerUI();

signals:
    void gameStart();//发出开始游戏的信号

    void setChangePlayerUI();

    void loadStart();//发出载入文件的信号

    void receiveLoadSetsChange();

private slots:
    void on_gameStartButton_clicked();

    void on_quitButton_clicked();

    void on_setButton_clicked();

    void on_refreshButton_clicked();

    void on_loadButton_clicked();

protected:
    void paintEvent(QPaintEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::playerUI *ui;

    gameSettings gset;
    Settings *s;
};

#endif // PLAYERUI_H
