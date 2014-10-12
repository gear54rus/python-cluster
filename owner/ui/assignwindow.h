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
    QString nodeName, nodeAddress, nodePython, filePath;
    QStringList nodeModules;
    bool hadJob, inputChanged, codeChanged;
    QString jobPath;
    QByteArray input, code;
public slots:
    int exec();
    void accept();

private slots:
    void on_buttonLoadTask_clicked();

    void on_buttonRemoveJob_clicked();

    void on_editInput_textChanged();

    void on_editCode_textChanged();

private:
    Ui::AssignWindow* ui;
};

#endif // ASSIGNWINDOW_H
