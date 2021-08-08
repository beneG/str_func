#include<iostream>
#include "shared.h"

CMyClass *get_instance()
{
    return new CMyClass();
}

CMyClass::CMyClass()
{
    std::cout << "CMyClass ctor() called" << std::endl;
//    m_one = 1; m_two = 2; m_three = 3;
}

void CMyClass::raiseEvent1()
{
    std::cout << "Event1 raised! " << m_one << std::endl;
}
void CMyClass::raiseEvent2()
{
    std::cout << "Event2 raised! " << m_two << std::endl;
}
void CMyClass::raiseEvent3()
{
    std::cout << "Event3 raised! " << m_three << std::endl;
}

extern "C" 
{
void raiseEvent1()
{
    std::cout << "extern C Event1 raised!" << std::endl;
}
}
