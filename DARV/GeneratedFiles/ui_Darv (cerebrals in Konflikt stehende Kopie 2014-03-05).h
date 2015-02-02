/********************************************************************************
** Form generated from reading UI file 'Darv.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DARV_H
#define UI_DARV_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DARVClass
{
public:
    QWidget *centralWidget;
    QFrame *line;
    QLabel *_lStereoSetting;
    QLabel *_lVirtualSetting;
    QLabel *_lARSetting;
    QCheckBox *_checkboxStereo;
    QRadioButton *_rButtonOST;
    QRadioButton *_rButtonVST;
    QRadioButton *_rButtonPC;
    QListWidget *_listBackground;
    QLabel *_lBackground;
    QLabel *_lView;
    QLineEdit *_editParallax;
    QRadioButton *_rButtonSideBySide;
    QRadioButton *_rButtonAnaglyph;
    QListWidget *_listVObject;
    QLabel *_lVObject;
    QLabel *_lParallax;
    QLineEdit *_editFovy;
    QLabel *_lFovy;
    QLineEdit *_editEyeSep;
    QLabel *_lEyeSep;
    QLabel *_lPos;
    QLabel *_lTrackingState;
    QWidget *_camWidget;
    QLabel *_lEyeCoord;
    QLineEdit *_editEyeTransY;
    QLineEdit *_editEyeTransZ;
    QLineEdit *_editEyeTransX;
    QCheckBox *_checkBoxObject;
    QLineEdit *_editObjectTransX;
    QLabel *_lObjectCoord;
    QLineEdit *_editObjectTransY;
    QLineEdit *_editObjectTransZ;
    QPushButton *_buttonEyeDiff;
    QLineEdit *_editEyeRotY;
    QLabel *_lEyeCoord_2;
    QLineEdit *_editEyeRotX;
    QLineEdit *_editEyeRotZ;
    QLineEdit *_editEyeRotW;
    QLabel *_lEyeCoord_3;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *_layoutMarker;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QButtonGroup *_bgView;
    QButtonGroup *_bgStereo;

    void setupUi(QMainWindow *DARVClass)
    {
        if (DARVClass->objectName().isEmpty())
            DARVClass->setObjectName(QStringLiteral("DARVClass"));
        DARVClass->resize(1024, 769);
        centralWidget = new QWidget(DARVClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(520, 0, 20, 751));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        _lStereoSetting = new QLabel(centralWidget);
        _lStereoSetting->setObjectName(QStringLiteral("_lStereoSetting"));
        _lStereoSetting->setGeometry(QRect(30, 300, 481, 16));
        _lVirtualSetting = new QLabel(centralWidget);
        _lVirtualSetting->setObjectName(QStringLiteral("_lVirtualSetting"));
        _lVirtualSetting->setGeometry(QRect(30, 510, 510, 16));
        _lARSetting = new QLabel(centralWidget);
        _lARSetting->setObjectName(QStringLiteral("_lARSetting"));
        _lARSetting->setGeometry(QRect(30, 10, 481, 16));
        _checkboxStereo = new QCheckBox(centralWidget);
        _checkboxStereo->setObjectName(QStringLiteral("_checkboxStereo"));
        _checkboxStereo->setGeometry(QRect(40, 330, 211, 20));
        _rButtonOST = new QRadioButton(centralWidget);
        _bgView = new QButtonGroup(DARVClass);
        _bgView->setObjectName(QStringLiteral("_bgView"));
        _bgView->addButton(_rButtonOST);
        _rButtonOST->setObjectName(QStringLiteral("_rButtonOST"));
        _rButtonOST->setGeometry(QRect(60, 70, 95, 20));
        _rButtonOST->setCheckable(true);
        _rButtonOST->setChecked(true);
        _rButtonVST = new QRadioButton(centralWidget);
        _bgView->addButton(_rButtonVST);
        _rButtonVST->setObjectName(QStringLiteral("_rButtonVST"));
        _rButtonVST->setGeometry(QRect(230, 70, 95, 20));
        _rButtonVST->setCheckable(false);
        _rButtonPC = new QRadioButton(centralWidget);
        _bgView->addButton(_rButtonPC);
        _rButtonPC->setObjectName(QStringLiteral("_rButtonPC"));
        _rButtonPC->setGeometry(QRect(390, 70, 141, 20));
        _rButtonPC->setCheckable(false);
        _listBackground = new QListWidget(centralWidget);
        _listBackground->setObjectName(QStringLiteral("_listBackground"));
        _listBackground->setGeometry(QRect(50, 120, 431, 31));
        _lBackground = new QLabel(centralWidget);
        _lBackground->setObjectName(QStringLiteral("_lBackground"));
        _lBackground->setGeometry(QRect(50, 100, 481, 16));
        _lView = new QLabel(centralWidget);
        _lView->setObjectName(QStringLiteral("_lView"));
        _lView->setGeometry(QRect(50, 40, 481, 16));
        _editParallax = new QLineEdit(centralWidget);
        _editParallax->setObjectName(QStringLiteral("_editParallax"));
        _editParallax->setGeometry(QRect(180, 360, 291, 22));
        _rButtonSideBySide = new QRadioButton(centralWidget);
        _bgStereo = new QButtonGroup(DARVClass);
        _bgStereo->setObjectName(QStringLiteral("_bgStereo"));
        _bgStereo->addButton(_rButtonSideBySide);
        _rButtonSideBySide->setObjectName(QStringLiteral("_rButtonSideBySide"));
        _rButtonSideBySide->setGeometry(QRect(360, 330, 171, 20));
        _rButtonAnaglyph = new QRadioButton(centralWidget);
        _bgStereo->addButton(_rButtonAnaglyph);
        _rButtonAnaglyph->setObjectName(QStringLiteral("_rButtonAnaglyph"));
        _rButtonAnaglyph->setGeometry(QRect(230, 330, 95, 20));
        _listVObject = new QListWidget(centralWidget);
        _listVObject->setObjectName(QStringLiteral("_listVObject"));
        _listVObject->setGeometry(QRect(50, 560, 271, 91));
        _lVObject = new QLabel(centralWidget);
        _lVObject->setObjectName(QStringLiteral("_lVObject"));
        _lVObject->setGeometry(QRect(50, 540, 510, 16));
        _lParallax = new QLabel(centralWidget);
        _lParallax->setObjectName(QStringLiteral("_lParallax"));
        _lParallax->setGeometry(QRect(50, 360, 121, 16));
        _editFovy = new QLineEdit(centralWidget);
        _editFovy->setObjectName(QStringLiteral("_editFovy"));
        _editFovy->setGeometry(QRect(180, 410, 291, 22));
        _lFovy = new QLabel(centralWidget);
        _lFovy->setObjectName(QStringLiteral("_lFovy"));
        _lFovy->setGeometry(QRect(50, 410, 111, 16));
        _editEyeSep = new QLineEdit(centralWidget);
        _editEyeSep->setObjectName(QStringLiteral("_editEyeSep"));
        _editEyeSep->setGeometry(QRect(180, 450, 291, 22));
        _lEyeSep = new QLabel(centralWidget);
        _lEyeSep->setObjectName(QStringLiteral("_lEyeSep"));
        _lEyeSep->setGeometry(QRect(50, 450, 111, 16));
        _lPos = new QLabel(centralWidget);
        _lPos->setObjectName(QStringLiteral("_lPos"));
        _lPos->setGeometry(QRect(30, 170, 510, 16));
        _lTrackingState = new QLabel(centralWidget);
        _lTrackingState->setObjectName(QStringLiteral("_lTrackingState"));
        _lTrackingState->setGeometry(QRect(610, 40, 481, 16));
        _camWidget = new QWidget(centralWidget);
        _camWidget->setObjectName(QStringLiteral("_camWidget"));
        _camWidget->setGeometry(QRect(600, 80, 320, 240));
        _lEyeCoord = new QLabel(centralWidget);
        _lEyeCoord->setObjectName(QStringLiteral("_lEyeCoord"));
        _lEyeCoord->setGeometry(QRect(40, 200, 510, 16));
        _editEyeTransY = new QLineEdit(centralWidget);
        _editEyeTransY->setObjectName(QStringLiteral("_editEyeTransY"));
        _editEyeTransY->setGeometry(QRect(160, 200, 61, 22));
        _editEyeTransZ = new QLineEdit(centralWidget);
        _editEyeTransZ->setObjectName(QStringLiteral("_editEyeTransZ"));
        _editEyeTransZ->setGeometry(QRect(260, 200, 61, 22));
        _editEyeTransX = new QLineEdit(centralWidget);
        _editEyeTransX->setObjectName(QStringLiteral("_editEyeTransX"));
        _editEyeTransX->setGeometry(QRect(60, 200, 61, 22));
        _checkBoxObject = new QCheckBox(centralWidget);
        _checkBoxObject->setObjectName(QStringLiteral("_checkBoxObject"));
        _checkBoxObject->setGeometry(QRect(340, 590, 271, 17));
        _editObjectTransX = new QLineEdit(centralWidget);
        _editObjectTransX->setObjectName(QStringLiteral("_editObjectTransX"));
        _editObjectTransX->setGeometry(QRect(70, 660, 100, 22));
        _lObjectCoord = new QLabel(centralWidget);
        _lObjectCoord->setObjectName(QStringLiteral("_lObjectCoord"));
        _lObjectCoord->setGeometry(QRect(50, 660, 510, 16));
        _editObjectTransY = new QLineEdit(centralWidget);
        _editObjectTransY->setObjectName(QStringLiteral("_editObjectTransY"));
        _editObjectTransY->setGeometry(QRect(230, 660, 100, 22));
        _editObjectTransZ = new QLineEdit(centralWidget);
        _editObjectTransZ->setObjectName(QStringLiteral("_editObjectTransZ"));
        _editObjectTransZ->setGeometry(QRect(390, 660, 100, 22));
        _buttonEyeDiff = new QPushButton(centralWidget);
        _buttonEyeDiff->setObjectName(QStringLiteral("_buttonEyeDiff"));
        _buttonEyeDiff->setGeometry(QRect(340, 200, 141, 101));
        _editEyeRotY = new QLineEdit(centralWidget);
        _editEyeRotY->setObjectName(QStringLiteral("_editEyeRotY"));
        _editEyeRotY->setGeometry(QRect(160, 240, 61, 22));
        _lEyeCoord_2 = new QLabel(centralWidget);
        _lEyeCoord_2->setObjectName(QStringLiteral("_lEyeCoord_2"));
        _lEyeCoord_2->setGeometry(QRect(40, 240, 510, 16));
        _editEyeRotX = new QLineEdit(centralWidget);
        _editEyeRotX->setObjectName(QStringLiteral("_editEyeRotX"));
        _editEyeRotX->setGeometry(QRect(60, 240, 61, 22));
        _editEyeRotZ = new QLineEdit(centralWidget);
        _editEyeRotZ->setObjectName(QStringLiteral("_editEyeRotZ"));
        _editEyeRotZ->setGeometry(QRect(260, 240, 61, 22));
        _editEyeRotW = new QLineEdit(centralWidget);
        _editEyeRotW->setObjectName(QStringLiteral("_editEyeRotW"));
        _editEyeRotW->setGeometry(QRect(260, 280, 61, 22));
        _lEyeCoord_3 = new QLabel(centralWidget);
        _lEyeCoord_3->setObjectName(QStringLiteral("_lEyeCoord_3"));
        _lEyeCoord_3->setGeometry(QRect(230, 280, 31, 16));
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(600, 360, 321, 301));
        _layoutMarker = new QVBoxLayout(verticalLayoutWidget);
        _layoutMarker->setSpacing(6);
        _layoutMarker->setContentsMargins(11, 11, 11, 11);
        _layoutMarker->setObjectName(QStringLiteral("_layoutMarker"));
        _layoutMarker->setContentsMargins(0, 0, 0, 0);
        DARVClass->setCentralWidget(centralWidget);
        _lObjectCoord->raise();
        line->raise();
        _lStereoSetting->raise();
        _lVirtualSetting->raise();
        _lARSetting->raise();
        _checkboxStereo->raise();
        _rButtonOST->raise();
        _rButtonVST->raise();
        _rButtonPC->raise();
        _listBackground->raise();
        _lBackground->raise();
        _lView->raise();
        _editParallax->raise();
        _rButtonSideBySide->raise();
        _rButtonAnaglyph->raise();
        _listVObject->raise();
        _lVObject->raise();
        _lParallax->raise();
        _editFovy->raise();
        _lFovy->raise();
        _editEyeSep->raise();
        _lEyeSep->raise();
        _lPos->raise();
        _lTrackingState->raise();
        _camWidget->raise();
        _lEyeCoord->raise();
        _editEyeTransY->raise();
        _editEyeTransZ->raise();
        _editEyeTransX->raise();
        _checkBoxObject->raise();
        _editObjectTransX->raise();
        _editObjectTransY->raise();
        _editObjectTransZ->raise();
        _buttonEyeDiff->raise();
        _editEyeRotY->raise();
        _lEyeCoord_2->raise();
        _editEyeRotX->raise();
        _editEyeRotZ->raise();
        _editEyeRotW->raise();
        _lEyeCoord_3->raise();
        verticalLayoutWidget->raise();
        menuBar = new QMenuBar(DARVClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 31));
        DARVClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(DARVClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        DARVClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(DARVClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        DARVClass->setStatusBar(statusBar);

        retranslateUi(DARVClass);

        QMetaObject::connectSlotsByName(DARVClass);
    } // setupUi

    void retranslateUi(QMainWindow *DARVClass)
    {
        DARVClass->setWindowTitle(QApplication::translate("DARVClass", "DARV ", 0));
        _lStereoSetting->setText(QApplication::translate("DARVClass", "Stereo Setting", 0));
        _lVirtualSetting->setText(QApplication::translate("DARVClass", "Virtual Setting", 0));
        _lARSetting->setText(QApplication::translate("DARVClass", "AR Setting", 0));
        _checkboxStereo->setText(QApplication::translate("DARVClass", "Stereo View", 0));
        _rButtonOST->setText(QApplication::translate("DARVClass", "OST", 0));
        _rButtonVST->setText(QApplication::translate("DARVClass", "VST", 0));
        _rButtonPC->setText(QApplication::translate("DARVClass", "PC Monitor", 0));
        _lBackground->setText(QApplication::translate("DARVClass", "Hintergrund", 0));
        _lView->setText(QApplication::translate("DARVClass", "View", 0));
        _rButtonSideBySide->setText(QApplication::translate("DARVClass", "Side - by - Side", 0));
        _rButtonAnaglyph->setText(QApplication::translate("DARVClass", "Anaglyph", 0));
        _lVObject->setText(QApplication::translate("DARVClass", "Virtuell Object", 0));
        _lParallax->setText(QApplication::translate("DARVClass", "z-0-Parallaxe", 0));
        _lFovy->setText(QApplication::translate("DARVClass", "FOV Y", 0));
        _lEyeSep->setText(QApplication::translate("DARVClass", "Augabstand", 0));
        _lPos->setText(QApplication::translate("DARVClass", "Transformation Brille zu Auge", 0));
        _lTrackingState->setText(QApplication::translate("DARVClass", "Tracking Status", 0));
        _lEyeCoord->setText(QApplication::translate("DARVClass", "X                       Y                      Z", 0));
        _checkBoxObject->setText(QApplication::translate("DARVClass", "    An Marker pinnen", 0));
        _lObjectCoord->setText(QApplication::translate("DARVClass", "X                            Y                             Z", 0));
        _buttonEyeDiff->setText(QApplication::translate("DARVClass", "Abweichung\n"
" speichern", 0));
        _lEyeCoord_2->setText(QApplication::translate("DARVClass", "Rx                    Ry                      Rz                  ", 0));
        _lEyeCoord_3->setText(QApplication::translate("DARVClass", " Rw", 0));
    } // retranslateUi

};

namespace Ui {
    class DARVClass: public Ui_DARVClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DARV_H
