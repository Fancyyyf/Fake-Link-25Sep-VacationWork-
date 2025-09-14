#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QPaintEvent>
#include <QRandomGenerator>
#include <QPainter>
#include <QString>
#include <QPointF>
#include <QSettings>
#include <QTimer>

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
    MainWindow(QWidget *parent = nullptr, int row = 8, int col = 10, int numTypes = 3);
    ~MainWindow();

signals:
    void backToPrep();

    void setChangeMainWindow();


protected:
    void keyPressEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mapInit();//地图初始化

    void paintEvent(QPaintEvent *event) override; // 绘制地图

    QPointF pixelToLogical(const QPointF &pixel) const;//坐标变换

    void linkStart(int row,int col);//开始配对

    void checkGameFinished();


private slots:
    void gameContinue();

    void setRecieved();//接收设置改变的信号


private:
    Ui::MainWindow *ui;
    int backgroundNum = 0;//第几张地图
    //以下是默认设置，后续可在设置中修改。行列与方块种类
    int col, row;
    int numTypes;

    //连连看
    bool firstClicked;//判断是否已经选中格子
    bool secondClicked;//选中第二个格子
    bool match;//判断是否配对成功
    int selRow1 = -1, selCol1 = -1; // -1 表示未选中，第一个格子
    int selRow2 = -1, selCol2 = -1; // -1 表示未选中，第二个格子

    QVector<QVector<int>> board; // 地图，0 表示空格

    QVector<QPixmap> boxImages;     // 连连看对象数组

    QLabel *tipLabel;//配对失败提示


    bool allZero;//判断游戏结束
};


#endif // MAINWINDOW_H
