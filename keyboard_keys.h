#ifndef KEYBOARD_KEYS_H
#define KEYBOARD_KEYS_H

#include <stdint.h>
#include <string>
#include <list>


// код нажатой кнопки
class key
{
public:
    key();
    virtual ~key();

    // основные методы чтения кода нажатой кнопки
    int32_t             get_CodeOut();
    void                set_CodeOut(int32_t v_Value);
    int32_t             get_CodeIn();
    void                set_CodeIn(int32_t  v_Value);

    // вспомогательная функция: прочитать код нажатой кнопки в виде одного числа
    virtual int32_t             getAsCode();

    // вспомогательная функция: прочитать код нажатой кнопки в виде строки
    virtual std::string         getAsString();

protected:
    int32_t     m_out; // номер выходной линии
    int32_t     m_in;  // номер входноя линии

};


// содержит событие от блока кнопок
class keyboard_keys
{
public:
    keyboard_keys();
    virtual ~keyboard_keys();

    // получить событие в виде строки.
    virtual std::string  AsString();


protected:

    // контейнер содержит список нажатых кнопок
    std::list<key>  m_list_keys;
    bool            m_key_down; // true -- сработало прерывание на нажатие кнопки; false -- отпускание кнопки



};

#endif // KEYBOARD_KEYS_H
