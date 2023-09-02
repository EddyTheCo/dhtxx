#pragma once

#include<cstdlib>
#include<QObject>
using namespace std;
class dhtxx : public QObject
{
    Q_OBJECT
public:
    dhtxx(quint8 gpio_number=4,QObject *parent=nullptr);
    void read();
    void setValues(const float& t,const float& h)
    {
        temperature=t;
        humidity=h;
        emit got_reading();
    };
    float temp()const{return temperature;}
    float humi()const{return humidity;}
signals:
    void got_reading();
private:
    void init();
    float temperature,humidity;
    quint8 m_gpio_number;
    int chip;
};
