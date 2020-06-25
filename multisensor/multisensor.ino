#include "main.h"
#include "climate.h"
#include "light.h"

SynTexMain m;
Climate climate;
Light light;

void setup()
{
  if(m.SETUP("multi", "5.0.0", 10000, "[]") && m.checkConnection())
  {
    climate.SETUP(m.BridgeIP, m.WebhookPort, 10000, "[]");
    light.SETUP(m.BridgeIP, m.WebhookPort, 10000, "[]");
    
    if(m.Active)
    {
      climate.UPDATE(true);
      light.UPDATE(true);
    }
  }
}

void loop()
{
  m.LOOP();

  if(m.checkConnection() && m.Active)
  {
    climate.UPDATE(false);
    light.UPDATE(false);
  }
}
