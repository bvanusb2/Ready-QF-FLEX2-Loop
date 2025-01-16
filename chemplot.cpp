#include <cmath>
#include "chemplot.h"


ChemPlot::ChemPlot() {

}
//
// Function name: setPlot
//
// Purpose: Sets plot parameters and pointer to plot in UI.
//
void ChemPlot::setPlot(
        QCustomPlot *_plotWidget,
        QColor color,
        QString analyteLabel,
        double maxAnalyteVal = 1.0,
        size_t numDatapointsInPlot = mDefaultNumDatapointsInPlot)
{
    mPlotWidget = _plotWidget;
    mAnalyteLabel = analyteLabel;
    mMaxAnalyteValue = maxAnalyteVal;
    mNumDatapointsInPlot = numDatapointsInPlot;

    // set locale to english, so we get US month names:
    mPlotWidget->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

    mPlotWidget->addGraph();


    // red, green, blue, alpha
    //QColor color(0,200, 50, 250);
    //QColor color("Blue");

    //plotWidget->graph()->setLineStyle(QCPGraph::lsLine);
    mPlotWidget->graph()->setScatterStyle(QCPScatterStyle::ssSquare);
    mPlotWidget->graph()->setPen(QPen(color));

    // Setting 'brush' fills below the line graph with same color
    //plotWidget->graph()->setBrush(QBrush(color));


    // seconds of current time, we'll use it as starting point in time for data:
    //double now = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
    double now = QDateTime::currentDateTime().toSecsSinceEpoch();

    // configure bottom axis to show date instead of number:
    // why do we need to add an hour to the start time for the plot to look "ok"?
    mStartXPlotTime = std::floor(now / mNumSecondsInDay) * mNumSecondsInDay;
    double endXPlotTime = mStartXPlotTime + 5 * 24 * 3600;

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("MM/dd/yy\nhh:mm");
    dateTicker->setTickCount(2);

    // The graph software picks a horrible internal offset of 7 hrs for plotting time
    // This means the tick labels are always at a time of "15:00".  This offset fixes it
    dateTicker->setTickOrigin(7*3600);

    mPlotWidget->xAxis->setTicker(dateTicker);

   //mPlotWidget->xAxis->setTickLabelRotation(45);

//    mPlotWidget->xAxis->setRange(now - mNumSecondsInDay, now + mNumSecondsInDay * numDaysToPlot);
    mPlotWidget->xAxis->setRange(mStartXPlotTime, endXPlotTime);

    // configure left axis text labels:
    mPlotWidget->yAxis->setLabel(mAnalyteLabel);

    //QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setTickStep(0.2);
    mPlotWidget->yAxis->setTicker(fixedTicker); // was textTicker

    // Y-axis - analyte description
    mPlotWidget->yAxis->setTicks(true);
    mPlotWidget->yAxis->setRange(0, mMaxAnalyteValue);

    //textTicker->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssMeetTickCount);

    // set a more compact font size for bottom and left axis tick labels:
    mPlotWidget->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    mPlotWidget->yAxis->setTickLabelFont(QFont(QFont().family(), 8));



    // make top and right axes visible but without ticks and labels:
    mPlotWidget->xAxis2->setVisible(true);
    mPlotWidget->yAxis2->setVisible(true);

    mPlotWidget->xAxis2->setTicks(false);
    mPlotWidget->yAxis2->setTicks(false);

    mPlotWidget->xAxis2->setTickLabels(false);
    mPlotWidget->yAxis2->setTickLabels(false);


    // If we're only showing one analyte per graph, then no legend
    // show legend with slightly transparent background brush:
//    ui->plotWidget->legend->setVisible(true);
//    ui->plotWidget->legend->setBrush(QColor(255, 255, 255, 150));

    // Show the plot with correct labels
    mPlotWidget->replot();

}

void ChemPlot::addDataPoint(double timePt, double dataPt)
{
    QCPGraphData dataPoint;

    dataPoint.key = timePt;
    dataPoint.value = dataPt;

    // The plot window scrolls the data.  The time data vector has a
    // specific size, so when full, the oldest point is pushed out and a
    // new point is put in at the end
    if (mTimeData.size() < mNumDatapointsInPlot) {
        mTimeData.push_back(dataPoint);
    } else {
        for (size_t i = 0; i < mNumDatapointsInPlot - 1; i++) {
            mTimeData[i] = mTimeData[i + 1];
        }
        mTimeData[mNumDatapointsInPlot - 1] = dataPoint;
    }


    mPlotWidget->graph()->data()->set(mTimeData);

    // start at midnight the first day of the first sample, and end at midnight
    // of the day of the last sample
    mStartXPlotTime = std::floor(mTimeData[0].key / mNumSecondsInDay) * mNumSecondsInDay + 3600;
    double endXPlotTime = std::ceil(mTimeData.back().key / mNumSecondsInDay) * mNumSecondsInDay + 3600;

    mPlotWidget->xAxis->setRange(mStartXPlotTime, endXPlotTime);


    mPlotWidget->replot();

}
