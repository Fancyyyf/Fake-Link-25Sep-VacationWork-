#ifndef PAUSEDIALOG_H
#define PAUSEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>


namespace Ui {
class pauseDialog;
}

class pauseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit pauseDialog(QWidget *parent = nullptr);
    ~pauseDialog();

signals:
    void requestBackToPrep(); // 请求返回准备界面
    void backToGame();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void on_backRequestButton_clicked();

    void on_backToGameButton_clicked();

private:
    Ui::pauseDialog *ui;
};

#endif // PAUSEDIALOG_H
