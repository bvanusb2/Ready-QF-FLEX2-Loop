#include "QFprocessor.h"

// for debug
#include "QDebug"

QFprocessor::QFprocessor(QThread *parent) : ThreadContainer{parent} {

}

QFprocessor::~QFprocessor() {
    terminate();
}

void QFprocessor::terminate() {
    if (mPythonSeleniumProcessPtr != nullptr) {
        mPythonSeleniumProcessPtr->terminate();
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

            // You'll need the script location/name and IP addr of QF DEO
            qDebug() << "QFprocess spawn: " << msg.mCommandStringList[0] << ", addr: " << msg.mCommandStringList[1];
            if (mPythonSeleniumProcessPtr->spawnProcess(msg.mCommandStringList)) {
                msgResult.mResponseStr = "Python Selenium processor spawned";
            } else {
                msgResult.mCommand = QFmessage::Command::ConnectToSeleniumFailed;
                msgResult.mResponseStr = "Python Selenium processor failed to spawn!";
            }
            svoutput.push_back(msgResult);
            break;

        case QFmessage::Command::GetECCirc_PumpCapRepoDisposablePumpStatus_accumVolMl:
            // this is a blocking call - ok because this is a separate thread
            qDebug() << "QFprocess rcve msg for pump mL";
            mPythonSeleniumProcessPtr->sendToProcess("ECCirc_PumpCapRepoDisposablePumpStatus._accumVolMl.Value", result);
            msgResult.mResponseStr = result.toStdString();
            qDebug() << "qfprocessor rcvd result str: " << result;
            svoutput.push_back(msgResult);
            break;

        default:
            break;
        }
    }

}
