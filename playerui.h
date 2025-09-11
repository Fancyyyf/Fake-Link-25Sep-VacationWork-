#ifndef PLAYERUI_H
#define PLAYERUI_H

#include <QWidget>

namespace Ui {
class playerUI;
}

class playerUI : public QWidget
{
    Q_OBJECT

public:
    explicit playerUI(QWidget *parent = nullptr);
    ~playerUI();

private:
    Ui::playerUI *ui;
};

#endif // PLAYERUI_H
