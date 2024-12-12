#ifndef THREADCONTAINER_H
#define THREADCONTAINER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <complex>

#include <vector>
#include <queue>
#include <memory>
#include <utility> // for std::move?



template <class T, class U = T>
class ThreadContainer : public QThread
{
public:

    ThreadContainer(QObject *parent = nullptr) {};

    ~ThreadContainer() override {
        stopThread();
    };

    void startThread()
    {
        runThread = true;
        threadIsDone = false;

        start(QThread::NormalPriority);
    }

    void stopThread()
    {
        runThread = false;

        // Could be the thread hasn't exited yet, so sleep a bit.  Don't let
        // the main thread exit until this thread is done.
        if (isRunning()) {

            // wake the thread up
            mQWaitBufferReady.notify_all();

            while(!threadIsDone) {
                QThread::usleep(2);
            }
        }
    }

    //
    // Function name: pushSampleData
    //
    // Purpose: Pushes data into a vector for the timing thread to analyze
    //
    void pushSampleData(std::vector<T>& samples) {

        if (samples.size() == 0)
            return;

        inputVecPtr = std::make_unique<std::vector<T>>();
        *inputVecPtr = samples;

        {
            const QMutexLocker locker(&mMutexQueueInput);
            queueBufferInputPtrs.push(std::move(inputVecPtr));
        }
        mQWaitBufferReady.notify_all();

    }

    bool pullDataReady() {
        return !queueBufferOutputPtrs.empty();
    }

    void pullProcessedData(std::vector<U>& dataOut) {

        {
            const QMutexLocker locker(&mMutexQueueOutput);
            outputVecPtr = std::move(queueBufferOutputPtrs.front());
            queueBufferOutputPtrs.pop();
        }

        dataOut = *outputVecPtr;
    }


private:

    void run() override
    {
        std::vector<T> inputdata;
        std::vector<U> outputdata;


        std::unique_ptr<std::vector<T>> inputProcVecPtr;
        std::unique_ptr<std::vector<U>> outputProcVecPtr;

        mWaitBufferMutex.lock();


        while(runThread) {

            // I believe the mutex is unlocked once wait is complete
            mQWaitBufferReady.wait(&mWaitBufferMutex);

            if (!runThread) {
                continue;
            }

            while(!queueBufferInputPtrs.empty()) {

                {
                    const QMutexLocker locker(&mMutexQueueInput);

                    // access the first element
                    inputProcVecPtr = std::move(queueBufferInputPtrs.front());

                    // Removes the first element
                    queueBufferInputPtrs.pop();
                }

                inputdata = *inputProcVecPtr; // we could pass the pointer to the processor but that would require proc mods

                outputdata.clear();
                process(inputdata, outputdata);

                outputProcVecPtr = std::make_unique<std::vector<U>>();
                *outputProcVecPtr = outputdata;

                {
                    const QMutexLocker locker(&mMutexQueueOutput);
                    queueBufferOutputPtrs.push(std::move(outputProcVecPtr));
                }

            }
        }

        threadIsDone = true;



    }


    virtual void process(std::vector<T> &input, std::vector<U>& output) = 0;

    std::atomic<bool> threadIsDone = false;
    std::atomic<bool> runThread = true;

    std::unique_ptr<std::vector<T>> inputVecPtr;
    std::unique_ptr<std::vector<U>> outputVecPtr;

    std::queue<std::unique_ptr<std::vector<T>>> queueBufferInputPtrs;
    std::queue<std::unique_ptr<std::vector<U>>> queueBufferOutputPtrs;

    size_t debugPoint = 0;

    QMutex mWaitBufferMutex;

    QMutex mMutexQueueInput;
    QMutex mMutexQueueOutput;

    QWaitCondition mQWaitBufferReady;
};

#endif // THREADCONTAINER_H
