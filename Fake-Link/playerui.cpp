#include "playerui.h"
#include "ui_playerui.h"

playerUI::playerUI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::playerUI)
{
    ui->setupUi(this);

    s = new Settings;
    connect(s, &Settings::setUpdated, this, &playerUI::setChangePlayerUI);
    connect(this, &playerUI::receiveLoadSetsChange, this, &playerUI::on_loadButton_clicked);

    QPixmap pixmap(":/images/Images/ui/Title.png");//主页图片
    ui->titleLabel->setPixmap(pixmap);
    ui->titleLabel->setScaledContents(true);  // 图片自适应 label 大小
    ui->titleLabel->setFixedSize(231, 97);

    this->setWindowTitle("Wink~");
}

playerUI::~playerUI() {
    delete ui;
    delete s;
}

void playerUI::on_gameStartButton_clicked() {
    emit gameStart();
}


void playerUI::on_quitButton_clicked() {
    this->close();
}

gameSettings playerUI::preSets() {
    QSettings settings("config.txt", QSettings::IniFormat);//随程序发布统一配置

    gset.col = settings.value("block/col", 6).toInt();
    gset.row = settings.value("block/row", 6).toInt();
    gset.numTypes = settings.value("block/numTypes", 4).toInt();
    gset.maxTurns = settings.value("block/maxTurns", 2).toInt();
    gset.character = settings.value("checkBox/character", false).toBool();
    gset.doubleCharacter = settings.value("checkBox/doubleCharacter", false).toBool();

    //qDebug() << gset.character;
    return gset;
}


void playerUI::on_setButton_clicked() {
    while (s->exec()) {
        s->show();
    }
}

void playerUI::on_loadButton_clicked() {
    QSettings settings("local.txt", QSettings::IniFormat);//随程序发布统一配置
    settings.sync();

    //添加空文件判断无存档
    QStringList keys = settings.allKeys();
    if (keys.isEmpty()) {
        QMessageBox::warning(this,
                             "提示",
                             "未检测到存档文件，请先和Elaina一起游玩吧😘~",
                             QMessageBox::Ok);
        return;
    }

    emit loadStart();//发出载入信号传给MainWindow

    gset.col = settings.value("map/col", 6).toInt();
    gset.row = settings.value("map/row", 6).toInt();
    gset.numTypes = settings.value("map/numTypes", 4).toInt();
    gset.maxTurns = settings.value("map/maxTurns", 2).toInt();
    gset.character = settings.value("Model/characterSet", false).toBool();
    gset.doubleCharacter = settings.value("Model/doubleCharacter", false).toBool();

    s->loadSets(gset);

    QMessageBox::information(this,
                         "载入成功",
                         "Elaina想起来上一次的回忆啦~恰咯😜",
                         QMessageBox::Ok);
}


void playerUI::on_refreshButton_clicked() {
    emit setChangePlayerUI();
    QMessageBox::information(this, "提示", "刷新成功！");
}


void playerUI::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // 或绘制图片背景
    QPixmap pix(":/images/Images/Background/startBG.png");
    painter.drawPixmap(this->rect(), pix);
}

void playerUI::keyPressEvent(QKeyEvent *event){
    if ((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_R) {
        ui->refreshButton->clicked();
    }
}

