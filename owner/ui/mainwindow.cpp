#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QStringList>
#include <algorithm>
#include <ctime>
#include <QDateTime>
#include <QListWidgetItem>
#include <QVariant>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    listenWindow(new ListenWindow(this, Qt::CustomizeWindowHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)),
    assignWindow(new AssignWindow(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint)),
    core(new Core()),
    logTypes(QStringList("MSG") << "WARN" << "ERROR")
{
    ui->setupUi(this);
    log(Info, "Initializing...");
    QObject::connect(this, SIGNAL(newTask(Task*)), core, SLOT(newTask(Task*)));
    QObject::connect(core, SIGNAL(taskFinished(Task*)), this, SLOT(taskFinished(Task*)));
    QObject::connect(core, SIGNAL(newEvent(Event*)), this, SLOT(newEvent(Event*)));
    {
        QStringList* l = new QStringList();
        QFile f(":/nodeNames.txt");
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream s(&f);
        while(!s.atEnd())
            l->append(s.readLine());
        std::srand(std::time(0));
        std::random_shuffle(l->begin(), l->end());
        core->setNameList(l);
    }
    QDir dir(QApplication::applicationDirPath());
    if(!dir.cd("tasks")) {
        dir.mkdir("tasks");
        dir.cd("tasks");
    }
    log(Info, "Started!");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::taskFinished(Task* task)
{
    auto nodes = core->getNodeList();
    switch(task->getType()) {
        case Task::Listen: {
            auto t = static_cast<ListenTask*>(task);
            if(t->getCode())
                log(Error, QString("Unable to bind to %1:%2 (%3). Stopped listening.").arg(t->address.toString(), QSN(t->port), t->getMessage()));
            else
                log(Info, QString("Successfully bound to %1:%2").arg(t->address.toString(), QSN(t->port)));
            break;
        }
        case Task::Assign: {
            auto t = static_cast<AssignTask*>(task);
            quint32 index =  t->nodeIndex;
            Node* node = nodes->at(index);
            if(t->getCode())
                log(Info, QString("[%1] %2 has rejected the task: %3!").arg(QSN(node->getId()), node->getName(), t->getMessage()));
            else
                log(Error, QString("Task was successfully assigned to [%1] %2!").arg(QSN(node->getId()), QString(node->getName())));
            ui->listNodes->item(index)->setText(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            break;
        }
        case Task::GetStatus: {
            auto t = static_cast<AssignTask*>(task);
            quint32 index =  t->nodeIndex;
            Node* node = nodes->at(index);
            if(t->getCode())
                log(Error, QString("Unable to retrieve status of [%1] %2!").arg(QSN(node->getId()), QString(node->getName())));
            else
                log(Info, QString("Status of [%1] %2 is: %3!").arg(QSN(node->getId()), node->getName(), node->getStatus()));
            ui->listNodes->item(index)->setText(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            break;
        }
        default: {
            log(Warning, QString("Unknown task finished, code: %1, message: %2.").arg(QSN(task->getCode()), task->getMessage()));
        }
    }
    delete task;
}

void MainWindow::newEvent(Event* event)
{
    switch(event->getType()) {
        case Event::UnexpectedMessage: {
            auto e = static_cast<UnexpectedMessageEvent*>(event);
            Node* node = core->getNodeList()->at(e->index);
            log(Warning, QString("Received unexpected message from [%1] %2: %3!").arg(QSN(node->getId()), node->getName(), e->reason));
            break;
        }
        case Event::MalformedMessage: {
            auto e = static_cast<MalformedMessageEvent*>(event);
            Node* node = core->getNodeList()->at(e->index);
            delete ui->listNodes->takeItem(e->index);
            log(Warning, QString("Received unexpected message from [%1] %2: %3! Node was kicked!").arg(QSN(node->getId()), node->getName(), e->reason));
            break;
        }
        case Event::JoinError: {
            auto e = static_cast<JoinErrorEvent*>(event);
            log(Warning, QString("%1 failed to join the cluster: %2!").arg(e->address, e->reason));
            break;
        }
        case Event::NodeJoined: {
            auto e = static_cast<NodeJoinedEvent*>(event);
            Node* node = core->getNodeList()->at(e->index);
            ui->listNodes->addItem(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            log(Info, QString("'%1' (%2) has joined the cluster with ID: %3.").arg(node->getName(), node->getAddress(), QSN(node->getId())));
            break;
        }
        case Event::NodeLeft: {
            auto e = static_cast<NodeLeftEvent*>(event);
            delete ui->listNodes->takeItem(e->index);
            log(Info, QString("[%1] '%2' has left the cluster: %3.").arg(QSN(e->id), e->name, e->leaveDesctiption));
            break;
        }
        case Event::NodeStatusChanged: {
            auto e = static_cast<NodeStatusChangedEvent*>(event);
            Node* node = core->getNodeList()->at(e->index);
            ui->listNodes->item(e->index)->setText(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            log(Info, QString("[%1] '%2' has changed its status to: %3.").arg(QSN(node->getId()), node->getName(), node->getStatus()));
            break;
        }
        case Event::JobFinished: {
            auto e = static_cast<JobFinishedEvent*>(event);
            Node* node = core->getNodeList()->at(e->index);
            log(Info, QString("[%1] '%2' has finished the job!").arg(QSN(node->getId()), QString(node->getName())));
            //need to do smth with data
            break;
        }
    }
    delete event;
}

void MainWindow::on_buttonListen_clicked()
{
    if(listenWindow->exec()) {
        ListenTask* task = new ListenTask(listenWindow->address, listenWindow->port);
        log(Info, QString("Attempting to bind to %1:%2...").arg(task->address.toString(), QSN(task->port)));
        emit newTask(task);
    }
}

void MainWindow::log(LogType type, const QString& message)
{
    ui->editLog->appendPlainText(QString("%1 [%2] %3").arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz"), logTypes[type], message));
}

void MainWindow::on_listNodes_currentRowChanged(int currentRow)
{
    if(currentRow == -1) {
        ui->buttonAssign->setEnabled(false);
        ui->buttonStatus->setEnabled(false);
        ui->buttonKick->setEnabled(false);
    } else {
        ui->buttonAssign->setEnabled(true);
        ui->buttonStatus->setEnabled(true);
        ui->buttonKick->setEnabled(true);
    }
}

void MainWindow::on_buttonAssign_clicked()
{
    quint32 index = ui->listNodes->currentRow();
    auto nodes = core->getNodeList();
    Node* node = nodes->at(index);
    assignWindow->id = node->getId();
    assignWindow->name = node->getName();
    assignWindow->address = node->getAddress();
    assignWindow->python = node->getVersion();
    assignWindow->modules = node->getModules();
    if(!assignWindow->exec())
        return;
    if(nodes->at(index) == node) {
        //create local folder
        log(Info, QString("Assigning '%1' to [%2] '%3'...").arg(assignWindow->path, QSN(node->getId()), node->getName()));
        emit newTask(new AssignTask(index, assignWindow->input, assignWindow->code));
    } else
        log(Warning, QString("Not assigning '%1', node has left the cluster!").arg(assignWindow->path));
}

void MainWindow::on_buttonStatus_clicked()
{
    quint32 index = ui->listNodes->currentRow();
    Node* node = core->getNodeList()->at(index);
    log(Info, QString("Checking status of [%2] '%3'...").arg(QSN(node->getId()), QString(node->getName())));
    emit newTask(new GetStatusTask(index));
}

void MainWindow::on_buttonKick_clicked()
{
    quint32 index = ui->listNodes->currentRow();
    Node* node = core->getNodeList()->at(index);
    log(Info, QString("Kicking [%2] '%3'...").arg(QSN(node->getId()), QString(node->getName())));
    emit newTask(new KickTask(index));
}
