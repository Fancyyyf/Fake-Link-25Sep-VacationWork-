#include "playerui.h"
#include "ui_playerui.h"



playerUI::playerUI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::playerUI)
{
    ui->setupUi(this);

    s = new Settings;
    connect(s, &Settings::setUpdated, this, &playerUI::setChangePlayerUI);
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

    gset.col = settings.value("block/col", 10).toInt();
    gset.row  = settings.value("block/row", 8).toInt();
    gset.numTypes = settings.value("block/numTypes", 3).toInt();

    //qDebug() << gset.numTypes;
    return gset;
}


void playerUI::on_setButton_clicked()
{
    while(s->exec()){
        s->show();
    }
}

