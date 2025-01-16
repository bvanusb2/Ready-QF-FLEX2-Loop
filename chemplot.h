#ifndef CHEMPLOT_H
#define CHEMPLOT_H

#include "qcustomplot.h"

class ChemPlot
{
public:
    ChemPlot();
    void addDataPoint(double timePt, double dataPt);
    void setPlot(QCustomPlot *_plotWidget, QColor color, QString analyteLabel, double maxAnalyteVal, size_t numDatapointsInPlot);
    void setAnalyteLabel(QString label) {
        mAnalyteLabel = label;
    }
    void setMaxAnalyteValue(double maxVal) {
        mMaxAnalyteValue = maxVal;
    }

private:
    QCustomPlot * mPlotWidget;

    // weird, how QVector::size() returns "int" and not size_t
    QVector<QCPGraphData> mTimeData;

    static const int mDefaultNumDatapointsInPlot = 20;
    int mNumDatapointsInPlot; // e.g 4 meas per day * 5 days.
    const double mNumSecondsInDay = (24.0 * 3600.0);

    QString mAnalyteLabel;
    double mMaxAnalyteValue = 1.0;

    // todo not sure I want this
    double mStartXPlotTime;

};

#endif // CHEMPLOT_H
