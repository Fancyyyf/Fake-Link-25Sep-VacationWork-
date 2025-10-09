#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/Images/Icon/setIcon.png"));

    //界面设置
    QSettings settings("config.txt", QSettings::IniFormat);//随程序发布统一配置

    gset.col = settings.value("block/col", 6).toInt();
    gset.row  = settings.value("block/row", 6).toInt();
    gset.numTypes = settings.value("block/numTypes", 4).toInt();
    gset.character = settings.value("checkBox/character", false).toBool();
    gset.doubleCharacter = settings.value("checkBox/doubleCharacter", false).toBool();
    gset.maxTurns = settings.value("block/maxTurns", 2).toInt();

    ui->colEdit->setText(QString::number(gset.col));
    ui->colEdit->setPlaceholderText("请输入列数");
    ui->rowEdit->setText(QString::number(gset.row));
    ui->rowEdit->setPlaceholderText("请输入行数");
    ui->numTypeEdit->setText(QString::number(gset.numTypes));
    ui->numTypeEdit->setPlaceholderText("请输入格子种类");
    ui->turnsEdit->setText(QString::number(gset.maxTurns));
    ui->turnsEdit->setPlaceholderText("请输入最大拐点数");

    ui->characterBox->setChecked(gset.character);
    ui->doubleCharacterBox->setChecked(gset.doubleCharacter);


    connect(ui->characterBox, &QCheckBox::toggled, this, [=](bool checked){
        gset.character = checked;  // 每次点击都会更新
        qDebug() << "当前 isBoxChecked =" << gset.character;
    });

    connect(ui->doubleCharacterBox, &QCheckBox::toggled, this, [=](bool checked){
        gset.doubleCharacter = checked;  // 每次点击都会更新
        qDebug() << "当前 doubleCharacterBoxChecked =" << gset.doubleCharacter;
    });

}

Settings::~Settings()
{
    delete ui;
}

bool Settings::saveSets(){
    // 指定文件路径和格式
    // QSettings::IniFormat 表示保存为文本INI格式
    QSettings settings("config.txt", QSettings::IniFormat);

    QString pcol = ui->colEdit->text();
    QString prow = ui->rowEdit->text();
    QString pnumTypes = ui->numTypeEdit->text();
    QString pTurns = ui->turnsEdit->text();

    if(pcol.toInt() < 2 || pcol.toInt() > 20||
        prow.toInt() < 2 || prow.toInt() > 20||
        pTurns.toInt() < 1 ||
        pnumTypes.toInt() > 20 || pnumTypes.toInt() < 2) return false;



    settings.setValue("block/col", pcol);
    settings.setValue("block/row", prow);
    settings.setValue("block/numTypes", pnumTypes);
    settings.setValue("block/maxTurns", pTurns);

    if(gset.character == false && gset.doubleCharacter == true){

        gset.character = true;
        ui->characterBox->setChecked(gset.character);

        settings.setValue("checkBox/character", gset.character);
        settings.setValue("checkBox/doubleCharacter", gset.doubleCharacter);


        // 弹出消息框提示角色选择
        QMessageBox *Box = new QMessageBox(this);
        Box->setWindowTitle("提示");
        Box->setText("禁止双角色但选择鼠标模式，已协助勾选characterSet");
        Box->setIcon(QMessageBox::Information); // 信息图标
        Box->setStandardButtons(QMessageBox::NoButton); // 不显示按钮
        Box->show();
        QPoint center = this->geometry().center();
        QRect boxRect = Box->geometry();
        Box->move(center.x() - boxRect.width() / 2, center.y() - boxRect.height() / 2 - 140);

        // 3秒后关闭
        QTimer::singleShot(2000, Box, &QMessageBox::accept);
        return true;
    }

    settings.setValue("checkBox/character", gset.character);
    settings.setValue("checkBox/doubleCharacter", gset.doubleCharacter);

    return true;
}


void Settings::on_saveButton_clicked()
{
    if(saveSets())
    {
        emit setUpdated();

        // 弹出消息框提示保存成功
        QMessageBox msgBox;
        msgBox.setWindowTitle("提示");
        msgBox.setText("设置已成功保存！");
        msgBox.setIcon(QMessageBox::Information); // 信息图标
        msgBox.setStandardButtons(QMessageBox::Ok); // 只有一个“确定”按钮
        msgBox.exec(); // 显示消息框并等待用户点击
    }else{
        QMessageBox bugBox;
        bugBox.setWindowTitle("警告");
        bugBox.setText("保存失败，请按照范围保存"
                       "(row,col:2~20,maxTurns: > 0,numTypes:3~20)");
        bugBox.setIcon(QMessageBox::Warning); // 信息图标
        bugBox.setStandardButtons(QMessageBox::Ok); // 只有一个“确定”按钮
        bugBox.exec(); // 显示消息框并等待用户点击
    }
}


void Settings::on_quitButton_clicked()
{
    this->close();
}


void Settings::paintEvent(QPaintEvent* event){
    QPainter painter(this);

    // 或绘制图片背景
    QPixmap pix(":/images/Images/Background/setBG.png");
    painter.drawPixmap(this->rect(), pix);

}


void Settings::keyPressEvent(QKeyEvent* event){
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        ui->saveButton->clicked();  // 模拟点击
    }
}

