#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;

unsigned int cols, rows, neighborhood_r, neighborhood_s, x=0,y=0, i, j, counterFissures = 0, counterHole = 0, aux, aux2;
Mat img, gray_img;
cv::Scalar mean_value, stddev;
bool foundFissure = false, foundHole = false;


char* image_window = "Grayscale Image";
char* connected_elements = "Image of connected elements ";
char* result_window = "Histogram";

cv::Mat normalize(cv::Mat const &depth_map)
{
    double min;
    double max;
    cv::minMaxIdx(depth_map, &min, &max);
    cv::Mat adjMap;
    cv::convertScaleAbs(depth_map, adjMap, 255 / max);
    return adjMap;
}

int main()
{
  //gray_img = imread("grieta.jpg", CV_LOAD_IMAGE_GRAYSCALE);
  VideoCapture cap;
  cap.open( "video1.mp4" );

  while(1)
  {
    cap >> img;
    cvtColor(img, gray_img, CV_BGR2GRAY);

    // Check for invalid input
    if(! gray_img.data )    
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( image_window, WINDOW_AUTOSIZE );
    //namedWindow( connected_elements, WINDOW_AUTOSIZE );
    //namedWindow( result_window, WINDOW_AUTOSIZE );

    cols = gray_img.cols;
    rows = gray_img.rows;

    Mat img_result(rows,cols,CV_8U,0.0);

    /****************************************************************************************/
    /***********************************CONNECTED ELEMENTS***********************************/
    /****************************************************************************************/
    //Square kernel
    neighborhood_r = 1;
    neighborhood_s = 1;

    for(i=0;i<rows;i++)
    {
      for(j=0;j<cols;j++)
      { 
        //Special cases (if pixel is in corner, ...)
        if(i==0 && j==0)                                                                //·#
        {                                                                               //##
          Mat neighborhood(neighborhood_r+1,neighborhood_s+1,CV_8S,0.0);                  
          for(x=0;x<neighborhood_r;x++)     
          {
            for(y=0;y<neighborhood_s;y++)
            {
              neighborhood.at<uchar>(Point(x,y)) =  gray_img.at<uchar>(Point(x,y));
            }
          }
          meanStdDev(normalize(neighborhood), mean_value, stddev, cv::Mat());
        }
        else if(i==0 && j==cols)                                                        //#·
        {                                                                               //##
          Mat neighborhood(neighborhood_r+1,neighborhood_s+1,CV_8S,0.0);                 
          for(x=0;x<neighborhood_r;x++)
          {
            for(y=0;y<neighborhood_s;y++)
            {
              neighborhood.at<uchar>(x,y) = gray_img.at<uchar>(i+x,j-neighborhood_s+y);
            }
          }
          meanStdDev(normalize(neighborhood), mean_value, stddev, cv::Mat());
        }
        else if(i==rows && j==cols)                                                     //##
        {                                                                               //#·
          Mat neighborhood(neighborhood_r+1,neighborhood_s+1,CV_8S,0.0);                  
          for(x=0;x<neighborhood_r;x++)
          {
            for(y=0;y<neighborhood_s;y++)
            {
              neighborhood.at<uchar>(x,y) = gray_img.at<uchar>(i-neighborhood_r+x,j-neighborhood_s+y);
            }
          }
          meanStdDev(normalize(neighborhood), mean_value, stddev, cv::Mat());
        }
        else if(i==0 && j!=0 && j!= cols)                                               //#·#
        {                                                                               //###
          Mat neighborhood(neighborhood_r+1,neighborhood_s+2,CV_8S,0.0);                  
          for(x=0;x<neighborhood_r;x++)
          {
            for(y=0;y<2*neighborhood_s;y++)
            {
              neighborhood.at<uchar>(x,y) = gray_img.at<uchar>(i+x,j-neighborhood_s+y);
            }
          }
          meanStdDev(normalize(neighborhood), mean_value, stddev, cv::Mat());
        }
        else if(i==rows && j!=0 && j!= cols)                                            //###
        {                                                                               //#·#
          Mat neighborhood(neighborhood_r+1,neighborhood_s+2,CV_8S,0.0);                   
          for(x=0;x<neighborhood_r;x++)
          {
            for(y=0;y<2*neighborhood_s;y++)
            {
              neighborhood.at<uchar>(x,y) = gray_img.at<uchar>(i-neighborhood_r+x,j-neighborhood_s+y);
            }
          }
          meanStdDev(normalize(neighborhood), mean_value, stddev, cv::Mat());
        }
        else if(i==rows && j==0)                                                        //##
        {                                                                               //·#
          Mat neighborhood(neighborhood_r+1,neighborhood_s+1,CV_8S,0.0);       
          for(x=0;x<neighborhood_r;x++)     
          {
            for(y=0;y<neighborhood_s;y++)
            {
              neighborhood.at<uchar>(x,y) = gray_img.at<uchar>(i-neighborhood_r+x,j+y);
            }
          }               
          meanStdDev(normalize(neighborhood), mean_value, stddev, cv::Mat());
        }
        else if(i!=rows && i!=0 && j==0)                                                 //##
        {                                                                                //·#
          Mat neighborhood(neighborhood_r+1,neighborhood_s+1,CV_8S,0.0);                 //##
          for(x=0;x<2*neighborhood_r;x++)     
          {                   
            for(y=0;y<neighborhood_s;y++)
            {
              neighborhood.at<uchar>(x,y) = gray_img.at<uchar>(i-neighborhood_r+x,j+y);
            }
          } 
          meanStdDev(normalize(neighborhood), mean_value, stddev, cv::Mat());
        }                     
        else if(i!=rows && i!=0 && j==cols)                                               //##
        {                                                                                 //#·
          Mat neighborhood(neighborhood_r+2,neighborhood_s+1,CV_8S,0.0);                  //##
          for(x=0;x<2*neighborhood_r;x++)     
          {                   
            for(y=0;y<neighborhood_s;y++)
            {
              neighborhood.at<uchar>(x,y) = gray_img.at<uchar>(i-neighborhood_r+x,j-y);
            }
          }
          meanStdDev(normalize(neighborhood), mean_value, stddev, cv::Mat());
        }                     
        else                                                                              //###
        {                                                                                 //#·# 
          Mat neighborhood(neighborhood_r+2,neighborhood_s+2,CV_8S,0.0);                  //###               
          for(x=0;x<2*neighborhood_r;x++)     
          {
            for(y=0;y<2*neighborhood_s;y++)
            {
              neighborhood.at<uchar>(x,y) = gray_img.at<uchar>(i-neighborhood_r+x,j-neighborhood_s+y);
            }
          }
          meanStdDev(normalize(neighborhood), mean_value, stddev, cv::Mat());
        }
        
        //Checking standard deviation value
        if(stddev[0] < 0.1)
        {
          img_result.at<uchar>(i,j) = mean_value[0];
        }
        else
        {
          img_result.at<uchar>(i,j) = gray_img.at<uchar>(i,j);
        }

        //Assigning white value to pixels corresponding to an imperfection grayscale value
        if(img_result.at<uchar>(i,j) > 1 && img_result.at<uchar>(i,j) < 50)
        {
          gray_img.at<uchar>(i,j) = 250;
          if(counterFissures == 0)
            aux = j;
          //summing up white pixels in the same row
          counterFissures++;
        }
      }
      if(counterFissures > 50)
      {
        aux2 = i;
        foundFissure = true;
      }
      counterFissures = 0;
    }
    //If there is a fissure we draw a rectangle on it
    if(foundFissure)
    {
      rectangle(gray_img, Point(aux,aux2-10), Point(aux+100,aux2+20), Scalar::all(0), 2, 8, 0);
      putText(gray_img,"Fissure",Point(aux,aux2-15),FONT_HERSHEY_SIMPLEX, 1, 0, 2, 8, false);  
    }
    foundFissure = false;
    
    for(i=0;i<rows;i++)
    {
      for(j=0;j<cols;j++)
      { 
        if(gray_img.at<uchar>(i,j) == 250)
        {
          //Searching for a 10 x 10 white area (i.e. hole)
          for(int a=i;a<i+10;a++)
          {
            for(int  b=j;b<j+10;b++)
            { 
              if(gray_img.at<uchar>(a,b) == 250)
              {
                counterHole++;
              }
              else
              {
                counterHole = 0;
              }
            }
          }
          if(counterHole == 20)
          {
            foundHole = true;
          }
          counterHole = 0;
          //If there is a hole we draw a rectangle on it
          if(foundHole)
          {
            rectangle(gray_img, Point(j-10,i-10), Point(j+20,i+20), Scalar::all(0), 2, 8, 0);
            putText(gray_img,"Hole",Point(j,i-5),FONT_HERSHEY_SIMPLEX, 1, 0, 2, 8, false);
          }
          foundHole = false;          
        }
      }
    }

    putText(gray_img,"Seeking",Point(5,rows-10),FONT_HERSHEY_SIMPLEX, 1, 0, 2, 8, false);
    int histogram[256];

    // initialize all intensity values to 0
    for(int i = 0; i < 255; i++)
    {
        histogram[i] = 0;
    }

    // calculate the nº of pixels for each intensity values
    for(int y = 0; y < img_result.rows; y++)
        for(int x = 0; x < img_result.cols; x++)
            histogram[(int)img_result.at<uchar>(y,x)]++;

    // draw the histograms
    int hist_w = 255; int hist_h = 400;
    int bin_w = cvRound((double) hist_w/256);

    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255));

    // find the maximum intensity element from histogram
    int max = histogram[0];
    for(int i = 1; i < 256; i++){
        if(max < histogram[i]){
            max = histogram[i];
        }
    }

    // normalize the histogram between 0 and histImage.rows
    for(int i = 0; i < 255; i++){
        histogram[i] = ((double)histogram[i]/1500)*histImage.rows;
    }

    // draw the intensity line for histogram
    for(int i = 0; i < 255; i++)
    {
        line(histImage, Point(bin_w*(i), hist_h),
                              Point(bin_w*(i), hist_h - histogram[i]),
             Scalar(0,0,0), 1, 8, 0);
    }

    imshow(image_window, gray_img);
    //imshow(connected_elements, img_result);
    //imshow(result_window, histImage);
    waitKey(1);
  }  
  return 0;
}
 
