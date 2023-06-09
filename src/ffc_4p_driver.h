#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <thread>
#include <ros/ros.h>

#include "depthai/depthai.hpp"
#include <depthai/utility/Clock.hpp>
#include <chrono>

namespace OAKCAM{

class FFC4PDriver
{  
 public:
   struct CameraModuleConfig{
      bool show_img = false;
      bool auto_expose = false;
      bool ros_defined_freq = true;
      bool enable_compressed_img = true;
      bool show_img_info = false;
      bool auto_awb = false;  
      int32_t fps = 20.0;
      int32_t resolution = 720;
      int32_t expose_time_us = 10000;
      int32_t iso = 400;
      int32_t awb_value = 4000;
   };
   
   struct FFCCameraConfig{
      dai::CameraBoardSocket socket;
      dai::ColorCameraProperties::SensorResolution resolution = dai::ColorCameraProperties::SensorResolution::THE_720_P;
      std::string stream_name;
      bool is_master;
      FFCCameraConfig(dai::CameraBoardSocket sck,dai::ColorCameraProperties::SensorResolution res, std::string name,bool master):
         socket(sck),resolution(res),stream_name(name),is_master(master){};
   };
   struct ImageNode
   {
      std::shared_ptr<dai::DataOutputQueue> data_output_q = nullptr;
      std::string topic;
      ros::Publisher ros_publisher;
      cv::Mat image;
      std::chrono::time_point<std::chrono::steady_clock, std::chrono::steady_clock::duration> cap_time_stamp;
      int32_t frame_counter;
      ImageNode( std::shared_ptr<dai::DataOutputQueue> data_output_q,std::string topic):data_output_q(data_output_q),topic(topic){
      }
   };
   
   std::vector<FFCCameraConfig> CameraList = 
      {{dai::CameraBoardSocket::CAM_A,dai::ColorCameraProperties::SensorResolution::THE_720_P, std::string("CAM_A"), true}, //
       {dai::CameraBoardSocket::CAM_B,dai::ColorCameraProperties::SensorResolution::THE_720_P, std::string("CAM_B"), false},
       {dai::CameraBoardSocket::CAM_C,dai::ColorCameraProperties::SensorResolution::THE_720_P, std::string("CAM_C"), false},
       {dai::CameraBoardSocket::CAM_D,dai::ColorCameraProperties::SensorResolution::THE_720_P, std::string("CAM_D"),false}};
   FFC4PDriver(std::shared_ptr<ros::NodeHandle>& nh);
   ~FFC4PDriver();
   int32_t InitPipeline();
   int32_t SetAllCameraSychron();
   int32_t SetVedioOutputQueue();
   void GetParameters(ros::NodeHandle& nh);
   void StartVideoStream();

 private:
   void RosGrabImgThread(const ros::TimerEvent &event);
   void StdGrabImgThread();
   void GrabImg();

   void ShowImg(ImageNode & image_node, std::chrono::_V2::steady_clock::time_point& time_now);

   std::shared_ptr<dai::Pipeline> pipeline_ = nullptr;
   std::shared_ptr<dai::Device> device_ = nullptr;
   std::list<ImageNode> image_queue_;
   int32_t device_is_detected_ = 0;
   int32_t pipeline_is_init_ = 0;
   CameraModuleConfig module_config_;
   
   //config translate
   dai::ColorCameraProperties::SensorResolution resolution_;

   //ros
   std::shared_ptr<ros::NodeHandle> ros_node_ = nullptr;
   ros::Timer thread_timer_;


   //thread
   std::thread grab_thread_;
   bool is_run_ = true;
   //image_tmp
};

double Clearness(cv::Mat &img); //calculate clearness to manuel focus

}