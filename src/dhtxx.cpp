#include"dhtxx.hpp"
#include <unistd.h>
#include<QDebug>


#include"lgpio.h"
#define DHTAUTO 0
#define DHT11   1
#define DHTXX   2

#define DHT_GOOD         0
#define DHT_BAD_CHECKSUM 1
#define DHT_BAD_DATA     2
#define DHT_TIMEOUT      3

dhtxx::dhtxx(quint8 gpio_number,QObject *parent):QObject(parent),temperature(0),humidity(0),m_gpio_number(gpio_number),chip(0),mstate(Ready)
{
    init();
};
int decode_dhtxx(uint64_t reading, int model, float *rh, float *temp)
{
    /*
      +-------+-------+
      | DHT11 | DHTXX |
      +-------+-------+
Temp C| 0-50  |-40-125|
      +-------+-------+
RH%   | 20-80 | 0-100 |
      +-------+-------+

         0      1      2      3      4
      +------+------+------+------+------+
DHT11 |check-| 0    | temp |  0   | RH%  |
      |sum   |      |      |      |      |
      +------+------+------+------+------+
DHT21 |check-| temp | temp | RH%  | RH%  |
DHT22 |sum   | LSB  | MSB  | LSB  | MSB  |
DHT33 |      |      |      |      |      |
DHT44 |      |      |      |      |      |
      +------+------+------+------+------+

*/
    uint8_t byte[5];
    uint8_t chksum;
    float div;
    float t, h;
    int valid;
    int status;

    byte[0] = (reading    ) & 255;
    byte[1] = (reading>> 8) & 255;
    byte[2] = (reading>>16) & 255;
    byte[3] = (reading>>24) & 255;
    byte[4] = (reading>>32) & 255;

    chksum = (byte[1] + byte[2] + byte[3] + byte[4]) & 0xFF;

    valid = 0;
    qDebug()<<"chksum :"<<chksum;
    qDebug()<<"byte[0] :"<<byte[0];
    qDebug()<<"byte[1] :"<<byte[1];
    qDebug()<<"byte[2] :"<<byte[2];
    qDebug()<<"byte[3] :"<<byte[3];
    qDebug()<<"byte[4] :"<<byte[4];
    if (chksum == byte[0])
    {
        qDebug()<<"chksum == byte[0]";
        if (model == DHT11)
        {
            qDebug()<<"model == DHT11";
            if ((byte[1] == 0) && (byte[3] == 0))
            {
                qDebug()<<"byte[1] == 0) && (byte[3] == 0";
                valid = 1;

                t = byte[2];

                if (t > 60.0) valid = 0;

                h = byte[4];

                if ((h < 10.0) || (h > 90.0)) valid = 0;
            }
        }
        else if (model == DHTXX)
        {
            qDebug()<<"model == DHTXX";
            valid = 1;

            h = ((float)((byte[4]<<8) + byte[3]))/10.0;

            if (h > 110.0) valid = 0;

            if (byte[2] & 128) div = -10.0; else div = 10.0;

            t = ((float)(((byte[2]&127)<<8) + byte[1])) / div;

            if ((t < -50.0) || (t > 135.0)) valid = 0;
        }
        else /* AUTO */
        {
            qDebug()<<"AUTO";
            valid = 1;

            /* Try DHTXX first. */

            h = ((float)((byte[4]<<8) + byte[3]))/10.0;

            if (h > 110.0) valid = 0;

            if (byte[2] & 128) div = -10.0; else div = 10.0;

            t = ((float)(((byte[2]&127)<<8) + byte[1])) / div;

            if ((t < -50.0) || (t > 135.0)) valid = 0;

            if (!valid)
            {
                qDebug()<<"!valid";
                /* If not DHTXX try DHT11. */

                if ((byte[1] == 0) && (byte[3] == 0))
                {
                    qDebug()<<"If not DHTXX try DHT11.";
                    valid = 1;

                    t = byte[2];

                    if (t > 60.0) valid = 0;

                    h = byte[4];

                    if ((h < 10.0) || (h > 90.0)) valid = 0;
                }
            }
        }

        if (valid)
        {
            qDebug()<<"valid";
            status = DHT_GOOD;

            *rh = h;
            *temp = t;
        }
        else
        {
            qDebug()<<"no valid";
            status = DHT_BAD_DATA;
        }
    }
    else status = DHT_BAD_CHECKSUM;

    return status;
}

void afunc(int e, lgGpioAlert_p evt, void *data)
{
    int i;
    uint64_t edge_len, now_tick;
    static int bits = 0;
    static uint64_t reading = 0;
    static uint64_t last_tick = 0;

    for (i=0; i<e; i++)
    {
        if (bits<40)
        {

            now_tick = evt[i].report.timestamp;
            edge_len = now_tick - last_tick;
            last_tick = now_tick;

            reading <<= 1;
            if (edge_len > 1e5) reading |= 1; // longer than 100 micros
            ++bits;

        }
        else
        {
            float t,h;
            qDebug()<<"ready to decode:"<<reading;
            if(decode_dhtxx(reading, DHTAUTO, &t, &h)==DHT_GOOD)
            {

                qDebug()<<"decode:"<<t<<" "<<h;

                static_cast<dhtxx*>(data)->setValues(t,h);
            }
            reading = 0;
            bits = 0;
            return;
        }
    }
}

void dhtxx::init(void)
{
    chip = lgGpiochipOpen(0);
    lgGpioSetUser(chip, "niagra");
    lgGpioSetSamplesFunc(afunc, (void*)this);
}
void dhtxx::read()
{
    if(mstate)
    {
        mstate=Null;
        qDebug()<<"dhtxx::read";
        auto err = lgGpioClaimOutput(chip, 0, m_gpio_number, 0);
        if (err) qDebug()<<"Set out err"<<err;

        err = lgGpioClaimAlert(
                    chip, 0, LG_RISING_EDGE, m_gpio_number, -1);
        if (err) qDebug()<<"set event err"<< err;
    }

}
