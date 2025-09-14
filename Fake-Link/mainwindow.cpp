#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, int row, int col, int numTypes)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , board(row, QVector<int>(col, 0)),
    row(row),col(col),numTypes(numTypes),
    firstClicked(false), secondClicked(false), match(false),
    allZero(false)
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

    connect(this, &MainWindow::setChangeMainWindow, this, &MainWindow::setRecieved);

    //初始化失败弹窗
    // 创建提示 QLabel
    tipLabel = new QLabel("配对失败！", this);
    tipLabel->setStyleSheet("background-color: rgba(255,0,0,200); color: white; "
                            "font-weight: bold; padding: 10px; border-radius: 5px;");
    tipLabel->setAlignment(Qt::AlignCenter);
    tipLabel->setFixedSize(120, 50);
    tipLabel->move((width() - tipLabel->width()) / 2, (height() - tipLabel->height()) / 2);
    tipLabel->hide();  // 初始隐藏
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

void MainWindow::mousePressEvent(QMouseEvent *event) {
    // 鼠标像素坐标
    QPointF pixel = event->position();
    QPointF logic = pixelToLogical(pixel);
    if(event->button() == Qt::LeftButton){
        if (!std::isnan(logic.x())) {//是否有效坐标
            int col = (int)std::floor(logic.x());
            int row = (int)std::floor(logic.y()) + 1;// 点击边角会存在问题，点击图像本身即可修正
            qDebug() << "logic:" << logic << " -> row,col =" << row << col;

            linkStart(row, col);
        } else {
            qDebug() << "点击在绘制区域外";
        }
    }
    if(event->button() == Qt::RightButton){//右键取消
        if(firstClicked){
            firstClicked = false;
            selRow1 = selCol1 = -1;

            update();
        }
    }
}


void MainWindow::mapInit(){
    if (numTypes < 3) numTypes = 3; // 至少 3 种
    if (numTypes > 6) numTypes = 6; // 至多 6 种

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
    painter.setRenderHint(QPainter::Antialiasing,true);//抗锯齿
    int cellSize = 1; // 每个格子的大小

    //绘制背景
    painter.setOpacity(0.6);  // 设置透明度 0.0~1.0
    QString path = QString(":/images/Images/Background/pixbg") + QString::number(1 + backgroundNum)+ QString(".png");
    QPixmap bg(path);
    painter.drawPixmap(rect(), bg);
    painter.setOpacity(1);//透明度还原

    //设置视口比例，防止地图变形
    int heightSide, widthSide;
    if (((double)(width())/(double)(height())) > ((double)(col) / (double)(row))) {
        heightSide = height();
        widthSide =  col * heightSide / row;
    } else {
        widthSide = width();
        heightSide = row * widthSide / col;
    }
    painter.setViewport((width()-widthSide)/2,(height()-heightSide)/2,widthSide,heightSide);

    //设置painter的坐标，方便画图
    double marginRatio = 0.20; // 边距比例：2%
    double widSpace = col * marginRatio;
    double heiSpace = row * marginRatio;
    painter.setWindow(-widSpace, -heiSpace, col+2*widSpace, row+2*heiSpace);
    //让上方空格更大一些

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

            if (firstClicked && r == selRow1 && c == selCol1) {
                QColor overlay(100, 100, 100, 120); // RGBA，alpha=120 半透明
                painter.fillRect(rect, overlay);
            }
            if (secondClicked && r == selRow2 && c == selCol2) {
                QColor overlay(100, 100, 100, 120); // RGBA，alpha=120 半透明
                painter.fillRect(rect, overlay);
            }
        }
    }

    checkGameFinished();
    if (allZero) {
        QMessageBox::information(this, "胜利", "恭喜你，全部消除完成！");
    }
}


QPointF MainWindow::pixelToLogical(const QPointF &pixel) const {
    //设置视口比例，防止地图变形
    int heightSide, widthSide;
    if (((double)(width())/(double)(height())) > ((double)(col) / (double)(row))) {
        heightSide = height();
        widthSide =  col * heightSide / row;
    } else {
        widthSide = width();
        heightSide = row * widthSide / col;
    }
    QRect viewport((width() - widthSide) / 2, (height() - heightSide) / 2, widthSide, heightSide);

    double marginRatio = 0.20; // 边距比例：20%
    double widSpace = col * marginRatio;
    double heiSpace = row * marginRatio;
    QRectF windowRect(-widSpace, -heiSpace, col + 2*widSpace, row + 2*heiSpace);

    //如果点击不在 viewport 内，可选择忽略或裁剪
    if (!viewport.contains(pixel.toPoint())) {
        return QPointF(std::numeric_limits<double>::quiet_NaN(),
                       std::numeric_limits<double>::quiet_NaN());
    }//NaN = Not a Number（非数字），是 IEEE 754 浮点标准定义的一种特殊值
     //quiet_NaN() 返回一个 安静的 NaN 值（不会触发异常/中断）


    //线性映射 (像素 -> 归一化 -> 逻辑)
    double nx = (pixel.x() - viewport.left()) / (double)viewport.width();
    double ny = (pixel.y() - viewport.top())  / (double)viewport.height();

    double logicX = windowRect.left() + nx * windowRect.width();
    double logicY = windowRect.top()  + ny * windowRect.height();
    return QPointF(logicX, logicY);

    // 修正：去掉边距偏移
    // double colIndex = (logicX + widSpace);     // 把左移的坐标拉回来
    // double rowIndex = (logicY + 1.8*heiSpace); // 把上移的坐标拉回来
    // return QPointF(colIndex, rowIndex);
}

void MainWindow::setRecieved(){
    QSettings settings("config.txt", QSettings::IniFormat);//随程序发布统一配置

    col = settings.value("block/col", 10).toInt();
    row  = settings.value("block/row", 8).toInt();
    numTypes = settings.value("block/numTypes", 3).toInt();

    mapInit();
}

void MainWindow::linkStart(int r,int c){

    if(r < 0 || r > row - 1 || c < 0 || c > col - 1 ) return;
    if(board[r][c] == 0) return;

    if(!firstClicked){//如果不存在已经选中的图标
        selRow1 = r;
        selCol1 = c;
        firstClicked = true;

        update();
    }else{
        if(secondClicked) return;
        if(r == selRow1 && c == selCol1){//排除重复点击同一个的bug
            firstClicked = false;
            selRow1 = selCol1 = -1;
            update();
            return;
        }
        selRow2 = r;
        selCol2 = c;
        secondClicked = true;

        update();

        if(board[selRow1][selCol1] == board[selRow2][selCol2]){
            match = true;//配对成功

            update();

            QTimer::singleShot(500, this, [this]() {
                qDebug() << "1 second later";
                // 这里可以执行延时后的操作，比如 update() 绘制

                board[selRow1][selCol1] = board[selRow2][selCol2] = 0;
                selRow1 = selCol1 = selRow2 = selCol2 = -1;
                firstClicked = false;
                secondClicked = false;
                match = false;
                update();
            });
        }else{
            tipLabel->show();

            QTimer::singleShot(1000, this, [this]() {
                qDebug() << "1 second later";
                // 这里可以执行延时后的操作，比如 update() 绘制

                selRow1 = selCol1 = selRow2 = selCol2 = -1;
                firstClicked = false;
                secondClicked = false;
                tipLabel->hide();
                update();
            });
        }


    }

}


void MainWindow::checkGameFinished(){
    allZero = std::all_of(board.begin(), board.end(), [](const QVector<int>& row){
        return std::all_of(row.begin(), row.end(), [](int val){ return val == 0; });
    });
}
