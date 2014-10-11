#include <QMessageBox>

#include "../global.h"
#include "assignwindow.h"
#include "ui_assignwindow.h"

AssignWindow::AssignWindow(QWidget* parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::AssignWindow)
{
    ui->setupUi(this);
}

AssignWindow::~AssignWindow()
{
    delete ui;
}

int AssignWindow::exec()
{
    return QDialog::exec();
}

void AssignWindow::accept()
{
}

void AssignWindow::on_buttonLoadTask_clicked()
{
}

void AssignWindow::on_buttonCancel_clicked()
{
    QDialog::reject();
}

void AssignWindow::on_buttonAssignJob_clicked()
{
}

void AssignWindow::on_buttonRemoveJob_clicked()
{
}
