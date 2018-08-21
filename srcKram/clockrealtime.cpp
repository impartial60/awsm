#include "clockrealtime.h"

/**
 * @class ClockRealTime
 * @brief Работа с часами Linux систем.
 */

///// Создает запущенный таймера.
//ClockRealTime::ClockRealTime()
//{
//    start();
//}

/// Вернет значение системных часов, сек.
double ClockRealTime::realTime()
{
    struct timespec time;
    ::clock_gettime(CLOCK_REALTIME, &time);
    return (double)time.tv_sec + ((double)(time.tv_nsec / (double)1000000000.0));
}

/// Запускает таймера.
void ClockRealTime::start()
{
    ::clock_gettime(CLOCK_MONOTONIC, &m_start);
}

/**
 * @brief Вернет време прошедшее между запуском таймера и вызовом этого метода.
 * @return время, сек.
 */
double ClockRealTime::elapsed()
{
    ::clock_gettime(CLOCK_MONOTONIC, &m_stop);
    return diff(m_start, m_stop);
}

/**
 * @brief Измерение интервалов.
 *
 *   Вернет време прошедшее между запуском таймера и вызовом этого метода.
 * Таймер автоматически перезапускается этим методом.
 * @return время, сек.
 */
double ClockRealTime::interval()
{
    ::clock_gettime(CLOCK_MONOTONIC, &m_stop);
    double dT = diff(m_start, m_stop);
    m_start = m_stop;
    return dT;
}

/**
 * @brief Вернет разничу между отсчетами времени.
 *
 * Вернет t2-t1 в секyндах.
 * @param start время t1.
 * @param stop время t2.
 * @return разница времени, сек.
 */
double ClockRealTime::diff(struct timespec &start, struct timespec &stop)
{
    return (double)(stop.tv_sec - start.tv_sec) + ((double)(stop.tv_nsec - start.tv_nsec) / (double)1000000000.0F);
}
