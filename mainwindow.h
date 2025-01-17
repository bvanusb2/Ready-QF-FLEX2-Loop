#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "FLEX2processor.h"
#include "QFprocessor.h"
#include "chemplot.h"

// for "can't find element" string
// #include "pythonprocess.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void mQueryResponseTimerSlot();
    void mProcessQfMessages();
    void mProcessFlex2Messages();
    void on_pushButtonGetLacticAcidFlex_clicked();

    void on_pushButtonGetPhFlex_clicked();

    void on_pushButtonConnToSelQf_clicked();

    void on_pushButtonQueryQFflow_clicked();

    void on_pushButtonSaveWidgetAsImage_clicked();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<FLEX2processor> mFlex2processorPtr = nullptr;
    std::unique_ptr<QFprocessor> mQFprocessorPtr = nullptr;

    void connectedToQF();
    void disconnectedFromQF();

    // For now, just using a simple timer to check for responses.
    // Might add another timer to periodically query FLEX2 and Quautum devices
    QTimer * mQueryResponseTimer;
    int mQueryResponseTimerInterval = 100; // mS
    bool mConnectedQF = false;
    std::string mPrevQFsystemTime;

    // Plots!!!
    ChemPlot lactosePlot;
    ChemPlot pHPlot;


};
#endif // MAINWINDOW_H
