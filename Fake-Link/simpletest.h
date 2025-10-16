#ifndef SIMPLETEST_H
#define SIMPLETEST_H

#include <QObject>
#include <QtTest/QtTest>

#include "mainwindow.h"

class simpleTest: public QObject {
    Q_OBJECT
public:
    simpleTest();

private slots:
    void testLink_data();

    void testLink();

    void initTestCase();

private:
    MainWindow *testWindow = new MainWindow(nullptr);
};

#endif // SIMPLETEST_H
