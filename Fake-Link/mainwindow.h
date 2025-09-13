#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QPaintEvent>
#include <QRandomGenerator>
#include <QPainter>
#include <QString>

#include "pausedialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void backToPrep();


protected:
    void keyPressEvent(QKeyEvent *event) override;

    void mapInit();//地图初始化

    void paintEvent(QPaintEvent *event) override; // 绘制地图


private slots:
    void gameContinue();

private:
    Ui::MainWindow *ui;
    int backgroundNum = 0;//第几张地图

    //以下是默认设置，后续可在设置中修改
    int col = 10, row = 8;
    int numTypes = 3;
    int height = 80, width = 100;

    QVector<QVector<int>> board; // 地图，0 表示空格

    QVector<QPixmap> boxImages;     // 连连看对象数组
};


#endif // MAINWINDOW_H
