#pragma once

class FSMCondition
{    
public:
    FSMCondition(){};
    
    virtual bool TestCondition()
    {
        return false;
    };
};
