/********************************************************************************
** Form generated from reading UI file 'playerui.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYERUI_H
#define UI_PLAYERUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_playerUI
{
public:

    void setupUi(QWidget *playerUI)
    {
        if (playerUI->objectName().isEmpty())
            playerUI->setObjectName("playerUI");
        playerUI->resize(400, 300);

        retranslateUi(playerUI);

        QMetaObject::connectSlotsByName(playerUI);
    } // setupUi

    void retranslateUi(QWidget *playerUI)
    {
        playerUI->setWindowTitle(QCoreApplication::translate("playerUI", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class playerUI: public Ui_playerUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYERUI_H
