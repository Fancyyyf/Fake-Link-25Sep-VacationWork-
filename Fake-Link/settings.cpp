#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);

    //界面设置
    QSettings settings("config.txt", QSettings::IniFormat);//随程序发布统一配置

    gset.col = settings.value("block/col", 10).toInt();
    gset.row  = settings.value("block/row", 8).toInt();
    gset.numTypes = settings.value("block/numTypes", 3).toInt();

    ui->colEdit->setText(QString::number(gset.col));
    ui->colEdit->setPlaceholderText("请输入列数");
    ui->rowEdit->setText(QString::number(gset.row));
    ui->rowEdit->setPlaceholderText("请输入行数");
    ui->numTypeEdit->setText(QString::number(gset.numTypes));
    ui->numTypeEdit->setPlaceholderText("请输入格子种类");
}

Settings::~Settings()
{
    delete ui;
}

void Settings::saveSets(){
    // 指定文件路径和格式
    // QSettings::IniFormat 表示保存为文本INI格式
    QSettings settings("config.txt", QSettings::IniFormat);

    QString pcol = ui->colEdit->text();
    QString prow = ui->rowEdit->text();
    QString pnumTypes = ui->numTypeEdit->text();

    settings.setValue("block/col", pcol);
    settings.setValue("block/row", prow);
    settings.setValue("block/numTypes", pnumTypes);

}


void Settings::on_saveButton_clicked()
{
    saveSets();
    emit setUpdated();

    // 弹出消息框提示保存成功
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("设置已成功保存！");
    msgBox.setIcon(QMessageBox::Information); // 信息图标
    msgBox.setStandardButtons(QMessageBox::Ok); // 只有一个“确定”按钮
    msgBox.exec(); // 显示消息框并等待用户点击
}


void Settings::on_quitButton_clicked()
{
    this->close();
}

