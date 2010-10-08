#ifndef camera_h
#define camera_h

#include <dc1394/camera.h>

class camera
{
  public:
    void AcquireImage();
    void Initialize();

  private:
    dc1394camera_t* firewireCamera;
};

#endif