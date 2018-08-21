#include "keyboard_keys.h"

keyboard_keys::keyboard_keys()
{

}

keyboard_keys::~keyboard_keys()
{

}

std::string keyboard_keys::AsString()
{

}

key::key():
    m_in(0), m_out(0)
{
}

key::~key()
{

}

int32_t key::get_CodeOut()
{
    return m_out;
}

void key::set_CodeOut(int32_t v_Value)
{
    m_out = v_Value;
}

int32_t key::get_CodeIn()
{
    return m_in;

}

void key::set_CodeIn(int32_t v_Value)
{
    m_in = v_Value;
}

int32_t key::getAsCode()
{
    return ((m_out  << 16) & m_in );
}

std::string key::getAsString()
{
    return "test";
}
