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
    quint32 id;
    QByteArray input;
    QByteArray code;
public slots:
    int exec();
    void accept();

private slots:
    void on_buttonLoadTask_clicked();

private:
    Ui::AssignWindow* ui;
    QFile taskFile;
};

#endif // ASSIGNWINDOW_H
