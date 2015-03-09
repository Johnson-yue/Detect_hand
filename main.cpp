#include <iostream>
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
#include<cvaux.h>


using namespace std;
IplImage *HalfHand(IplImage *input);
void FindContour(IplImage *src, IplImage *display);
int main()
{
    IplImage *src = cvLoadImage("E:/Study_Resource/OPENCV_Project/photo/001.bmp");
    IplImage *srcHSV = cvCreateImage(cvGetSize(src),8,3);
    cvCvtColor(src,srcHSV,CV_BGR2HSV);

    IplImage *srcH = cvCreateImage(cvGetSize(src),8,1);cvZero(srcH);
    IplImage *srcS = cvCreateImage(cvGetSize(src),8,1);cvZero(srcS);
    IplImage *srcV = cvCreateImage(cvGetSize(src),8,1);cvZero(srcV);
    cvCvtPixToPlane(srcHSV,srcH,srcS,srcV,0);


    IplImage *frame = cvLoadImage("E:/Study_Resource/OPENCV_Project/photo/009.bmp");
    IplImage *frameHSV = cvCreateImage(cvGetSize(frame),8,3);
    cvCvtColor(frame,frameHSV,CV_BGR2HSV);

    IplImage *frameH = cvCreateImage(cvGetSize(frame),8,1);cvZero(frameH);
    IplImage *frameS = cvCreateImage(cvGetSize(frame),8,1);cvZero(frameS);
    IplImage *frameV = cvCreateImage(cvGetSize(frame),8,1);cvZero(frameV);
    cvCvtPixToPlane(frameHSV,frameH,frameS,frameV,0);

    cvAbsDiff(srcH,frameH,frameH);
    cvAbsDiff(srcS,frameS,frameS);
    cvAbsDiff(srcV,frameV,frameV);

    cvThreshold(frameV,frameV,20,255,CV_THRESH_BINARY);  //对V通道阈值化
    IplConvKernel* kernel = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT); //膨胀/腐蚀的kernel
    IplImage *temp = cvCreateImage(cvGetSize(frameS),8,1);cvZero(temp);


    cvDilate(frameV,temp,kernel,2);
    cvErode(temp,temp,kernel,7);
    cvDilate(temp,temp,kernel,3);

    frameHSV = HalfHand(temp);

    cvNamedWindow("frameV");
    cvNamedWindow("frameHSV");
    cvNamedWindow("frameS");

    cvShowImage("frameV",frameV);
    cvShowImage("frameHSV",frameHSV);
    cvShowImage("frameS",temp);

    cvNamedWindow("display");
    FindContour(frameHSV,frame);

    cvWaitKey(0);
    cvDestroyAllWindows();

    return 0;
}
/*********************************************************
 * 函数名称: HalfHand
 * 函数功能：在二值化以后的图像中统计前60%非零像素点
 * 补充说明：因为手是从图像右侧进入，且占据图像大约70%的部分，
 *         所以将图像左侧30%的非零像素点视为噪点，忽略掉。
 * 函数输入：一幅灰度图像
 * 函数输出：一幅灰度图像（0，128，255三种灰度）
 * 涉及功能：图像遍历、像素级处理
 ********************************************************/
IplImage *HalfHand(IplImage *input){

    int heigh = input->height;
    int width = input->width;
    int SumPix = heigh*width;               //计算总像素数
    int step = input->widthStep;
    uchar *data = (uchar*)input->imageData;

    int NonZero = cvCountNonZero(input);    //计算非零像素数
    float NonZeroPrecent ;
    int num =cvCountNonZero(input)/10*6;     //对前60%非零像素不处理，保留（注：此值应于alpha的取值有关）
    int sum = 0;
    int stop = 0;

    NonZeroPrecent = (float)NonZero/SumPix*100;//计算 非零像素在总像素中所占的比例 记为系数——alpha

    printf("SumPix = %d\nNonZero = %d\nNonZeroPrecent = %f\n",SumPix,NonZero,NonZeroPrecent);

    for(int j=0;j<width;j++){
        for(int i =0; i<heigh; i++){
            if((data[i*step+j]!=0) && (j<=width/10*3))  //将图像左侧30%部分视为噪点忽略掉
            {
                data[i*step+j] = 0;
            }
            else {
                if((data[i*step+j]!=0) && (stop == 0))
                {
                    sum++;
                    if(num == sum) stop=1;
                }
                else   if((data[i*step+j]!=0) && (stop == 1))
                {
                    data[i*step+j] = 0;                 //对后40%的非零像素做处理
                }
            }

        }
    }
    return input;
}
/****************************************************
 * 函数名称：FindContour
 * 函数功能：找到二值图像中的轮廓
 * 函数输入：一幅二值图像 (IplImage *src)
 * 函数输出：一幅带有二值图像轮廓的BGR图像(IplImage *display)
 *******************************************************/
void FindContour(IplImage *src, IplImage *display){

    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *contour = NULL, *hull = NULL;

    CvContourScanner scanner = cvStartFindContours(src,storage);

    int Nc = cvFindContours(src,storage,&contour,sizeof(CvContour));
    int n = 0;

    printf("Total contours Detected :%d \n",Nc);

    for(CvSeq*c = contour;c !=NULL; c=c->h_next)
    {
        if(c->total>40){
            cvDrawContours(display,c,CV_RGB(255,0,0),CV_RGB(0,0,255),0,2);
            printf("c.total = %d\n",c->total);
            cout<<cvCheckContourConvexity(contour)<<endl;

            hull = cvConvexHull2(contour);

            CvPoint pt0 = **(CvPoint**)cvGetSeqElem(hull,hull->total-1);

            for(int i=0;i<hull->total; ++i){
                CvPoint pt1 = **(CvPoint**)cvGetSeqElem(hull,i);
                cvLine(display,pt0,pt1,CV_RGB(255,0,255),3);
                pt0 = pt1;
                printf("pt0 = <%d, %d>\n",pt0.x,pt0.y);
            }

            CvSeq *defect = cvConvexityDefects(contour,hull);
            printf("defect`s total = %d\n",defect->total);

            for(int i=0; i<defect->total; ++i){
                CvConvexityDefect df = *(CvConvexityDefect*)cvGetSeqElem((defect),i);
                if((int)df.depth>20){
                    cvCircle(display,*df.depth_point,5,CV_RGB(0,255,255),2);
                    printf("depth = %f \n",df.depth);
                }
            }
            cvShowImage("display",display);
            cvWaitKey(0);
            n++;
        }
    }

    cvEndFindContours(&scanner);
    printf("Find finished \n");
}









