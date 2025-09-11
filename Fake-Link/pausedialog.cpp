#include "pausedialog.h"
#include "ui_pausedialog.h"

pauseDialog::pauseDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::pauseDialog)
{
    setWindowTitle("暂停菜单");
    setModal(true);
    setFixedSize(300, 200);
    ui->setupUi(this);

    QLabel *label = new QLabel("游戏已暂停", this);
    label->setAlignment(Qt::AlignCenter);

}

pauseDialog::~pauseDialog()
{
    delete ui;
}

void pauseDialog::on_backRequestButton_clicked()
{
    emit requestBackToPrep();
}


void pauseDialog::on_backToGameButton_clicked()
{
    emit backToGame();
    this->close();
}

