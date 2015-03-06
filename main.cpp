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

    CvRect roi = cvRect(150,0,400,480);
    cvNamedWindow("fore");
    cvNamedWindow("contoured");

    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *contour = NULL ,*hull = NULL;

    BackGround = cvLoadImage("E:/Study_Resource/OPENCV_Project/photo/001.bmp");
         frame = cvLoadImage("E:/Study_Resource/OPENCV_Project/photo/002.bmp");

    BackGround_gray   =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
         frame_gray   =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
         fore         =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
         temp         =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);cvZero(temp);
         fore_edge    =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
         contoured    =   cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);

    cvZero(BackGround_gray);
    cvZero(frame_gray);
    cvZero(fore);
    cvZero(fore_edge);
    cvZero(contoured);

/**********************************************
 * 将读取的图片灰度化，然后帧差去前景，开运算，阈值化二值化
 *********************************************/
    cvCvtColor(BackGround,BackGround_gray,CV_BGR2GRAY);
    cvCvtColor(frame,frame_gray,CV_BGR2GRAY);

    cvAbsDiff(BackGround_gray,frame_gray,fore);

    IplConvKernel* kernel =cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT);

    cvMorphologyEx(fore,fore,temp,kernel,CV_MOP_OPEN,2);
    cvDilate(fore,fore);

    cvThreshold(fore,fore_edge,35,255,CV_THRESH_BINARY);

/**********************************************
 * 轮廓扫描，找到凸包，还有凸缺陷
 * ******************************************
    CvContourScanner scanner = cvStartFindContours(fore,storage);
    while((contour = cvFindNextContour(scanner)) != NULL)
    {
        cvCvtColor(fore,contoured,CV_GRAY2BGR);

        cvDrawContours(contoured,contour,CV_RGB(255,0,0),CV_RGB(0,255,0),0);

        cout<<cvCheckContourConvexity(contour)<<endl;

        hull = cvConvexHull2(contour,0,CV_CLOCKWISE,0);

        CvPoint pt0 = **(CvPoint**)cvGetSeqElem(hull,hull->total-1);
        for(int i = 0;i<hull->total;++i)
        {
            CvPoint pt1 = **(CvPoint**)cvGetSeqElem(hull,i);
            cvLine(contoured,pt0,pt1,CV_RGB(0,0,255));
            pt0 = pt1;
        }

        CvSeq *defect = cvConvexityDefects(contour,hull);

        for(int i = 0; i<defect->total; ++i)
        {
            CvConvexityDefect df = *(CvConvexityDefect*)cvGetSeqElem(defect,i);
            cvCircle(contoured,*df.start,2,CV_RGB(255,100,0),-1);
            cvCircle(contoured,*df.end,2,CV_RGB(255,255,0),-1);
            cvCircle(contoured,*df.depth_point,2,CV_RGB(255,255,0),-1);

            cout <<"depth of df="<<&df.depth<<endl;

        }

        cvShowImage("contoured",fore);

    }*/

/**********************************************************
 * 网上的不好使，在学习opencv的书上找到例子，然后自己试着写
 * ********************************************************/
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
/*        printf("Contour #%d\n",n);
//        cvShowImage("contored",frame);

//        printf(" %d elements:\n", c->total);
//        for(int i =0; i<c->total; ++i)
//        {
//            CvPoint* p = CV_GET_SEQ_ELEM(CvPoint,c,i);
//            printf(" (%d,%d) \n", p->x,p->y);
//
//        }
//        cvWaitKey(0);
        n++;  */
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
//                cvCircle(frame,*df.start,20,CV_RGB(255,255,0),2);
//                cvCircle(frame,*df.end,20,CV_RGB(255,255,0),2);
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

    cv::waitKey(0);
    cvDestroyAllWindows();


    return 0;
}

