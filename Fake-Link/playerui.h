#ifndef PLAYERUI_H
#define PLAYERUI_H

#include <QWidget>

namespace Ui {
class playerUI;
}

class playerUI : public QWidget
{
    Q_OBJECT

public:
    explicit playerUI(QWidget *parent = nullptr);
    ~playerUI();

signals:
    void gameStart();//发出开始游戏的信号

private slots:
    void on_pushButton_clicked();

private:
    Ui::playerUI *ui;
};

#endif // PLAYERUI_H
