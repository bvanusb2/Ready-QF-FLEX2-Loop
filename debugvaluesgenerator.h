#ifndef DEBUGVALUESGENERATOR_H
#define DEBUGVALUESGENERATOR_H




class DebugValuesGenerator
{
public:
    enum DebugValuesGeneratorType {
        LacticAcid,
        pH,
        Glucose
    };

    DebugValuesGenerator(DebugValuesGeneratorType _valueType);

    void setStartValue(double value);
    void getValueAndDate(double &value, double &date);


private:
    double randLacticAcid();
    double randpH();

    DebugValuesGeneratorType mDebugValuesGeneratorType;
    double mStartValue;
    double mStartDate;  //seconds since epoch
};

#endif // DEBUGVALUESGENERATOR_H
