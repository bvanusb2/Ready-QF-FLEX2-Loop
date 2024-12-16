//
// Filename: QFprocessor.cpp
//
// Purpose: Provides thread and interfaces to communicating with QF through DEO and
//          Selenium
//
#include "QFprocessor.h"

// for debug
#include "QDebug"

QFprocessor::QFprocessor(QString folder, QThread *parent) :
    ThreadContainer{parent}
    , mPythonScriptFolderName{folder} {
}

QFprocessor::~QFprocessor() {
}

//
// Function name: sendQuitMsgToPySel
//
// Purpose: Called by mainWindow or other thread, sends a message request to the
//          QFprocessor to tell the python script to close the DEO window and terminate
//          itself
//
void QFprocessor::sendQuitMsgToPySel() {
    QFmessage msg;
    std::vector<QFmessage> msgs;
    msg.mCommand = QFmessage::Command::Quit;
    msgs.push_back(msg);
    pushSampleData(msgs);
}

void QFprocessor::terminateProc() {
    if (mPythonSeleniumProcessPtr != nullptr) {
        mPythonSeleniumProcessPtr = nullptr;
    }
}

void QFprocessor::process(std::vector<QFmessage> &svinput, std::vector<QFmessage> &svoutput) {

    QString result;
    QFmessage msgResult;

    if (mPythonSeleniumProcessPtr == nullptr)
        mPythonSeleniumProcessPtr = std::make_unique<PythonProcess>();

    for (QFmessage& msg : svinput) {

        msgResult = msg;

        switch(msg.mCommand) {
        case QFmessage::Command::ConnectToSelenium:
            // Insert the folder location before script filename
            msg.mCommandStringList[0].insert(0, mPythonScriptFolderName);

            qDebug() << "QFprocess spawn: " << msg.mCommandStringList[0] << ", addr: " << msg.mCommandStringList[1];

            if (mPythonSeleniumProcessPtr->spawnProcess(msg.mCommandStringList)) {
                msgResult.mResponseStr = "Python Selenium processor spawned";
            } else {
                msgResult.mCommand = QFmessage::Command::ConnectToSeleniumFailed;
                msgResult.mResponseStr = "Python Selenium processor failed to spawn!";
            }
            svoutput.push_back(msgResult);
            break;

        case QFmessage::Command::GetSystemTime:
            mPythonSeleniumProcessPtr->sendToProcess("systemTime", result);
            msgResult.mResponseStr = result.toStdString();
            qDebug() << "Result from cmd systemTime: " << result;
            svoutput.push_back(msgResult);
            break;


        // todo - warning - this field went away in DEO, and the py script barfed when it
        // coudln't find it!  Furthermore, this app barfed when it couldn't talk to the py
        // script!
        case QFmessage::Command::GetECCirc_PumpCapRepoDisposablePumpStatus_accumVolMl:
            mPythonSeleniumProcessPtr->sendToProcess("ECCirc_PumpCapRepoDisposablePumpStatus._accumVolMl.Value", result);
            msgResult.mResponseStr = result.toStdString();
            qDebug() << "Result from cmd ECCirc_Pump: " << result;
            svoutput.push_back(msgResult);
            break;

        case QFmessage::Command::Quit:
            mPythonSeleniumProcessPtr->sendToProcess(SeleniumQFInterfaceQuitString, result);
            msgResult.mResponseStr = result.toStdString();
            svoutput.push_back(msgResult);
            break;

        default:
            break;
        }
    }

}
