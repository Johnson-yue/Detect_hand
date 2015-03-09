#include <iostream>
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
#include<cvaux.h>


using namespace std;

int main()
{
    IplImage *BackGround, *BackGround_gray;
    IplImage *frame, *frame_gray ;
    IplImage *fore;
    IplImage *contoured;
    IplImage *fore_edge;
    IplImage *temp;

    IplImage *standard;

    CvRect roi = cvRect(150,0,400,480);
    cvNamedWindow("fore");
    cvNamedWindow("contoured");

    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *contour = NULL ,*hull = NULL;

    BackGround = cvLoadImage("E:/Study_Resource/OPENCV_Project/photo/001.bmp");
         frame = cvLoadImage("E:/Study_Resource/OPENCV_Project/photo/002.bmp");

      standard = cvLoadImage("E:/Study_Resource/OPENCV_Project/photo/standard.bmp");
      IplImage* hsv = cvCreateImage(cvGetSize(standard),8,3);
      cvCvtColor(standard,hsv,CV_BGR2HSV);

    BackGround_gray   =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
         frame_gray   =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
         fore         =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
         temp         =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);cvZero(temp);
         fore_edge    =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
         contoured    =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);

    IplImage* h_plane = cvCreateImage(cvGetSize(standard),8,1);cvZero(h_plane);
    IplImage* s_plane = cvCreateImage(cvGetSize(standard),8,1);cvZero(s_plane);
    IplImage* v_plane = cvCreateImage(cvGetSize(standard),8,1);cvZero(v_plane);
    IplImage* planes[]= {h_plane, s_plane, v_plane};
    cvCvtPixToPlane(hsv,h_plane, s_plane, v_plane,0);

    int h_bins=30, s_bins=32;

    cvZero(BackGround_gray);
    cvZero(frame_gray);
    cvZero(fore);
    cvZero(fore_edge);
    cvZero(contoured);
/*****************************************************
 * 建立直方图函数
 * ****************************************************/
    CvHistogram* hist;
    {
        int hist_size[] = {h_bins, s_bins};
      float  h_ranges[] = {0, 180};
      float  s_ranges[] = {0, 255};
      float* ranges[]   = {h_ranges,s_ranges};
      hist = cvCreateHist(2,hist_size,CV_HIST_ARRAY,ranges,1);
    }
    cvCalcHist(planes, hist,0,0);
    cvNormalizeHist(hist,1.0);

    int scale = 10;

    IplImage* hist_img = cvCreateImage(cvSize(h_bins*scale,s_bins*scale),8,3);cvZero(hist_img);
    float max_value = 0;
    cvGetMinMaxHistValue(hist,0,&max_value,0,0);

    for(int h =0; h<h_bins; h++){
        for(int s = 0; s<s_bins;s++)
            {
                float bin_val = cvQueryHistValue_2D(hist,h,s);
                int intensity = cvRound(bin_val*255/max_value);
                cvRectangle(hist_img,cvPoint(h*scale,s*scale),cvPoint((h+1)*scale-1,(s+1)*scale-1),
                            CV_RGB(intensity,intensity,intensity),CV_FILLED);
            }
    }
    cvNamedWindow("standard");
    cvNamedWindow("hist");

    cvShowImage("standard",standard);
    cvShowImage("hist",hist_img);

    IplImage* img2 = cvLoadImage("E:/Study_Resource/OPENCV_Project/photo/002.bmp");
    IplImage* hsv2 = cvCreateImage(cvGetSize(img2), IPL_DEPTH_8U, 3);
    cvCvtColor(img2, hsv2, CV_BGR2HSV);
    IplImage* h2 = cvCreateImage(cvGetSize(img2), IPL_DEPTH_8U, 1);
    IplImage* s2 = cvCreateImage(cvGetSize(img2), IPL_DEPTH_8U, 1);
    IplImage* v2 = cvCreateImage(cvGetSize(img2), IPL_DEPTH_8U, 1);
    IplImage* planes2[] = {h2, s2};
    cvCvtPixToPlane(hsv2, h2, s2, v2, 0);

    IplImage* backProject = cvCreateImage(cvGetSize(img2), IPL_DEPTH_8U, 1);
    cvSetZero(backProject);
//    cvNormalizeHist(hist, sum);
    cvCalcBackProject(planes2, backProject, hist);


    cvNamedWindow("img2");
    cvShowImage("img2", img2);
    cvNamedWindow("back project");
    cvShowImage("back project", backProject);
/**********************************************
 * 将读取的图片灰度化，然后帧差去前景，开运算，阈值化二值化
 ********************************************
    cvCvtColor(BackGround,BackGround_gray,CV_BGR2GRAY);
    cvCvtColor(frame,frame_gray,CV_BGR2GRAY);

    cvAbsDiff(BackGround_gray,frame_gray,fore);

    IplConvKernel* kernel =cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT);

    cvMorphologyEx(fore,fore,temp,kernel,CV_MOP_OPEN,2);
    cvDilate(fore,fore);

    cvThreshold(fore,fore_edge,35,255,CV_THRESH_BINARY);

*/
/**********************************************************
 * 网上的不好使，在学习opencv的书上找到例子，然后自己试着写
 * *******************************************************
    cvSetImageROI(fore_edge,roi);
    cvShowImage("fore",fore_edge);
    CvContourScanner scanner = cvStartFindContours(fore_edge,storage);

    int Nc = cvFindContours(fore_edge,storage,&contour,sizeof(CvContour),CV_RETR_LIST);
    int n = 0;

    printf("Total Contours Detected: %d\n",Nc);

    for(CvSeq* c = contour; c!=NULL; c=c->h_next)
    {
        if(c->total>40){                    //剔除噪声轮廓
        cvDrawContours(frame,c,CV_RGB(255,0,0),CV_RGB(0,0,255),0,2);

        printf("c.total = %d\n", c->total);

        cout<<cvCheckContourConvexity(contour)<<endl;

        hull = cvConvexHull2(contour,0,CV_CLOCKWISE,0);              //根据已知的轮廓计算凸包

        CvPoint pt0 = **(CvPoint**)cvGetSeqElem(hull,hull->total-1);

            for(int i=0; i<hull->total; ++i)
            {
                CvPoint pt1 = **(CvPoint**)cvGetSeqElem(hull,i);
                cvLine(frame,pt0,pt1,CV_RGB(255,0,255),3);  //pink
                pt0 = pt1;
                printf("pt0 = < %d , %d>\n",pt0.x,pt0.y);
            }

        CvSeq *defect =   cvConvexityDefects(contour,hull);
        printf("defect`s total = %d\n",defect->total);

            for(int i = 0; i<defect->total; ++i)
            {
                CvConvexityDefect df = *(CvConvexityDefect*)cvGetSeqElem((defect),i);

                if((int)df.depth > 20){
                cvCircle(frame,*df.depth_point,5,CV_RGB(0,255,255),2);
                printf("depth = %f \n",df.depth);
                printf("00000000\n");
                }
            }

            cvShowImage("contored",frame);
            cv::waitKey(0);
            n++;

        }
    }

    printf("Finished all contours.\n");

    cvEndFindContours(&scanner);
*/
    cv::waitKey(0);
    cvDestroyAllWindows();


    return 0;
}

