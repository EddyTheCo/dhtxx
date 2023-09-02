#include"dhtxx.hpp"


#include <QCoreApplication>

#include<QTimer>

int main(int argc, char** argv)
{

    auto a=QCoreApplication(argc, argv);

    QTimer *timer = new QTimer(&a);
    auto sensor=dhtxx(4,&a);

    QObject::connect(&sensor, &dhtxx::got_reading,&a, [&sensor](){
        qDebug()<<"Temperature:"<<sensor.temp();
        qDebug()<<"Humidity:"<<sensor.humi();
    });
    QObject::connect(timer, &QTimer::timeout, &sensor, &dhtxx::read);
    timer->start(1000);

    return a.exec();
}
