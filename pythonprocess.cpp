#include "pythonprocess.h"
#include "QProcess"
#include "qdebug.h"


PythonProcess::PythonProcess()
{

}

PythonProcess::~PythonProcess() {
    terminate();
}

void PythonProcess::terminate() {
    QString msgOut;
    sendToProcess(SeleniumQFInterfaceQuitString, msgOut);

    if (mProcess.processId() != 0) {
        mProcess.terminate();
    }
}

//
// Function name: request
//
// Purpose: Uses QProcess to invoke python3 on a script.  Takes stdout produced by the python script
//          and pushes into a msg out object.  The script execution is synchronous, that is, the call
//          doesn't return until execution is complete.  This way the stdout msg is ready when it's
//          time to return.
//
void PythonProcess::request(const QStringList& argsIn, std::string& msgOutStr, QString& stderr) {

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

// QStringList must include pythonScriptName.py, and any parameters expected
// at script startup
bool PythonProcess::spawnProcess(QStringList pyScriptNameAndParams) {

    bool started = false;

    mProcess.start("python3", pyScriptNameAndParams);

    // The problem is, python3 spins up just fine regardless of finding the script
    // you need the script to emit a message, and look for the message for confirmation
    // that it's running

    // do we seriously wait up to four sec for the message?  It takes about that long
    // for the webpage to spin up!!!
    mProcess.waitForReadyRead(4000);  // mS or -1 to wait indf.
    QString msgOutStr = mProcess.readAll();

    if (msgOutStr.contains(SeleniumQFInterfaceReadyString)) {
        started = true;
    }

    return started;
}

void PythonProcess::sendToProcess(QString msgInStr, QString& msgOutStr) {


    // first call starting, 2nd call running
    QProcess::ProcessState state = mProcess.state();
    if(mProcess.state() != QProcess::NotRunning) {

        // The msg to the python process needs a "\n" to act as a cr/lf (enter key press)
        msgInStr += "\n";

        mProcess.write(msgInStr.toUtf8());

        // todo - magic wait time, 500mS seems to work
        mProcess.waitForReadyRead(500);  // mS or -1 to wait indf.
        msgOutStr = mProcess.readAll();

        QString stderr = mProcess.readAllStandardError(); // todo - what about this?
    }
    else {
        qDebug() << "Python process not running!!!";
        msgOutStr = "";
    }

}
