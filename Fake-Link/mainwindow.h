#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

#include "pausedialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void backToPrep();


protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameContinue();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
