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
}

void AssignWindow::accept()
{
}

void AssignWindow::on_buttonLoadTask_clicked()
{
    QString taskFolder = QFileDialog::getExistingDirectory(this, "Select a task folder...", ".");
    QFile code(taskFolder + "/code");
    if(!code.exists()) {
        QMessageBox::warning(this, "File error", "File 'code' does not exist!");
        return;
    }
    QFile input(taskFolder + "/input");
    if(!input.exists()) {
        QMessageBox::warning(this, "File error", "File 'input' does not exist!");
        return;
    }
    if((code.size() + input.size()) > MAX_MESSAGE_LENGTH) {
        QMessageBox::warning(this, "File error", "Added files size is greater than maximum network message length!");
        return;
    }
    if(!code.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, "File error", "Unable to read 'code' file!");
        return;
    }
    if(!input.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, "File error", "Unable to read 'input' file!");
        return;
    }
    ui->labelTaskFile->setText(taskFolder);
    this->input = input.readAll();
    this->code = code.readAll();
    if(this->input.size() < 1024 * 1024) {
        //ui->editInput->
    } else {
    }
    if(this->code.size() < 1024 * 1024) {
    } else {
    }
}
