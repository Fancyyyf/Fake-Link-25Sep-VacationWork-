#include "playerui.h"
#include "ui_playerui.h"

playerUI::playerUI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::playerUI)
{
    ui->setupUi(this);
}

playerUI::~playerUI()
{
    delete ui;
}

void playerUI::on_pushButton_clicked()
{

}

