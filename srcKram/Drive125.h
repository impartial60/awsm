#ifndef Drive125_H
#define Drive125_H
#include <QTimer>
#include <QUdpSocket>
#include <QHostAddress>
#include "clockrealtime.h"

#include <QObject>


#define START_PACK _Pragma ("pack(push, 1)")
#define END_PACK   _Pragma ("pack(pop)")

#define N_ID_enc_new        0x3F // (63) 	При наладке адрес энкодера по умолчанию
#define N_ID_incl           0x3E // (62) 	Адрес инклинометра (при запуске нового и в процессе работы одинаковый)
#define N_ID_enc_azimuth    0x3D // (61)	Энкодер азимута.
#define N_ID_enc_elv        0x3C // (60) 	Энкодер угла места
#define N_ID_lenze_az       0x3B // (59) 	Привод азимута.
#define N_ID_lenze_elv      0x3A // (58)	Привод угла места.
#define N_ID_PLC            0x39 // (57)	PLC Schneider.


class Drive125 : public QObject
{
    Q_OBJECT

public:

    explicit  Drive125();
    ~Drive125();

    inline int32_t double_to_lenze(double n){return ((int)(n*10000.0));}
    inline double lenze_to_double(int32_t n){return (double(n)/10000.0);}

    void slot_target_motion();
    void get_realtime() {time_diff = rt->elapsed();rt->start();}
//---------------------------------------------

    inline double unv_getpos_az()  {return (lenze_to_double(unv.enc_angle_pos_az));}
    inline double unv_getpos_elv() {return(lenze_to_double (unv.enc_angle_pos_elv));}

    inline void unv_setpos_az (double pos) {apu_unv.angle_pos_az = double_to_lenze(pos);}
    inline void unv_setpos_elv(double pos) {apu_unv.angle_pos_elv= double_to_lenze(pos);}

    inline void unv_drive_az_on() {apu_unv.az_on = 1;}
    inline void unv_drive_az_off() {apu_unv.az_on = 0;}

    inline void unv_drive_elv_on() {apu_unv.elv_on = 1;}
    inline void unv_drive_elv_off() {apu_unv.elv_on = 0;}

    inline void unv_drive_az_en() {apu_unv.az_en = 1;}
    inline void unv_drive_az_dis() {apu_unv.az_en = 0;}

    inline void unv_drive_elv_en() {apu_unv.elv_en = 1;}
    inline void unv_drive_elv_dis() {apu_unv.elv_en = 0;}
        double time_diff;
//---------------------------------------------



//Структура данных обмена — АПУ->УНВ  и пусковых
private:
START_PACK
struct apu_slave
{
    const uint32_t header = 0xaa55aa55;
/*4 */uint32_t ID_packet;
/*8 */int32_t angle_pos_az;               //DINT 	(32) 		 Угловая координата азимута.
/*12*/int32_t angle_pos_elv ;            //DINT 	(32) 		 Угловая координата угла места.
/*16*/int32_t angle_speed_az ;           //DINT 	(32) 		 Угловая скорость по азимуту.
/*20*/int32_t angle_speed_elv ;          //DINT 	(32) 		 Угловая скорость по углу места.
/*24*/uint32_t send_SDO1;                 //UDINT[2] (64)	 	 SDO На передачу.
/*28*/uint32_t send_SDO2;                 //UDINT[2] (64)	 	 SDO На передачу.
/*30*/uint16_t code_az;       //WORD (16)		Номер кода  в запросе.(азимут).(Номер кода внутри блока управления приводом)
/*32*/uint16_t subcode_az;    //WORD (16)		Номер субкода  в запросе.(азимут) (Номер кода внутри блока управления приводом)
/*34*/uint16_t code_elv;      //WORD (16)		Номер кода  в запросе.(угол места).(Номер кода внутри блока управления приводом)
/*36*/uint16_t subcode_elv;   //WORD (16)		Номер субкода  в запросе.(угол места) (Номер кода внутри блока управления приводом)
/*40*/uint32_t data_az;       //UNION(32)		Данные для записи.(угол места)
/*44*/uint32_t data_elv;      //UNION(32)		Данные для записи.(азимут)
/*48*/uint32_t                //DWORD (32) 		Управление приводом
    az_on:1,//бит 0 		Включение привода азимута
    elv_on:1,//бит 1 		Включение привода угла места
    az_en:1,//бит 2 		Разрешение работы интегратора азимута
    elv_en:1,//бит 3 		Разрешение работы интегратора угла места
    az_int_clear:1,//бит 4 		Сброс интегратора азимута
    elv_int_clear:1,//бит 5 		Сброс интегратора угла места
    az_pos_speed_controll:1,//бит 6 		Управление по позиции — 0 по скорости 1 (азимут)
    elv_pos_speed_controll:1,//бит 7 		Управление по позиции — 0 по скорости 1 (угол места)
    az_error_clear:1,//бит 8 		Сброс ошибки привода азимута
    elv_error_clear:1,//бит 9		Сброс ошибки привода угла места
    CAN_SDO_ADR:3,//биты 10-12 	Адрес устройства на CAN шине при обмене SDO .
    az_code_write:1,//Бит 13	 	Признак записи для кодов (азимут).
    elv_code_write:1,//Бит 14	 	Признак записи для кодов (угол места).
    not_used:17;        //bit 15-32


};
END_PACK
//Структура данных обмена —УНВ  и пусковые →АПУ
START_PACK
struct slave_apu
{
uint32_t header;// = 0x55aa55aa;
uint32_t ID_packet;
int32_t enc_angle_pos_az; //DINT 	(32) 		 Угловая координата азимута. (по энкодеру)
int32_t enc_angle_pos_elv;//DINT 	(32) 		 Угловая координата угла места.(по энкодеру)
int32_t enc_angle_speed_az ;//DINT 	(32) 		 Угловая скорость по азимуту.(по энкодеру)
int32_t enc_angle_speed_elv ; //DINT 	(32) 		 Угловая скорость по углу места.(по энкодеру)
uint32_t motor_encoder_az;     //DWORD (32) Энкодер двигателя азимута
uint32_t motor_encoder_elv;    //DWORD (32) Энкодер двигателя угла места

uint64_t receive_SDO;           //UDINT[2] (64)		 SDO На прием.
uint16_t code_az;       //WORD (16)		Номер кода  в ответе.(азимут).(Номер кода внутри блока управления приводом)
uint16_t subcode_az;             //WORD (16)		Номер субкода  в ответе.(азимут) (Номер кода внутри блока управления приводом)
uint16_t code_elv;      //WORD (16)		Номер кода  в ответе.(угол места).(Номер кода внутри блока управления приводом)
uint16_t subcode_elv;   //WORD (16)		Номер субкода  в ответе (угол места) (Номер кода внутри блока управления приводом)
uint32_t data_az;       //UNION(32)		Данные по коду.(азимут)
uint32_t data_elv;      //UNION(32)		Данные по коду.(угол места)
uint32_t az_errors;     //UDINT (32) 		Код ошибки привода azimuth
uint32_t elv_errors;     //UDINT (32) 		Код ошибки привода elv

uint32_t            //UDINT (32) 		Состояние привода.
    az_error:1,     //бит 0 		Ошибка привода азимута
    elv_error:1,    //бит 1 		Ошибка привода угла места
    rez1:6,         //биты 2-7		резерв
    CAN_SDO_RD:3,   //биты 8-10 	Адрес устройства на CAN шине при обмене SDO .
    rez2:21;        //биты 11-31 	резерв.

};
END_PACK
apu_slave apu_unv,apu_p1,apu_p2,apu_p3,apu_p4;
slave_apu unv,p1,p2,p3,p4;
int cnt=0;

public slots:




    void slot_udpSocket();
    void slot_timeout();
private:
    ClockRealTime *rt;

    QTimer      *mono;
    QUdpSocket   *udpSocket;
    QHostAddress hb_device_r[5];

    quint16      port_dev;
    quint16      port_recv[5];
    //IP Адреса устройств Enhernet. (Порт 50000)

    QHostAddress ip_apu;// = QHostAddress(QString("10.5.0.100"));// 	 АПУ -
    QHostAddress ip_unv;// = QHostAddress(QString("10.5.0.101"));// 	 УНВ -
    QHostAddress ip_p1 ;// = QHostAddress(QString("10.5.0.102"));// 	 Пусковая 1.
    QHostAddress ip_p2 ;// = QHostAddress(QString("10.5.0.103"));// 	 Пусковая 2.
    QHostAddress ip_p3 ;// = QHostAddress(QString("10.5.0.104"));// 	 Пусковая 3.
    QHostAddress ip_p4 ;// = QHostAddress(QString("10.5.0.105"));// 	 Пусковая 4.

};
#endif // Drive125_H
