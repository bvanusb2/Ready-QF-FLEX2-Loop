
#include <string>
#include <charconv>  // std::from_chars

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


    // For debugging purposes, this will run perhaps at a one minute rate.
    // For actual work, probably will run once an hour / half-hour?  We do want to display
    // the flex results right after running.  So, maybe query for new results every minute,
    // and if new results are ready, then request them asap
    mQueryResponseTimer->start(mQueryResponseTimerInterval);

    // The OPC thread takes requests from the GUI (or a timer) and invokes
    // synchonous Python scripts system calls.  It pushes the responses into a queue
    // which can later be pulled from.
    if (mFlex2processorPtr == nullptr) {
        mFlex2processorPtr = std::make_unique<FLEX2processor>(ui->lineEditAddrPythonScriptsFolder->text());
        mFlex2processorPtr->startThread();
    }

    if (mQFprocessorPtr == nullptr) {
        mQFprocessorPtr = std::make_unique<QFprocessor>(ui->lineEditAddrPythonScriptsFolder->text());
        mQFprocessorPtr->startThread();
    }

    // the last param is max num points in plot.  Keep at 30, which is 5 days at 6 samples per day
    lactosePlot.setPlot(ui->widgetPlotLacticAcid, QColorConstants::Blue, "Lactic Acid, mg/mL", 1.2, 0.2, 30);
    pHPlot.setPlot(ui->widgetPlotPhAcid, QColorConstants::Green, "pH", 14, 2, 30);
}

MainWindow::~MainWindow()
{
    // Send a message to the QF processor thread to instruct the python script
    // to close the DEO window and terminate
    // todo - still getting a QMutes: destroying locked mutex msg
    mQFprocessorPtr->sendQuitMsgToPySel();

    // Allow enough time for script to close DEO and clean up
    QThread::sleep(2);

    mFlex2processorPtr->terminateProc();

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

    // update display on connection status (up/down)
    // If still up, send a query to Flex 2 for any new data, and to QF for any updates

    // Process any data received from Flex2 and QF
    // How to do this?  Update fields and graph
    mProcessFlex2Messages();
    mProcessQfMessages();

}


//
// Function name: mProcessFlex2Messages
//
// Purpose: Timer driven, polls the OPC thread for any new messages, and if so,
//          processes them.
//
void MainWindow::mProcessFlex2Messages() {
    while (mFlex2processorPtr->pullDataReady()) {

        std::vector<FLEX2message> msgs;

        mFlex2processorPtr->pullProcessedData(msgs);

        qDebug() << "opcThread.pullProcessedData";

        // The FLEX2 processor needs to know about the format, not this thread. Fix.
        for (FLEX2message& s : msgs) {

            if (s.mCommand == FLEX2message::Command::GetLacticAcidConc) {
                std::string resp = s.mResponseStr;

                double val = s.mResultDouble;
                double date = s.mResultDate;  // for plotting
                ui->lineEditLacticAcidConc->setText(QString::fromStdString(std::to_string(val)));

                // we'll need a control object to hold these date & values
                lactosePlot.addDataPoint(date, val);
                qDebug() << "found lactic acid response, conc: " << val;
            }
            else if (s.mCommand == FLEX2message::Command::GetpH) {
                std::string resp = s.mResponseStr;

                double val = s.mResultDouble;
                double date = s.mResultDate;  // for plotting
                ui->lineEditPh->setText(QString::fromStdString(std::to_string(val)));
                pHPlot.addDataPoint(date, val);
                qDebug() << "found pH response, conc: " << val;
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
void MainWindow::mProcessQfMessages() {

    // This timer driven fcn looks for msgs from QF processor thread
    while (mQFprocessorPtr->pullDataReady()) {

        std::vector<QFmessage> msgs;
        mQFprocessorPtr->pullProcessedData(msgs);

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
        mQFprocessorPtr->pushSampleData(msgs);
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
    mFlex2processorPtr->pushSampleData(msgs);
}

void MainWindow::on_pushButtonGetPhFlex_clicked()
{
    FLEX2message msg;
    std::vector<FLEX2message> msgs;

    msg.mCommand = FLEX2message::Command::GetpH;
    msgs.push_back(msg);
    mFlex2processorPtr->pushSampleData(msgs);

}


void MainWindow::on_pushButtonConnToSelQf_clicked()
{
    QFmessage msg;
    std::vector<QFmessage> msgs;

    msg.mCommand = QFmessage::Command::ConnectToSelenium;

    // todo - move name to QFprocessor class as static const defn.
    // should it be entirely contained in class?
    msg.mCommandStringList.push_back("QFseleniumInterface.py");

    QString ipAddr = ui->lineEditQfIpAddr->text();
    msg.mCommandStringList.push_back(ipAddr);
    msgs.push_back(msg);

    mQFprocessorPtr->pushSampleData(msgs);
}


void MainWindow::on_pushButtonQueryQFflow_clicked()
{
    QFmessage msg;
    std::vector<QFmessage> msgs;
    msg.mCommand = QFmessage::Command::GetECCirc_PumpCapRepoDisposablePumpStatus_accumVolMl;
    msgs.push_back(msg);
    mQFprocessorPtr->pushSampleData(msgs);

}


void MainWindow::on_pushButtonSaveWidgetAsImage_clicked()
{
    // https://forum.qt.io/topic/56727/saving-widget-in-image/3
    ui->centralwidget->grab().save("widget.png");

    //ui->widget->grab().save("image.png");
}

