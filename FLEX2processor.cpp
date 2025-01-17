#include "QProcess"
#include "QStringList"
#include "QDebug"

#include "FLEX2processor.h"

FLEX2processor::FLEX2processor(QString folder, QThread *parent) :
    ThreadContainer{parent}
  , mPythonScriptFolderName(folder)
  , debugValuesGenerator_pH{7.0, 1.0}
  , debugValuesGenerator_lacticAcid{0.5, 0.2}
{


}

FLEX2processor::~FLEX2processor() {
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

        // todo - data/string formatting should be done here, so consumers don't have to
        // know about any formats

        if (msgIn.mCommand == FLEX2message::Command::GetTime) {

            // example arg list with dummy args.  They are not used.
            QStringList arg;
            arg.push_back(mPythonScriptFolderName + "getTime.py");
            arg.push_back( "3.0" );
            arg.push_back( "wgs");
            procPythonInOut(arg, msgIn, msgOut, p_stderr);
            svoutput.push_back(msgOut);
        }
        else if (msgIn.mCommand == FLEX2message::Command::GetLacticAcidConc) {
            QStringList arg;
            arg.push_back(mPythonScriptFolderName + "getLacticAcid.py");
            procPythonInOut(arg, msgIn, msgOut, p_stderr);
            svoutput.push_back(msgOut);
        }
        else if (msgIn.mCommand == FLEX2message::Command::GetpH) {
            QStringList arg;
            arg.push_back(mPythonScriptFolderName + "getpH.py");
            procPythonInOut(arg, msgIn, msgOut, p_stderr);
            svoutput.push_back(msgOut);
        }
    }

}

//
// Function name: procPythonInOut
//
// Purpose: Takes a Flex2 OPC request string and pushes it to the python OPC
//          script processor.  This process requests data via OPC from the Flex2
//          and populates the message out struct with the data.
//
//          The script is a blocking process so this function is called by a
//          non-gui thread.
//
void FLEX2processor::procPythonInOut(QStringList& arg, FLEX2message& msgIn, FLEX2message& msgOut, QString p_stderr) {

    std::string msgOutStr;

    // Return the response to the caller
    msgOut = msgIn;
    msgOut.mResponseStr = msgOutStr;  // todo - deprecate?

    double value = 0;
    double date = 0;

    // Send OPC command to OPC server, wait for response
    // The string list is a list of words for the command line request
    if (mDebugFakeFlexResponse) {

        // TODO - You need to parse on pH, lactic acid or glucose!
        if (msgIn.mCommand == FLEX2message::GetLacticAcidConc) {
            debugValuesGenerator_lacticAcid.getValueAndDate(value, date);
        }
        else if (msgIn.mCommand == FLEX2message::GetpH) {
            debugValuesGenerator_pH.getValueAndDate(value, date);
        }
        msgOut.mResultDouble = value;
        msgOut.mResultDate = date;

    } else {
        mPythonOneShot.request(arg, msgOutStr, p_stderr);
    }


}

