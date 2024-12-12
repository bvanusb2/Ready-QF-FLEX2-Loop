#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "FLEX2processor.h"
#include "QFprocessor.h"

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

    void mQueryResponse();
    void on_pushButtonGetLacticAcidFlex_clicked();

    void on_pushButtonGetPhFlex_clicked();

    void on_pushButtonConnToSelQf_clicked();

    void on_pushButtonQueryQFflow_clicked();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<FLEX2processor> mOpcThreadPtr = nullptr;
    std::unique_ptr<QFprocessor> mQFThreadPtr = nullptr;

    // For now, just using a simple timer to check for responses.
    // Might add another timer to periodically query FLEX2 and Quautum devices
    QTimer * mQueryResponseTimer;
    int mQueryResponseTimerInterval = 200;

};
#endif // MAINWINDOW_H
