#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStringList>
#include <algorithm>
#include <ctime>
#include <QDateTime>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

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
    QDir::setCurrent(QApplication::applicationDirPath());
    {
        QStringList* l = new QStringList();
        QFile f("names.txt");
        if(f.exists() && f.open(QFile::ReadOnly))
            log(Info, "Using external node name list.");
        else {
            f.setFileName(":/nodeNames.txt");
            f.open(QFile::ReadOnly);
            log(Info, "Using internal node name list.");
        }
        QTextStream s(&f);
        QString str;
        while(!s.atEnd()) {
            str = s.readLine().trimmed();
            if(!str.isEmpty())
                l->append(str);
        }
        std::srand(std::time(0));
        std::random_shuffle(l->begin(), l->end());
        core->setNameList(l);
    }
}

int MainWindow::show()
{
    QDir::setCurrent(QApplication::applicationDirPath());
    QFile pythonBin("python/python.exe");
    if(!pythonBin.exists()) {
        QMessageBox::critical(this, "Error", "Python interpreter not found ('python/python.exe')!");
        return 1;
    }
    runner.start(pythonBin.fileName() + " -V");
    runner.waitForFinished(3000);
    if(runner.state() == QProcess::Running) {
        QMessageBox::critical(this, "Error", "Python interpreter did not respond in time!");
        return 1;
    }
    QRegularExpressionMatch match = QRegularExpression("Python (\\d+\\.\\d+\\.\\d+)\\r\\n").match(runner.readAll());
    if(!match.hasMatch()) {
        QMessageBox::critical(this, "Error", "Unexpected output from Python interpreter!");
        return 1;
    }
    log(Info, QString("Python version %1 found.").arg(match.captured(1)));
    QDir tasksDir("jobs");
    tasksDir.removeRecursively();
    tasksDir.mkpath(".");
    QDir resultsDir("results");
    if(!resultsDir.exists()) {
        log(Info, "Created 'results' directory.");
        resultsDir.mkpath(".");
    } else
        log(Info, "Found 'results' directory.");
    QWidget::show();
    log(Info, "Started!");
    return 0;
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
        case Task::GetStatus: {
            auto t = static_cast<AssignTask*>(task);
            quint32 index =  t->nodeIndex;
            Node* node = nodes->at(index);
            if(t->getCode())
                log(Error, QString("Unable to retrieve status of [%1] '%2'!").arg(QSN(node->getId()), QString(node->getName())));
            else
                log(Info, QString("Status of [%1] '%2' is: %3!").arg(QSN(node->getId()), node->getName(), node->getStatus()));
            ui->listNodes->item(index)->setText(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            break;
        }
        case Task::Assign: {
            auto t = static_cast<AssignTask*>(task);
            quint32 index =  t->nodeIndex;
            Node* node = nodes->at(index);
            if(t->getCode()) {
                if(t->codeBytes.length())
                    log(Error, QString("[%1] '%2' has rejected the job: %3!").arg(QSN(node->getId()), node->getName(), t->getMessage()));
            } else {
                if(t->codeBytes.length())
                    log(Info, QString("Task was successfully assigned to [%1] '%2'!").arg(QSN(node->getId()), QString(node->getName())));
                else
                    log(Info, QString("Task was successfully removed from [%1] '%2'!").arg(QSN(node->getId()), QString(node->getName())));
            }
            ui->listNodes->item(index)->setText(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            break;
        }
        case Task::Start: {
            auto t = static_cast<StartTask*>(task);
            quint32 index =  t->nodeIndex;
            Node* node = nodes->at(index);
            QString dtFormat("dd-MM hh:mm:ss.zzz");
            if(t->getCode())
                log(Error, QString("[%1] '%2' has failed to start job: %3.").arg(QSN(node->getId()), QString(node->getName()), t->getMessage()));
            else
                log(Info, QString("[%1] '%2' has started its job at: %3 (remote), %4 (local).").arg(QSN(node->getId()), node->getName(), QDateTime::fromMSecsSinceEpoch(node->jobStartedAt).toString(dtFormat), QDateTime::fromMSecsSinceEpoch(node->jobStartedAtLocal).toString(dtFormat)));
            ui->listNodes->item(index)->setText(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            break;
        }
        case Task::Stop: {
            auto t = static_cast<StopTask*>(task);
            quint32 index =  t->nodeIndex;
            Node* node = nodes->at(index);
            if(t->getCode())
                log(Error, QString("[%1] '%2' has failed to stop job: %3.").arg(QSN(node->getId()), QString(node->getName()), t->getMessage()));
            else
                log(Info, QString("[%1] '%2' has stopped its job.").arg(QSN(node->getId()), QString(node->getName())));
            ui->listNodes->item(index)->setText(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            break;
        }
        case Task::Kick: {
            auto t = static_cast<KickTask*>(task);
            quint32 index =  t->nodeIndex;
            if(t->getCode()) {
                Node* node = nodes->at(index);
                log(Error, QString("Unable to kick [%1] '%2' from the cluster: %3.").arg(QSN(node->getId()), QString(node->getName()), t->getMessage()));
                ui->listNodes->item(index)->setText(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            } else {
                log(Info, QString("[%1] '%2' was kicked from the cluster.").arg(QSN(t->nodeId), QString(t->nodeName)));
                delete ui->listNodes->takeItem(index);
            }
            if(!nodes->size()) {
                ui->buttonKickAll->setEnabled(false);
                ui->buttonStatusAll->setEnabled(false);
            }
            {
                bool found = false;
                for(qint32 i = 0; i < nodes->length(); i++) {
                    if(nodes->at(i)) {
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    ui->buttonStartRemote->setText("Start remote");
                    ui->buttonStartRemote->setEnabled(false);
                    runningRemote = false;
                }
            }
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
            log(Warning, QString("Received unexpected message from [%1] '%2': %3!").arg(QSN(node->getId()), node->getName(), e->reason));
            break;
        }
        case Event::MalformedMessage: {
            auto e = static_cast<MalformedMessageEvent*>(event);
            auto nodes = core->getNodeList();
            delete ui->listNodes->takeItem(e->index);
            QDir("jobs/" + QSN(e->id)).removeRecursively();
            log(Warning, QString("Received malformed message from [%1] '%2': %3! Node was kicked!").arg(QSN(e->id), e->name, e->reason));
            if(!nodes->size()) {
                ui->buttonKickAll->setEnabled(false);
                ui->buttonStatusAll->setEnabled(false);
            }
            {
                bool found = false;
                for(qint32 i = 0; i < nodes->length(); i++) {
                    if(nodes->at(i)) {
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    ui->buttonStartRemote->setText("Start remote");
                    ui->buttonStartRemote->setEnabled(false);
                    runningRemote = false;
                }
            }
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
            ui->buttonKickAll->setEnabled(true);
            ui->buttonStatusAll->setEnabled(true);
            break;
        }
        case Event::NodeLeft: {
            auto e = static_cast<NodeLeftEvent*>(event);
            auto nodes = core->getNodeList();
            delete ui->listNodes->takeItem(e->index);
            QDir("jobs/" + QSN(e->id)).removeRecursively();
            log(Info, QString("[%1] '%2' has left the cluster: %3.").arg(QSN(e->id), e->name, e->leaveDesctiption));
            if(!nodes->size()) {
                ui->buttonKickAll->setEnabled(false);
                ui->buttonStatusAll->setEnabled(false);
            }
            {
                bool found = false;
                for(qint32 i = 0; i < nodes->length(); i++) {
                    if(nodes->at(i)) {
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    ui->buttonStartRemote->setText("Start remote");
                    ui->buttonStartRemote->setEnabled(false);
                    runningRemote = false;
                }
            }
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
            QString dtFormat("dd-MM hh:mm:ss.zzz"),
                    resultPath("results/" + QDateTime::fromMSecsSinceEpoch(remoteResultTimeStamp).toString("dd.MM.yyyy_HH-mm-ss.zzz") + "/");
            ui->listNodes->item(e->index)->setText(QString("[%1] %2 (%3) - %4").arg(QSN(node->getId()), node->getName(), node->getAddress(), node->getStatus()));
            {
                QDir(resultPath).mkpath(".");
                QString jobPath("jobs/" + QSN(node->getId()) + "/");
                QFile path(jobPath + "path"), input(jobPath + "input"), meta(resultPath + "meta"), output(resultPath + "output");
                path.open(QFile::ReadOnly);
                meta.open(QFile::WriteOnly);
                output.open(QFile::WriteOnly);
                meta.write(QString("type: remote\nnode: [%1] %2 (%3)\ncode: %4\nstarted: %5\nfinished: %6\n").arg(QSN(node->getId()), node->getName(), node->getAddress(), QString(path.readAll()), QDateTime::fromMSecsSinceEpoch(node->jobStartedAt).toUTC().toString(Qt::ISODate), QDateTime::fromMSecsSinceEpoch(node->jobFinishedAt).toUTC().toString(Qt::ISODate)).toLatin1());
                input.copy(resultPath + "input");
                output.write(e->output);
            }
            log(Info, QString("[%1] '%2' has finished the job at: %3 (remote), %4 (local)! Result folder is '%5'.").arg(QSN(node->getId()), QString(node->getName()), QDateTime::fromMSecsSinceEpoch(node->jobFinishedAt).toString(dtFormat), QDateTime::fromMSecsSinceEpoch(node->jobFinishedAtLocal).toString(dtFormat), resultPath));
            break;
        }
        default: {
            log(Warning, QString("Unknown task finished, type: %1.").arg(QSN(event->getType())));
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
        auto nodes = core->getNodeList();
        Node* node = nodes->at(currentRow);
        ui->buttonAssign->setEnabled(node->isAcceptingTasks() ? true : false);
        ui->buttonStatus->setEnabled(true);
        ui->buttonKick->setEnabled(true);
    }
}
void MainWindow::on_buttonAssign_clicked()
{
    quint32 index = ui->listNodes->currentRow();
    auto nodes = core->getNodeList();
    Node* node = nodes->at(index);
    quint32 nodeId = node->getId();
    QString nodeName = node->getName();
    QDir taskDir("jobs/" + QSN(node->getId()));
    QFile path(taskDir.path() + "/path"), code(taskDir.path() + "/code"), input(taskDir.path() + "/input");
    assignWindow->nodeId = nodeId;
    assignWindow->nodeName = nodeName;
    assignWindow->nodeAddress = node->getAddress();
    assignWindow->nodePython = node->getVersion();
    assignWindow->nodeModules = node->getModules();
    if(!taskDir.exists()) {
        assignWindow->hadJob = false;
        assignWindow->jobPath.clear();
        assignWindow->code.clear();
        assignWindow->input.clear();
    } else {
        assignWindow->hadJob = true;
        path.open(QFile::ReadOnly);
        assignWindow->jobPath = path.readAll();
        path.close();
        code.open(QFile::ReadOnly);
        if(code.size() < MAX_EDITOR_DISPLAY_LENGTH)
            assignWindow->code = code.readAll();
        else
            assignWindow->code.clear();
        code.close();
        input.open(QFile::ReadOnly);
        if(input.size() < MAX_EDITOR_DISPLAY_LENGTH)
            assignWindow->input = input.readAll();
        else
            assignWindow->input.clear();
        input.close();
    }
    assignWindow->inputChanged = false;
    assignWindow->codeChanged = false;
    if(assignWindow->exec()) {
        if(nodes->at(index) == node) {
            taskDir.mkpath(".");
            if(assignWindow->code.length()) {
                path.open(QFile::WriteOnly | QFile::Truncate);
                path.write(QDir::toNativeSeparators(assignWindow->jobPath).toLocal8Bit());
                input.open(QFile::WriteOnly | QFile::Truncate);
                input.write(assignWindow->input);
                code.open(QFile::WriteOnly | QFile::Truncate);
                code.write(assignWindow->code);
                log(Info, QString("Assigning '%1' to [%2] '%3'...").arg(assignWindow->jobPath, QSN(nodeId), nodeName));
            } else {
                taskDir.removeRecursively();
                log(Info, QString("Removing task from [%1] '%2'...").arg(QSN(nodeId), nodeName));
            }
            emit newTask(new AssignTask(index, assignWindow->input, assignWindow->code));
        } else
            log(Warning, QString("Failed to assign task: [%1] '%2' is no longer in the cluster!").arg(QSN(nodeId), nodeName));
    }
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
void MainWindow::on_listNodes_itemDoubleClicked()
{
    on_buttonAssign_clicked();
}

void MainWindow::on_buttonStartRemote_clicked()
{
    remoteResultTimeStamp = QDateTime::currentMSecsSinceEpoch();
    auto nodes = core->getNodeList();
    Node* node;
    for(qint32 i = 0; i < nodes->length(); i++) {
        node = nodes->at(i);
        if(node->isReadyToStart()) {
            log(Info, QString("Starting remote task on [%1] \'%2\'...").arg(QSN(node->getId()), QString(node->getName())));
            emit newTask(new StartTask(i));
        }
    }
}

void MainWindow::on_buttonStartLocal_clicked()
{
}

void MainWindow::on_buttonStopAll_clicked()
{
}
