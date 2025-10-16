#include "pausedialog.h"
#include "ui_pausedialog.h"

pauseDialog::pauseDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::pauseDialog)
{
    setModal(true);
    setFixedSize(300, 200);
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/images/Images/Icon/pauseIcon.png"));//修改角标
    this->setWindowTitle(QString("Game Paused"));
}

pauseDialog::~pauseDialog() {
    delete ui;
}

void pauseDialog::on_backRequestButton_clicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "是否退出",
                                  "退出前请确保存档！",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        emit requestBackToPrep();
        qDebug() << "用户选择了 Yes";
    } else {
        qDebug() << "用户选择了 No";
    }
}


void pauseDialog::on_backToGameButton_clicked() {
    emit backToGame();
    this->close();
}


void pauseDialog::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // 或绘制图片背景
    QPixmap pix(":/images/Images/Background/quitBG.png");
    painter.drawPixmap(this->rect(), pix);
}

void pauseDialog::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        on_backToGameButton_clicked();
    }
}

void pauseDialog::closeEvent(QCloseEvent *event) {
    on_backToGameButton_clicked();
}

void pauseDialog::on_saveButton_clicked() {
    emit sendSave();
}


void pauseDialog::on_loadButton_clicked() {
    emit dialogLoad();
}

