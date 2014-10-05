#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>

#include "listenwindow.h"
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
    ~MainWindow();

signals:
    void newTask(Task* task);

private slots:
    void taskFinished(Task* task);
    void newEvent(Event* event);
    void on_buttonListen_clicked();

private:
    enum LogType {
        Info = 0,
        Warning,
        Error
    };
    Ui::MainWindow* ui;
    ListenWindow* listenWindow;
    Core* core;
    QStringList logTypes;
    void log(LogType type, const QString& message);
};

#endif // MAINWINDOW_H
