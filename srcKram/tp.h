
#ifndef TP_H
#define TP_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct tp_t {
    double pos_cmd;		/* Позиция в которую необходимо переместиться */
    double max_vel;		/* Лимит скорости */
    double max_acc;		/* Лимит ускорения */
    int enable;		/* Если "0" движение останавливается "ASAP" */
    double curr_pos;	/* Текущая позиция */
    double curr_vel;	/* Текущая скорость */
    int active;		/* "1" если продолжается движение */
    } tp_t;



/*  Главная функция simple_tp_update (...). Если 'enablе' истинно, то
    вычисляет новое значение "curr_pos", который движется в сторону "pos_cmd '
    опираясь на пределы max_vel и max_accel. Она также устанавливает
    active, если движение идет, и очищает его, когда движение
    останавливается на заданном положении. Команда или любое из
    ограничений могут быть изменены в любое время. Если 'enable' ложно, сбрасывается
    скорость до нуля, а затем очищается "active" и выравнивает
    'pos_cmd', для соответствия 'curr_pos ", чтобы избежать движения при следующем
    включении. 'period' - период между вызовами в секундах.
*/
extern void tp_update(tp_t *tp, double period);


#ifdef __cplusplus
}
#endif
#endif	/* TP_H */
