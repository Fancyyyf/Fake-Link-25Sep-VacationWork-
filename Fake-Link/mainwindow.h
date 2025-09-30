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
#include <QTransform>
#include <deque>
#include <tuple>
#include <array>
#include <climits>
#include <QtGlobal>   // qRound
#include <QPainterPath>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsDropShadowEffect>

#include "pausedialog.h"
#include "scoreboard.h"
#include "playercharacter.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, int row = 6, int col = 6, int numTypes = 4
               , bool character = false, int maxTurns = 2);
    ~MainWindow();
    void gameTimerStart();

signals:
    void backToPrep();

    void setChangeMainWindow();


protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void mapInit();//地图初始化

    void paintEvent(QPaintEvent *event) override; // 绘制地图

    QTransform computeLogicalToDeviceTransform() const;

    QPointF pixelToLogical(const QPointF &pixel) const;//坐标变换

    void linkStart(int row,int col);//开始配对

    void checkGameFinished();

    void setProtection();//保护上下限

    void updateTimerDisplay();//倒计时展示

    void onGameOver();//游戏失败

    void comboAwardScores();//连击奖励分数

    void timeAwardScores();//剩余时间得分加成

    QPixmap loadWithOpacity(const QString &path, qreal opacity, const QSize &size);
    //背景透明度设置


    //寻路函数
    QVector<QPoint> findLinkPath(
        const QPoint &a, const QPoint &b,
        const QVector<QVector<int>> &map,
        int maxTurns
        );




private slots:
    void gameContinue();

    void setRecieved();//接收设置改变的信号

    void updateTimer();//计时器更新

    void tryMove();

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

    bool characterSet;//判断采用角色还是鼠标

    bool success;//判断游戏结束
    bool portal;//一键通关的后门

    //拐点限制
    int maxTurns;
    QVector<QPoint> path;//寻找连接路径

    //倒计时
    QTimer *gameTimer;
    int remainingTime; // 剩余秒数
    double timerMagnification;//倒计时倍率
    QLabel *timerLabel;//倒计时展示框

    //计分板
    scoreBoard* scoreboard;

    //连击加分
    int combo;
    QLabel *comboLabel;

    //角色移动处理
    bool wPressed,aPressed,sPressed,dPressed;//用于处理斜向移动
    playerCharacter *player1;
    QTimer *moveTimer;
    double playerSpeed;

    //角色选择格子
    int selTempRow = -1, selTempCol = -1; // -1 表示未选中，第一个格子


};


#endif // MAINWINDOW_H
