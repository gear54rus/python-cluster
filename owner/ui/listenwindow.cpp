#include <QPushButton>
#include <QHostAddress>
#include <QMessageBox>

#include "../global.h"
#include "listenwindow.h"
#include "ui_listenwindow.h"

ListenWindow::ListenWindow(QWidget* parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    address(QHostAddress::AnyIPv4),
    port(DEFAULT_PORT),
    ui(new Ui::ListenWindow)
{
    ui->setupUi(this);
    ui->bboxOkCancel->button(QDialogButtonBox::Ok)->setDefault(true);
}

ListenWindow::~ListenWindow()
{
    delete ui;
}

int ListenWindow::exec()
{
    ui->editIP->setText(address.toString());
    ui->editPort->setText(QString::number(port, 10));
    return QDialog::exec();
}

void ListenWindow::accept()
{
    QHostAddress address(ui->editIP->text());
    quint16 port(ui->editPort->text().toUInt(nullptr, 10));
    if((address.protocol() != QAbstractSocket::IPv4Protocol) || (!port))
        QMessageBox::warning(this, "Error", "IP address or port is invalid!");
    else {
        this->address = address;
        this->port = port;
        QDialog::accept();
    }
}
