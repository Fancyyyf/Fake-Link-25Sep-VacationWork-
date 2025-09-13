#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , board(row, QVector<int>(col, 0))
{
    ui->setupUi(this);
    srand((int)time(0));
    this->resize(1600, 900);

    backgroundNum  = std::rand()%6;

    setWindowTitle("Fake Link");

    //加载图片
    boxImages.append(QPixmap(":/images/Images/Blocks/box1.png"));
    boxImages.append(QPixmap(":/images/Images/Blocks/box2.png"));
    boxImages.append(QPixmap(":/images/Images/Blocks/box3.png"));
    boxImages.append(QPixmap(":/images/Images/Blocks/box4.png"));
    boxImages.append(QPixmap(":/images/Images/Blocks/box5.png"));
    boxImages.append(QPixmap(":/images/Images/Blocks/box6.png"));

    mapInit();
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
            backgroundNum  = std::rand()%6;
        });

        //继续游戏
        connect(&dlg, &pauseDialog::backToGame, this, &MainWindow::gameContinue);

        dlg.exec();  // 模态运行
    } else {
        QMainWindow::keyPressEvent(event);
    }
}


void MainWindow::mapInit(){
    if (numTypes < 3) numTypes = 3; // 至少 3 种
    if (numTypes > 6) numTypes = 6; // 至多 8 种

    int total = row * col;
    bool needEmpty = (total % 2 == 1);
    int usable = total - (needEmpty ? 1 : 0);
    int pairs = usable / 2;

    if (numTypes > pairs) numTypes = pairs; // 每种至少出现一次

    QVector<int> tiles;
    tiles.reserve(usable);


    QVector<int> pairsPerType(numTypes, 1);// 保证每种至少一对,通过随机数计算每种方格有几个
    int remaining = pairs - numTypes;

    while (remaining > 0) {
        int t = (std::rand()) % numTypes;
        pairsPerType[t]++;
        remaining--;
    }

    // 生成 tiles
    for (int t = 0; t < numTypes; ++t) {
        for (int k = 0; k < pairsPerType[t]; ++k) {
            tiles.append(t + 1);
            tiles.append(t + 1);
        }
    }

    if(needEmpty) tiles.append(0);//补上第奇数个，为空

    // 随机打乱
    std::shuffle(tiles.begin(), tiles.end(), *QRandomGenerator::global());

    int idx = 0;
    for (int r = 0; r < row; ++r) {
        for (int c = 0; c < col; ++c) {
            board[r][c] = tiles[idx++];
        }
    }

    update();
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    int cellSize = 80; // 每个格子的大小

    //绘制背景
    painter.setOpacity(0.6);  // 设置透明度 0.0~1.0
    QString path = QString(":/images/Images/Background/pixbg") + QString::number(1 + backgroundNum)+ QString(".png");
    QPixmap bg(path);
    painter.drawPixmap(rect(), bg);
    painter.setOpacity(1);//透明度还原

    //纯色格子版本
    /*for (int r = 0; r < row; ++r) {
        for (int c = 0; c < col; ++c) {
            QRect rect(c * cellSize, r * cellSize, cellSize, cellSize);

            // 画格子边框
            painter.setPen(Qt::black);
            painter.drawRect(rect);

            // 根据值画不同颜色的箱子
            int val = board[r][c];
            if (val > 0) {
                QColor color;
                switch (val) {
                case 1: color = Qt::red; break;
                case 2: color = Qt::green; break;
                case 3: color = Qt::blue; break;
                case 4: color = Qt::yellow; break;
                default: color = Qt::gray; break;
                }
                painter.fillRect(rect.adjusted(2, 2, -2, -2), color);
            }
        }
    }*/

    //图片版本
    for (int r = 0; r < row; ++r) {
        for (int c = 0; c < col; ++c) {
            QRect rect(c * cellSize, r * cellSize, cellSize, cellSize);

            // 画格子边框
            painter.setPen(Qt::NoPen);
            painter.drawRect(rect);

            int val = board[r][c];
            if (val > 0 && val <= boxImages.size()) {
                // 把图片缩放到格子大小再绘制
                painter.drawPixmap(rect, boxImages[val - 1]);
            }
        }
    }
}
