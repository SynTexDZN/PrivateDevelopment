#ifndef Climate_H
#define Climate_H

#include "main.h"
#include "Arduino.h"

class Climate
{
  public:
    Climate();
    void SETUP(String ip, String port, int interval, String events);
    void UPDATE(boolean force);
};

#endif
