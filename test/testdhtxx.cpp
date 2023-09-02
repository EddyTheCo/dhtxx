#include"dhtxx.hpp"


#include <QCoreApplication>

#include<QTimer>

int main(int argc, char** argv)
{

	qDebug()<<"Application Started";
    auto a=QCoreApplication(argc, argv);

    quint8 gpio=4,secs=3;
    if(argc>1)gpio=atoi(argv[1]);
    if(argc>2)secs=atoi(argv[2]);
    
    QTimer *timer = new QTimer(&a);
    auto sensor=dhtxx(gpio,&a);

    QObject::connect(&sensor, &dhtxx::got_reading,&a, [&sensor](){
        qDebug()<<"Temperature:"<<sensor.temp();
        qDebug()<<"Humidity:"<<sensor.humi();
    });
    QObject::connect(timer, &QTimer::timeout, &sensor,[&sensor]()
		    {
	qDebug()<<"timer execute";
		    sensor.read();
		    });
    timer->start(secs*1000);

    return a.exec();
}
