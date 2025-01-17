#ifndef DEBUGVALUESGENERATOR_H
#define DEBUGVALUESGENERATOR_H




class DebugValuesGenerator
{
public:
//    enum DebugValuesGeneratorType {
//        LacticAcid,
//        pH,
//        Glucose
//    };

    DebugValuesGenerator(double mean, double variance); //DebugValuesGeneratorType _valueType);

    void setStartValue(double value);
    void getValueAndDate(double &value, double &date);


private:
    double rand();

    double mMean = 0.0;
    double mVar = 1.0;


    double mStartValue;
    double mStartDate;  //seconds since epoch
};

#endif // DEBUGVALUESGENERATOR_H
