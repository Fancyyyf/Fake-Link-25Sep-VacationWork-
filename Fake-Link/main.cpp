#include "mainwindow.h"
#include "playerui.h"
#include "gameSettings.h"

#include <QApplication>
#include <QApplication>
#include <QMovie>
#include <QTimer>
#include <QStackedWidget>
#include <QElapsedTimer>
#include <QWidget>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    playerUI *prep = new playerUI;

    gameSettings gset = prep->preSets();
    MainWindow *game = new MainWindow(nullptr, gset.row, gset.col, gset.numTypes, gset.character, gset.maxTurns, gset.doubleCharacter);


//启动动画
    QMovie movie(":/gifs/Temp/GenshinStart.gif");
    QLabel label;

    label.resize(320, 360);
    label.setMovie(&movie);
    label.setScaledContents(true); //自动适应窗口大小
    label.setWindowFlags(Qt::FramelessWindowHint); //去除边框

    movie.start();
    label.show();

    //延迟5秒
    QElapsedTimer timer;
    timer.start();  // 启动计时器

    while (timer.elapsed() < 3000)  // 判断播放时长
    {
        QApplication::processEvents();  // 保持界面响应
    }

    movie.stop();
    label.close();
//动画播放结束

    prep->show();//调出开始界面

    //设置更改
    QObject::connect(prep, &playerUI::setChangePlayerUI, game, &MainWindow::setChangeMainWindow);
    //连接载入存档
    QObject::connect(prep, &playerUI::loadStart, game, &MainWindow::receiveLoad);

    //界面切换
    QObject::connect(prep, &playerUI::gameStart, [=]() {
        prep->hide();
        game->show();
        game->gameTimerStart();
    });

    QObject::connect(game, &MainWindow::backToPrep,[=](){
        game->close();
        prep->show();
    });

    return a.exec();
}
