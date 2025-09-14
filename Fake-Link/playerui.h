#ifndef PLAYERUI_H
#define PLAYERUI_H

#include <QWidget>
#include <QSettings>
#include <QString>
#include <QPoint>

#include "settings.h"

namespace Ui {
class playerUI;
}

class playerUI : public QWidget
{
    Q_OBJECT

public:
    gameSettings preSets();

    explicit playerUI(QWidget *parent = nullptr);
    ~playerUI();

signals:
    void gameStart();//发出开始游戏的信号

    void setChangePlayerUI();

private slots:
    void on_gameStartButton_clicked();

    void on_pushButton_clicked();

    void on_setButton_clicked();

private:
    Ui::playerUI *ui;

    gameSettings gset;
    Settings* s;
};

#endif // PLAYERUI_H
