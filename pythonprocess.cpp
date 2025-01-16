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
}

// QStringList must include pythonScriptName.py, and any parameters expected
// at script startup
bool PythonProcess::spawnProcess(QStringList pyScriptNameAndParams) {

    bool started = false;

    // QProcess apparently isn't the most robust when things don't go as planned.
    // It would appear that "kill" and "terminate" don't work well with our app.
    // Better to use a pointer, and discard when a web page can't spin up.
    // If the page doesn't spin up, it will eventually close on it's own
    // so don't try to kill/terminate
    mProcessPtr = std::make_shared<QProcess>();
    mProcessPtr->start("python3", pyScriptNameAndParams);

    mProcessPtr->waitForReadyRead(PROCESS_WAIT_READY_READ_TIMEOUT_MS);
    QString msgOutStr = mProcessPtr->readAll();

    if (msgOutStr.contains(SeleniumQFInterfaceReadyString)) {
        started = true;
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

            // Critical! Must be longer than QFseleniumInterface.py timeout, or
            // it returns "" while py script is still waiting for response!!!
            mProcessPtr->waitForReadyRead(PROCESS_WAIT_READY_READ_TIMEOUT_MS);
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
