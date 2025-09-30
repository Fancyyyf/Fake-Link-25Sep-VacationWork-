#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, int row, int col, int numTypes
                       , bool character, int maxTurns)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , board(row + 2, QVector<int>(col + 2, 0)),//扩展上下限，便于寻路
    row(row),col(col),numTypes(numTypes),
    firstClicked(false), secondClicked(false), match(false),
    success(false),portal(false),
    characterSet(character),
    maxTurns(maxTurns),
    combo(0),
    wPressed(false), aPressed(false), sPressed(false), dPressed(false)

{
    //qDebug() << characterSet;

    setProtection();

    if(!characterSet){
        timerMagnification = 0.8 + 0.1 * numTypes;
    }else{
        timerMagnification = 1.8 + 0.1 * numTypes;
    }

    ui->setupUi(this);
    srand((int)time(0));
    this->resize(1600, 900);
    this->setWindowIcon(QIcon(":/images/Images/Icon/mainWIndowIcon.png"));

    //地图背景设置
    backgroundNum  = std::rand()%6;
    QString pathPix = QString(":/images/Images/Background/pixbg") + QString::number(1 + backgroundNum)+ QString(".png");
    QPixmap bg = loadWithOpacity(pathPix, 0.7, this->size());;
    bg = bg.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPalette pal;
    pal.setBrush(QPalette::Window, bg);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    setWindowTitle("Fake Link");

    //加载图片
    for(int i = 0;i < 20; i++){
        QString pathPix = QString(":/images/Images/Noita/noitablock") + QString::number(i + 1)+ QString(".png");
        boxImages.append(QPixmap(pathPix));
    }

    connect(this, &MainWindow::setChangeMainWindow, this, &MainWindow::setRecieved);

    //设置按钮图标
    ui->quitButton->setFlat(true);  // 去掉立体边框
    ui->quitButton->setStyleSheet("QPushButton { border: none; background: transparent; }");
    ui->quitButton->setIcon(QIcon(":/images/Images/Icon/quitIcon.png"));   // qrc 资源或绝对/相对路径
    ui->quitButton->setIconSize(QSize(80, 80));              // 设置图标大小


    //初始化失败弹窗
    // 创建提示 QLabel
    tipLabel = new QLabel("配对失败！", this);
    tipLabel->setStyleSheet("background-color: rgba(255,0,0,200); color: white; "
                            "font-weight: bold; padding: 10px; border-radius: 5px;");
    tipLabel->setAlignment(Qt::AlignCenter);
    tipLabel->setFixedSize(120, 50);
    tipLabel->move((width() - tipLabel->width()) / 2, (height() - tipLabel->height()) / 2);
    tipLabel->hide();  // 初始隐藏

    ui->quitButton->setFocusPolicy(Qt::NoFocus);//禁止退出按钮的焦点
    connect(ui->quitButton, &QPushButton::clicked, this, [this]() {
        //连接退出按钮与ESC
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
        QCoreApplication::sendEvent(this, &event);
    });

    //计时器设置
    // 根据格子数设置时间
    remainingTime = static_cast<int>(row * col * timerMagnification);
    timerLabel = new QLabel(this);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateTimer);
    //gameTimer->start(1000); // 每秒触发一次


    QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);
    // 计时标签显示
    timerLabel->setAlignment(Qt::AlignCenter);
    timerLabel->setFont(QFont("Consolas", 32, QFont::Bold)); // 等宽字体
    timerLabel->setStyleSheet("color: white; background-color: transparent;");
    layout->addWidget(timerLabel, 0, Qt::AlignHCenter | Qt::AlignTop);

    //计分板
    scoreboard = new scoreBoard(this);
    scoreboard->move(width() - scoreboard->width() - 20, 10); // 右上角悬浮
    scoreboard->show();

    comboLabel = new QLabel(this);
    comboLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    comboLabel->move(width()/5 - comboLabel->width()/2, height()/4);
    auto shadow = new QGraphicsDropShadowEffect(comboLabel);
    shadow->setBlurRadius(15);
    shadow->setOffset(2,2);
    shadow->setColor(Qt::black);
    comboLabel->setGraphicsEffect(shadow);


    //RPG角色
    player1 = new playerCharacter(this);
    player1->setPosition(QPointF(0.0, 0.0));
    // playerSpeed = qMax(row, col) *  0.1;//速度调整在mapInit中实现
    // player1->setSpeed(playerSpeed);

    // 游戏循环定时器
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &MainWindow::tryMove);
    moveTimer->start(20); // ~60fps


    mapInit();
}

MainWindow::~MainWindow(){
    delete ui;
    delete tipLabel;
    delete gameTimer;
    delete scoreboard;
    delete comboLabel;
}

void MainWindow::gameTimerStart(){
    gameTimer->start(1000);
    updateTimer();
}

void MainWindow::gameContinue(){
    gameTimer->start();
    qDebug() << "Game Continued";
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    //后门，一键通关
    if(event->key() == Qt::Key_F){
        success = true;
        qDebug() << "press F";
    }
    if(event->key() == Qt::Key_I && success){
        portal = true;
        success = false;
        qDebug() << "press I";
        checkGameFinished();
    }

    if (event->key() == Qt::Key_Escape) {
        pauseDialog dlg(this);
        gameTimer->stop();
        // 连接暂停菜单的“返回准备界面”信号
        connect(&dlg, &pauseDialog::requestBackToPrep, this, [this,&dlg]() {
            emit backToPrep();
            dlg.close();
            //backgroundNum  = std::rand()%6;
        });

        //继续游戏
        connect(&dlg, &pauseDialog::backToGame, this, &MainWindow::gameContinue);
        dlg.exec();  // 模态运行
    }

    if(characterSet){//使用角色时

        playerCharacter::Direction curDir = player1 -> getDir();
        if (event->key() == Qt::Key_W) {
            wPressed = true;
            if( curDir == playerCharacter::Up && selTempRow != -1){
                linkStart(selTempRow, selTempCol);
            }

        } else if (event->key() == Qt::Key_A) {
            aPressed = true;
            if( curDir == playerCharacter::Left && selTempRow != -1){
                linkStart(selTempRow, selTempCol);
            }

        } else if (event->key() == Qt::Key_S) {
            sPressed = true;
            if( curDir == playerCharacter::Down && selTempRow != -1){
                linkStart(selTempRow, selTempCol);
            }

        } else if (event->key() == Qt::Key_D) {
            dPressed = true;
            if( curDir == playerCharacter::Right && selTempRow != -1){
                linkStart(selTempRow, selTempCol);
            }

        }

        if(event->key() == Qt::Key_Space && (selTempRow != -1)){
            linkStart(selTempRow, selTempCol);
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(characterSet){
        if (event->key() == Qt::Key_W) {
            wPressed = false;
        } else if (event->key() == Qt::Key_A) {
            aPressed = false;
        } else if (event->key() == Qt::Key_S) {
            sPressed = false;
        } else if (event->key() == Qt::Key_D) {
            dPressed = false;
        }
    }

    QWidget::keyReleaseEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if(!characterSet){
        // 鼠标像素坐标
        QPointF pixel = event->position();
        QPointF logic = pixelToLogical(pixel);
        if(event->button() == Qt::LeftButton){
            if (!std::isnan(logic.x())) {//是否有效坐标
                int col = (int)std::floor(logic.x());
                int row = (int)std::floor(logic.y());
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

    if(event->button() == Qt::RightButton){//右键取消
        if(firstClicked){
            firstClicked = false;
            selRow1 = selCol1 = -1;

            update();
        }
    }
    QMainWindow::mousePressEvent(event);

}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (scoreboard) {
        scoreboard->move(width() - scoreboard->width() - 20, 10);
    }

    QString pathPix = QString(":/images/Images/Background/pixbg") + QString::number(1 + backgroundNum)+ QString(".png");
    QPixmap bg = loadWithOpacity(pathPix, 0.7, this->size());;
    bg = bg.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPalette pal;
    pal.setBrush(QPalette::Window, bg);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

}


QPixmap MainWindow::loadWithOpacity(const QString &path, qreal opacity, const QSize &size) {
    QPixmap src(path);
    src = src.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPixmap result(src.size());
    result.fill(Qt::white); // 先用白色填充作为背景
    QPainter p(&result);
    p.setOpacity(opacity);
    p.drawPixmap(0, 0, src);
    p.end();

    return result;
}


void MainWindow::mapInit(){
    //board.clear();
    if (numTypes < 2) numTypes = 2; // 至少 3 种
    if (numTypes > 20) numTypes = 20; // 至多 20 种

    int maxNum = row * col / 2;
    if(numTypes > maxNum){//保护防止过度设置种类
        numTypes = maxNum;
    }

    backgroundNum  = std::rand()%6;

    firstClicked = false;
    selRow1 = selCol1 = -1;

    //角色还原
    QPointF p(-1, -1);
    player1 -> setPosition(p);

    int total = row * col;
    bool needEmpty = (total % 2 == 1);
    int usable = total - (needEmpty ? 1 : 0);
    int pairs = usable / 2;

    if (numTypes > pairs) numTypes = pairs; // 每种至少出现一次

    playerSpeed = qMin(qMax(row, col), 8) *  0.02;
    player1->setSpeed(playerSpeed);//速度设置

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
    for (int r = 1; r < row + 1; ++r) {
        for (int c = 1; c < col + 1; ++c) {
            board[r][c] = tiles[idx++];
        }
    }
    path.clear();

    //临时改一下再改回去，帮忙刷新背景
    QSize s = this->size();
    this->resize(s.width()+1, s.height()); // 先+1
    this->resize(s);
    update();
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);//抗锯齿
    int cellSize = 1; // 每个格子的大小

    //绘制背景,放弃这个方法，反复重画挤占线程
    // painter.setOpacity(0.6);  // 设置透明度 0.0~1.0
    // QString pathPix = QString(":/images/Images/Background/pixbg") + QString::number(1 + backgroundNum)+ QString(".png");
    // QPixmap bg(pathPix);
    // painter.drawPixmap(rect(), bg);
    // painter.setOpacity(1);//透明度还原

    // 计算变换并把它设置到 painter（此时 painter 会把“逻辑坐标”映射到 widget 像素）
    QTransform t = computeLogicalToDeviceTransform();

    painter.save();
    painter.setTransform(t); // 关键：之后所有以逻辑坐标绘制会被变换到屏幕上


    //纯色格子版本
    // for (int r = 1; r < row + 1; ++r) {
    //     for (int c = 1; c < col + 1; ++c) {
    //         QRect rect(c * cellSize, r * cellSize, cellSize, cellSize);

    //         // 画格子边框
    //         painter.setPen(Qt::black);
    //         painter.drawRect(rect);

    //         // 根据值画不同颜色的箱子
    //         int val = board[r][c];
    //         if (val > 0) {
    //             QColor color;
    //             switch (val) {
    //             case 1: color = Qt::red; break;
    //             case 2: color = Qt::green; break;
    //             case 3: color = Qt::blue; break;
    //             case 4: color = Qt::yellow; break;
    //             default: color = Qt::gray; break;
    //             }
    //             painter.fillRect(rect.adjusted(2, 2, -2, -2), color);
    //         }
    //     }
    // }
    //图片版本
    for (int r = 1; r < row + 1; ++r) {
        for (int c = 1; c < col + 1; ++c) {
            QRect rect(c * cellSize, r * cellSize, cellSize, cellSize);

            // 画格子边框
            painter.setPen(Qt::NoPen);

            int val = board[r][c];
            if (val > 0 && val <= boxImages.size()) {
                // 把图片缩放到格子大小再绘制
                painter.drawPixmap(rect, boxImages[val - 1]);
            }

            if (characterSet && r == selTempRow && c == selTempCol) {
                QColor overlay(100, 100, 100, 120); // RGBA，alpha=120 半透明
                painter.fillRect(rect, overlay);
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

    // QImage img(":/images/Images/Player/down.png");
    // QRectF targetRect(0, 0, 0.8, 0.8); // 浮点矩形
    // painter.drawImage(targetRect, img);

    if(characterSet){
        // 绘制 player（将逻辑坐标变换到像素）
        player1->draw(&painter, t);
    }


    if(!path.isEmpty()){
        qDebug() << "drawPath";

        //QPoint只能整数坐标，于是更新QPointF方便绘画
        QVector<QPointF> drawPath;
        drawPath.reserve(path.size());

        for (const auto &pt : path) {
            drawPath.append(QPointF(pt.x() + 0.5, pt.y() + 0.5));
        }//修改路径，使画笔处在图标中心

        QPainterPath pathLine;
        pathLine.moveTo(drawPath[0]);
        for (int i = 1; i < drawPath.size(); ++i) {
            pathLine.lineTo(drawPath[i]);
        }

        QLinearGradient grad(drawPath[0], drawPath[drawPath.size()-1]);
        grad.setColorAt(0, QColor(75 , 0  , 130, 200));//紫色
        grad.setColorAt(1, QColor(255, 215, 0  , 200));//金色
        //屏幕/计算机 → RGB（三基色是红绿蓝，加色模式）,故QColor采用RGB
        //绘画/颜料 → RYB（三基色是红黄蓝，减色模式）

        QPen pen(QBrush(grad), 0.1);
        pen.setCapStyle(Qt::RoundCap);    // 圆头
        pen.setJoinStyle(Qt::RoundJoin);  // 圆角
        painter.setPen(pen);

        painter.drawPath(pathLine);

        checkGameFinished();
    }

    painter.restore();//恢复到坐标变换前
}


QTransform MainWindow::computeLogicalToDeviceTransform() const{
    QRect viewport;
    QRectF windowRect;

    int heightSide, widthSide;
    if ((double)width() / (double)height() > (double)(col + 2) / (double)(row + 2)) {
        heightSide = height();
        widthSide  = (col + 2) * heightSide / (row + 2);
    } else {
        widthSide  = width();
        heightSide = (row + 2) * widthSide / (col + 2);
    }
    viewport = QRect((width() - widthSide) / 2,
                        (height() - heightSide) / 2,
                        widthSide, heightSide);

    double marginRatio = 0.14;//边框倍率
    double widSpace = (col + 2) * marginRatio;
    double heiSpace = (row + 2) * marginRatio;

    windowRect = QRectF(-widSpace, -1.4 * heiSpace, (col + 2) + 2 * widSpace, (row + 2) + 2.4 * heiSpace);

    double a = double(viewport.width())  / windowRect.width();   // scaleX
    double c = double(viewport.height()) / windowRect.height();  // scaleY
    double b = double(viewport.left())  - windowRect.left() * a; // translateX
    double d = double(viewport.top())   - windowRect.top()  * c; // translateY

    // QTransform(m11, m12, m13, m21, m22, m23, m31, m32, m33)
    return QTransform(a, 0, 0,
                      0, c, 0,
                      b, d, 1);
}


QPointF MainWindow::pixelToLogical(const QPointF &pixel) const{
    QRect viewport;
    QRectF windowRect;

    //设置视口比例，防止地图变形
    int heightSide, widthSide;
    if ((double)width() / (double)height() > (double)(col + 2) / (double)(row + 2)) {
        heightSide = height();
        widthSide  = (col + 2) * heightSide / (row + 2);
    } else {
        widthSide  = width();
        heightSide = (row + 2) * widthSide / (col + 2);
    }
    viewport = QRect((width() - widthSide) / 2,
                     (height() - heightSide) / 2,
                     widthSide, heightSide);

    double marginRatio = 0.14;
    double widSpace = (col + 2) * marginRatio;
    double heiSpace = (row + 2) * marginRatio;

    windowRect = QRectF(-widSpace, -1.4 * heiSpace, (col + 2) + 2 * widSpace, (row + 2) + 2.4 * heiSpace);


    //如果点击不在 viewport 内，可选择忽略或裁剪
    if (!viewport.contains(pixel.toPoint())) {
        return QPointF(std::numeric_limits<double>::quiet_NaN(),
                       std::numeric_limits<double>::quiet_NaN());
    }//NaN = Not a Number（非数字），是 IEEE 754 浮点标准定义的一种特殊值
     //quiet_NaN() 返回一个 安静的 NaN 值（不会触发异常/中断）


    QTransform t = computeLogicalToDeviceTransform();
    bool invertible = false;
    QTransform inv = t.inverted(&invertible);
    if (!invertible) {
        // 不可逆（极少见），当作无效
        return QPointF(std::numeric_limits<double>::quiet_NaN(),
                       std::numeric_limits<double>::quiet_NaN());
    }

    // 直接把 pixel（widget 像素坐标）映射回逻辑坐标
    return inv.map(pixel);
}

void MainWindow::setRecieved(){
    QSettings settings("config.txt", QSettings::IniFormat);//随程序发布统一配置

    col = settings.value("block/col", 6).toInt();
    row  = settings.value("block/row", 6).toInt();
    numTypes = settings.value("block/numTypes", 4).toInt();
    characterSet = settings.value("checkBox/character", false).toBool();
    maxTurns = settings.value("block/maxTurns", 2).toInt();

    if(!characterSet){
        timerMagnification = 1.3;
    }else{
        timerMagnification = 1.6;
    }

    remainingTime = static_cast<int>(row * col * timerMagnification);
    success = portal = false;
    combo = 0;

    scoreboard->resetScore();

    setProtection();

    board.clear();//清空并重新开始初始化
    board = QVector<QVector<int>>(row + 2, QVector<int>(col + 2, 0));

    //qDebug() << maxTurns;
    mapInit();
}


void MainWindow::setProtection(){
    if(maxTurns < 1 ) maxTurns = 1;
    if(row > 20) row = 20;
    if(col > 20) col = 20;
    if(row < 2) row = 2;
    if(col < 2) col = 2;
    if(numTypes > 20) numTypes = 20;
    if(numTypes < 2) numTypes = 2;
}


void MainWindow::updateTimer() {
    //qDebug() << "Test";

    remainingTime--;
    updateTimerDisplay();
    checkGameFinished();

    if (remainingTime <= 0) {
        gameTimer->stop();
        remainingTime = 0;
        // 触发游戏结束逻辑
        onGameOver();
        return;
    }

}

void MainWindow::onGameOver(){
    if(!success){
        QMessageBox::StandardButton gameOver;
        gameOver = QMessageBox::information(this,
                                         "Loser",
                                         "真菜啊你🤣👉",
                                         QMessageBox::Ok);  // 只有一个按钮 OK

        if (gameOver == QMessageBox::Ok) {
            emit backToPrep();
            setRecieved();
        }
    }
}

void MainWindow::updateTimerDisplay() {
    // 分钟与秒
    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;

    // 格式化
    QString timeText = QString("%1:%2")
                           .arg(minutes, 1, 10, QChar('0'))  // 分钟至少1位
                           .arg(seconds, 2, 10, QChar('0')); // 秒两位

    timerLabel->setText(timeText);


    if (remainingTime < 15) {// 时间低于15秒，显示红色
        timerLabel->setStyleSheet("color: red; font-weight: bold;");
    } else {// 正常状态
        timerLabel->setStyleSheet("color: white;");
    }
}


void MainWindow::linkStart(int r,int c){

    if(r < 1 || r > row || c < 1 || c > col ) return;
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

        QPoint a(selCol1,selRow1);
        QPoint b(selCol2,selRow2);

        path = findLinkPath(a ,b ,board ,maxTurns);//寻路算法依然有问题
        // !path.isEmpty()
        //board[selRow1][selCol1] == board[selRow2][selCol2]

        qDebug() << path;
        if(!path.isEmpty()){
            match = true;//配对成功

            update();

            QTimer::singleShot(300, this, [this]() {
                qDebug() << "0.3 second later";
                // 这里可以执行延时后的操作，比如 update() 绘制

                scoreboard->addScore(board[selRow1][selCol1]);
                combo ++;
                qDebug() << combo;
                comboAwardScores();

                board[selRow1][selCol1] = board[selRow2][selCol2] = 0;
                selRow1 = selCol1 = selRow2 = selCol2 = -1;
                firstClicked = false;
                secondClicked = false;
                match = false;

                path.clear();
                update();
            });
        }else{
            comboLabel->hide();
            tipLabel->show();

            QTimer::singleShot(1000, this, [this]() {
                qDebug() << "1 second later";
                // 这里可以执行延时后的操作，比如 update() 绘制

                combo = 0;
                qDebug() << combo;

                selRow1 = selCol1 = selRow2 = selCol2 = -1;
                firstClicked = false;
                secondClicked = false;
                tipLabel->hide();

                path.clear();
                update();
            });
        }

    }

}

void MainWindow::comboAwardScores(){

    int awardscore = 0;
    // 选择颜色
    QString color;
    if (combo < 5) {
        return;
    } else if (combo < 15) {
        color = "yellow";
        awardscore = 2;
    } else if(combo < 25){
        color = "orange";
        awardscore = 5;
    }else{
        color = "red";
        awardscore = 8;
    }
    scoreboard->addScore(awardscore);
    qDebug() << "combo success";

    // 设置样式
    comboLabel->setStyleSheet(
        QString("QLabel {"
                "   color: %1;"
                "   font: bold 28px 'Consolas';"
                "   background: transparent;"
                "}").arg(color)
        );

    comboLabel->setText(QString("Combo x%1! Award+%2").arg(combo).arg(awardscore));
    comboLabel->adjustSize();

    // comboLabel->show();
    // comboLabel->raise();
    // 动画：淡入 + 上升 + 淡出
    QPropertyAnimation *animOpacity = new QPropertyAnimation(comboLabel, "windowOpacity");
    animOpacity->setDuration(1500);
    animOpacity->setKeyValueAt(0, 0.0);
    animOpacity->setKeyValueAt(0.2, 0.6);
    animOpacity->setKeyValueAt(0.8, 1.0);
    animOpacity->setKeyValueAt(1.0, 0.0);

    // QPropertyAnimation *animPos = new QPropertyAnimation(comboLabel, "pos");
    // animPos->setDuration(1500);
    // animPos->setStartValue(comboLabel->pos());
    // animPos->setEndValue(comboLabel->pos() - QPoint(0, 50));

    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    group->addAnimation(animOpacity);
    // group->addAnimation(animPos);
    connect(group, &QParallelAnimationGroup::finished, comboLabel, &QLabel::hide);

    comboLabel->show();
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::checkGameFinished(){
    if(success){
        success = false;
        return;
    };

    bool allZero = std::all_of(board.begin(), board.end(), [](const QVector<int>& row){
        return std::all_of(row.begin(), row.end(), [](int val){ return val == 0; });
    });
    //qDebug() << allZero;
    if ((allZero && !firstClicked) || portal) {
        success = true;

        gameTimer->stop();
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this,
                                         "胜利",
                                         "恭喜你，全部消除完成！",
                                         QMessageBox::Ok);  // 只有一个按钮 OK

        if (reply == QMessageBox::Ok) {
            emit backToPrep();
            combo = 0;
            success = false;
            portal = false;
            remainingTime = static_cast<int>(row * col * timerMagnification);
            comboLabel->close();

            mapInit();
        }
    }
}

constexpr int DIR_START = 5;
//寻路函数，采用0-1 BFS搜索

/**
 * @brief 在连连看棋盘中寻找从 a 到 b 的一条合法路径
 *
 * 规则：
 *  - 路径只能经过空格 (map[x][y] == 0) 或终点 b
 *  - 路径不能穿过未消除的格子
 *  - 拐点个数 <= maxTurns
 *  - 初始方向不计入拐点（第一段直走算作 0 拐点）
 *
 * @param start 起点（逻辑坐标：行x,列y）
 * @param end 终点（逻辑坐标）
 * @param map 游戏棋盘：0 表示空格，非0 表示方块
 * @param maxTurns 最大允许拐点数
 * @return QVector<QPoint> 路径（包含起点、拐点、终点），若无合法路径返回空
 */
QVector<QPoint> MainWindow::findLinkPath(
    const QPoint &start, const QPoint &end,
    const QVector<QVector<int>> &map,//已初始化为0~row+1,0~col+1,预留一圈空位
    int maxTurns
    ) {//获取坐标
    int startCol = start.x();
    int startRow = start.y();
    int endCol = end.x();
    int endRow = end.y();

    if(map[startRow][startCol] != map[endRow][endCol]) return {};//判断值是否相等

    // 边界检查（注意顺序 col/row）
    auto inBounds = [&](int c, int r) {
        return c >= 0 && c <= col + 1 && r >= 0 && r <= row + 1;
    };

    //方向向量（以 col,row 为单位）
    // 上, 右, 下, 左
    const int dcol[4] = { 0 , 1 ,  0 , -1 };
    const int drow[4] = { -1 , 0 ,  1 , 0 };

    // dist[row][col][dir] = 到达该状态时的最小拐点数（dir 是最后一步方向）

    const int INF = INT_MAX / 4;
    struct State {
        int col; int row; int d;
        State(int c = -2, int r = -2, int dir = DIR_START) : col(c), row(r), d(dir) {}
    };

    QVector<QVector<std::array<int,4>>> dist(
        (row + 2), QVector<std::array<int,4>>(col + 2)
        );
    QVector<QVector<std::array<State,4>>> parent(
        (row + 2), QVector<std::array<State,4>>(col + 2)
        );

    // 初始化 dist 和 parent
    for (int r = 0; r < row + 2; ++r) {
        for (int c = 0; c < col + 2; ++c) {
            for (int d = 0; d < 4; ++d) {
                dist[r][c][d] = INF;
                parent[r][c][d] = {-2, -2, DIR_START};
            }
        }
    }

    std::deque<State> dq;

    // === 从起点向四个方向发射 ===
    // 初始直走段不计拐点（turns = 0），把沿该方向能到达的所有连续可通行格子入队
    for (int d = 0; d < 4; ++d) {
        int nc = startCol + dcol[d];
        int nr = startRow + drow[d];

        // 沿方向延伸直到遇到边界或障碍（障碍为非0且不是终点）
        while (inBounds(nc, nr) && ( map[nr][nc] == 0 || (nc == endCol && nr == endRow))) {
            if (dist[nr][nc][d] == INF) {
                dist[nr][nc][d] = 0;                    // 初始直行段拐点 = 0
                parent[nr][nc][d] = { startCol, startRow, DIR_START }; // 父状态标为起点，dir=5 表示起点
                dq.push_front({ nc, nr, d });           // 0-cost -> push_front (0-1 BFS)
            }
            nc += dcol[d];
            nr += drow[d];
        }
    }


    //  0-1 BFS
    while (!dq.empty()) {
        State cur = dq.front();
        dq.pop_front();
        int curTurns = dist[cur.row][cur.col][cur.d];

        // 到达终点且拐点数合法 -> 回溯并返回路径
        if (cur.col == endCol && cur.row == endRow && curTurns <= maxTurns) {
            // 回溯路径
            QVector<QPoint> path;
            State s = cur;
            while (!(s.col == startCol && s.row == startRow && s.d == DIR_START)) {
                path.push_back(QPoint(s.col, s.row));
                State p = parent[s.row][s.col][s.d];
                s = p;
            }
            // 加上起点
            path.push_back(QPoint(startCol, startRow));
            std::reverse(path.begin(), path.end());

            return path;
        }

        // 向四个方向扩展一格（并在后续循环中继续沿方向延伸）
        for (int nd = 0; nd < 4; ++nd) {
            int nc = cur.col + dcol[nd];
            int nr = cur.row + drow[nd];
            if (!inBounds(nc, nr)) continue;
            if (!(map[nr][nc] == 0 || (nc == endCol && nr == endRow))) continue;

            int add = (nd == cur.d) ? 0 : 1; // 同方向：0，换方向：+1 拐点
            int ndist = curTurns + add;
            if (ndist > maxTurns) continue; // 剪枝：超过允许拐点直接跳过

            if (ndist < dist[nr][nc][nd]) {
                dist[nr][nc][nd] = ndist;
                parent[nr][nc][nd] = cur;
                if (add == 0) dq.push_front({ nc, nr, nd }); // 0-cost 边优先
                else dq.push_back({ nc, nr, nd });          // 1-cost 边放队尾
            }
        }
    }

    return {};// 没有找到满足条件的路径
}



//角色移动
void MainWindow::tryMove(){
    // 1) 计算方向向量 (逻辑单位)
    double vx = 0.0, vy = 0.0;
    if (wPressed) vy -= 1.0;
    if (sPressed) vy += 1.0;
    if (aPressed) vx -= 1.0;
    if (dPressed) vx += 1.0;

    // 2) 归一化（保证斜向移动与轴向速度一致）
    double len = qSqrt(vx*vx + vy*vy);
    double dx = 0.0, dy = 0.0;
    if (len > 1e-9) {
        dx = vx / len * playerSpeed;
        dy = vy / len * playerSpeed;
    }

    QTransform trans = computeLogicalToDeviceTransform();
    QTransform invTrans = trans.inverted(); // 从屏幕坐标 → 逻辑坐标

    // 把窗口的四个边角点转换成逻辑坐标
    QPointF topLeft     = invTrans.map(QPointF(0, 0));
    QPointF topRight    = invTrans.map(QPointF(width(), 0));
    QPointF bottomLeft  = invTrans.map(QPointF(0, height()));
    QPointF bottomRight = invTrans.map(QPointF(width(), height()));

    //调用 player 的移动尝试
    player1->Move(dx, dy, topLeft.x(), bottomRight.x(), topLeft.y(), bottomRight.y(), board);

    //选择最近朝向的一点
    QPoint p(-1, -1);
    QPoint q = player1->selectTips(board);
    if(p != q){
        selTempRow = q.y();
        selTempCol = q.x();
    }else{
        selTempRow = selTempCol = -1;
    }

    update();
}
