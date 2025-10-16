#include "mainwindow.h"
#include "playerui.h"
#include "gameSettings.h"

#include <QApplication>
#include <QApplication>
#include <QElapsedTimer>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QStackedWidget>
#include <QStyleFactory>
#include <QWidget>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    // 固定使用 Fusion 样式（不受系统主题变化影响）,避免深色主题下字体变白
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // 创建固定调色板（浅色主题）
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 255, 255));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    palette.setColor(QPalette::Base, QColor(240, 240, 240));
    palette.setColor(QPalette::AlternateBase, QColor(255, 255, 255));
    palette.setColor(QPalette::Text, QColor(0, 0, 0));
    palette.setColor(QPalette::Button, QColor(240, 240, 240));
    palette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    palette.setColor(QPalette::Highlight, QColor(76, 163, 224));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    QApplication::setPalette(palette);

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

    //延迟3秒
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

    QObject::connect(game, &MainWindow::sendLoadSet, prep, &playerUI::receiveLoadSetsChange);

    //界面切换
    QObject::connect(prep, &playerUI::gameStart, [=]() {
        prep->hide();
        game->show();
        game->gameTimerStart();
    });

    QObject::connect(game, &MainWindow::backToPrep,[=]() {
        game->close();
        prep->show();
    });

    return a.exec();
}
