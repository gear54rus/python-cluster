#ifndef ASSIGNWINDOW_H
#define ASSIGNWINDOW_H

#include <QDialog>
#include <QFileDialog>
#include <QByteArray>
#include <QFile>

namespace Ui {
class AssignWindow;
}

class AssignWindow : public QDialog {
    Q_OBJECT

public:
    explicit AssignWindow(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
    ~AssignWindow();
    quint32 nodeId;
    QByteArray nodeName;
    QString nodeAddress, nodePython, filePath;
    QStringList nodeModules;
    QByteArray input, code;
public slots:
    int exec();
    void accept();

private slots:
    void on_buttonLoadTask_clicked();

    void on_buttonCancel_clicked();

    void on_buttonAssignJob_clicked();

    void on_buttonRemoveJob_clicked();

private:
    Ui::AssignWindow* ui;
};

#endif // ASSIGNWINDOW_H
