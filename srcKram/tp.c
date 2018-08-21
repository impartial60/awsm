
// Планировщик траектории

#include "tp.h"
#include <math.h>



void tp_update(tp_t *tp, double period /* в секундах!!! */)
{
    double max_dv, tiny_dp, pos_err, vel_req;

    tp->active = 0;
    /* Вычисляем приращение скорости за сервопериод*/
    max_dv = tp->max_acc * period;
    /* Высисляем ширишу зоны (по позиции)  которую считаем нолем (мертвая зона) */ 
    tiny_dp = max_dv * period * 0.001;
    /* Расчет скорости */
    if (tp->enable) {
     /*Если включен планировщик запросили скорость которая необходима для уменьшения ошибки позиции,
       но позволяет остановиться без перескока через заданную позицию
      */    
	pos_err = tp->pos_cmd - tp->curr_pos;
        /* Положитенльная и отрицательная ошибка требует раздельной обработки
           чтобы исключить извлечение квадратного корня из отрицательного числа*/
	if (pos_err > tiny_dp) {
	    vel_req = -max_dv +
		       sqrt(2.0 * tp->max_acc * pos_err + max_dv * max_dv);
	    /* mark planner as active */
	    tp->active = 1;
	} else if (pos_err < -tiny_dp) {
	    vel_req =  max_dv -
		       sqrt(-2.0 * tp->max_acc * pos_err + max_dv * max_dv);
	    /* mark planner as active */
	    tp->active = 1;
	} else {
        /* позиция в пределах допуска. Перемещать не нужно*/
	    vel_req = 0.0;
	}
    } else {
     /* запрет планировшика. Запросу на скорость отвечаем нолем и уравниваем позиции  чтобы избежать движения при следующем разрешении*/ 
	vel_req = 0.0;
	tp->pos_cmd = tp->curr_pos;
    }
    /* Ограничение по скорости */
    if (vel_req > tp->max_vel) {
        vel_req = tp->max_vel;
    } else if (vel_req < -tp->max_vel) {
	vel_req = -tp->max_vel;
    }
    /* Проверка по лимитам ускорения */
    if (vel_req > tp->curr_vel + max_dv) {
	tp->curr_vel += max_dv;
    } else if (vel_req < tp->curr_vel - max_dv) {
	tp->curr_vel -= max_dv;
    } else {
	tp->curr_vel = vel_req;
    }
    if (tp->curr_vel != 0.0) {
	tp->active = 1;
    }
    /* Вычисление новой позиции */
    tp->curr_pos += tp->curr_vel * period;
}
