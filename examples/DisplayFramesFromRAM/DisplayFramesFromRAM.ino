#include <PanelsController.hpp>


PanelsController dev;

void setup()
{
  dev.setup();
}

void loop()
{
  dev.displayFrameFromRAM();
}
