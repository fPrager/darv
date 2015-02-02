/********************************************************************************
** Form generated from reading UI file 'OptiTrackDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTITRACKDIALOG_H
#define UI_OPTITRACKDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QPushButton *_bConnection;
    QPushButton *_bCancel;
    QLineEdit *_IPLocal1;
    QLineEdit *_IPLocal2;
    QLineEdit *_IPLocal3;
    QLineEdit *_IPLocal4;
    QLabel *label;
    QLineEdit *_IPServer1;
    QLineEdit *_IPServer2;
    QLabel *label_2;
    QLineEdit *_IPServer4;
    QLineEdit *_IPServer3;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(400, 209);
        _bConnection = new QPushButton(Dialog);
        _bConnection->setObjectName(QStringLiteral("_bConnection"));
        _bConnection->setGeometry(QRect(30, 110, 341, 28));
        _bCancel = new QPushButton(Dialog);
        _bCancel->setObjectName(QStringLiteral("_bCancel"));
        _bCancel->setGeometry(QRect(30, 150, 341, 28));
        _IPLocal1 = new QLineEdit(Dialog);
        _IPLocal1->setObjectName(QStringLiteral("_IPLocal1"));
        _IPLocal1->setGeometry(QRect(150, 30, 41, 22));
        _IPLocal2 = new QLineEdit(Dialog);
        _IPLocal2->setObjectName(QStringLiteral("_IPLocal2"));
        _IPLocal2->setGeometry(QRect(210, 30, 41, 22));
        _IPLocal3 = new QLineEdit(Dialog);
        _IPLocal3->setObjectName(QStringLiteral("_IPLocal3"));
        _IPLocal3->setGeometry(QRect(270, 30, 41, 22));
        _IPLocal4 = new QLineEdit(Dialog);
        _IPLocal4->setObjectName(QStringLiteral("_IPLocal4"));
        _IPLocal4->setGeometry(QRect(330, 30, 41, 22));
        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 30, 111, 16));
        _IPServer1 = new QLineEdit(Dialog);
        _IPServer1->setObjectName(QStringLiteral("_IPServer1"));
        _IPServer1->setGeometry(QRect(150, 70, 41, 22));
        _IPServer2 = new QLineEdit(Dialog);
        _IPServer2->setObjectName(QStringLiteral("_IPServer2"));
        _IPServer2->setGeometry(QRect(210, 70, 41, 22));
        label_2 = new QLabel(Dialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 70, 111, 16));
        _IPServer4 = new QLineEdit(Dialog);
        _IPServer4->setObjectName(QStringLiteral("_IPServer4"));
        _IPServer4->setGeometry(QRect(330, 70, 41, 22));
        _IPServer3 = new QLineEdit(Dialog);
        _IPServer3->setObjectName(QStringLiteral("_IPServer3"));
        _IPServer3->setGeometry(QRect(270, 70, 41, 22));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "OptiTrack Dialog", 0));
        _bConnection->setText(QApplication::translate("Dialog", "Connect", 0));
        _bCancel->setText(QApplication::translate("Dialog", "Close", 0));
        _IPLocal1->setText(QApplication::translate("Dialog", "192", 0));
        _IPLocal2->setText(QApplication::translate("Dialog", "168", 0));
        _IPLocal3->setText(QApplication::translate("Dialog", "1", 0));
        _IPLocal4->setText(QApplication::translate("Dialog", "1", 0));
        label->setText(QApplication::translate("Dialog", "Client IP Address", 0));
        _IPServer1->setText(QApplication::translate("Dialog", "192", 0));
        _IPServer2->setText(QApplication::translate("Dialog", "168", 0));
        label_2->setText(QApplication::translate("Dialog", "Server IP Address", 0));
        _IPServer4->setText(QApplication::translate("Dialog", "1", 0));
        _IPServer3->setText(QApplication::translate("Dialog", "1", 0));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTITRACKDIALOG_H
