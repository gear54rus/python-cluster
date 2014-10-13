#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include "../global.h"
#include "assignwindow.h"
#include "ui_assignwindow.h"

AssignWindow::AssignWindow(QWidget* parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::AssignWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui->buttonAssignJob, SIGNAL(clicked()), this, SLOT(accept()));
}

AssignWindow::~AssignWindow()
{
    delete ui;
}

int AssignWindow::exec()
{
    ui->buttonLoadTask->setFocus();
    ui->labelNodeID->setText(QSN(nodeId));
    ui->labelNodeName->setText(nodeName);
    ui->labelNodeIP->setText(nodeAddress);
    ui->labelNodeVer->setText(nodePython);
    ui->listModules->clear();
    ui->listModules->addItems(nodeModules);
    QString tooLong("Text is too long to display here...");
    if(hadJob) {
        ui->labelJobFile->setText(jobPath);
        if(code.size() < MAX_EDITOR_DISPLAY_LENGTH) {
            ui->editCode->setPlainText(code);
            ui->editCode->setEnabled(true);
        } else {
            ui->editCode->setPlainText(tooLong);
            ui->editCode->setEnabled(false);
        }
        if(input.size() < MAX_EDITOR_DISPLAY_LENGTH) {
            ui->editInput->setPlainText(input);
            ui->editInput->setEnabled(true);
        } else {
            ui->editInput->setPlainText(input);
            ui->editInput->setEnabled(false);
        }
        ui->buttonAssignJob->setEnabled(true);
        ui->buttonRemoveJob->setEnabled(true);
    } else {
        ui->labelJobFile->setText("None");
        ui->editCode->clear();
        ui->editCode->setEnabled(false);
        ui->editInput->clear();
        ui->editInput->setEnabled(false);
        ui->buttonAssignJob->setEnabled(false);
        ui->buttonRemoveJob->setEnabled(false);
    }
    return QDialog::exec();
}

void AssignWindow::accept()
{
    QString code = ui->editCode->toPlainText();
    if(code.isEmpty()) {
        QMessageBox::warning(this, "Validation error!", "Code field is empty!");
        return;
    }
    if(codeChanged)
        this->code = ui->editCode->toPlainText().toLatin1();
    if(inputChanged)
        this->input = ui->editInput->toPlainText().toLatin1();
    QDialog::accept();
}

void AssignWindow::on_buttonLoadTask_clicked()
{
    QString tooLong("Text is too long to display here...");
    jobPath = QFileDialog::getOpenFileName(this, "Select job file...", !jobPath.isEmpty() ? QFileInfo(jobPath).dir().path() : ".");
    if(jobPath.isEmpty())
        return;
    QFile code(jobPath), input(jobPath + ".input");
    if(code.open(QFile::ReadOnly)) {
        if(code.size()) {
            this->code = code.readAll();
            if(this->code.size() < MAX_EDITOR_DISPLAY_LENGTH) {
                ui->editCode->setPlainText(this->code);
                ui->editCode->setEnabled(true);
            } else {
                ui->editCode->setPlainText(tooLong);
                ui->editCode->setEnabled(false);
            }
        } else {
            QMessageBox::warning(this, "File error!", QString("File '%1' is empty.").arg(jobPath));
            return;
        }
    } else {
        QMessageBox::warning(this, "File error!", QString("Unable to open '%1'!").arg(jobPath));
        return;
    }
    ui->labelJobFile->setText(jobPath);
    ui->editInput->setEnabled(true);
    if(input.open(QFile::ReadOnly)) {
        this->input = input.readAll();
        if(this->input.size() < MAX_EDITOR_DISPLAY_LENGTH)
            ui->editInput->setPlainText(this->input);
        else {
            ui->editInput->setPlainText(tooLong);
            ui->editInput->setEnabled(false);
        }
    }
    inputChanged = false;
    codeChanged = false;
    ui->buttonAssignJob->setEnabled(true);
}


void AssignWindow::on_buttonRemoveJob_clicked()
{
    code.clear();
    input.clear();
    QDialog::accept();
}

void AssignWindow::on_editInput_textChanged()
{
    inputChanged = true;
}

void AssignWindow::on_editCode_textChanged()
{
    codeChanged = true;
}
