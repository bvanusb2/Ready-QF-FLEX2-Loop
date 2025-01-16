#include <random>
#include "QDateTime"
#include "debugvaluesgenerator.h"


DebugValuesGenerator::DebugValuesGenerator(DebugValuesGeneratorType _valueType) : mDebugValuesGeneratorType(_valueType)
{

    if(mDebugValuesGeneratorType == DebugValuesGeneratorType::pH)
        mStartValue = 7.0;
    else if (mDebugValuesGeneratorType == DebugValuesGeneratorType::Glucose)
        mStartValue = 0.0;
    else
        mStartValue = 0.0;

    mStartDate = QDateTime::currentDateTime().toSecsSinceEpoch();

}

void DebugValuesGenerator::setStartValue(double value) {
    mStartValue = value;
}

void DebugValuesGenerator::getValueAndDate(double& value, double& date) {

    if(mDebugValuesGeneratorType == DebugValuesGeneratorType::pH)
        value = randpH();
    else if (mDebugValuesGeneratorType == DebugValuesGeneratorType::LacticAcid)
        value = randLacticAcid();
    else
        mStartValue = 0.0; // todo glucose?


    // increment by four hours 3600 sec/hr * 4hrs
    date = mStartDate;
    mStartDate += 3600 * 4;
}

double DebugValuesGenerator::randLacticAcid() {
    std::random_device r;

    std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()};
    std::mt19937 e2(seed2);
    double mean = 0.0;
    std::normal_distribution<> normal_dist(mean, 0.1);

    double val = (normal_dist(e2) + 0.5);  // why add 0.5 here when you can set mean?
    if (val < 0.0)
        val = 0.0;

    return val;

}

double DebugValuesGenerator::randpH() {
    std::random_device r;

    std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()};
    std::mt19937 e2(seed2);
    double mean = 7.0;
    double var = 0.02;
    std::normal_distribution<> normal_dist(mean, var);

    double val = (normal_dist(e2) + 0.5);
    if (val < 0.0)
        val = 0.0;

    return val;

}
