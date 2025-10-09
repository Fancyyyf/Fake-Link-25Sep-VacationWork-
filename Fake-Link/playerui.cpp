#include "playerui.h"
#include "ui_playerui.h"



playerUI::playerUI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::playerUI)
{
    ui->setupUi(this);

    s = new Settings;
    connect(s, &Settings::setUpdated, this, &playerUI::setChangePlayerUI);

    QPixmap pixmap(":/images/Images/ui/Title.png");
    ui->titleLabel->setPixmap(pixmap);
    ui->titleLabel->setScaledContents(true);  // 图片自适应 label 大小
    ui->titleLabel->setFixedSize(231,97);

}

playerUI::~playerUI()
{
    delete ui;
    delete s;
}

void playerUI::on_gameStartButton_clicked()
{
    emit gameStart();
}


void playerUI::on_pushButton_clicked()
{
    this->close();
}

gameSettings playerUI::preSets(){
    QSettings settings("config.txt", QSettings::IniFormat);//随程序发布统一配置

    gset.col = settings.value("block/col", 6).toInt();
    gset.row  = settings.value("block/row", 6).toInt();
    gset.numTypes = settings.value("block/numTypes", 4).toInt();
    gset.maxTurns = settings.value("block/maxTurns", 2).toInt();
    gset.character = settings.value("checkBox/character", false).toBool();
    gset.doubleCharacter = settings.value("checkBox/doubleCharacter", false).toBool();

    //qDebug() << gset.character;
    return gset;
}


void playerUI::on_setButton_clicked()
{
    while(s->exec()){
        s->show();
    }
}


void playerUI::on_refreshButton_clicked()
{
    emit setChangePlayerUI();
    QMessageBox::information(this, "提示", "刷新成功！");
}


void playerUI::paintEvent(QPaintEvent* event){
    QPainter painter(this);

    // 或绘制图片背景
    QPixmap pix(":/images/Images/Background/startBG.png");
    painter.drawPixmap(this->rect(), pix);
}

void playerUI::keyPressEvent(QKeyEvent* event){
    if ((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_R) {
        ui->refreshButton->clicked();
    }
}
