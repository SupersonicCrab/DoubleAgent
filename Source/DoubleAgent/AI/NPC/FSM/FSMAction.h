#pragma once

class FSMAction
{    
public:
    FSMAction(){};
    
    virtual bool PerformAction()
    {
        return false;
    };
};
