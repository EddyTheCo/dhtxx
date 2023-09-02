#include"dhtxx.hpp"


#include <QCoreApplication>

#include<QTimer>

int main(int argc, char** argv)
{

    auto a=QCoreApplication(argc, argv);

    quint8 gpio=4;
    if(argc>1)gpio=atoi(argv[1]);    
    
    QTimer *timer = new QTimer(&a);
    auto sensor=dhtxx(gpio,&a);

    QObject::connect(&sensor, &dhtxx::got_reading,&a, [&sensor](){
        qDebug()<<"Temperature:"<<sensor.temp();
        qDebug()<<"Humidity:"<<sensor.humi();
    });
    QObject::connect(timer, &QTimer::timeout, &sensor, &dhtxx::read);
    timer->start(3000);

    return a.exec();
}
