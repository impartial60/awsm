#include "Drive125.h"

#define main_clock_works 40
#define time_outs 15

#define grad_to_radians(n) (M_PI/180)*n
#define radians_to_grad(n) (180/M_PI)*n


Drive125::Drive125()
{
    ip_apu.setAddress("10.5.0.100");//
    ip_unv.setAddress("10.5.0.101");//
    ip_p1 .setAddress("10.5.0.102");//
    ip_p2 .setAddress("10.5.0.103");//
    ip_p3 .setAddress("10.5.0.104");//
    ip_p4 .setAddress("10.5.0.105");//

    port_dev  = 50000;

     udpSocket = new QUdpSocket();

     udpSocket->bind(ip_apu,port_dev/*QAbstractSocket::DontShareAddress*/);
    rt = new ClockRealTime();

     mono  = new QTimer();
     mono->setSingleShot(true);


    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(slot_udpSocket()));
    connect(mono,SIGNAL(timeout()),this,SLOT(slot_timeout()));

    mono->start(main_clock_works);



}

Drive125::~Drive125()
{
    delete rt;
    delete mono;
    delete udpSocket;
}




 void Drive125::slot_target_motion()
 {

      apu_unv.ID_packet = cnt++;//1234567890;

     udpSocket->writeDatagram((char*)&this->apu_unv,sizeof(apu_unv), ip_unv ,port_dev);
 //    qDebug()<<"servoDriveExecuter_Drive125_send";

 }

 void Drive125::slot_udpSocket()
 {
     int lendg;
     while (udpSocket->hasPendingDatagrams()) {

     lendg=  udpSocket->readDatagram((char*)&this->unv,sizeof(unv),&hb_device_r[0],&port_recv[0]);

       mono->start(main_clock_works+time_outs);

   //    qDebug()<<"servoDriveExecuter_Drive125:" << lendg;

     }

 }

void Drive125::slot_timeout()
{

}
