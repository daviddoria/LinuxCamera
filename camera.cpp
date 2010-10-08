/*
You need to ensure the dc1394 and raw1394 libraries (and associated development packages) are installed. In Fedora 13, you can do
sudo yum install libdc1394* libraw1394*

*/

#include "camera.h"

#include <dc1394/dc1394.h>


void camera::Initialize()
{
  dc1394camera_t **cameras=NULL;

  uint32_t numCameras, i;
  dc1394featureset_t features;
  dc1394framerates_t framerates;
  dc1394video_modes_t video_modes;
  dc1394framerate_t framerate;
  dc1394video_mode_t video_mode;
  dc1394color_coding_t coding;

  /* Find cameras */
  int err = dc1394_find_cameras(&cameras, &numCameras);

  if (numCameras < 1)
  {
    printf("No cameras found.\n");
    exit(1);
  }
  /*
  // free the other cameras
  for (i=0;i<numCameras;i++)
  {
    printf ("Reseting bus...\n");
    dc1394_reset_bus (cameras[i]);
  }
*/

  for (i=1;i<numCameras;i++)
  {
    //dc1394_free_camera(cameras[i]);
    //dc1394_reset_camera(cameras[i]);
  }

  video_mode = DC1394_VIDEO_MODE_1024x768_MONO8;
  framerate = DC1394_FRAMERATE_15;
  int cam_counter;
  for (int cam_counter = 0; cam_counter < numCameras; cam_counter++)
  {
    //settings particular to this camera
    dc1394_video_set_iso_speed(cameras[cam_counter], DC1394_ISO_SPEED_400);
    //printf("Set speed.\n");
    dc1394_video_set_mode(cameras[cam_counter], video_mode);
    //printf("Set mode.\n");
    dc1394_video_set_framerate(cameras[cam_counter], framerate);
    //printf("Set framerate.\n");
    dc1394_capture_setup(cameras[cam_counter], 4, DC1394_CAPTURE_FLAGS_DEFAULT);
    //printf("Setup capture.\n");
  }

  return cameras;
}

camera::~camera()
{
  dc1394_video_set_transmission(this->firewireCamera, DC1394_OFF);
  dc1394_capture_stop(this->firewireCamera);
//  dc1394_free_camera(this->firewireCamera);
  dc1394_free(this->firewireCamera);
  exit(1);
}

void camera::AcquireImage()
{
  unsigned int width, height;
  dc1394video_frame_t *frame;

  // Have the camera start sending us data
  dc1394_video_set_transmission(this->firewireCamera, DC1394_ON);

  //Sleep untill the camera has a transmission
  dc1394switch_t status = DC1394_OFF;

  int i = 0;
  while( status == DC1394_OFF && i++ < 5 )
  {
    usleep(50000);
    if (dc1394_video_get_transmission(camera, &status)!=DC1394_SUCCESS)
      {
      fprintf(stderr, "unable to get transmision status\n");
      cleanup_and_exit(camera);
      }
  }

  if( i == 5 )
  {
    fprintf(stderr,"Camera doesn't seem to want to turn on!\n");
    cleanup_and_exit(camera);
  }

  // Capture one frame
  dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame);


  // Stop data transmission
  dc1394_video_set_transmission(camera,DC1394_OFF);

  dc1394_get_image_size_from_video_mode(camera, DC1394_VIDEO_MODE_1024x768_MONO8, &width, &height);

  dc1394color_filter_t pattern =DC1394_COLOR_FILTER_GBRG;

  uchar_t* pRGBImage = new uchar_t[ width * height * 3 ];

  dc1394_bayer_decoding_8bit( frame->image, pRGBImage, width, height, pattern, DC1394_BAYER_METHOD_NEAREST );

  std::string filename = "test.ppm";


  FILE* imagefile_color;
  imagefile_color = fopen( filename, "w" );

  if (imagefile_color == NULL)
  {
    printf("Cannot open file.\n");
    exit(0);
  }
  // note: assuming a 24 bit image
  fprintf( imagefile_color, "P6\n%u %u 255\n", width, height );
  fwrite( pRGBImage, 3, width * height, imagefile_color );
  fclose( imagefile_color );
  if (DEBUG_ON)
  {
    printf( "wrote: '%s'\n", filename );
  }

  dc1394_capture_enqueue(camera, frame);

}
