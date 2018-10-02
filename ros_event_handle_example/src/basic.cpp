#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <boost/thread/thread.hpp>
#include <ros/ros.h>
#include "std_msgs/String.h"
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include "math_helper.h"
#include "string_convertor.h"
#include "transformation2D.h"
#include "signature_visualization/pathData.h"
#include "wam_msgs/MatrixMN.h"
#include "wam_srvs/JointMove.h"
#include "sensor_msgs/JointState.h"

// pthread_event.cpp :
//

#include <unistd.h>
#include "MyEvent.h"

#define PRINT_TIMES 10

//
CMyEvent g_myEvent;
int g_iNum = 0;


//Thread
void * ThreadProc1(void *pParam)
{
    for (int i = 0; i < PRINT_TIMES; i++)
    {
        g_iNum++;
        cout<<"ThreadProc1 do print, Num = "<<g_iNum<<endl;

        //signal
        g_myEvent.Set();

        sleep(1);
    }

    return (void *)0;
}

//Thread2
void * ThreadProc2(void *pParam)
{
    bool bRet = false;
    while ( 1 )
    {
        if ( g_iNum >= PRINT_TIMES )
        {
            break;
        }

        //signal
        bRet = g_myEvent.Wait();
        if ( bRet )
        {
            cout<<"ThreadProc2 do print, Num = "<<g_iNum<<endl;

            //signal
            g_myEvent.Reset();
        }
        else
        {
            cout<<"ThreadProc2 system exception"<<endl;
        }
    }

    return (void *)0;
}



using namespace cv;
using namespace std;

cv::Mat recvImg;

wam_srvs::JointMove mv_srv;
ros::ServiceClient Joint_move_client ;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
  {
   try
    {
      recvImg=cv_bridge::toCvShare(msg, "bgr8")->image;//using for simulation display
      namedWindow("Alignment",CV_WINDOW_NORMAL);

      imshow("Alignment", recvImg);
      // cv::waitKey(30);
    }//end of try
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
 }
//===========================MAIN FUNCTION START===========================

int main(int argc, char* argv[]){

  ros::init(argc, argv, "basic");

  ros::NodeHandle nh;

   pthread_t thread1,thread2;
    pthread_attr_t attr1,attr2;


    //create two working threads
    pthread_attr_init(&attr1);
    pthread_attr_setdetachstate(&attr1,PTHREAD_CREATE_JOINABLE);
    if (pthread_create(&thread1,&attr1, ThreadProc1,NULL) == -1)
    {
        cout<<"Thread 1: create failed"<<endl;
    }
    pthread_attr_init(&attr2);
    pthread_attr_setdetachstate(&attr2,PTHREAD_CREATE_JOINABLE);
    if (pthread_create(&thread2,&attr2, ThreadProc2,NULL) == -1)
    {
        cout<<"Thread 2: create failed"<<endl;
    }

    //wait for thread terminated.
    void *result;
    pthread_join(thread1,&result);
    pthread_join(thread2,&result);

    //close thread, release resources
    pthread_attr_destroy(&attr1);
    pthread_attr_destroy(&attr2);

    int iWait;
    cin>>iWait;


  //ros::Publisher pubTask = nh.advertise<signature_visualization::pathData>("/path_data", 1, true);//task will be only published once
  //ros::Publisher pubTask2 = nh.advertise<std_msgs::String>("/chris/strokes", 1, true);//task will be only published once
  //image_transport::ImageTransport it(nh);
  //image_transport::Subscriber subImage = it.subscribe("/camera/rgb/image_rect_color", 1, imageCallback);///camera/rgb/image_rect_color  /usb_cam/image_raw
  //ros::Subscriber sub2 = nh.subscribe("/chris/targetPoints_2D", 1000, signature_data_callback);
  Joint_move_client = nh.serviceClient<wam_srvs::JointMove>("/zeus/wam/joint_move");

  ros::spin();


    //cv::namedWindow("view");
    //cv::startWindowThread();

      //declaration of function
  	//cv::destroyWindow("view");
    return 0;
}
