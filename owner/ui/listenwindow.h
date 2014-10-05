#ifndef LISTENWINDOW_H
#define LISTENWINDOW_H

#include <QDialog>
#include <QHostAddress>

namespace Ui {
class ListenWindow;
}

class ListenWindow : public QDialog {
    Q_OBJECT

public:
    explicit ListenWindow(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
    ~ListenWindow();
    QHostAddress address;
    quint16 port;
public slots:
    int exec();
    void accept();

private:
    Ui::ListenWindow* ui;
};

#endif // LISTENWINDOW_H
