/********************************************************************************
** Form generated from reading UI file 'TimerWidget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMERWIDGET_H
#define UI_TIMERWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TimerWidget
{
public:
    QVBoxLayout *verticalLayout_Main;
    QGroupBox *targetGroupBox;
    QFormLayout *formLayout;
    QLabel *subjectLabel;
    QComboBox *subjectComboBox;
    QLabel *taskLabel;
    QComboBox *taskComboBox;
    QLabel *timerDisplayLabel;
    QLabel *pomodoroStatusLabel;
    QHBoxLayout *controlsLayout;
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *resetButton;
    QGroupBox *pomodoroGroupBox;
    QGridLayout *gridLayout;
    QLabel *workDurationLabel;
    QSpinBox *workDurationSpinBox;
    QLabel *shortBreakLabel;
    QSpinBox *shortBreakSpinBox;
    QLabel *longBreakLabel;
    QSpinBox *longBreakSpinBox;
    QLabel *cyclesLabel;
    QSpinBox *cyclesSpinBox;
    QGroupBox *programMonitorGroupBox;
    QVBoxLayout *programMonitorLayout;
    QLabel *programStatusLabel;
    QLabel *programTimeLabel;
    QProgressBar *programTimeProgressBar;
    QGroupBox *notesGroupBox;
    QVBoxLayout *verticalLayout_2;
    QPlainTextEdit *notesTextEdit;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *TimerWidget)
    {
        if (TimerWidget->objectName().isEmpty())
            TimerWidget->setObjectName("TimerWidget");
        TimerWidget->resize(350, 500);
        verticalLayout_Main = new QVBoxLayout(TimerWidget);
        verticalLayout_Main->setObjectName("verticalLayout_Main");
        targetGroupBox = new QGroupBox(TimerWidget);
        targetGroupBox->setObjectName("targetGroupBox");
        formLayout = new QFormLayout(targetGroupBox);
        formLayout->setObjectName("formLayout");
        subjectLabel = new QLabel(targetGroupBox);
        subjectLabel->setObjectName("subjectLabel");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, subjectLabel);

        subjectComboBox = new QComboBox(targetGroupBox);
        subjectComboBox->setObjectName("subjectComboBox");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, subjectComboBox);

        taskLabel = new QLabel(targetGroupBox);
        taskLabel->setObjectName("taskLabel");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, taskLabel);

        taskComboBox = new QComboBox(targetGroupBox);
        taskComboBox->setObjectName("taskComboBox");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, taskComboBox);


        verticalLayout_Main->addWidget(targetGroupBox);

        timerDisplayLabel = new QLabel(TimerWidget);
        timerDisplayLabel->setObjectName("timerDisplayLabel");
        QFont font;
        font.setPointSize(28);
        font.setBold(true);
        timerDisplayLabel->setFont(font);
        timerDisplayLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_Main->addWidget(timerDisplayLabel);

        pomodoroStatusLabel = new QLabel(TimerWidget);
        pomodoroStatusLabel->setObjectName("pomodoroStatusLabel");
        pomodoroStatusLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_Main->addWidget(pomodoroStatusLabel);

        controlsLayout = new QHBoxLayout();
        controlsLayout->setObjectName("controlsLayout");
        startButton = new QPushButton(TimerWidget);
        startButton->setObjectName("startButton");
        QIcon icon(QIcon::fromTheme(QString::fromUtf8("media-playback-start")));
        startButton->setIcon(icon);

        controlsLayout->addWidget(startButton);

        pauseButton = new QPushButton(TimerWidget);
        pauseButton->setObjectName("pauseButton");
        pauseButton->setEnabled(false);
        QIcon icon1(QIcon::fromTheme(QString::fromUtf8("media-playback-pause")));
        pauseButton->setIcon(icon1);

        controlsLayout->addWidget(pauseButton);

        stopButton = new QPushButton(TimerWidget);
        stopButton->setObjectName("stopButton");
        stopButton->setEnabled(false);
        QIcon icon2(QIcon::fromTheme(QString::fromUtf8("media-record")));
        stopButton->setIcon(icon2);

        controlsLayout->addWidget(stopButton);

        resetButton = new QPushButton(TimerWidget);
        resetButton->setObjectName("resetButton");
        resetButton->setEnabled(false);
        QIcon icon3(QIcon::fromTheme(QString::fromUtf8("view-refresh")));
        resetButton->setIcon(icon3);

        controlsLayout->addWidget(resetButton);


        verticalLayout_Main->addLayout(controlsLayout);

        pomodoroGroupBox = new QGroupBox(TimerWidget);
        pomodoroGroupBox->setObjectName("pomodoroGroupBox");
        pomodoroGroupBox->setCheckable(true);
        pomodoroGroupBox->setChecked(false);
        gridLayout = new QGridLayout(pomodoroGroupBox);
        gridLayout->setObjectName("gridLayout");
        workDurationLabel = new QLabel(pomodoroGroupBox);
        workDurationLabel->setObjectName("workDurationLabel");

        gridLayout->addWidget(workDurationLabel, 0, 0, 1, 1);

        workDurationSpinBox = new QSpinBox(pomodoroGroupBox);
        workDurationSpinBox->setObjectName("workDurationSpinBox");
        workDurationSpinBox->setMinimum(1);
        workDurationSpinBox->setMaximum(120);
        workDurationSpinBox->setValue(25);

        gridLayout->addWidget(workDurationSpinBox, 0, 1, 1, 1);

        shortBreakLabel = new QLabel(pomodoroGroupBox);
        shortBreakLabel->setObjectName("shortBreakLabel");

        gridLayout->addWidget(shortBreakLabel, 1, 0, 1, 1);

        shortBreakSpinBox = new QSpinBox(pomodoroGroupBox);
        shortBreakSpinBox->setObjectName("shortBreakSpinBox");
        shortBreakSpinBox->setMinimum(1);
        shortBreakSpinBox->setMaximum(30);
        shortBreakSpinBox->setValue(5);

        gridLayout->addWidget(shortBreakSpinBox, 1, 1, 1, 1);

        longBreakLabel = new QLabel(pomodoroGroupBox);
        longBreakLabel->setObjectName("longBreakLabel");

        gridLayout->addWidget(longBreakLabel, 2, 0, 1, 1);

        longBreakSpinBox = new QSpinBox(pomodoroGroupBox);
        longBreakSpinBox->setObjectName("longBreakSpinBox");
        longBreakSpinBox->setMinimum(5);
        longBreakSpinBox->setMaximum(60);
        longBreakSpinBox->setValue(15);

        gridLayout->addWidget(longBreakSpinBox, 2, 1, 1, 1);

        cyclesLabel = new QLabel(pomodoroGroupBox);
        cyclesLabel->setObjectName("cyclesLabel");

        gridLayout->addWidget(cyclesLabel, 3, 0, 1, 1);

        cyclesSpinBox = new QSpinBox(pomodoroGroupBox);
        cyclesSpinBox->setObjectName("cyclesSpinBox");
        cyclesSpinBox->setMinimum(2);
        cyclesSpinBox->setMaximum(8);
        cyclesSpinBox->setValue(4);

        gridLayout->addWidget(cyclesSpinBox, 3, 1, 1, 1);


        verticalLayout_Main->addWidget(pomodoroGroupBox);

        programMonitorGroupBox = new QGroupBox(TimerWidget);
        programMonitorGroupBox->setObjectName("programMonitorGroupBox");
        programMonitorLayout = new QVBoxLayout(programMonitorGroupBox);
        programMonitorLayout->setObjectName("programMonitorLayout");
        programStatusLabel = new QLabel(programMonitorGroupBox);
        programStatusLabel->setObjectName("programStatusLabel");
        programStatusLabel->setAlignment(Qt::AlignCenter);

        programMonitorLayout->addWidget(programStatusLabel);

        programTimeLabel = new QLabel(programMonitorGroupBox);
        programTimeLabel->setObjectName("programTimeLabel");
        programTimeLabel->setAlignment(Qt::AlignCenter);
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(true);
        programTimeLabel->setFont(font1);

        programMonitorLayout->addWidget(programTimeLabel);

        programTimeProgressBar = new QProgressBar(programMonitorGroupBox);
        programTimeProgressBar->setObjectName("programTimeProgressBar");
        programTimeProgressBar->setVisible(false);
        programTimeProgressBar->setMaximum(100);
        programTimeProgressBar->setValue(0);
        programTimeProgressBar->setTextVisible(false);

        programMonitorLayout->addWidget(programTimeProgressBar);


        verticalLayout_Main->addWidget(programMonitorGroupBox);

        notesGroupBox = new QGroupBox(TimerWidget);
        notesGroupBox->setObjectName("notesGroupBox");
        verticalLayout_2 = new QVBoxLayout(notesGroupBox);
        verticalLayout_2->setObjectName("verticalLayout_2");
        notesTextEdit = new QPlainTextEdit(notesGroupBox);
        notesTextEdit->setObjectName("notesTextEdit");
        notesTextEdit->setMaximumSize(QSize(16777215, 80));

        verticalLayout_2->addWidget(notesTextEdit);


        verticalLayout_Main->addWidget(notesGroupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_Main->addItem(verticalSpacer);


        retranslateUi(TimerWidget);

        QMetaObject::connectSlotsByName(TimerWidget);
    } // setupUi

    void retranslateUi(QWidget *TimerWidget)
    {
        TimerWidget->setWindowTitle(QCoreApplication::translate("TimerWidget", "\345\255\246\344\271\240\350\256\241\346\227\266\345\231\250", nullptr));
        targetGroupBox->setTitle(QCoreApplication::translate("TimerWidget", "\350\256\241\346\227\266\347\233\256\346\240\207", nullptr));
        subjectLabel->setText(QCoreApplication::translate("TimerWidget", "\347\247\221\347\233\256:", nullptr));
        taskLabel->setText(QCoreApplication::translate("TimerWidget", "\344\273\273\345\212\241:", nullptr));
#if QT_CONFIG(tooltip)
        taskComboBox->setToolTip(QCoreApplication::translate("TimerWidget", "\345\217\257\351\200\211\357\274\214\347\225\231\347\251\272\345\210\231\350\256\241\346\227\266\351\222\210\345\257\271\346\225\264\344\270\252\347\247\221\347\233\256", nullptr));
#endif // QT_CONFIG(tooltip)
        timerDisplayLabel->setText(QCoreApplication::translate("TimerWidget", "00:00:00", nullptr));
        pomodoroStatusLabel->setText(QString());
        startButton->setText(QCoreApplication::translate("TimerWidget", "\345\274\200\345\247\213", nullptr));
        pauseButton->setText(QCoreApplication::translate("TimerWidget", "\346\232\202\345\201\234", nullptr));
        stopButton->setText(QCoreApplication::translate("TimerWidget", "\345\201\234\346\255\242\345\271\266\344\277\235\345\255\230", nullptr));
        resetButton->setText(QCoreApplication::translate("TimerWidget", "\351\207\215\347\275\256", nullptr));
        pomodoroGroupBox->setTitle(QCoreApplication::translate("TimerWidget", "\347\225\252\350\214\204\345\267\245\344\275\234\346\263\225", nullptr));
        workDurationLabel->setText(QCoreApplication::translate("TimerWidget", "\345\267\245\344\275\234\346\227\266\351\225\277 (\345\210\206):", nullptr));
        shortBreakLabel->setText(QCoreApplication::translate("TimerWidget", "\347\237\255\346\227\266\344\274\221\346\201\257 (\345\210\206):", nullptr));
        longBreakLabel->setText(QCoreApplication::translate("TimerWidget", "\351\225\277\346\227\266\344\274\221\346\201\257 (\345\210\206):", nullptr));
        cyclesLabel->setText(QCoreApplication::translate("TimerWidget", "\351\225\277\344\274\221\345\211\215\345\276\252\347\216\257\346\254\241\346\225\260:", nullptr));
        programMonitorGroupBox->setTitle(QCoreApplication::translate("TimerWidget", "\347\250\213\345\272\217\350\207\252\345\212\250\350\256\241\346\227\266", nullptr));
        programStatusLabel->setText(QCoreApplication::translate("TimerWidget", "\346\234\252\346\243\200\346\265\213\345\210\260\345\205\263\350\201\224\347\250\213\345\272\217", nullptr));
        programStatusLabel->setStyleSheet(QCoreApplication::translate("TimerWidget", "color: #666;", nullptr));
        programTimeLabel->setText(QCoreApplication::translate("TimerWidget", "\347\250\213\345\272\217\350\256\241\346\227\266: 00:00:00", nullptr));
        programTimeLabel->setStyleSheet(QCoreApplication::translate("TimerWidget", "color: #2E8B57;", nullptr));
        notesGroupBox->setTitle(QCoreApplication::translate("TimerWidget", "\344\274\232\350\257\235\345\244\207\346\263\250 (\345\217\257\351\200\211)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TimerWidget: public Ui_TimerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMERWIDGET_H
