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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DARVClass
{
public:
    QWidget *centralWidget;
    QWidget *_camWidget;
    QTabWidget *tW_data;
    QWidget *tab;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_2;
    QTreeWidget *_tWMarker;
    QWidget *tab_2;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *_hLUncer;
    QWidget *tab_4;
    QTreeWidget *_tWIMU;
    QWidget *_rotWidget;
    QWidget *tab_6;
    QTreeWidget *_tWTimes;
    QTabWidget *tW_settings;
    QWidget *_tabAR;
    QScrollArea *scrollArea;
    QWidget *_scrollAreaWidgetContents;
    QFrame *verticalFrame_2;
    QVBoxLayout *verticalLayout;
    QLabel *_lVObject;
    QListWidget *_listVObject;
    QFrame *line;
    QLabel *_lARSetting_2;
    QCheckBox *_checkBoxObject;
    QHBoxLayout *horizontalLayout;
    QLabel *_lObjectCoord;
    QLineEdit *_editObjectTransX;
    QHBoxLayout *horizontalLayout_3;
    QLabel *_lObjectCoord_2;
    QLineEdit *_editObjectTransY;
    QHBoxLayout *horizontalLayout_4;
    QLabel *_lObjectCoord_3;
    QLineEdit *_editObjectTransZ;
    QWidget *tab_5;
    QFrame *verticalFrame;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *_cbDARV;
    QHBoxLayout *horizontalLayout_35;
    QCheckBox *_cbFixedQuality;
    QCheckBox *_cbClassification;
    QHBoxLayout *horizontalLayout_36;
    QLabel *_lAD;
    QSlider *_vSAD;
    QLabel *_lADValue;
    QWidget *_tabSIM;
    QScrollArea *_scrollArea2;
    QWidget *_scrollAreaWidgetContents2;
    QFrame *verticalFrame1;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_13;
    QLabel *_lEyeCorrection_2;
    QLabel *_lData;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *_pBData_3;
    QPushButton *_pBPause;
    QPushButton *_pBData_4;
    QPushButton *_pBData;
    QFrame *line_8;
    QCheckBox *_cbUseRecords;
    QListWidget *_listRecords;
    QFrame *line_2;
    QHBoxLayout *horizontalLayout_15;
    QLabel *_lEyeCorrection_4;
    QLabel *_lIMU;
    QPushButton *_pBResetRot;
    QHBoxLayout *horizontalLayout_9;
    QRadioButton *_rButtonVUZIX;
    QRadioButton *_rButtonSIMON;
    QHBoxLayout *horizontalLayout_8;
    QSlider *_hSInterpol;
    QLabel *_lInterpol;
    QFrame *line_7;
    QHBoxLayout *horizontalLayout_14;
    QLabel *_lEyeCorrection_3;
    QLabel *label;
    QCheckBox *_cbUseRefData;
    QLabel *_lRefData;
    QPushButton *_pBConnect;
    QPushButton *_pBResetRef;
    QFrame *line_4;
    QLabel *_lBlur;
    QPushButton *_pBBlur;
    QWidget *tab_3;
    QFrame *verticalFrame2;
    QVBoxLayout *verticalLayout_4;
    QLabel *_lARSetting;
    QCheckBox *_checkboxStereo;
    QHBoxLayout *horizontalLayout_10;
    QRadioButton *_rButtonVST;
    QRadioButton *_rButtonOST;
    QFrame *line_6;
    QLabel *_lView;
    QHBoxLayout *horizontalLayout_5;
    QLabel *_lFovy;
    QLineEdit *_editFovy;
    QHBoxLayout *horizontalLayout_6;
    QLabel *_lEyeSep;
    QLineEdit *_editEyeSep;
    QHBoxLayout *horizontalLayout_7;
    QLabel *_lEyeCorrection;
    QLineEdit *_editEyeCorrection;
    QLabel *_lARSetting_3;
    QStatusBar *statusBar;
    QButtonGroup *_bgIMUTracking;
    QButtonGroup *_bgView;

    void setupUi(QMainWindow *DARVClass)
    {
        if (DARVClass->objectName().isEmpty())
            DARVClass->setObjectName(QStringLiteral("DARVClass"));
        DARVClass->resize(1285, 807);
        DARVClass->setStyleSheet(QLatin1String("QToolTip\n"
"{\n"
"     border: 1px solid black;\n"
"     background-color: #53A6AA;\n"
"     padding: 1px;\n"
"     border-radius: 3px;\n"
"     opacity: 100;\n"
"}\n"
"\n"
"QWidget\n"
"{\n"
"    color: #b1b1b1;\n"
"    background-color: #323232;\n"
"}\n"
"\n"
"QWidget:item:hover\n"
"{\n"
"    background-color: #53A6AA;\n"
"    color: #000000;\n"
"}\n"
"\n"
"QWidget:item:selected\n"
"{\n"
"    background-color: #53A6AA;\n"
"}\n"
"\n"
"QMenuBar::item\n"
"{\n"
"    background: transparent;\n"
"}\n"
"\n"
"QMenuBar::item:selected\n"
"{\n"
"    background: transparent;\n"
"    border: 1px solid #53A6AA;\n"
"}\n"
"\n"
"QMenuBar::item:pressed\n"
"{\n"
"    background: #444;\n"
"    border: 1px solid #000;\n"
"    background-color: QLinearGradient(\n"
"        x1:0, y1:0,\n"
"        x2:0, y2:1,\n"
"        stop:1 #212121,\n"
"        stop:0.4 #343434/*,\n"
"        stop:0.2 #343434,\n"
"        stop:0.1 #ffaa00*/\n"
"    );\n"
"    margin-bottom:-1px;\n"
"    padding-bottom:1px;\n"
"}\n"
"\n"
"QMenu\n"
"{\n"
"    bo"
                        "rder: 1px solid #000;\n"
"}\n"
"\n"
"QMenu::item\n"
"{\n"
"    padding: 2px 20px 2px 20px;\n"
"}\n"
"\n"
"QMenu::item:selected\n"
"{\n"
"    color: #000000;\n"
"}\n"
"\n"
"QWidget:disabled\n"
"{\n"
"    color: #404040;\n"
"    background-color: #323232;\n"
"}\n"
"\n"
"QAbstractItemView\n"
"{\n"
"    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #4d4d4d, stop: 0.1 #646464, stop: 1 #5d5d5d);\n"
"}\n"
"\n"
"QWidget:focus\n"
"{\n"
"    /*border: 2px solid QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a);*/\n"
"}\n"
"\n"
"QLineEdit\n"
"{\n"
"    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #4d4d4d, stop: 0 #646464, stop: 1 #5d5d5d);\n"
"    padding: 1px;\n"
"    border-style: solid;\n"
"    border: 1px solid #1e1e1e;\n"
"    border-radius: 5;\n"
"}\n"
"\n"
"QPushButton\n"
"{\n"
"    color: #b1b1b1;\n"
"    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #565656, stop: 0.1 #525252, stop: 0.5 #4e4e4e, stop: 0.9 #4a"
                        "4a4a, stop: 1 #464646);\n"
"    border-width: 1px;\n"
"    border-color: #1e1e1e;\n"
"    border-style: solid;\n"
"    border-radius: 6;\n"
"    padding: 5px;\n"
"    font-size: 15px;\n"
"    padding-left: 5px;\n"
"    padding-right: 5px;\n"
"}\n"
"\n"
"QPushButton:pressed\n"
"{\n"
"    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2d2d2d, stop: 0.1 #2b2b2b, stop: 0.5 #292929, stop: 0.9 #282828, stop: 1 #252525);\n"
"}\n"
"\n"
"QComboBox\n"
"{\n"
"    selection-background-color: #64797B;\n"
"    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #565656, stop: 0.1 #525252, stop: 0.5 #4e4e4e, stop: 0.9 #4a4a4a, stop: 1 #464646);\n"
"    border-style: solid;\n"
"    border: 1px solid #1e1e1e;\n"
"    border-radius: 5;\n"
"}\n"
"\n"
"QComboBox:hover,QPushButton:hover\n"
"{\n"
"    border: 2px solid #53A6AA;\n"
"}\n"
"\n"
"\n"
"QComboBox:on\n"
"{\n"
"    padding-top: 4px;\n"
"    padding-left: 4px;\n"
"    background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, "
                        "stop: 0 #2d2d2d, stop: 0.1 #2b2b2b, stop: 0.5 #292929, stop: 0.9 #282828, stop: 1 #252525);\n"
"    selection-background-color: #53A6AA;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView\n"
"{\n"
"    border: 2px solid darkgray;\n"
"    selection-background-color:#53A6AA;\n"
"}\n"
"\n"
"QComboBox::drop-down\n"
"{\n"
"     subcontrol-origin: padding;\n"
"     subcontrol-position: top right;\n"
"     width: 15px;\n"
"\n"
"     border-left-width: 0px;\n"
"     border-left-color: darkgray;\n"
"     border-left-style: solid; /* just a single line */\n"
"     border-top-right-radius: 3px; /* same radius as the QComboBox */\n"
"     border-bottom-right-radius: 3px;\n"
" }\n"
"\n"
"QComboBox::down-arrow\n"
"{\n"
"     image: url(:/down_arrow.png);\n"
"}\n"
"\n"
"QGroupBox:focus\n"
"{\n"
"border: 2px solid #53A6AA;\n"
"}\n"
"\n"
"QTextEdit:focus\n"
"{\n"
"    border: 2px solid #53A6AA;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"     border: 1px solid #222222;\n"
"     background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, s"
                        "top: 0.0 #121212, stop: 0.2 #282828, stop: 1 #484848);\n"
"     height: 10px;\n"
"     margin: 0px 60px 0 16px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal\n"
"{\n"
"      background: #53A6AA;\n"
"      min-height: 50px;\n"
"      border-radius: 30px;\n"
"}\n"
"\n"
"QScrollBar::add-line:horizontal {\n"
"      border: 1px solid #1b1b19;\n"
"      border-radius: 2px;\n"
"      background: #53A6AA;\n"
"      width: 14px;\n"
"      subcontrol-position: right;\n"
"      subcontrol-origin: margin;\n"
"}\n"
"\n"
"QScrollBar::sub-line:horizontal {\n"
"      border: 1px solid #1b1b19;\n"
"      border-radius: 2px;\n"
"      background: #53A6AA;\n"
"      width: 14px;\n"
"     subcontrol-position: left;\n"
"     subcontrol-origin: margin;\n"
"}\n"
"\n"
"QScrollBar::right-arrow:horizontal, QScrollBar::left-arrow:horizontal\n"
"{\n"
"      border: 1px solid black;\n"
"      width: 1px;\n"
"      height: 1px;\n"
"      background: white;\n"
"}\n"
"\n"
"QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal\n"
"{"
                        "\n"
"      background: none;\n"
"}\n"
"\n"
"QScrollBar:vertical\n"
"{\n"
"      background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0.0 #121212, stop: 0.2 #282828, stop: 1 #484848);\n"
"      width:10px;\n"
"      margin: 16px 0 16px 0;\n"
"      border: 1px solid #222222;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical\n"
"{\n"
"      background: #53A6AA;\n"
"      min-height: 20px;\n"
"      border-radius: 2px;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical\n"
"{\n"
"      border: 1px solid #1b1b19;\n"
"      border-radius: 2px;\n"
"      background: #53A6AA;\n"
"      height: 14px;\n"
"      subcontrol-position: bottom;\n"
"      subcontrol-origin: margin;\n"
"}\n"
"\n"
"QScrollBar::sub-line:vertical\n"
"{\n"
"      border: 1px solid #1b1b19;\n"
"      border-radius: 2px;\n"
"      background: #53A6AA;\n"
"      height: 14px;\n"
"      subcontrol-position: top;\n"
"      subcontrol-origin: margin;\n"
"}\n"
"\n"
"QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical\n"
"{\n"
"      border: 1px s"
                        "olid black;\n"
"      width: 1px;\n"
"      height: 1px;\n"
"      background: white;\n"
"}\n"
"\n"
"\n"
"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical\n"
"{\n"
"      background: none;\n"
"}\n"
"\n"
"QTextEdit\n"
"{\n"
"    background-color: #242424;\n"
"}\n"
"\n"
"QPlainTextEdit\n"
"{\n"
"    background-color: #242424;\n"
"}\n"
"\n"
"QHeaderView::section\n"
"{\n"
"    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #616161, stop: 0.5 #505050, stop: 0.6 #434343, stop:1 #656565);\n"
"    color: white;\n"
"    padding-left: 4px;\n"
"    border: 1px solid #6c6c6c;\n"
"}\n"
"\n"
"QCheckBox:disabled\n"
"{\n"
"color: #414141;\n"
"}\n"
"\n"
"QDockWidget::title\n"
"{\n"
"    text-align: center;\n"
"    spacing: 3px; /* spacing between items in the tool bar */\n"
"    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #323232, stop: 0.5 #242424, stop:1 #323232);\n"
"}\n"
"\n"
"QDockWidget::close-button, QDockWidget::float-button\n"
"{\n"
"    text-align: center;\n"
"    sp"
                        "acing: 1px; /* spacing between items in the tool bar */\n"
"    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #323232, stop: 0.5 #242424, stop:1 #323232);\n"
"}\n"
"\n"
"QDockWidget::close-button:hover, QDockWidget::float-button:hover\n"
"{\n"
"    background: #242424;\n"
"}\n"
"\n"
"QDockWidget::close-button:pressed, QDockWidget::float-button:pressed\n"
"{\n"
"    padding: 1px -1px -1px 1px;\n"
"}\n"
"\n"
"QMainWindow::separator\n"
"{\n"
"    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #161616, stop: 0.5 #151515, stop: 0.6 #212121, stop:1 #343434);\n"
"    color: white;\n"
"    padding-left: 4px;\n"
"    border: 1px solid #4c4c4c;\n"
"    spacing: 3px; /* spacing between items in the tool bar */\n"
"}\n"
"\n"
"QMainWindow::separator:hover\n"
"{\n"
"\n"
"    background-color: #53A6AA;\n"
"    color: white;\n"
"    padding-left: 4px;\n"
"    border: 1px solid #6c6c6c;\n"
"    spacing: 3px; /* spacing between items in the tool bar */\n"
"}\n"
"\n"
"QToolBar::handle\n"
"{\n"
""
                        "     spacing: 3px; /* spacing between items in the tool bar */\n"
"     background: url(:/images/handle.png);\n"
"}\n"
"\n"
"QMenu::separator\n"
"{\n"
"    height: 2px;\n"
"    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:0 #161616, stop: 0.5 #151515, stop: 0.6 #212121, stop:1 #343434);\n"
"    color: white;\n"
"    padding-left: 4px;\n"
"    margin-left: 10px;\n"
"    margin-right: 5px;\n"
"}\n"
"\n"
"QProgressBar\n"
"{\n"
"    border: 2px solid grey;\n"
"    border-radius: 5px;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QProgressBar::chunk\n"
"{\n"
"    background-color: #64797B;\n"
"    width: 2.15px;\n"
"    margin: 0.5px;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    color: #b1b1b1;\n"
"    border: 1px solid #444;\n"
"    border-bottom-style: none;\n"
"    background-color: #323232;\n"
"    padding-left: 10px;\n"
"    padding-right: 10px;\n"
"    padding-top: 3px;\n"
"    padding-bottom: 2px;\n"
"    margin-right: -1px;\n"
"}\n"
"\n"
"QTabWidget::pane {\n"
"    border: 1px solid #444;\n"
"    to"
                        "p: 1px;\n"
"}\n"
"\n"
"QTabBar::tab:last\n"
"{\n"
"    margin-right: 0; /* the last selected tab has nothing to overlap with on the right */\n"
"    border-top-right-radius: 3px;\n"
"}\n"
"\n"
"QTabBar::tab:first:!selected\n"
"{\n"
" margin-left: 0px; /* the last selected tab has nothing to overlap with on the right */\n"
"\n"
"\n"
"    border-top-left-radius: 3px;\n"
"}\n"
"\n"
"QTabBar::tab:!selected\n"
"{\n"
"    color: #b1b1b1;\n"
"    border-bottom-style: solid;\n"
"    margin-top: 3px;\n"
"    background-color: QLinearGradient(x1:0, y1:0, x2:0, y2:1, stop:1 #212121, stop:.4 #343434);\n"
"}\n"
"\n"
"QTabBar::tab:selected\n"
"{\n"
"    border-top-left-radius: 3px;\n"
"    border-top-right-radius: 3px;\n"
"    margin-bottom: 0px;\n"
"}\n"
"\n"
"QTabBar::tab:!selected:hover\n"
"{\n"
"    /*border-top: 2px solid #ffaa00;\n"
"    padding-bottom: 3px;*/\n"
"    border-top-left-radius: 3px;\n"
"    border-top-right-radius: 3px;\n"
"    background-color:#53A6AA;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked, QRa"
                        "dioButton::indicator:unchecked{\n"
"    color: #b1b1b1;\n"
"    background-color: #323232;\n"
"    border: 1px solid #b1b1b1;\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked\n"
"{\n"
"    background-color: #53A6AA;\n"
"}\n"
"\n"
"QCheckBox::indicator{\n"
"    color: #53A6AA;\n"
"    background-color: #323232;\n"
"    border: 1px solid #53A6AA;\n"
"    width: 9px;\n"
"    height: 9px;\n"
"}\n"
"\n"
"QRadioButton::indicator\n"
"{\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"QRadioButton::indicator:hover, QCheckBox::indicator:hover\n"
"{\n"
"    border: 1px solid #64797B;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked\n"
"{\n"
"     background-color: #53A6AA;\n"
"}\n"
"\n"
"QCheckBox::indicator:disabled, QRadioButton::indicator:disabled\n"
"{\n"
"    border: 1px solid #444;\n"
"}\n"
""));
        DARVClass->setAnimated(true);
        centralWidget = new QWidget(DARVClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setStyleSheet(QStringLiteral(""));
        _camWidget = new QWidget(centralWidget);
        _camWidget->setObjectName(QStringLiteral("_camWidget"));
        _camWidget->setGeometry(QRect(340, 10, 931, 581));
        QFont font;
        font.setFamily(QStringLiteral("Segoe UI"));
        _camWidget->setFont(font);
        tW_data = new QTabWidget(centralWidget);
        tW_data->setObjectName(QStringLiteral("tW_data"));
        tW_data->setGeometry(QRect(340, 580, 941, 201));
        tW_data->setTabPosition(QTabWidget::North);
        tW_data->setTabShape(QTabWidget::Rounded);
        tW_data->setElideMode(Qt::ElideNone);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        scrollArea_2 = new QScrollArea(tab);
        scrollArea_2->setObjectName(QStringLiteral("scrollArea_2"));
        scrollArea_2->setGeometry(QRect(0, 0, 931, 195));
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QStringLiteral("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 929, 193));
        _tWMarker = new QTreeWidget(scrollAreaWidgetContents_2);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        _tWMarker->setHeaderItem(__qtreewidgetitem);
        _tWMarker->setObjectName(QStringLiteral("_tWMarker"));
        _tWMarker->setGeometry(QRect(0, 0, 931, 171));
        _tWMarker->setFont(font);
        _tWMarker->setFrameShape(QFrame::NoFrame);
        _tWMarker->setItemsExpandable(true);
        _tWMarker->header()->setDefaultSectionSize(200);
        _tWMarker->header()->setStretchLastSection(false);
        scrollArea_2->setWidget(scrollAreaWidgetContents_2);
        tW_data->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        horizontalLayoutWidget = new QWidget(tab_2);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 911, 191));
        _hLUncer = new QHBoxLayout(horizontalLayoutWidget);
        _hLUncer->setSpacing(0);
        _hLUncer->setContentsMargins(11, 11, 11, 11);
        _hLUncer->setObjectName(QStringLiteral("_hLUncer"));
        _hLUncer->setSizeConstraint(QLayout::SetDefaultConstraint);
        _hLUncer->setContentsMargins(0, 0, 0, 0);
        tW_data->addTab(tab_2, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        _tWIMU = new QTreeWidget(tab_4);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QStringLiteral("1"));
        _tWIMU->setHeaderItem(__qtreewidgetitem1);
        _tWIMU->setObjectName(QStringLiteral("_tWIMU"));
        _tWIMU->setGeometry(QRect(0, 0, 751, 195));
        _tWIMU->setFont(font);
        _tWIMU->setFrameShape(QFrame::NoFrame);
        _rotWidget = new QWidget(tab_4);
        _rotWidget->setObjectName(QStringLiteral("_rotWidget"));
        _rotWidget->setGeometry(QRect(760, 0, 161, 170));
        _rotWidget->setFont(font);
        tW_data->addTab(tab_4, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        _tWTimes = new QTreeWidget(tab_6);
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::NoBrush);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem();
        __qtreewidgetitem2->setText(0, QStringLiteral("1"));
        __qtreewidgetitem2->setBackground(0, QColor(255, 255, 255));
        __qtreewidgetitem2->setForeground(0, brush);
        _tWTimes->setHeaderItem(__qtreewidgetitem2);
        _tWTimes->setObjectName(QStringLiteral("_tWTimes"));
        _tWTimes->setGeometry(QRect(0, 0, 941, 171));
        QFont font1;
        font1.setFamily(QStringLiteral("Segoe UI"));
        font1.setPointSize(11);
        _tWTimes->setFont(font1);
        _tWTimes->setStyleSheet(QStringLiteral(""));
        _tWTimes->setFrameShape(QFrame::NoFrame);
        _tWTimes->setFrameShadow(QFrame::Plain);
        _tWTimes->setLineWidth(0);
        _tWTimes->header()->setVisible(false);
        tW_data->addTab(tab_6, QString());
        tW_settings = new QTabWidget(centralWidget);
        tW_settings->setObjectName(QStringLiteral("tW_settings"));
        tW_settings->setGeometry(QRect(0, 40, 321, 800));
        tW_settings->setTabPosition(QTabWidget::North);
        tW_settings->setTabShape(QTabWidget::Rounded);
        tW_settings->setElideMode(Qt::ElideNone);
        tW_settings->setUsesScrollButtons(true);
        _tabAR = new QWidget();
        _tabAR->setObjectName(QStringLiteral("_tabAR"));
        scrollArea = new QScrollArea(_tabAR);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setGeometry(QRect(-20, -30, 331, 900));
        scrollArea->setMinimumSize(QSize(100, 900));
        scrollArea->setFont(font);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setFrameShadow(QFrame::Plain);
        scrollArea->setLineWidth(0);
        scrollArea->setWidgetResizable(true);
        _scrollAreaWidgetContents = new QWidget();
        _scrollAreaWidgetContents->setObjectName(QStringLiteral("_scrollAreaWidgetContents"));
        _scrollAreaWidgetContents->setGeometry(QRect(0, 0, 331, 900));
        _scrollAreaWidgetContents->setMinimumSize(QSize(0, 781));
        verticalFrame_2 = new QFrame(_scrollAreaWidgetContents);
        verticalFrame_2->setObjectName(QStringLiteral("verticalFrame_2"));
        verticalFrame_2->setGeometry(QRect(20, 30, 311, 531));
        QPalette palette;
        QBrush brush1(QColor(177, 177, 177, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush1);
        QBrush brush2(QColor(50, 50, 50, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush1);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        QBrush brush3(QColor(64, 64, 64, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        verticalFrame_2->setPalette(palette);
        verticalFrame_2->setFont(font);
        verticalFrame_2->setAutoFillBackground(false);
        verticalLayout = new QVBoxLayout(verticalFrame_2);
        verticalLayout->setSpacing(20);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(-1, 1, -1, -1);
        _lVObject = new QLabel(verticalFrame_2);
        _lVObject->setObjectName(QStringLiteral("_lVObject"));
        QFont font2;
        font2.setFamily(QStringLiteral("Segoe UI"));
        font2.setPointSize(10);
        _lVObject->setFont(font2);

        verticalLayout->addWidget(_lVObject);

        _listVObject = new QListWidget(verticalFrame_2);
        _listVObject->setObjectName(QStringLiteral("_listVObject"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(_listVObject->sizePolicy().hasHeightForWidth());
        _listVObject->setSizePolicy(sizePolicy);
        _listVObject->setMinimumSize(QSize(0, 150));
        _listVObject->setMaximumSize(QSize(16777215, 300));
        _listVObject->setSizeIncrement(QSize(0, 0));
        _listVObject->setBaseSize(QSize(0, 0));
        _listVObject->setFont(font);
        _listVObject->setSortingEnabled(false);

        verticalLayout->addWidget(_listVObject);

        line = new QFrame(verticalFrame_2);
        line->setObjectName(QStringLiteral("line"));
        line->setFont(font);
        line->setFrameShadow(QFrame::Plain);
        line->setFrameShape(QFrame::HLine);

        verticalLayout->addWidget(line);

        _lARSetting_2 = new QLabel(verticalFrame_2);
        _lARSetting_2->setObjectName(QStringLiteral("_lARSetting_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(_lARSetting_2->sizePolicy().hasHeightForWidth());
        _lARSetting_2->setSizePolicy(sizePolicy1);
        _lARSetting_2->setFont(font2);

        verticalLayout->addWidget(_lARSetting_2);

        _checkBoxObject = new QCheckBox(verticalFrame_2);
        _checkBoxObject->setObjectName(QStringLiteral("_checkBoxObject"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(_checkBoxObject->sizePolicy().hasHeightForWidth());
        _checkBoxObject->setSizePolicy(sizePolicy2);
        _checkBoxObject->setFont(font2);
        _checkBoxObject->setChecked(true);

        verticalLayout->addWidget(_checkBoxObject);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        _lObjectCoord = new QLabel(verticalFrame_2);
        _lObjectCoord->setObjectName(QStringLiteral("_lObjectCoord"));
        _lObjectCoord->setFont(font2);

        horizontalLayout->addWidget(_lObjectCoord);

        _editObjectTransX = new QLineEdit(verticalFrame_2);
        _editObjectTransX->setObjectName(QStringLiteral("_editObjectTransX"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(_editObjectTransX->sizePolicy().hasHeightForWidth());
        _editObjectTransX->setSizePolicy(sizePolicy3);
        _editObjectTransX->setFont(font);

        horizontalLayout->addWidget(_editObjectTransX);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        _lObjectCoord_2 = new QLabel(verticalFrame_2);
        _lObjectCoord_2->setObjectName(QStringLiteral("_lObjectCoord_2"));
        _lObjectCoord_2->setFont(font2);

        horizontalLayout_3->addWidget(_lObjectCoord_2);

        _editObjectTransY = new QLineEdit(verticalFrame_2);
        _editObjectTransY->setObjectName(QStringLiteral("_editObjectTransY"));
        sizePolicy3.setHeightForWidth(_editObjectTransY->sizePolicy().hasHeightForWidth());
        _editObjectTransY->setSizePolicy(sizePolicy3);
        _editObjectTransY->setFont(font);

        horizontalLayout_3->addWidget(_editObjectTransY);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        _lObjectCoord_3 = new QLabel(verticalFrame_2);
        _lObjectCoord_3->setObjectName(QStringLiteral("_lObjectCoord_3"));
        _lObjectCoord_3->setFont(font2);

        horizontalLayout_4->addWidget(_lObjectCoord_3);

        _editObjectTransZ = new QLineEdit(verticalFrame_2);
        _editObjectTransZ->setObjectName(QStringLiteral("_editObjectTransZ"));
        sizePolicy3.setHeightForWidth(_editObjectTransZ->sizePolicy().hasHeightForWidth());
        _editObjectTransZ->setSizePolicy(sizePolicy3);
        _editObjectTransZ->setFont(font);

        horizontalLayout_4->addWidget(_editObjectTransZ);


        verticalLayout->addLayout(horizontalLayout_4);

        scrollArea->setWidget(_scrollAreaWidgetContents);
        tW_settings->addTab(_tabAR, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        verticalFrame = new QFrame(tab_5);
        verticalFrame->setObjectName(QStringLiteral("verticalFrame"));
        verticalFrame->setGeometry(QRect(10, 0, 301, 201));
        verticalLayout_2 = new QVBoxLayout(verticalFrame);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        _cbDARV = new QCheckBox(verticalFrame);
        _cbDARV->setObjectName(QStringLiteral("_cbDARV"));
        sizePolicy2.setHeightForWidth(_cbDARV->sizePolicy().hasHeightForWidth());
        _cbDARV->setSizePolicy(sizePolicy2);
        _cbDARV->setFont(font2);

        verticalLayout_2->addWidget(_cbDARV);

        horizontalLayout_35 = new QHBoxLayout();
        horizontalLayout_35->setSpacing(6);
        horizontalLayout_35->setObjectName(QStringLiteral("horizontalLayout_35"));
        _cbFixedQuality = new QCheckBox(verticalFrame);
        _cbFixedQuality->setObjectName(QStringLiteral("_cbFixedQuality"));
        sizePolicy2.setHeightForWidth(_cbFixedQuality->sizePolicy().hasHeightForWidth());
        _cbFixedQuality->setSizePolicy(sizePolicy2);
        _cbFixedQuality->setFont(font2);
        _cbFixedQuality->setChecked(true);

        horizontalLayout_35->addWidget(_cbFixedQuality);

        _cbClassification = new QCheckBox(verticalFrame);
        _cbClassification->setObjectName(QStringLiteral("_cbClassification"));
        sizePolicy2.setHeightForWidth(_cbClassification->sizePolicy().hasHeightForWidth());
        _cbClassification->setSizePolicy(sizePolicy2);
        _cbClassification->setFont(font2);
        _cbClassification->setChecked(true);

        horizontalLayout_35->addWidget(_cbClassification);


        verticalLayout_2->addLayout(horizontalLayout_35);

        horizontalLayout_36 = new QHBoxLayout();
        horizontalLayout_36->setSpacing(6);
        horizontalLayout_36->setObjectName(QStringLiteral("horizontalLayout_36"));
        _lAD = new QLabel(verticalFrame);
        _lAD->setObjectName(QStringLiteral("_lAD"));
        _lAD->setMinimumSize(QSize(100, 0));
        _lAD->setFont(font2);
        _lAD->setAlignment(Qt::AlignCenter);

        horizontalLayout_36->addWidget(_lAD);

        _vSAD = new QSlider(verticalFrame);
        _vSAD->setObjectName(QStringLiteral("_vSAD"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(_vSAD->sizePolicy().hasHeightForWidth());
        _vSAD->setSizePolicy(sizePolicy4);
        _vSAD->setFont(font);
        _vSAD->setMinimum(0);
        _vSAD->setMaximum(100);
        _vSAD->setValue(0);
        _vSAD->setOrientation(Qt::Horizontal);

        horizontalLayout_36->addWidget(_vSAD);

        _lADValue = new QLabel(verticalFrame);
        _lADValue->setObjectName(QStringLiteral("_lADValue"));
        _lADValue->setFont(font2);
        _lADValue->setAlignment(Qt::AlignCenter);

        horizontalLayout_36->addWidget(_lADValue);


        verticalLayout_2->addLayout(horizontalLayout_36);

        tW_settings->addTab(tab_5, QString());
        _tabSIM = new QWidget();
        _tabSIM->setObjectName(QStringLiteral("_tabSIM"));
        _scrollArea2 = new QScrollArea(_tabSIM);
        _scrollArea2->setObjectName(QStringLiteral("_scrollArea2"));
        _scrollArea2->setGeometry(QRect(-10, 0, 331, 711));
        _scrollArea2->setWidgetResizable(true);
        _scrollAreaWidgetContents2 = new QWidget();
        _scrollAreaWidgetContents2->setObjectName(QStringLiteral("_scrollAreaWidgetContents2"));
        _scrollAreaWidgetContents2->setGeometry(QRect(0, 0, 319, 1200));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(_scrollAreaWidgetContents2->sizePolicy().hasHeightForWidth());
        _scrollAreaWidgetContents2->setSizePolicy(sizePolicy5);
        _scrollAreaWidgetContents2->setMinimumSize(QSize(0, 1200));
        verticalFrame1 = new QFrame(_scrollAreaWidgetContents2);
        verticalFrame1->setObjectName(QStringLiteral("verticalFrame1"));
        verticalFrame1->setGeometry(QRect(10, 0, 311, 1041));
        verticalLayout_3 = new QVBoxLayout(verticalFrame1);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(10);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        horizontalLayout_13->setContentsMargins(-1, 0, -1, -1);
        _lEyeCorrection_2 = new QLabel(verticalFrame1);
        _lEyeCorrection_2->setObjectName(QStringLiteral("_lEyeCorrection_2"));
        sizePolicy4.setHeightForWidth(_lEyeCorrection_2->sizePolicy().hasHeightForWidth());
        _lEyeCorrection_2->setSizePolicy(sizePolicy4);
        _lEyeCorrection_2->setMinimumSize(QSize(10, 10));
        _lEyeCorrection_2->setMaximumSize(QSize(30, 30));
        _lEyeCorrection_2->setFont(font2);
        _lEyeCorrection_2->setPixmap(QPixmap(QString::fromUtf8(":/DARV/Resources/Skin/iconCapture.png")));
        _lEyeCorrection_2->setScaledContents(true);

        horizontalLayout_13->addWidget(_lEyeCorrection_2);

        _lData = new QLabel(verticalFrame1);
        _lData->setObjectName(QStringLiteral("_lData"));
        _lData->setFont(font2);
        _lData->setMouseTracking(true);
        _lData->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(_lData);


        verticalLayout_3->addLayout(horizontalLayout_13);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        _pBData_3 = new QPushButton(verticalFrame1);
        _pBData_3->setObjectName(QStringLiteral("_pBData_3"));
        QFont font3;
        _pBData_3->setFont(font3);

        horizontalLayout_11->addWidget(_pBData_3);

        _pBPause = new QPushButton(verticalFrame1);
        _pBPause->setObjectName(QStringLiteral("_pBPause"));
        _pBPause->setFont(font3);

        horizontalLayout_11->addWidget(_pBPause);

        _pBData_4 = new QPushButton(verticalFrame1);
        _pBData_4->setObjectName(QStringLiteral("_pBData_4"));
        _pBData_4->setFont(font3);

        horizontalLayout_11->addWidget(_pBData_4);

        _pBData = new QPushButton(verticalFrame1);
        _pBData->setObjectName(QStringLiteral("_pBData"));
        _pBData->setFont(font3);

        horizontalLayout_11->addWidget(_pBData);


        verticalLayout_3->addLayout(horizontalLayout_11);

        line_8 = new QFrame(verticalFrame1);
        line_8->setObjectName(QStringLiteral("line_8"));
        line_8->setFont(font);
        line_8->setFrameShadow(QFrame::Plain);
        line_8->setLineWidth(1);
        line_8->setMidLineWidth(0);
        line_8->setFrameShape(QFrame::HLine);

        verticalLayout_3->addWidget(line_8);

        _cbUseRecords = new QCheckBox(verticalFrame1);
        _cbUseRecords->setObjectName(QStringLiteral("_cbUseRecords"));
        sizePolicy2.setHeightForWidth(_cbUseRecords->sizePolicy().hasHeightForWidth());
        _cbUseRecords->setSizePolicy(sizePolicy2);
        _cbUseRecords->setMinimumSize(QSize(0, 0));
        _cbUseRecords->setFont(font2);
        _cbUseRecords->setChecked(false);

        verticalLayout_3->addWidget(_cbUseRecords);

        _listRecords = new QListWidget(verticalFrame1);
        _listRecords->setObjectName(QStringLiteral("_listRecords"));
        sizePolicy.setHeightForWidth(_listRecords->sizePolicy().hasHeightForWidth());
        _listRecords->setSizePolicy(sizePolicy);
        _listRecords->setMinimumSize(QSize(0, 150));
        _listRecords->setMaximumSize(QSize(16777215, 300));
        _listRecords->setSizeIncrement(QSize(0, 0));
        _listRecords->setBaseSize(QSize(0, 0));
        _listRecords->setFont(font);
        _listRecords->setSortingEnabled(true);

        verticalLayout_3->addWidget(_listRecords);

        line_2 = new QFrame(verticalFrame1);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFont(font);
        line_2->setFrameShadow(QFrame::Plain);
        line_2->setFrameShape(QFrame::HLine);

        verticalLayout_3->addWidget(line_2);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(10);
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        horizontalLayout_15->setContentsMargins(-1, 0, -1, -1);
        _lEyeCorrection_4 = new QLabel(verticalFrame1);
        _lEyeCorrection_4->setObjectName(QStringLiteral("_lEyeCorrection_4"));
        sizePolicy4.setHeightForWidth(_lEyeCorrection_4->sizePolicy().hasHeightForWidth());
        _lEyeCorrection_4->setSizePolicy(sizePolicy4);
        _lEyeCorrection_4->setMinimumSize(QSize(10, 10));
        _lEyeCorrection_4->setMaximumSize(QSize(30, 30));
        _lEyeCorrection_4->setFont(font2);
        _lEyeCorrection_4->setPixmap(QPixmap(QString::fromUtf8(":/DARV/Resources/Skin/iconIMUSetting.png")));
        _lEyeCorrection_4->setScaledContents(true);

        horizontalLayout_15->addWidget(_lEyeCorrection_4);

        _lIMU = new QLabel(verticalFrame1);
        _lIMU->setObjectName(QStringLiteral("_lIMU"));
        _lIMU->setFont(font2);
        _lIMU->setAlignment(Qt::AlignCenter);

        horizontalLayout_15->addWidget(_lIMU);


        verticalLayout_3->addLayout(horizontalLayout_15);

        _pBResetRot = new QPushButton(verticalFrame1);
        _pBResetRot->setObjectName(QStringLiteral("_pBResetRot"));
        _pBResetRot->setFont(font3);

        verticalLayout_3->addWidget(_pBResetRot);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(-1, 10, -1, 10);
        _rButtonVUZIX = new QRadioButton(verticalFrame1);
        _bgIMUTracking = new QButtonGroup(DARVClass);
        _bgIMUTracking->setObjectName(QStringLiteral("_bgIMUTracking"));
        _bgIMUTracking->addButton(_rButtonVUZIX);
        _rButtonVUZIX->setObjectName(QStringLiteral("_rButtonVUZIX"));
        QFont font4;
        font4.setFamily(QStringLiteral("Segoe UI"));
        font4.setPointSize(9);
        _rButtonVUZIX->setFont(font4);
        _rButtonVUZIX->setChecked(true);

        horizontalLayout_9->addWidget(_rButtonVUZIX);

        _rButtonSIMON = new QRadioButton(verticalFrame1);
        _bgIMUTracking->addButton(_rButtonSIMON);
        _rButtonSIMON->setObjectName(QStringLiteral("_rButtonSIMON"));
        _rButtonSIMON->setFont(font4);

        horizontalLayout_9->addWidget(_rButtonSIMON);


        verticalLayout_3->addLayout(horizontalLayout_9);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(-1, 20, -1, 20);
        _hSInterpol = new QSlider(verticalFrame1);
        _hSInterpol->setObjectName(QStringLiteral("_hSInterpol"));
        _hSInterpol->setFont(font);
        _hSInterpol->setMaximum(360);
        _hSInterpol->setOrientation(Qt::Horizontal);

        horizontalLayout_8->addWidget(_hSInterpol);

        _lInterpol = new QLabel(verticalFrame1);
        _lInterpol->setObjectName(QStringLiteral("_lInterpol"));
        _lInterpol->setFont(font2);

        horizontalLayout_8->addWidget(_lInterpol);


        verticalLayout_3->addLayout(horizontalLayout_8);

        line_7 = new QFrame(verticalFrame1);
        line_7->setObjectName(QStringLiteral("line_7"));
        line_7->setFont(font);
        line_7->setFrameShadow(QFrame::Plain);
        line_7->setFrameShape(QFrame::HLine);

        verticalLayout_3->addWidget(line_7);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setSpacing(10);
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        horizontalLayout_14->setContentsMargins(-1, 0, -1, -1);
        _lEyeCorrection_3 = new QLabel(verticalFrame1);
        _lEyeCorrection_3->setObjectName(QStringLiteral("_lEyeCorrection_3"));
        sizePolicy4.setHeightForWidth(_lEyeCorrection_3->sizePolicy().hasHeightForWidth());
        _lEyeCorrection_3->setSizePolicy(sizePolicy4);
        _lEyeCorrection_3->setMinimumSize(QSize(10, 10));
        _lEyeCorrection_3->setMaximumSize(QSize(30, 30));
        _lEyeCorrection_3->setFont(font2);
        _lEyeCorrection_3->setPixmap(QPixmap(QString::fromUtf8(":/DARV/Resources/Skin/iconOptiSetting.png")));
        _lEyeCorrection_3->setScaledContents(true);

        horizontalLayout_14->addWidget(_lEyeCorrection_3);

        label = new QLabel(verticalFrame1);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font2);
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout_14->addWidget(label);


        verticalLayout_3->addLayout(horizontalLayout_14);

        _cbUseRefData = new QCheckBox(verticalFrame1);
        _cbUseRefData->setObjectName(QStringLiteral("_cbUseRefData"));
        sizePolicy2.setHeightForWidth(_cbUseRefData->sizePolicy().hasHeightForWidth());
        _cbUseRefData->setSizePolicy(sizePolicy2);
        _cbUseRefData->setFont(font2);
        _cbUseRefData->setChecked(false);

        verticalLayout_3->addWidget(_cbUseRefData);

        _lRefData = new QLabel(verticalFrame1);
        _lRefData->setObjectName(QStringLiteral("_lRefData"));
        _lRefData->setFont(font2);

        verticalLayout_3->addWidget(_lRefData);

        _pBConnect = new QPushButton(verticalFrame1);
        _pBConnect->setObjectName(QStringLiteral("_pBConnect"));
        _pBConnect->setFont(font3);

        verticalLayout_3->addWidget(_pBConnect);

        _pBResetRef = new QPushButton(verticalFrame1);
        _pBResetRef->setObjectName(QStringLiteral("_pBResetRef"));
        _pBResetRef->setFont(font3);

        verticalLayout_3->addWidget(_pBResetRef);

        line_4 = new QFrame(verticalFrame1);
        line_4->setObjectName(QStringLiteral("line_4"));
        line_4->setBaseSize(QSize(0, 0));
        line_4->setFont(font);
        line_4->setFrameShadow(QFrame::Plain);
        line_4->setFrameShape(QFrame::HLine);

        verticalLayout_3->addWidget(line_4);

        _lBlur = new QLabel(verticalFrame1);
        _lBlur->setObjectName(QStringLiteral("_lBlur"));
        _lBlur->setFont(font2);
        _lBlur->setMouseTracking(true);
        _lBlur->setAlignment(Qt::AlignCenter);
        _lBlur->setMargin(10);

        verticalLayout_3->addWidget(_lBlur);

        _pBBlur = new QPushButton(verticalFrame1);
        _pBBlur->setObjectName(QStringLiteral("_pBBlur"));
        _pBBlur->setFont(font3);
        _pBBlur->setCheckable(true);
        _pBBlur->setFlat(false);

        verticalLayout_3->addWidget(_pBBlur);

        _scrollArea2->setWidget(_scrollAreaWidgetContents2);
        tW_settings->addTab(_tabSIM, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        verticalFrame2 = new QFrame(tab_3);
        verticalFrame2->setObjectName(QStringLiteral("verticalFrame2"));
        verticalFrame2->setGeometry(QRect(10, 0, 301, 301));
        verticalLayout_4 = new QVBoxLayout(verticalFrame2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        _lARSetting = new QLabel(verticalFrame2);
        _lARSetting->setObjectName(QStringLiteral("_lARSetting"));
        sizePolicy1.setHeightForWidth(_lARSetting->sizePolicy().hasHeightForWidth());
        _lARSetting->setSizePolicy(sizePolicy1);
        _lARSetting->setFont(font2);

        verticalLayout_4->addWidget(_lARSetting);

        _checkboxStereo = new QCheckBox(verticalFrame2);
        _checkboxStereo->setObjectName(QStringLiteral("_checkboxStereo"));
        sizePolicy2.setHeightForWidth(_checkboxStereo->sizePolicy().hasHeightForWidth());
        _checkboxStereo->setSizePolicy(sizePolicy2);
        _checkboxStereo->setFont(font2);
        _checkboxStereo->setChecked(true);

        verticalLayout_4->addWidget(_checkboxStereo);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(-1, 10, -1, 10);
        _rButtonVST = new QRadioButton(verticalFrame2);
        _bgView = new QButtonGroup(DARVClass);
        _bgView->setObjectName(QStringLiteral("_bgView"));
        _bgView->addButton(_rButtonVST);
        _rButtonVST->setObjectName(QStringLiteral("_rButtonVST"));
        _rButtonVST->setFont(font2);
        _rButtonVST->setCheckable(false);

        horizontalLayout_10->addWidget(_rButtonVST);

        _rButtonOST = new QRadioButton(verticalFrame2);
        _bgView->addButton(_rButtonOST);
        _rButtonOST->setObjectName(QStringLiteral("_rButtonOST"));
        _rButtonOST->setFont(font2);
        _rButtonOST->setCheckable(true);
        _rButtonOST->setChecked(true);

        horizontalLayout_10->addWidget(_rButtonOST);


        verticalLayout_4->addLayout(horizontalLayout_10);

        line_6 = new QFrame(verticalFrame2);
        line_6->setObjectName(QStringLiteral("line_6"));
        line_6->setFont(font);
        line_6->setFrameShadow(QFrame::Plain);
        line_6->setFrameShape(QFrame::HLine);

        verticalLayout_4->addWidget(line_6);

        _lView = new QLabel(verticalFrame2);
        _lView->setObjectName(QStringLiteral("_lView"));
        _lView->setFont(font2);

        verticalLayout_4->addWidget(_lView);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        _lFovy = new QLabel(verticalFrame2);
        _lFovy->setObjectName(QStringLiteral("_lFovy"));
        _lFovy->setFont(font2);

        horizontalLayout_5->addWidget(_lFovy);

        _editFovy = new QLineEdit(verticalFrame2);
        _editFovy->setObjectName(QStringLiteral("_editFovy"));
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(_editFovy->sizePolicy().hasHeightForWidth());
        _editFovy->setSizePolicy(sizePolicy6);
        _editFovy->setFont(font);

        horizontalLayout_5->addWidget(_editFovy);


        verticalLayout_4->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        _lEyeSep = new QLabel(verticalFrame2);
        _lEyeSep->setObjectName(QStringLiteral("_lEyeSep"));
        _lEyeSep->setFont(font2);

        horizontalLayout_6->addWidget(_lEyeSep);

        _editEyeSep = new QLineEdit(verticalFrame2);
        _editEyeSep->setObjectName(QStringLiteral("_editEyeSep"));
        sizePolicy6.setHeightForWidth(_editEyeSep->sizePolicy().hasHeightForWidth());
        _editEyeSep->setSizePolicy(sizePolicy6);
        _editEyeSep->setFont(font);

        horizontalLayout_6->addWidget(_editEyeSep);


        verticalLayout_4->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        _lEyeCorrection = new QLabel(verticalFrame2);
        _lEyeCorrection->setObjectName(QStringLiteral("_lEyeCorrection"));
        _lEyeCorrection->setFont(font2);

        horizontalLayout_7->addWidget(_lEyeCorrection);

        _editEyeCorrection = new QLineEdit(verticalFrame2);
        _editEyeCorrection->setObjectName(QStringLiteral("_editEyeCorrection"));
        sizePolicy6.setHeightForWidth(_editEyeCorrection->sizePolicy().hasHeightForWidth());
        _editEyeCorrection->setSizePolicy(sizePolicy6);
        _editEyeCorrection->setFont(font);

        horizontalLayout_7->addWidget(_editEyeCorrection);


        verticalLayout_4->addLayout(horizontalLayout_7);

        tW_settings->addTab(tab_3, QString());
        _lARSetting_3 = new QLabel(centralWidget);
        _lARSetting_3->setObjectName(QStringLiteral("_lARSetting_3"));
        _lARSetting_3->setGeometry(QRect(0, 10, 321, 23));
        sizePolicy1.setHeightForWidth(_lARSetting_3->sizePolicy().hasHeightForWidth());
        _lARSetting_3->setSizePolicy(sizePolicy1);
        QFont font5;
        font5.setFamily(QStringLiteral("Segoe UI"));
        font5.setPointSize(12);
        _lARSetting_3->setFont(font5);
        _lARSetting_3->setAlignment(Qt::AlignCenter);
        DARVClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(DARVClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        DARVClass->setStatusBar(statusBar);

        retranslateUi(DARVClass);

        tW_data->setCurrentIndex(0);
        tW_settings->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(DARVClass);
    } // setupUi

    void retranslateUi(QMainWindow *DARVClass)
    {
        DARVClass->setWindowTitle(QString());
        tW_data->setTabText(tW_data->indexOf(tab), QApplication::translate("DARVClass", "ROHDATEN", 0));
        tW_data->setTabText(tW_data->indexOf(tab_2), QApplication::translate("DARVClass", "MARKERG\303\234TE", 0));
        tW_data->setTabText(tW_data->indexOf(tab_4), QApplication::translate("DARVClass", "KAMERALAGE", 0));
        tW_data->setTabText(tW_data->indexOf(tab_6), QApplication::translate("DARVClass", "ZEITEN", 0));
        _lVObject->setText(QApplication::translate("DARVClass", "VIRTUELLES SETUP", 0));
        _lARSetting_2->setText(QApplication::translate("DARVClass", "POSITIONIERUNG", 0));
        _checkBoxObject->setText(QApplication::translate("DARVClass", "    Tracking nutzen", 0));
        _lObjectCoord->setText(QApplication::translate("DARVClass", "X ", 0));
        _lObjectCoord_2->setText(QApplication::translate("DARVClass", "Y", 0));
        _lObjectCoord_3->setText(QApplication::translate("DARVClass", "Z", 0));
        tW_settings->setTabText(tW_settings->indexOf(_tabAR), QApplication::translate("DARVClass", "AUGMENTATION", 0));
        _cbDARV->setText(QApplication::translate("DARVClass", "DARV", 0));
        _cbFixedQuality->setText(QApplication::translate("DARVClass", "Auto", 0));
        _cbClassification->setText(QApplication::translate("DARVClass", "Klassifizierung", 0));
        _lAD->setText(QApplication::translate("DARVClass", "Qualit\303\244t", 0));
        _lADValue->setText(QApplication::translate("DARVClass", "0", 0));
        tW_settings->setTabText(tW_settings->indexOf(tab_5), QApplication::translate("DARVClass", "G\303\234TE", 0));
        _lEyeCorrection_2->setText(QString());
        _lData->setText(QApplication::translate("DARVClass", "DATENWIEDERGABE", 0));
        _pBData_3->setText(QApplication::translate("DARVClass", "<<", 0));
        _pBPause->setText(QApplication::translate("DARVClass", "PAUSE", 0));
        _pBData_4->setText(QApplication::translate("DARVClass", ">>", 0));
        _pBData->setText(QApplication::translate("DARVClass", "REC", 0));
        _cbUseRecords->setText(QApplication::translate("DARVClass", "Aufnahme nutzen", 0));
        _lEyeCorrection_4->setText(QString());
        _lIMU->setText(QApplication::translate("DARVClass", "INTERTIALSENSOR", 0));
        _pBResetRot->setText(QApplication::translate("DARVClass", "Reset", 0));
        _rButtonVUZIX->setText(QApplication::translate("DARVClass", "nach VUZIX", 0));
        _rButtonSIMON->setText(QApplication::translate("DARVClass", "nach SIMON", 0));
        _lInterpol->setText(QApplication::translate("DARVClass", "0\302\260 IMU-ERSATZ", 0));
        _lEyeCorrection_3->setText(QString());
        label->setText(QApplication::translate("DARVClass", "OPTITRACK", 0));
        _cbUseRefData->setText(QApplication::translate("DARVClass", "Nutze OptiTrack-Daten", 0));
        _lRefData->setText(QApplication::translate("DARVClass", "... keine Daten", 0));
        _pBConnect->setText(QApplication::translate("DARVClass", "Verbinden", 0));
        _pBResetRef->setText(QApplication::translate("DARVClass", "Reset", 0));
        _lBlur->setText(QApplication::translate("DARVClass", "BEWEGUNGSUNSCH\303\204RFE", 0));
        _pBBlur->setText(QApplication::translate("DARVClass", "Start", 0));
        tW_settings->setTabText(tW_settings->indexOf(_tabSIM), QApplication::translate("DARVClass", "TRACKING", 0));
        _lARSetting->setText(QApplication::translate("DARVClass", "Augmentation-Typ", 0));
        _checkboxStereo->setText(QApplication::translate("DARVClass", "Stereo View", 0));
        _rButtonVST->setText(QApplication::translate("DARVClass", "VST", 0));
        _rButtonOST->setText(QApplication::translate("DARVClass", "OST", 0));
        _lView->setText(QApplication::translate("DARVClass", "Blickeinstellung", 0));
        _lFovy->setText(QApplication::translate("DARVClass", "FOV Y                ", 0));
        _lEyeSep->setText(QApplication::translate("DARVClass", "Augabstand       ", 0));
        _lEyeCorrection->setText(QApplication::translate("DARVClass", "Korrekturwinkel  ", 0));
        tW_settings->setTabText(tW_settings->indexOf(tab_3), QApplication::translate("DARVClass", "VIEW", 0));
        _lARSetting_3->setText(QApplication::translate("DARVClass", "EINSTELLUNGEN", 0));
    } // retranslateUi

};

namespace Ui {
    class DARVClass: public Ui_DARVClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DARV_H
