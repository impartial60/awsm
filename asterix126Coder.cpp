#include "externM.h"

int indexRaketi1 = -1;
int indexRaketi2 = -1;

int tPoliotaRaschetnoeRaketi1;
int tPoliotaRaschetnoeRaketi2;

void asterix_I126_050_Coder(uint8_t *sck)    //  I126/050        С О С Т О Я Н И Е   Ц Е Л Е В О Г О   К А Н А Л А   1
{
    //СК – Фаза работы ЗРК и состояние целевого канала

    memset(sck, 0, 30);

    uint8_t ck = 0b0;       //  0000 – канал свободен - исходное состояние
    //        0001 – ЦУ принято, ЗРК в поиске
    //        0011 – сопровождение в режиме 1
    //        0100 – сопровождение в режиме 2
    //        0101 – цель в зоне пуска
    //        0110 – отказ от ЦУ
    //        0111 – канал неисправен
    //        1000 – канал на размножении
    //        1001 – маневрирующая цель в гарантированной зоне пуска

    uint8_t nomK_FX = 0b0010; // всегда номер канала 1, по умолчанию информация о цели не выдаётся

    switch(apyInfoM.rejimPoiskaSNR)
    {
        case RejimPoiskaSNR::POISK_VIKLUCHEN: ck = 0b0000;  break;               // СНР ничего не делает
        case RejimPoiskaSNR::POISK_PEREKLYCHEN_V_ZAHVAT:
        case RejimPoiskaSNR::POISK_BSP:
        case RejimPoiskaSNR::POISK_MSP:   ck = 0b0001;      break;               // СНР находится в режиме отработки ЦУ, допоиска или автономного поиска
        case RejimPoiskaSNR::POISK_PEREKLYCHEN_V_SOPROVOJDENIE:   ck = 0b0011; nomK_FX += 0b0001;   // СНР сопровождает цель, информация по ней будет выдаваться на КП
    }

    sck[0] = (ck << 4) + nomK_FX;

    sck[1] = 0x0;   //  Байт 2 исходное состояние
    sck[2] = 0xff;  //  Байт 3 - Р Е З Е Р В
    sck[3] = 0x0;   //  Байт 4 - Время наведения от старта ц.м.р. 1 сек
    sck[4] = 0x0;   //  Байт 5 - Остаток времени до встречи  ц.м.р. 1 сек

    sck[5] = 0x0;   //  Байт 2 исходное состояние
    sck[6] = 0xff;  //  Байт 3 - Р Е З Е Р В
    sck[7] = 0x0;   //  Байт 4 - Время наведения от старта ц.м.р. 1 сек
    sck[8] = 0x0;   //  Байт 5 - Остаток времени до встречи  ц.м.р. 1 сек

    if(indexRaketi1 != -1 && startInfoM.launcherInfo[indexRaketi1 / MAXPY].missileInfo[indexRaketi1 % MAXPY].missileStatus == MissileStatus::RAKETA_OTSYTSTVIET)
    {
        indexRaketi1 = -1;
    }

    if(indexRaketi2 != -1 && startInfoM.launcherInfo[indexRaketi2 / MAXPY].missileInfo[indexRaketi2 % MAXPY].missileStatus == MissileStatus::RAKETA_OTSYTSTVIET)
    {
        indexRaketi2 = -1;
    }

    if(indexRaketi1 == -1 || indexRaketi2 == -1)
    {
        for(int i = 0; i < 16; i++)
        {
            if(indexRaketi1 == -1 && i != indexRaketi2 && startInfoM.launcherInfo[i / MAXPY].missileInfo[i % MAXPY].missileStatus == MissileStatus::NAVEDENIE)
            {
                indexRaketi1 = i;

                float rPar = apyInfoM.nextTockaRS[0].rangeParametr_km;
                float kPar = apyInfoM.nextTockaRS[0].kyrsovojParameter_km;

                tPoliotaRaschetnoeRaketi1 = (int)(sqrt(rPar * rPar + kPar * kPar) / VRAKETI);
            }

            if(indexRaketi2 == -1 && i != indexRaketi1 && startInfoM.launcherInfo[i / MAXPY].missileInfo[i % MAXPY].missileStatus == MissileStatus::NAVEDENIE)
            {
                indexRaketi2 = i;

                float rPar = apyInfoM.nextTockaRS[0].rangeParametr_km;
                float kPar = apyInfoM.nextTockaRS[0].kyrsovojParameter_km;

                tPoliotaRaschetnoeRaketi2 = (int)(sqrt(rPar * rPar + kPar * kPar) / VRAKETI);
            }
        }
    }

    if(indexRaketi1 != -1 || indexRaketi2 != -1)
    {
        float rPar = apyInfoM.nextTockaRS[0].rangeParametr_km;
        float kPar = apyInfoM.nextTockaRS[0].kyrsovojParameter_km;

        if(indexRaketi1 != -1)// && commanderConsole2InfoTable.launcher2Commander.missileStatuses[indexRaketi1] != MissileStatus::NAVEDENIE)
        {
            uint8_t byte2 = 0;
            byte2 += 0b10000000;    // ТР – тип ракеты : 10 – 27ДМ1,
            byte2 += 0b00100000;    // П – пуск:    0 – пуска нет,   1 – пуск есть.
            byte2 += 0b00010000;    // НР – наведение ракеты:  1 – ракета захвачена и управляется, 0 – ракета не управляется.
            byte2 += 0b00001000;    // МН – метод наведения:  0 – метод №1 (трехточка), 1 – метод №2 (половинное спрямление).
            //byte2 += 0b00000100;    // ВО – вид огня:   0 – очередь,  1 – залп.
            //byte2 += 0b00000010;    // ПЦ – поражение цели:   1 – цель поражена,  0 – цель не поражена.

            //int tPoliotaRaketiOtStarta = (int)commanderConsole2InfoTable.launcher2Commander.t_poleta_raket[indexRaketi1];
            int tPoliotaRaketiOtStarta = (int)startInfoM.launcherInfo[indexRaketi1 / MAXPY].missileInfo[indexRaketi1 % MAXPY].t_navedenie_sec;

            float rangeCeliAbs = sqrt(rPar * rPar + kPar * kPar);
            float rangeRaketiAbs = VRAKETI * tPoliotaRaketiOtStarta ;
            float dopRangeRaketiAbsDoTochkiOcenkiRezStrelbi = VRAKETI * 2.0; // 2 секунды оцениваем результаты стрельбы

            //float xMissileR = raketaVparam * tPoliotaRaketiOtStarta;

            int slolko_do_vstrechi = tPoliotaRaschetnoeRaketi1 - tPoliotaRaketiOtStarta;

            if(rangeRaketiAbs > rangeCeliAbs + dopRangeRaketiAbsDoTochkiOcenkiRezStrelbi && slolko_do_vstrechi < - 2)
            {
               byte2 += 0b00000010;    // ПЦ – поражение цели:   1 – цель поражена,  0 – цель не поражена.
            }

            sck[1] = byte2;

            sck[3] = tPoliotaRaketiOtStarta;


            if(slolko_do_vstrechi > 0)
                sck[4] = slolko_do_vstrechi;
            else
                sck[4] = 0x0;

            //qDebug()<<"asterix_I126_050_Coder tPoliotaRaketiOtStarta:"<<tPoliotaRaketiOtStarta<<"tPoliotaRaschetnoeRaketi1:"<<tPoliotaRaschetnoeRaketi1<<"slolko_do_vstrechi:"<<slolko_do_vstrechi;


//            if(raketaVparam > 0 && rPar > xMissileR)
//            {
//                float tRaketiDoTV = (rPar - xMissileR) / raketaVparam;
//                sck[4] = (int)tRaketiDoTV;
//            }
//            else
//                sck[4] = 0x0;
        }

        if(indexRaketi2 != -1)// && commanderConsole2InfoTable.launcher2Commander.missileStatuses[indexRaketi1] != MissileStatus::NAVEDENIE)
        {
            uint8_t byte6 = 0;
            byte6 += 0b10000000;    // ТР – тип ракеты : 10 – 27ДМ1,
            byte6 += 0b00100000;    // П – пуск:    0 – пуска нет,   1 – пуск есть.
            byte6 += 0b00010000;    // НР – наведение ракеты:  1 – ракета захвачена и управляется, 0 – ракета не управляется.
            byte6 += 0b00001000;    // МН – метод наведения:  0 – метод №1 (трехточка), 1 – метод №2 (половинное спрямление).
            //byte6 += 0b00000100;    // ВО – вид огня:   0 – очередь,  1 – залп.
            //byte6 += 0b00000010;    // ПЦ – поражение цели:   1 – цель поражена,  0 – цель не поражена.

            //int tPoliotaRaketiOtStarta = (int)commanderConsole2InfoTable.launcher2Commander.t_poleta_raket[indexRaketi2];
            int tPoliotaRaketiOtStarta = (int)startInfoM.launcherInfo[indexRaketi2 / MAXPY].missileInfo[indexRaketi2 % MAXPY].t_navedenie_sec;

//            float xMissileR = raketaVparam * tPoliotaRaketiOtStarta;
//            if(xMissileR > rPar)
//            {
//               byte6 += 0b00000010;    // ПЦ – поражение цели:   1 – цель поражена,  0 – цель не поражена.
//            }

            float rangeCeliAbs = sqrt(rPar * rPar + kPar * kPar);
            float rangeRaketiAbs = VRAKETI * tPoliotaRaketiOtStarta ;
            float dopRangeRaketiAbsDoTochkiOcenkiRezStrelbi = VRAKETI * 2.0; // 2 секунды оцениваем результаты стрельбы

            //float xMissileR = raketaVparam * tPoliotaRaketiOtStarta;

            int slolko_do_vstrechi = tPoliotaRaschetnoeRaketi2 - tPoliotaRaketiOtStarta;


            if(rangeRaketiAbs > rangeCeliAbs + dopRangeRaketiAbsDoTochkiOcenkiRezStrelbi && slolko_do_vstrechi < - 2)
            {
               byte6 += 0b00000010;    // ПЦ – поражение цели:   1 – цель поражена,  0 – цель не поражена.
            }

            sck[5] = byte6;

            sck[7] = (int)tPoliotaRaketiOtStarta;

            if(slolko_do_vstrechi > 0)
                sck[8] = slolko_do_vstrechi;
            else
                sck[8] = 0x0;

//            if(raketaVparam > 0 && rPar > xMissileR)
//            {
//                float tRaketiDoTV = (rPar - xMissileR) / raketaVparam;
//                sck[8] = (int)tRaketiDoTV;
//            }
//            else
//                sck[8] = 0x0;
        }
    }

    // какая цель ближе к ЦУ    -   уже не надо, проверка делается в а awsG125Console threadSimulator.cpp ProcessThread_awsCommander
//    float maxNeviazka = 3000;

//    int ind = 0;

//    for(int i = 0; i < 3; i++)
//    {
//        float dx = shMemC2U.vkp2zrk.cmplxYHX[i].x_km - shMemC2U.vkp2zrk.coordYHX.x_km;
//        float dy = shMemC2U.vkp2zrk.cmplxYHX[i].y_km - shMemC2U.vkp2zrk.coordYHX.y_km;
//        float dh = shMemC2U.vkp2zrk.cmplxYHX[i].h_km - shMemC2U.vkp2zrk.coordYHX.h_km;

//        float neviazka = sqrt(dx * dx + dy * dy + dh *dh);
//        if(neviazka < maxNeviazka)
//        {
//            ind = i;
//            maxNeviazka = neviazka;
//        }
//    }

    uint16_t nomerCeliKP = uint16_t_byte_reorder(vkp2zrkM.komandiPoCelevomyKanaly1.NceliKP);//uint16_t_byte_reorder(1234); //  Номер цели КП (в двоичном виде)
            //uint16_t_byte_reorder(shMemC2U.vkp2zrk.cmplxTrgtN[ind]);//uint16_t_byte_reorder(1234); //  Номер цели КП (в двоичном виде)

    memcpy(sck + 9, &nomerCeliKP, 2);

    sck[11] = vkp2zrkM.komandiPoCelevomyKanaly1.NceliKP + 10;//.cmplxTrgtN[ind] + 10;//shMemC2U.vkp2zrk.komandiPoCelevomyKanaly1.NceliKP + 10;// для отладки по просьбе искры    //1; // Номер цели ЗРК. Если номер цели равен 0, значит номера нет
    sck[12] = 0; // Признаки цели.
    sck[13] = 0; // ТипЦ ХВО
    sck[14] = 0; // опознавание
    sck[15] = 0; // опознавание

    sck[16] = 0; // групповая цель
    sck[17] = 0; // помеха в канале

    // К О О Р Д И Н А Т Ы   Ц Е Л И

//    int16_t crdY = (int)(20.0 * shMemC2U.vkp2zrk.cmplxYHX[ind].y_km + 1.0);   //              0.05 * 34.5;
//    int16_t crdH = (int)(20.0 * shMemC2U.vkp2zrk.cmplxYHX[ind].h_km + 1.0);   //              0.05 * 2345.6;
//    int16_t crdX = (int)(20.0 * shMemC2U.vkp2zrk.cmplxYHX[ind].x_km + 1.0);   //              0.05 * 67.0;

//    int16_t vY = shMemC2U.vkp2zrk.cmplxYHX[ind].vy_ms + 10;
//    int16_t vH = shMemC2U.vkp2zrk.cmplxYHX[ind].vh_ms + 10;  //  222.5;
//    int16_t vX = shMemC2U.vkp2zrk.cmplxYHX[ind].vx_ms + 10;  //  333.0;

    int16_t crdY = (int)(20.0 * vkp2zrkM.cmplxYHX[apyInfoM.targetIndexCY].y_km);   //              0.05 * 34.5;
    int16_t crdH = (int)(20.0 * vkp2zrkM.cmplxYHX[apyInfoM.targetIndexCY].h_km);   //              0.05 * 2345.6;
    int16_t crdX = (int)(20.0 * vkp2zrkM.cmplxYHX[apyInfoM.targetIndexCY].x_km);   //              0.05 * 67.0;

    int16_t vY = vkp2zrkM.cmplxYHX[apyInfoM.targetIndexCY].vy_ms;
    int16_t vH = vkp2zrkM.cmplxYHX[apyInfoM.targetIndexCY].vh_ms;  //  222.5;
    int16_t vX = vkp2zrkM.cmplxYHX[apyInfoM.targetIndexCY].vx_ms;  //  333.0;

//    int16_t crdY = (int)(20.0 * shMemC2U.vkp2zrk.coordYHX.y_km);   //              0.05 * 34.5;
//    int16_t crdH = (int)(20.0 * shMemC2U.vkp2zrk.coordYHX.h_km);   //              0.05 * 2345.6;
//    int16_t crdX = (int)(20.0 * shMemC2U.vkp2zrk.coordYHX.x_km);   //              0.05 * 67.0;

//    int16_t vY = shMemC2U.vkp2zrk.coordYHX.vy_ms;
//    int16_t vH = shMemC2U.vkp2zrk.coordYHX.vh_ms;  //  222.5;
//    int16_t vX = shMemC2U.vkp2zrk.coordYHX.vx_ms;  //  333.0;

//        int16_t crdY = (int)(20.0 * apyInfoM.nextTocka.groundY);            // ТЕСТ ПО ЦЕЛИ ИЗ АВТОНОМНОГО ПОИСКА
//        int16_t crdX = (int)(20.0 * apyInfoM.nextTocka.groundX);            // ТЕСТ ПО ЦЕЛИ ИЗ АВТОНОМНОГО ПОИСКА
//        int16_t crdH = (int)(20.0 * apyInfoM.nextTocka.seaLevelAltitude);   // ТЕСТ ПО ЦЕЛИ ИЗ АВТОНОМНОГО ПОИСКА

//        float kyrsRad = apyInfoM.nextTocka.kyrs / Rad2Grad;                 // ТЕСТ ПО ЦЕЛИ ИЗ АВТОНОМНОГО ПОИСКА
//        int16_t vY = (int)(apyInfoM.nextTocka.groundSpeed * cos(kyrsRad));  // ТЕСТ ПО ЦЕЛИ ИЗ АВТОНОМНОГО ПОИСКА
//        int16_t vH = 0;                                                             // ТЕСТ ПО ЦЕЛИ ИЗ АВТОНОМНОГО ПОИСКА
//        int16_t vX = (int)(apyInfoM.nextTocka.groundSpeed * sin(kyrsRad));  // ТЕСТ ПО ЦЕЛИ ИЗ АВТОНОМНОГО ПОИСКА

    sck[18] = (crdY &   0xFF00) >>  8;
    sck[19] = (crdY &     0xFF);

    sck[20] = (crdH &   0xFF00) >>  8;
    sck[21] = (crdH &     0xFF);

    sck[22] = (crdX &   0xFF00) >>  8;
    sck[23] = (crdX &     0xFF);

    sck[24] = (vY &   0xFF00) >>  8;
    sck[25] = (vY &     0xFF);

    sck[26] = (vH &   0xFF00) >>  8;
    sck[27] = (vH &     0xFF);

    sck[28] = (vX &   0xFF00) >>  8;
    sck[29] = (vX &     0xFF);
}
