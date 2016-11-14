#ifndef _FIZZ_MEMPOOL_H
#define _FIZZ_MEMPOOL_H

typedef unsigned long long ULLONG;

class GMemoryRecordSwitch
{
public:
    static GMemoryRecordSwitch& getInstance()
    {
        static GMemoryRecordSwitch s_instance;
        return s_instance;
    }

    void setSwitch(bool bSwitch);
    bool getSwitch(){return m_switch;}
    int  get_aquire_count(){return m_aquire_count;}

private:
    GMemoryRecordSwitch()
        :m_aquire_count(0), m_switch(false){};

    int m_aquire_count;
    bool m_switch;
};


class SMiniCheck
{
public:
    SMiniCheck()
    {
        GMemoryRecordSwitch::getInstance().setSwitch(true);
    }

    ~SMiniCheck()
    {
        GMemoryRecordSwitch::getInstance().setSwitch(false);
    }

private:
    SMiniCheck(const SMiniCheck&);
    SMiniCheck& operator=(const SMiniCheck&);
};


#endif /*_FIZZ_MEMPOOL_H*/

