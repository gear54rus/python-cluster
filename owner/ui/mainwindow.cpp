#include <QFile>
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
    listenWindow(new ListenWindow(this, Qt::MSWindowsFixedSizeDialogHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)),
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
    log(Info, "Started!");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::taskFinished(Task* task)
{
    switch(task->getType()) {
        case Task::Listen: {
            auto t = static_cast<ListenTask*>(task);
            if(t->getCode())
                log(Error, QString("Unable to bind to %1:%2 (%3). Stopped listening.").arg(t->address.toString(), QSN(t->port), t->getMessage()));
            else
                log(Info, QString("Successfully bound to %1:%2").arg(t->address.toString(), QSN(t->port)));
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
            break;
        }
        case Event::MalformedMessage: {
            break;
        }
        case Event::JoinError: {
            auto e = static_cast<JoinErrorEvent*>(event);
            log(Warning, QString("%1 failed to join the cluster: %2!").arg(e->address, e->reason));
            break;
        }
        case Event::NodeJoined: {
            auto e = static_cast<NodeJoinedEvent*>(event);
            auto nodes = core->getNodeMap();
            quint32 id = e->id;
            Node* node = (*nodes)[id];
            ui->listNodes->addItem(QString("[%1] %2 (%3) - %4").arg(QSN(id), node->getName(), node->getAddress(), node->getStatus()));
            visualToCore[ui->listNodes->count() - 1] = id;
            auto x = ui->listNodes->item(0);
            log(Info, QString("'%2' (%3) has joined the cluster with ID: %1.").arg(QSN(id), node->getName(), node->getAddress()));
            break;
        }
        case Event::NodeLeft: {
            break;
        }
        case Event::NodeStatusChanged: {
            break;
        }
        case Event::JobFinished: {
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
    ui->editLog->append(QString("%1 [%2] %3").arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz"), logTypes[type], message));
}

void MainWindow::on_listNodes_currentRowChanged(int currentRow)
{
}
