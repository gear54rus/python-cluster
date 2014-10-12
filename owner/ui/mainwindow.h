#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QProcess>

#include "listenwindow.h"
#include "assignwindow.h"
#include "../core/core.h"
#include "../core/event.h"
#include "../core/task.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    int show();
    ~MainWindow();

signals:
    void newTask(Task* task);

private slots:
    void taskFinished(Task* task);
    void newEvent(Event* event);
    void on_buttonListen_clicked();

    void on_listNodes_currentRowChanged(int currentRow);

    void on_buttonAssign_clicked();

    void on_buttonStatus_clicked();

    void on_buttonKick_clicked();

    void on_listNodes_itemDoubleClicked();

    void on_buttonStartRemote_clicked();

    void on_buttonStartLocal_clicked();

private:
    enum LogType {
        Info = 0,
        Warning,
        Error
    };
    Ui::MainWindow* ui;
    ListenWindow* listenWindow;
    AssignWindow* assignWindow;
    Core* core;
    QProcess runner;
    QStringList logTypes;
    void log(LogType type, const QString& message);
};

#endif // MAINWINDOW_H
