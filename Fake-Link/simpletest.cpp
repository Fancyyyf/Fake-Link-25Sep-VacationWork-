#include "simpletest.h"

simpleTest::simpleTest(){}

void simpleTest::initTestCase() {
    //消除QWARN(包含QPix未初始化等等)，让输出更好看
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &,
                              const QString &msg) {
        if (type == QtWarningMsg || type == QtCriticalMsg) {
            // 什么都不输出，屏蔽警告
            return;
        }
        // 保留普通输出
        fprintf(stderr, "%s\n", msg.toUtf8().constData());
    });

    testWindow->testInitMap(); // 只初始化一次
}

void simpleTest::testLink_data() {
    QTest::addColumn<int>("selRow1");
    QTest::addColumn<int>("selCol1");
    QTest::addColumn<int>("selRow2");
    QTest::addColumn<int>("selCol2");
    QTest::addColumn<QString>("expectedPairValue");


    QTest::newRow("test1") << 1 << 1 << 1 << 2 << "0,0";//外围相邻
    QTest::newRow("test2") << 2 << 2 << 1 << 4 << "0,0";//消除test1后可以连接
    QTest::newRow("test3") << 2 << 1 << 2 << 4 << "0,0";//消除test2后可以连接
    QTest::newRow("test4") << 3 << 1 << 6 << 1 << "0,0";//使用道具(避免shuffle)
    QTest::newRow("test5") << 4 << 4 << 5 << 4 << "0,0";//内部相邻
    QTest::newRow("test6") << 1 << 5 << 2 << 6 << "4,4";//三个拐点
    QTest::newRow("test7") << 1 << 6 << 3 << 6 << "0,0";//消除边角
    QTest::newRow("test8") << 1 << 5 << 2 << 6 << "0,0";//test4可以连接了
    QTest::newRow("test9") << 3 << 2 << 5 << 2 << "2,2";//内部间隔，无法连接
    QTest::newRow("test10") << 5 << 1 << 6 << 2 << "0,0";//边角一折
    QTest::newRow("test11") << 3 << 2 << 3 << 3 << "2,4";//内部相邻但不同

    QTest::newRow("testShuffle") << 3 << 4 << 3 << 5 << "0,0";
    //可能是fail的，因为重排过（但是由于总共36前面消除了24个实际上也大概率为0，0；但假如fail也是合理的）

}

void simpleTest::testLink() {
    // {0,  0,  0,  0,  0,  0,  0,  0},
    // {0,  1,  1,  2,  3,  4,  2,  0},
    // {0,  3,  3,  1,  3,  2,  4,  0},
    // {0, -1,  2,  4, -2, -1,  2,  0},
    // {0,  2,  1,  2,  3,  2,  4,  0},
    // {0,  3,  2,  2,  3,  1,  3,  0},
    // {0, -3,  3,  1,  4,  4,  4,  0},
    // {0,  0,  0,  0,  0,  0,  0,  0}

    QFETCH(int, selRow1);
    QFETCH(int, selCol1);
    QFETCH(int, selRow2);
    QFETCH(int, selCol2);
    QFETCH(QString, expectedPairValue);

    std::pair<int, int> p = testWindow->testLink(selRow1, selCol1, selRow2, selCol2);

    QString ans = QString::number(p.first) + "," + QString::number(p.second);

    QCOMPARE(ans, expectedPairValue);
}

QTEST_MAIN(simpleTest)
