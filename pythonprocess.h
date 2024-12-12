#ifndef PYTHONPROCESS_H
#define PYTHONPROCESS_H

#include "QProcess"
#include "QString"

class PythonProcess
{
public:
    PythonProcess();
    ~PythonProcess();

    void terminate();
    void terminateProcessRequest();
    //void request(const QStringList& argsIn, std::string& msgOutStr, QString& stderr);

    QProcess mProcess;

    bool spawnProcess(QStringList pyScriptNameAndParams);
    void sendToProcess(QString msgInStr, QString &msgOutStr);

private:
    bool init = false;

    // The Python Selenium QF script will print this string at
    // startup.
    inline static const QString SeleniumQFInterfaceReadyString = "SeleniumQFInterfaceReady";
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
