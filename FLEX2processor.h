#ifndef FLEX2PROCESSOR_H
#define FLEX2PROCESSOR_H
#include "ThreadContainer.h"
#include "pythonprocess.h"

class FLEX2message {
public:
    enum Command {
        None,
        GetTime, // debug, but might be good query of FLEX2
        GetLacticAcidConc,
        GetpH
    };

    Command mCommand;

    // todo - was considering std::any but I don't know if I want
    // to get that crazy.  Plus I'm not sure if I like std::any.  I hate
    // ambiguous code
    std::string mResponseStr;
    double mResponseDouble;

    FLEX2message() {
        mCommand = None;
        mResponseStr = "";
    }
};

class FLEX2processor : public ThreadContainer<FLEX2message>
{
public:
    explicit FLEX2processor(QThread *parent = nullptr);

private:

    void process(std::vector<FLEX2message>& svinput, std::vector<FLEX2message>& svoutput);
    //void pythonRequest(const QStringList& argsIn, std::string &msgOutStr, QString& stderr);
    void procPythonInOut(QStringList& arg, FLEX2message &msgIn, FLEX2message &msgOut, QString p_stderr);

    PythonProcess mPythonProcess;

};

#endif // FLEX2PROCESSOR_H