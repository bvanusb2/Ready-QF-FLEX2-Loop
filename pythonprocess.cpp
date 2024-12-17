//
// Filename: pythonprocess.cpp
//
// Purpose:

#include "pythonprocess.h"
#include "QProcess"
#include "qdebug.h"


PythonProcess::PythonProcess()
{

}

PythonProcess::~PythonProcess() {
    //terminate();
}

//void PythonProcess::terminate() {

//    // todo - as shutting down this process, the system is issuing these warnings...
//    // QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread
//    terminateProcessRequest();

//    // do we need to wait for sleep to finish?  Should we get rid of sleep in py script?
//    if (mProcess.processId() != 0) {
//        mProcess.terminate();
//    }
//}


// QStringList must include pythonScriptName.py, and any parameters expected
// at script startup
bool PythonProcess::spawnProcess(QStringList pyScriptNameAndParams) {

    bool started = false;

    // QProcess apparently isn't the most robust when things don't go as planned.
    // It would appear we'll have to use a pointer, and discard the old process ptr
    // if it can't open a web page
    mProcessPtr = std::make_shared<QProcess>();
    mProcessPtr->start("python3", pyScriptNameAndParams);

    // The problem is, python3 spins up just fine regardless of finding the script
    // you need the script to emit a message, and look for the message for confirmation
    // that it's running

    // do we seriously wait up to four sec for the message?  It takes about that long
    // for the webpage to spin up!!!
    mProcessPtr->waitForReadyRead(4000);  // mS or -1 to wait indf.
    QString msgOutStr = mProcessPtr->readAll();

    if (msgOutStr.contains(SeleniumQFInterfaceReadyString)) {
        started = true;
    }
    else {
        // chances are the wrong ip addr was used, or the QF is offline
        //mProcessPtr->kill();
    }

    return started;
}

void PythonProcess::sendToProcess(QString msgInStr, QString& msgOutStr) {

    // first call starting, 2nd call running
    if (mProcessPtr != nullptr) {

        QProcess::ProcessState state = mProcessPtr->state();

        if(state != QProcess::NotRunning) {

            // The msg to the python process needs a "\n" to act as a cr/lf (enter key press)
            msgInStr += "\n";

            mProcessPtr->write(msgInStr.toUtf8());

            // todo - !!! Critical! Must be longer than QFseleniumInterface.py timeout, or
            // it returns "" while py script is still waiting for response!!!
            mProcessPtr->waitForReadyRead(5000);  // mS or -1 to wait indf.
            msgOutStr = mProcessPtr->readAll();

            QString stderr = mProcessPtr->readAllStandardError(); // todo - what about this?
        }
        else {
            qDebug() << "Python process not running!!!";
            msgOutStr = "";
        }
    }

}

PythonOneShot::PythonOneShot()
{

}

PythonOneShot::~PythonOneShot() {
}

//
// Function name: request
//
// Purpose: Uses QProcess to invoke python3 on a script.  Takes stdout produced by the python script
//          and pushes into a msg out object.  The script execution is synchronous, that is, the call
//          doesn't return until execution is complete.  This way the stdout msg is ready when it's
//          time to return.
//
void PythonOneShot::request(const QStringList& argsIn, std::string& msgOutStr, QString& stderr) {

    QString p_stdout;

    QProcess p;
    p.start("python3", argsIn);
    p.waitForFinished();

    p_stdout = p.readAll();
    msgOutStr = p_stdout.toStdString();

    // Note - have not encountered error output, but this might happen if the script
    // doesn't exist or is flawed in some manner.
    stderr = p.readAllStandardError();

}
