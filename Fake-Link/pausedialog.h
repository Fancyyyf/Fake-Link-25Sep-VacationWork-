#ifndef PAUSEDIALOG_H
#define PAUSEDIALOG_H

#include <QDialog>
#include <QLabel>

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

private slots:
    void on_backRequestButton_clicked();

    void on_backToGameButton_clicked();

private:
    Ui::pauseDialog *ui;
};

#endif // PAUSEDIALOG_H
