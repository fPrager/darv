/********************************************************************************
** Form generated from reading UI file 'ARView.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ARVIEW_H
#define UI_ARVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ARView
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ARView)
    {
        if (ARView->objectName().isEmpty())
            ARView->setObjectName(QStringLiteral("ARView"));
        ARView->resize(800, 600);
        centralwidget = new QWidget(ARView);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        ARView->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ARView);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 26));
        ARView->setMenuBar(menubar);
        statusbar = new QStatusBar(ARView);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        ARView->setStatusBar(statusbar);

        retranslateUi(ARView);

        QMetaObject::connectSlotsByName(ARView);
    } // setupUi

    void retranslateUi(QMainWindow *ARView)
    {
        ARView->setWindowTitle(QApplication::translate("ARView", "MainWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class ARView: public Ui_ARView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ARVIEW_H
