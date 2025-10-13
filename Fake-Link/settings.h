#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QString>
#include <QPoint>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>

#include "gameSettings.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    bool saveSets();

    void loadSets(gameSettings s);

protected:
    void paintEvent(QPaintEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

signals:
    void setUpdated();

private slots:
    void on_saveButton_clicked();

    void on_quitButton_clicked();

private:
    Ui::Settings *ui;

    gameSettings gset;
};

#endif // SETTINGS_H
