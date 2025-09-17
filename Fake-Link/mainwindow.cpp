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
    maxTurns(maxTurns)
{
    //qDebug() << characterSet;

    setProtection();

    ui->setupUi(this);
    srand((int)time(0));
    this->resize(1600, 900);
    this->setWindowIcon(QIcon(":/images/Images/Icon/mainWIndowIcon.png"));

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

    connect(ui->quitButton, &QPushButton::clicked, this, [this]() {
        // 构造一个 Key_A 的事件
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
        QCoreApplication::sendEvent(this, &event);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::gameContinue(){
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
        // 连接暂停菜单的“返回准备界面”信号
        connect(&dlg, &pauseDialog::requestBackToPrep, this, [this,&dlg]() {
            emit backToPrep();
            dlg.close();
            backgroundNum  = std::rand()%6;
        });

        //继续游戏
        connect(&dlg, &pauseDialog::backToGame, this, &MainWindow::gameContinue);
        dlg.exec();  // 模态运行
    }
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
    }else{
        QMainWindow::mousePressEvent(event);
    }
}


void MainWindow::mapInit(){
    if (numTypes < 2) numTypes = 2; // 至少 3 种
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
    for (int r = 1; r < row + 1; ++r) {
        for (int c = 1; c < col + 1; ++c) {
            board[r][c] = tiles[idx++];
        }
    }

    path.clear();
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
    QString pathPix = QString(":/images/Images/Background/pixbg") + QString::number(1 + backgroundNum)+ QString(".png");
    QPixmap bg(pathPix);
    painter.drawPixmap(rect(), bg);
    painter.setOpacity(1);//透明度还原

    // //设置视口比例，防止地图变形
    // int heightSide, widthSide;
    // if (((double)(width())/(double)(height())) > ((double)(col) / (double)(row))) {
    //     heightSide = height();
    //     widthSide =  col * heightSide / row;
    // } else {
    //     widthSide = width();
    //     heightSide = row * widthSide / col;
    // }
    // painter.setViewport((width()-widthSide)/2,(height()-heightSide)/2,widthSide,heightSide);

    // //设置painter的坐标，方便画图
    // double marginRatio = 0.20; // 边距比例：2%
    // double widSpace = col * marginRatio;
    // double heiSpace = row * marginRatio;
    // painter.setWindow(-widSpace, -heiSpace, col+2*widSpace, row+2*heiSpace);
    // //让上方空格更大一些


    // 计算变换并把它设置到 painter（此时 painter 会把“逻辑坐标”映射到 widget 像素）
    QTransform t = computeLogicalToDeviceTransform();

    painter.save();
    painter.setTransform(t); // 关键：之后所有以逻辑坐标绘制会被变换到屏幕上


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
    for (int r = 1; r < row + 1; ++r) {
        for (int c = 1; c < col + 1; ++c) {
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



    if(!path.isEmpty()){
        qDebug() << "draw";

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
        grad.setColorAt(0, QColor(75, 0, 130, 200));//紫色
        grad.setColorAt(1, QColor(255, 215, 0, 200));//金色
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


QTransform MainWindow::computeLogicalToDeviceTransform() const {
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

    double marginRatio = 0.15;            // 与你原实现一致
    double widSpace = (col + 2) * marginRatio;
    double heiSpace = (row + 2) * marginRatio;

    windowRect = QRectF(-widSpace, -1.6 * heiSpace, (col + 2) + 2 * widSpace, (row + 2) + 2.6 * heiSpace);


    double a = double(viewport.width())  / windowRect.width();   // scaleX
    double c = double(viewport.height()) / windowRect.height();  // scaleY
    double b = double(viewport.left())  - windowRect.left() * a; // translateX
    double d = double(viewport.top())   - windowRect.top()  * c; // translateY

    // QTransform(m11, m12, m13, m21, m22, m23, m31, m32, m33)
    return QTransform(a, 0, 0,
                      0, c, 0,
                      b, d, 1);
}


QPointF MainWindow::pixelToLogical(const QPointF &pixel) const {
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

    double marginRatio = 0.15;            // 与你原实现一致
    double widSpace = (col + 2) * marginRatio;
    double heiSpace = (row + 2) * marginRatio;

    windowRect = QRectF(-widSpace, -1.6 * heiSpace, (col + 2) + 2 * widSpace, (row + 2) + 2.6 * heiSpace);


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

    col = settings.value("block/col", 10).toInt();
    row  = settings.value("block/row", 8).toInt();
    numTypes = settings.value("block/numTypes", 3).toInt();
    characterSet = settings.value("checkBox/character", false).toBool();
    maxTurns = settings.value("block/maxTurns", 3).toInt();

    setProtection();

    qDebug() << maxTurns;
    mapInit();
}


void MainWindow::setProtection(){
    if(maxTurns < 1 ) maxTurns = 1;
    if(row > 20) row = 20;
    if(col > 20) col = 20;
    if(row < 2) row = 2;
    if(col < 2) col = 2;
    if(numTypes > 6) numTypes = 6;
    if(numTypes < 2) numTypes = 2;
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

            QTimer::singleShot(500, this, [this]() {
                qDebug() << "1 second later";
                // 这里可以执行延时后的操作，比如 update() 绘制

                board[selRow1][selCol1] = board[selRow2][selCol2] = 0;
                selRow1 = selCol1 = selRow2 = selCol2 = -1;
                firstClicked = false;
                secondClicked = false;
                match = false;

                path.clear();
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

                path.clear();
                update();
            });
        }


    }

}


void MainWindow::checkGameFinished(){

    if(success){
        success = false;
        return;
    };

    bool allZero = std::all_of(board.begin(), board.end(), [](const QVector<int>& row){
        return std::all_of(row.begin(), row.end(), [](int val){ return val == 0; });
    });

    if ((allZero && !firstClicked) || portal) {
        success = true;

        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this,
                                         "胜利",
                                         "恭喜你，全部消除完成！",
                                         QMessageBox::Ok);  // 只有一个按钮 OK

        if (reply == QMessageBox::Ok) {
            emit backToPrep();
            success = false;
            portal = false;
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

    // 没有找到满足条件的路径
    return {};
}
