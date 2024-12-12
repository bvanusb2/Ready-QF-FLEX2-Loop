#ifndef QFPROCESSOR_H
#define QFPROCESSOR_H

#include "ThreadContainer.h"
#include "pythonprocess.h"


class QFmessage {
public:
    enum Command {
        None,
        ConnectToSelenium,
        ConnectToSeleniumFailed,
        GetECCirc_PumpCapRepoDisposablePumpStatus_accumVolMl,
    };

    Command mCommand;

    QStringList mCommandStringList;

    // todo - was considering std::any but I don't know if I want
    // to get that crazy.  Plus I'm not sure if I like std::any.  I hate
    // ambiguous code
    std::string mResponseStr;
    double mResponseDouble;

    QFmessage() {
        mCommand = None;
        mResponseStr = "";
    }
};


class QFprocessor : public  ThreadContainer<QFmessage>
{
public:
    explicit QFprocessor(QThread *parent = nullptr);
    ~QFprocessor();

    void terminate();

    void process(std::vector<QFmessage>& svinput, std::vector<QFmessage>& svoutput);

    // bvs I think this has to be moved into the process thread
    // so we can spawn the python script from there as well
    //PythonProcess mPythonSeleniumProcess;
    std::unique_ptr<PythonProcess> mPythonSeleniumProcessPtr;

};

#endif // QFPROCESSOR_H
