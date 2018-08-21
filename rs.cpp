#include "externM.h"

void rsTrgtPrmsCalculator()
{
    int indCY = apyInfoM.targetIndexCY;
    if(indCY < 0 || indCY >= MAXTOCKANUM)
        return;

    if(apyInfoM.rejimSoprovojdeniaSNR == SOPROVOJDENIE_AS)  //  2018-06-16    if(srvDrvInfoM.rejimSoprovojdeniaSNR == SOPROVOJDENIE_AS)
        memcpy(&apyInfoM.nextTockaRS[0], &apyInfoM.nextTocka[indCY], sizeof(TargetParams));
    else
    {
//        qDebug()<<"rsTrgtPrmsCalculator groundRange_km:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].groundRange_km
//                                                        <<apyInfoM.nextTockaRS[0].groundRange_km;

        if(fabs(apyInfoM.nextTocka[indCY].groundRange_km - apyInfoM.nextTockaRS[0].groundRange_km) > 0.2 )// ||
           //srvDrvInfoM.currentAzimuth != apyInfoM.nextTockaRS[0].azimuth360)   //  если есть приращение дальности или азимута
        {                                                                                   //  сдивигаем на один шаг историю о цели
            for(int i = MAXTOCKANUM - 1; i > 0; i--)
            {
                memcpy(&apyInfoM.nextTockaRS[i], &apyInfoM.nextTockaRS[i - 1], sizeof(TargetParams));
            }

            apyInfoM.nextTockaRS[0].targetNo = apyInfoM.nextTocka[indCY].targetNo;
            apyInfoM.nextTockaRS[0].slantRange_km = apyInfoM.nextTocka[indCY].slantRange_km;

            apyInfoM.nextTockaRS[0].azimuth360 = srvDrvInfoM.currentAzimuth;   //apyInfoM.nextTocka[indCY].azimuth360;
            apyInfoM.nextTockaRS[0].elevationAngle = srvDrvInfoM.currentDirectrisaElevationAngle;  //apyInfoM.nextTocka[indCY].elevationAngle;
            apyInfoM.nextTockaRS[0].groundRange_km = apyInfoM.nextTocka[indCY].slantRange_km * cos(apyInfoM.nextTockaRS[0].elevationAngle / Rad2Grad);

            //        qDebug()<<"L blinckingTimerTick  nextTochkaL.rangeParametr_km:"<<nextTochkaL.rangeParametr_km<<shMemC2U.commander2Launcher.srvDrvInfo.nextTockaRS[0].rangeParametr_km
            //                <<"nextTochkaL.kyrsovojParameter_km:"<<nextTochkaL.kyrsovojParameter_km<<shMemC2U.commander2Launcher.srvDrvInfo.nextTockaRS[0].kyrsovojParameter_km;

    //        qDebug()<<"rsTrgtPrmsCalculator nextTocka.azimuth360:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].azimuth360<<apyInfoM.nextTockaRS[0].azimuth360
    //                <<"nextTochka.elevationAngle:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].elevationAngle<<apyInfoM.nextTockaRS[0].elevationAngle;

            float az180 = apyInfoM.nextTockaRS[0].azimuth360;
            if(az180 > 180.0)
                az180 -= 360.0;

            apyInfoM.nextTockaRS[0].groundX = apyInfoM.nextTockaRS[0].groundRange_km * sin(az180 / Rad2Grad);   //apyInfoM.nextTocka[indCY].groundX;
            apyInfoM.nextTockaRS[0].groundY = apyInfoM.nextTockaRS[0].groundRange_km * cos(az180 / Rad2Grad);   //apyInfoM.nextTocka[indCY].groundY;

    //        qDebug()<<"rsTrgtPrmsCalculator nextTocka.groundX:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].groundX<<apyInfoM.nextTockaRS[0].groundX
    //                <<"nextTochka.groundY:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].groundY<<apyInfoM.nextTockaRS[0].groundY;

            float kyrsTemp;

            if(apyInfoM.nextTockaRS[0].azimuth360 != apyInfoM.nextTockaRS[1].azimuth360) // общий случай - курсовой параметр не нулевой
            {
                if(apyInfoM.nextTockaRS[0].groundX != apyInfoM.nextTockaRS[1].groundX || apyInfoM.nextTockaRS[0].groundY != apyInfoM.nextTockaRS[1].groundY)
                    kyrsTemp = atan2(apyInfoM.nextTockaRS[0].groundX - apyInfoM.nextTockaRS[1].groundX,
                                     apyInfoM.nextTockaRS[0].groundY - apyInfoM.nextTockaRS[1].groundY) * Rad2Grad;
                else
                    if(apyInfoM.nextTockaRS[0].groundX != apyInfoM.nextTockaRS[2].groundX || apyInfoM.nextTockaRS[0].groundY != apyInfoM.nextTockaRS[2].groundY)
                        kyrsTemp = atan2(apyInfoM.nextTockaRS[0].groundX - apyInfoM.nextTockaRS[2].groundX,
                                         apyInfoM.nextTockaRS[0].groundY - apyInfoM.nextTockaRS[2].groundY) * Rad2Grad;
                    else
                        if(apyInfoM.nextTockaRS[0].groundX != apyInfoM.nextTockaRS[3].groundX || apyInfoM.nextTockaRS[0].groundY != apyInfoM.nextTockaRS[3].groundY)
                            kyrsTemp = atan2(apyInfoM.nextTockaRS[0].groundX - apyInfoM.nextTockaRS[3].groundX,
                                             apyInfoM.nextTockaRS[0].groundY - apyInfoM.nextTockaRS[3].groundY) * Rad2Grad;


/*2018-02-23            qDebug()<<"rsTrgtPrmsCalculator handWheelFSpeed:"<<srvDrvInfoM.handWheelFSpeed[0]<<"groundX:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].groundX<<apyInfoM.nextTockaRS[0].groundX
                        <<"groundY:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].groundY<<apyInfoM.nextTockaRS[0].groundY
                         // "rejimPoiskaSNR:"<<srvDrvInfoM.rejimPoiskaSNR<<"rejimSoprovojdeniaSNR:"<<srvDrvInfoM.rejimSoprovojdeniaSNR
                        <<"nextTocka.kyrs:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].kyrs<<apyInfoM.nextTockaRS[0].kyrs;
                        //<<"nextTochka.rakyrs:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].rakyrs<<apyInfoM.nextTockaRS[0].rakyrs;
*/
            }
            else    // частный случай - курсовой параметр = 0
            {
                if(apyInfoM.nextTockaRS[0].groundRange_km > apyInfoM.nextTockaRS[1].groundRange_km) // цель удаляется
                    kyrsTemp = apyInfoM.nextTockaRS[0].azimuth360;
                else
                    kyrsTemp = apyInfoM.nextTockaRS[0].azimuth360 - 180.0;//apyInfoM.nextTockaRS[1].kyrs;   //  цель приближается
            }

    //        if(apyInfoM.nextTockaRS[0].groundX != apyInfoM.nextTockaRS[1].groundX || apyInfoM.nextTockaRS[0].groundY != apyInfoM.nextTockaRS[1].groundY)
    //            kyrsTemp = atan2(apyInfoM.nextTockaRS[0].groundX - apyInfoM.nextTockaRS[1].groundX,
    //                             apyInfoM.nextTockaRS[0].groundY - apyInfoM.nextTockaRS[1].groundY) * Rad2Grad;

            if(kyrsTemp < 0)
                kyrsTemp += 360.0;

            apyInfoM.nextTockaRS[0].kyrs = kyrsTemp;
            apyInfoM.nextTockaRS[0].rakyrs = 180.0 - apyInfoM.nextTockaRS[0].kyrs + az180;

//            qDebug()<<"rsTrgtPrmsCalculator handWheelFSpeed:"<<srvDrvInfoM.handWheelFSpeed[0]<<"groundX:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].groundX<<apyInfoM.nextTockaRS[0].groundX
//                    <<"groundY:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].groundY<<apyInfoM.nextTockaRS[0].groundY
//                     // "rejimPoiskaSNR:"<<srvDrvInfoM.rejimPoiskaSNR<<"rejimSoprovojdeniaSNR:"<<srvDrvInfoM.rejimSoprovojdeniaSNR
//                    <<"nextTocka.kyrs:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].kyrs<<apyInfoM.nextTockaRS[0].kyrs;
//                    //<<"nextTochka.rakyrs:"<<apyInfoM.nextTocka[srvDrvInfoM.simulatorTargetIndexCY].rakyrs<<apyInfoM.nextTockaRS[0].rakyrs;

            apyInfoM.nextTockaRS[0].radialSpeed_ms = apyInfoM.nextTocka[indCY].radialSpeed_ms;
            float cosRakyrs = cos(apyInfoM.nextTockaRS[0].rakyrs / Rad2Grad);

            if(cosRakyrs != 0)
                apyInfoM.nextTockaRS[0].groundSpeed_ms = apyInfoM.nextTockaRS[0].radialSpeed_ms / cosRakyrs;
            else
                apyInfoM.nextTockaRS[0].groundSpeed_ms = apyInfoM.nextTockaRS[0].radialSpeed_ms;

            // Т Е К У Щ И Й   К У Р С О В О Й   П А Р А М Е Т Р
            float deltaPrm = apyInfoM.nextTockaRS[0].rakyrs / Rad2Grad;
            apyInfoM.nextTockaRS[0].kyrsovojParameter_km = apyInfoM.nextTockaRS[0].groundRange_km * sin(deltaPrm);
            apyInfoM.nextTockaRS[0].rangeParametr_km = sqrt(apyInfoM.nextTockaRS[0].groundRange_km * apyInfoM.nextTockaRS[0].groundRange_km
                                                                  - apyInfoM.nextTockaRS[0].kyrsovojParameter_km * apyInfoM.nextTockaRS[0].kyrsovojParameter_km);
            ZonaPorazhRanges zpr = ZP::glybina_ZP(apyInfoM.nextTockaRS[0].kyrsovojParameter_km, apyInfoM.nextTockaRS[0].radarRelativeAltitude);
            apyInfoM.nextTockaRS[0].bGZoniPorParam_km = zpr.minValue_km;
            apyInfoM.nextTockaRS[0].dGZoniPorParam_km = zpr.maxValue_km;
            apyInfoM.nextTockaRS[0].maxKyrsovojParameter_km = zpr.maxParams;

            float dgzPor_abs = sqrt(zpr.maxValue_km * zpr.maxValue_km + apyInfoM.nextTockaRS[0].kyrsovojParameter_km * apyInfoM.nextTockaRS[0].kyrsovojParameter_km);
            float tPoliotaRaketiDGZPor = dgzPor_abs / VRAKETI;
            float groundSpeed_km_s = apyInfoM.nextTockaRS[0].groundSpeed_ms * 1.0e-3;

            apyInfoM.nextTockaRS[0].dGZoniPyskaParametr_km = zpr.maxValue_km + fabs(groundSpeed_km_s) * tPoliotaRaketiDGZPor;

        //    qDebug()<<"assignGradysnikParams: zpr->maxValue_km:"<<zpr->maxValue_km<<"kyrsovojParameter_km:"<<apyInfoM.nextTocka[ii].kyrsovojParameter_km
        //           <<"tPoliotaRaketiDGZPor:"<<tPoliotaRaketiDGZPor<<"dGZoniPyskaParametr_km:"<<apyInfoM.nextTocka[ii].dGZoniPyskaParametr_km;

            float a = groundSpeed_km_s * groundSpeed_km_s - VRAKETI * VRAKETI;

            float b =  - 2.0 * fabs(groundSpeed_km_s) * apyInfoM.nextTockaRS[0].rangeParametr_km;

            float c = apyInfoM.nextTockaRS[0].groundRange_km * apyInfoM.nextTockaRS[0].groundRange_km;

            float discr = sqrt(b * b - 4 * a * c);

            //float x1 = (- b + discr)  / 2.0 / a;
            float x2 = (- b - discr)  / 2.0 / a;

            apyInfoM.nextTockaRS[0].rangeParametrTV_km = apyInfoM.nextTockaRS[0].rangeParametr_km - x2 * groundSpeed_km_s;

            if(groundSpeed_km_s != 0.0)
            {
                apyInfoM.nextTockaRS[0].podletnoeVrema = (apyInfoM.nextTockaRS[0].rangeParametr_km - apyInfoM.nextTockaRS[0].dGZoniPyskaParametr_km) / groundSpeed_km_s;
            }
            //qDebug()<<"assignGradysnikParams ii:"<<ii<<"rangeParametr_km"<<apyInfoM.nextTocka[ii].rangeParametr_km<<"groundSpeed_km_s:"<<groundSpeed_km_s<<"podletnoeVrema:"<<apyInfoM.nextTocka[ii].podletnoeVrema;

            //delete zpr;

        }   //  если было приращение дальности или азимута
    }   //  для РС по углам
}
