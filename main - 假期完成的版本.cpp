#include <iostream>
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>


using namespace std;
/*
void GetFore(const IplImage* src1,const IplImage* src2,IplImage *dst){
    dst = cvCreateImage(cvGetSize(src1),IPL_DEPTH_8U,1);cvZero(dst);

    int width = src1->width;
    int heigh = src1->height;
    uchar *data1 = (uchar *)src1->imageData;
    uchar *data2 = (uchar *)src2->imageData;
    uchar *data3 = (uchar *)dst->imageData;

    int step = src1->widthStep/sizeof(uchar);


    for(int j=0; j<heigh; j++)
        for(int i=0; i<width; i++)
            if(data1[j*step+i]>data2[j*step+i])
                *(uchar*)dst->imageData = data1[j*step+i] - data2[j*step+i];
            else
                *(uchar*)dst->imageData = data2[j*step+i] - data1[j*step+i];


}*/

int main( ){
    cvNamedWindow("fore");
    cvNamedWindow("frame");
    cvNamedWindow("BackGround_gray");

    CvCapture* capture;
    capture= cvCreateCameraCapture(0);

    IplImage *frame, *frame_gray;
    IplImage *BackGround, *BackGround_gray;
    IplImage *fore, *fore_edge;
    IplImage *contoured;
    IplImage *temp;

    CvRect roi = cvRect(150,0,400,480);
    CvSize size = cvSize(640,480);
    int init = 0;

    CvMemStorage *storage = cvCreateMemStorage(0);


    CvSeq *contour = NULL, *hull = NULL;

    IplConvKernel* kernel = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT);

       BackGround   =   cvCreateImage(size,IPL_DEPTH_8U,3);cvZero(BackGround);
        frame_gray  =   cvCreateImage(size,IPL_DEPTH_8U,1);cvZero(frame_gray);
    BackGround_gray =   cvCreateImage(size,IPL_DEPTH_8U,1);cvZero(BackGround_gray);
        fore        =   cvCreateImage(size,IPL_DEPTH_8U,1);cvZero(fore);
        fore_edge   =   cvCreateImage(size,IPL_DEPTH_8U,1);cvZero(fore_edge);
        temp        =   cvCreateImage(size,IPL_DEPTH_8U,1);cvZero(temp);
        contoured   =   cvCreateImage(size,IPL_DEPTH_8U,3);cvZero(contoured);


    while (1) {
        while(init == 0)
        {
            frame = cvQueryFrame(capture);
            cvCvtColor(frame,frame_gray,CV_BGR2GRAY);
            cvShowImage("frame",frame_gray);
            cvCopy(frame_gray,BackGround_gray);
            if(BackGround_gray!=NULL){
                cvShowImage("BackGround_gray",BackGround_gray);
                init = 1;
                break;
            }
        }
        frame = cvQueryFrame(capture);       
        cvCvtColor(frame,frame_gray,CV_BGR2GRAY);
//        cvShowImage("frame",frame_gray);

        cvAbsDiff(BackGround_gray,frame_gray,fore);
        cvMorphologyEx(fore,fore,temp,kernel,CV_MOP_OPEN,2);
        cvDilate(fore,fore);

        cvThreshold(fore,fore_edge,50,255,CV_THRESH_BINARY);
//s        cvShowImage("fore",fore_edge);
/***********************************************
 * 输入图像：二值化以后的图像
 * 输出图像：有边缘的凸包的图像
 * ***********************************************/
        cvSetImageROI(fore_edge,roi);

        CvContourScanner scanner = cvStartFindContours(fore_edge,storage);
        int Nc = cvFindContours(fore_edge,storage,&contour,sizeof(CvContour));
        int n = 0;
        printf("Total contours Detected :%d \n",Nc);

        for(CvSeq*c = contour;c !=NULL; c=c->h_next)
        {
            if(c->total>40){
                cvDrawContours(frame,c,CV_RGB(255,0,0),CV_RGB(0,0,255),0,2);
                printf("c.total = %d\n",c->total);
                cout<<cvCheckContourConvexity(contour)<<endl;

                hull = cvConvexHull2(contour);

                CvPoint pt0 = **(CvPoint**)cvGetSeqElem(hull,hull->total-1);

                for(int i=0;i<hull->total; ++i){
                    CvPoint pt1 = **(CvPoint**)cvGetSeqElem(hull,i);
                    cvLine(frame,pt0,pt1,CV_RGB(255,0,255),3);
                    pt0 = pt1;
                    printf("pt0 = <%d, %d>\n",pt0.x,pt0.y);
                }

                CvSeq *defect = cvConvexityDefects(contour,hull);
                printf("defect`s total = %d\n",defect->total);

                for(int i=0; i<defect->total; ++i){
                    CvConvexityDefect df = *(CvConvexityDefect*)cvGetSeqElem((defect),i);
                    if((int)df.depth>20){
                        cvCircle(frame,*df.depth_point,5,CV_RGB(0,255,255),2);
                        printf("depth = %f \n",df.depth);
                    }
                }
                cvShowImage("frame",frame);
                cvWaitKey(25);
                n++;
            }
        }
        cvEndFindContours(&scanner);
        cvResetImageROI(fore_edge);
        cvShowImage("fore",fore_edge);
        printf("Finished all\n");
        cvWaitKey(33);

    }

    return 0;
}
