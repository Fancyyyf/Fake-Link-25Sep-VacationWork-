#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::gameContinue(){
    qDebug() << "Game Continued";
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        pauseDialog dlg(this);

        // 连接暂停菜单的“返回准备界面”信号
        connect(&dlg, &pauseDialog::requestBackToPrep, this, [this,&dlg]() {
            emit backToPrep();
            dlg.close();
        });

        connect(&dlg, &pauseDialog::backToGame, this, &MainWindow::gameContinue);

        dlg.exec();  // 模态运行
    } else {
        QMainWindow::keyPressEvent(event);
    }
}
