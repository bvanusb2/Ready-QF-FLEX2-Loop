#ifndef PYTHONPROCESS_H
#define PYTHONPROCESS_H

#include "memory"
#include "QProcess"
#include "QString"


class PythonProcess
{
public:
    PythonProcess();
    ~PythonProcess();


    std::shared_ptr<QProcess> mProcessPtr;

    bool spawnProcess(QStringList pyScriptNameAndParams);
    void sendToProcess(QString msgInStr, QString &msgOutStr);

    inline static const QString SeleniumQFCantFindElementString = "Can't find element\n";

private:
    bool init = false;

    // The Python Selenium QF script will print this string at
    // startup.
    inline static const QString SeleniumQFInterfaceReadyString = "SeleniumQFInterfaceReady";

    // Critical! Must be longer than QFseleniumInterface.py timeout, or
    // it returns "" while py script is still waiting for response!!!
    const int PROCESS_WAIT_READY_READ_TIMEOUT = 5000;
};

class PythonOneShot
{
public:
    PythonOneShot();
    ~PythonOneShot();

    void request(const QStringList& argsIn, std::string& msgOutStr, QString& stderr);

private:

};


#endif // PYTHONPROCESS_H
