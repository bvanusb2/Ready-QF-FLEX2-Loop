#include "QProcess"
#include "QStringList"
#include "QDebug"

#include "FLEX2processor.h"

FLEX2processor::FLEX2processor(QThread *parent) : ThreadContainer{parent} {

}

FLEX2processor::~FLEX2processor() {
    stopThread();
}

void FLEX2processor::terminateProc() {
    // If there is any kind of Python running apps, they are terminated here
    // see the QFprocessor
}


//
// Function name: process
//
// Purpose: Called by thread, takes input msgs sent by user, parses the msgs, invokes
//          python scripts as necessary, and pushes responses into output messages.
//
void FLEX2processor::process(std::vector<FLEX2message> &svinput, std::vector<FLEX2message> &svoutput) {

    QString p_stderr;   // todo not doing anything yet with err msgs, although we have yet to rcve any
    FLEX2message msgOut;

    for (FLEX2message& msgIn : svinput) {

        std::string msgOutStr;

        if (msgIn.mCommand == FLEX2message::Command::GetTime) {

            // example arg list with dummy args.  They are not used.
            QStringList arg {"/home/braddonvanslyke/Terumo/QtProject/SimpleGuiTest/getTime.py", "3.0", "wgs"};
            procPythonInOut(arg, msgIn, msgOut, p_stderr);
            svoutput.push_back(msgOut);
        }
        else if (msgIn.mCommand == FLEX2message::Command::GetLacticAcidConc) {

            QStringList arg {"/home/braddonvanslyke/Terumo/QtProject/SimpleGuiTest/getLacticAcid.py"};
            procPythonInOut(arg, msgIn, msgOut, p_stderr);
            svoutput.push_back(msgOut);
        }
        else if (msgIn.mCommand == FLEX2message::Command::GetpH) {

            QStringList arg {"/home/braddonvanslyke/Terumo/QtProject/SimpleGuiTest/getpH.py"};
            procPythonInOut(arg, msgIn, msgOut, p_stderr);
            svoutput.push_back(msgOut);
        }
    }

}

void FLEX2processor::procPythonInOut(QStringList& arg, FLEX2message& msgIn, FLEX2message& msgOut, QString p_stderr) {

    std::string msgOutStr;

    mPythonOneShot.request(arg, msgOutStr, p_stderr);

    msgOut = msgIn;
    msgOut.mResponseStr = msgOutStr;

}

