#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

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
}

void AssignWindow::accept()
{
}

void AssignWindow::on_buttonLoadTask_clicked()
{
    QString taskFolder = QFileDialog::getExistingDirectory(this, "Select a task folder...", ".");
}
