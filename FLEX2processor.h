#ifndef FLEX2PROCESSOR_H
#define FLEX2PROCESSOR_H
#include "ThreadContainer.h"
#include "pythonprocess.h"

#include "debugvaluesgenerator.h"

class FLEX2message {
public:
    enum Command {
        None,
        GetTime, // debug, but might be good query of FLEX2
        GetLacticAcidConc,
        GetpH
    };

    Command mCommand;

    std::string mResponseStr;
    double mResultDouble;
    double mResultDate; // e.g. QDateTime

    FLEX2message() {
        mCommand = None;
        mResponseStr = "";
    }
};

class FLEX2processor : public ThreadContainer<FLEX2message>
{
public:
    explicit FLEX2processor(QString folder = "/", QThread *parent = nullptr);
    ~FLEX2processor();

    void terminateProc();

private:

    void process(std::vector<FLEX2message>& svinput, std::vector<FLEX2message>& svoutput);
    //void pythonRequest(const QStringList& argsIn, std::string &msgOutStr, QString& stderr);
    void procPythonInOut(QStringList& arg, FLEX2message &msgIn, FLEX2message &msgOut, QString p_stderr);

    PythonOneShot mPythonOneShot;

    QString mPythonScriptFolderName;

    // todo Consider removing after test
    bool mDebugFakeFlexResponse = true;

    // for debugging
    DebugValuesGenerator debugValuesGenerator_pH;
    DebugValuesGenerator debugValuesGenerator_lacticAcid;

};

#endif // FLEX2PROCESSOR_H
