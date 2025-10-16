#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <climits>
#include <deque>
#include <tuple>
#include <array>
#include <QDebug>
#include <QtGlobal>
#include <QGraphicsDropShadowEffect>
#include <QMainWindow>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPointF>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QString>
#include <QSettings>
#include <QTimer>
#include <QTransform>
#include <QVBoxLayout>

#include "pausedialog.h"
#include "playercharacter.h"
#include "scoreboard.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, int row = 6, int col = 6, int numTypes = 4
               , bool character = false, int maxTurns = 2, bool doubleCharacter = false);
    ~MainWindow();

    void gameTimerStart();//开始计时器

    //测试函数
    void testInitMap();

    std::pair<int, int> testLink(int r1, int c1, int r2, int c2);

signals:
    void backToPrep();

    void setChangeMainWindow();//中转设置更改的信号

    void receiveLoad();//接受载入信号

    void sendLoadSet();//发出局内载入的设置修改信号


protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void mapInit();//地图初始化

    void paintEvent(QPaintEvent *event) override; // 绘制地图

    QTransform computeLogicalToDeviceTransform() const;//鼠标箭头坐标转换

    QPointF pixelToLogical(const QPointF &pixel) const;//坐标变换

    void linkStart(int row,int col);//角色1配对

    void linkStart2(int r, int c);//角色2配对

    void checkGameFinished();//定时检查游戏结束

    void setProtection();//保护设置上下限

    void updateTimerDisplay();//倒计时展示

    void onGameOver();//游戏失败与无解

    void comboAwardScores();//连击奖励分数

    void comboAwardScores2();//角色2连击奖励

    void timeAwardScores();//剩余时间得分加成

    //背景透明度设置
    QPixmap loadWithOpacity(const QString &path, qreal opacity, const QSize &size);

    //寻路函数
    QVector<QPoint> findLinkPath(
        const QPoint &a, const QPoint &b,
        const QVector<QVector<int>> &map,
        int maxTurns
        );

    //道具函数
    void useTool(int Num, int playerNum);

    void secDelayTool();

    void shuffleTool();

    void hintTool();

    void flashTool();

    void freezeTool(int playerNum);

    void dizzyTool(int playerNum);

    void canHint();//调用hint

    void tempLink(int r, int c);//hintTool实现

    bool detect(int r, int c);//探测上下左右是否有格子

    //探测空格是否可到达，若空格旁有空格则一定可到达
    bool detectEmptySurround(int r, int c);

    void flashMove(QPointF p);

    void loadMap();


private slots:
    void gameContinue();

    void setRecieved();//接收设置改变的信号

    void updateTimer();//计时器更新

    void tryMove();

    void tryMove2();

    //存档函数
    void saveLocal();

    void loadLocal();

private:
    Ui::MainWindow *ui;

    int backgroundNum = 0;//第几张地图
    //以下是默认设置，后续可在设置中修改。行列与方块种类
    int col, row;
    int numTypes;
    int maxTurns;//拐点限制
    bool characterSet;//判断采用角色还是鼠标
    bool doubleCharacter;//双人模式

    QVector<QVector<int>> board; // 地图，0 表示空格
    QVector<QPixmap> boxImages;  // 连连看对象图片

    //Player1
    bool firstClicked;//判断是否已经选中格子
    bool secondClicked;//选中第二个格子
    bool match;//判断是否配对成功
    int selRow1 = -1, selCol1 = -1; // -1 表示未选中，第一个格子
    int selRow2 = -1, selCol2 = -1; // -1 表示未选中，第二个格子
    QVector<QPoint> path;//寻找连接路径

    //角色1移动处理
    bool wPressed,aPressed,sPressed,dPressed;//用于处理斜向移动
    playerCharacter *player1;
    QTimer *moveTimer;
    double playerSpeed;

    //角色1选择格子
    int selTempRow1 = -1, selTempCol1 = -1; // -1 表示未选中，第一个格子

    QLabel *tipLabel;//配对失败提示

    bool success;//判断游戏结束
    bool portal;//一键通关的后门

    //倒计时
    QTimer *gameTimer;
    int remainingTime; // 剩余秒数
    double timerMagnification;//倒计时倍率
    QLabel *timerLabel;//倒计时展示框

    //计分板
    scoreBoard* scoreboard;

    //连击加分
    int combo = 0;
    QLabel *comboLabel;

    //道具图标
    QLabel *delayLabel;
    QLabel *shuffleLabel;
    QLabel *freezeLabel;
    QLabel *dizzyLabel;
    QLabel *hintLabel;
    QLabel *flashLabel;


    //角色2移动处理
    bool upPressed,leftPressed,downPressed,rightPressed;//用于处理斜向移动
    playerCharacter *player2;

    //角色2选择格子
    int selTempRow2 = -1, selTempCol2 = -1; // -1 表示未选中，第一个格子

    bool firstClicked2;//判断是否已经选中格子
    bool secondClicked2;//选中第二个格子
    bool match2;//判断是否配对成功
    int selRow12 = -1, selCol12 = -1; // -1 表示未选中，第一个格子
    int selRow22 = -1, selCol22 = -1; // -1 表示未选中，第二个格子

    QVector<QPoint> path2;//寻找连接路径2

    int combo2 = 0;
    QLabel *comboLabel2;

    //道具处理
    bool freeze1 = false, freeze2 = false;

    bool dizzy1 = false, dizzy2 = false;

    bool startHint = false;

    bool flash = false;

    //hint与检测无解
    int canLinkRow1 = -1, canLinkCol1 = -1; // -1 表示未选中，第一个格子
    int canLinkRow2 = -1, canLinkCol2 = -1; // -1 表示未选中，第二个格子
    bool canFirstClicked;//判断是否已经选中格子
    bool canSecondClicked;//选中第二个格子
    bool canMatch;//判断是否配对成功
    QVector<QPoint> canLinkPath;//寻找连接路径

    bool noSolution = false;
};


#endif // MAINWINDOW_H
