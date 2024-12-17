
#include <string>

#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "memory"   // for std::make_unique

#include "QDebug"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // This timer periodically checks for queued OPC responses
    mQueryResponseTimer = new QTimer(this);
    connect(mQueryResponseTimer, &QTimer::timeout, this, &MainWindow::mQueryResponseTimerSlot);

    mQueryResponseTimer->start(mQueryResponseTimerInterval);

    // The OPC thread takes requests from the GUI (or a timer) and invokes
    // synchonous Python scripts system calls.  It pushes the responses into a queue
    // which can later be pulled from.
    if (mOpcThreadPtr == nullptr) {
        mOpcThreadPtr = std::make_unique<FLEX2processor>(ui->lineEditAddrPythonScriptsFolder->text());
        mOpcThreadPtr->startThread();
    }

    if (mQFThreadPtr == nullptr) {
        mQFThreadPtr = std::make_unique<QFprocessor>(ui->lineEditAddrPythonScriptsFolder->text());
        mQFThreadPtr->startThread();
    }

}

MainWindow::~MainWindow()
{
    // Send a message to the QF processor thread to instruct the python script
    // to close the DEO window and terminate
    // todo - still getting a QMutes: destroying locked mutex msg
    mQFThreadPtr->sendQuitMsgToPySel();

    // Allow enough time for script to close DEO and clean up
    QThread::sleep(2);

    mOpcThreadPtr->terminateProc();

    delete ui;
    delete mQueryResponseTimer;

}

//
// Function name: mQueryResponse
//
// Purpose: Timer driven, polls the OPC thread for any new messages, and if so,
//          processes them.
//
void MainWindow::mQueryResponseTimerSlot() {

    mProcessPeriodicOPC();

    mProcessPeriodicQF();

}


// todo - consider renaming mProcessPeriodicFlex2
//
// Function name: mProcessPeriodicOpc
//
// Purpose: Timer driven, polls the OPC thread for any new messages, and if so,
//          processes them.
//
void MainWindow::mProcessPeriodicOPC() {
    while (mOpcThreadPtr->pullDataReady()) {

        std::vector<FLEX2message> msgs;

        mOpcThreadPtr->pullProcessedData(msgs);

        qDebug() << "opcThread.pullProcessedData";

        // TODO - Assuming that we're getting a string of format "analyte: value"
        // and we're doing the processing of that string here.  We could instead
        // process in OPCthread::pythonRequest and return just the "value" string
        // I don't think we want to convert to int/double because the processor does not
        // parse by value, and here we might receive a conc OR a status string...

        // The FLEX2 processor needs to know about the format, not this thread. Fix.
        for (FLEX2message& s : msgs) {

            if (s.mCommand == FLEX2message::Command::GetLacticAcidConc) {
                std::string resp = s.mResponseStr;
                std::string conc = resp.substr(resp.find(":") + 1);
                ui->lineEditLacticAcidConc->setText(QString::fromStdString(conc));

                qDebug() << "found lactic acid response, conc: " << QString::fromStdString(conc);
            }
            else if (s.mCommand == FLEX2message::Command::GetpH) {
                std::string resp = s.mResponseStr;
                std::string conc = resp.substr(resp.find(":") + 1);
                ui->lineEditPh->setText(QString::fromStdString(conc));

                qDebug() << "found pH response, conc: " << QString::fromStdString(conc);
            }

        }

    }

}

//
// Function name: mProcessPeriodicQF
//
// Purpose: Timer driven, polls the OPC thread for any new messages, and if so,
//          processes them.
//
void MainWindow::mProcessPeriodicQF() {

    // This timer driven fcn looks for msgs from QF processor thread
    while (mQFThreadPtr->pullDataReady()) {

        std::vector<QFmessage> msgs;
        mQFThreadPtr->pullProcessedData(msgs);

        for (QFmessage& s : msgs) {
            if (s.mCommand == QFmessage::Command::GetECCirc_PumpCapRepoDisposablePumpStatus_accumVolMl) {
                std::string resp = s.mResponseStr;
                ui->lineEditFlow->setText(QString::fromStdString(resp));
            }
            else if (s.mCommand == QFmessage::Command::ConnectToSeleniumFailed) {
                disconnectedFromQF();
            }
            else if (s.mCommand == QFmessage::Command::ConnectToSelenium) {
                connectedToQF();
            }
            else if (s.mCommand == QFmessage::Command::GetSystemTime) {
                std::string resp = s.mResponseStr;

                if (QString::fromStdString(resp) == QFprocessor::SeleniumQFCantFindElementString) {
                    disconnectedFromQF();
                    resp = "";
                }

                ui->lineEditQfSystemTime->setText(QString::fromStdString(resp));
            }
            else if (s.mCommand == QFmessage::Command::Quit) {
                // A msg sent from here, echoed back
            }
            else {
                qDebug() << "UNKNOWN command response!!!";
            }
        }
    }

    // periodically query the QF time.  Inform user if no response
    if (mConnectedQF) {
        QFmessage msg;
        std::vector<QFmessage> msgs;
        msg.mCommand = QFmessage::Command::GetSystemTime;
        msgs.push_back(msg);
        mQFThreadPtr->pushSampleData(msgs);
    }

}

void MainWindow::connectedToQF() {
    ui->lineEditQfConnStatus->setText("Connected to QF");
    mConnectedQF = true;

    QPalette palette;
    palette.setColor(QPalette::Base, Qt::green);
    ui->lineEditQfConnStatus->setPalette(palette);
}

void MainWindow::disconnectedFromQF() {
    ui->lineEditQfConnStatus->setText("Disconnected from QF");
    mConnectedQF = false;

    QPalette palette;
    palette.setColor(QPalette::Base, Qt::yellow);
    ui->lineEditQfConnStatus->setPalette(palette);
}

void MainWindow::on_pushButtonGetLacticAcidFlex_clicked()
{
    FLEX2message msg;
    std::vector<FLEX2message> msgs;

    msg.mCommand = FLEX2message::Command::GetLacticAcidConc;
    msgs.push_back(msg);
    mOpcThreadPtr->pushSampleData(msgs);
}

void MainWindow::on_pushButtonGetPhFlex_clicked()
{
    FLEX2message msg;
    std::vector<FLEX2message> msgs;

    msg.mCommand = FLEX2message::Command::GetpH;
    msgs.push_back(msg);
    mOpcThreadPtr->pushSampleData(msgs);

}


void MainWindow::on_pushButtonConnToSelQf_clicked()
{
    QFmessage msg;
    std::vector<QFmessage> msgs;

    // If the python interface was running when it lost its connection
    // make sure the process is stopped before restarting it and the web
    // page
//    msg.mCommand = QFmessage::Command::Quit;
//    msgs.push_back(msg);


    msg.mCommand = QFmessage::Command::ConnectToSelenium;

    // todo - move name to QFprocessor class as static const defn.
    // should it be entirely contained in class?
    msg.mCommandStringList.push_back("QFseleniumInterface.py");

    QString ipAddr = ui->lineEditQfIpAddr->text();
    msg.mCommandStringList.push_back(ipAddr);
    msgs.push_back(msg);

    mQFThreadPtr->pushSampleData(msgs);
}


void MainWindow::on_pushButtonQueryQFflow_clicked()
{
    QFmessage msg;
    std::vector<QFmessage> msgs;
    msg.mCommand = QFmessage::Command::GetECCirc_PumpCapRepoDisposablePumpStatus_accumVolMl;
    msgs.push_back(msg);
    mQFThreadPtr->pushSampleData(msgs);

    // You'll need to query later for the response!
}

