#include <QMessageBox>

#include "../global.h"
#include "assignwindow.h"
#include "ui_assignwindow.h"

AssignWindow::AssignWindow(QWidget* parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::AssignWindow),
    assign(false)
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
    if(assign) {
        this->code = ui->editCode->toPlainText().toLatin1();
        this->input = ui->editInput->toPlainText().toLatin1();
    }
    QDialog::accept();
}

void AssignWindow::on_buttonLoadTask_clicked()
{
    QString taskFolder = QFileDialog::getExistingDirectory(this, "Select a task folder...", ".");
    if(taskFolder.isEmpty())
        return;
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
    this->code = code.readAll();
    this->input = input.readAll();
    ui->editCode->appendPlainText((this->code.size() < 1024 * 1024) ? this->code : "File too big to display...");
    ui->editInput->appendPlainText((this->input.size() < 1024 * 1024) ? this->input : "File too big to display...");
    assign = true;
}
