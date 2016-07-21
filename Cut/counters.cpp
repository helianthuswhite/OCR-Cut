#include <opencv/cv.h>
#include <opencv2/highgui.hpp>  
#include <opencv/cxcore.h> 

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/test_img2.png"

using namespace cv;  
  

int main(int argc, char* argv[])  
{
//    加载图像
    IplImage* imgSrc = cvLoadImage(IMAGE,CV_LOAD_IMAGE_COLOR);
//    将图像转换为灰度图
    IplImage* img_gray = cvCreateImage(cvGetSize(imgSrc), IPL_DEPTH_8U, 1);
    cvCvtColor(imgSrc, img_gray, CV_BGR2GRAY);
//    二值化处理
    cvThreshold(img_gray, img_gray,100, 255,CV_THRESH_BINARY_INV);// CV_THRESH_BINARY_INV使得背景为黑色，字符为白色，这样找到的最外层才是字符的最外层
    cvShowImage("ThresholdImg",img_gray);
    CvSeq* contours = NULL;  
    CvMemStorage* storage = cvCreateMemStorage(0);
//    检索轮廓并返回检测到的轮廓数量
    int count = cvFindContours(img_gray, storage, &contours,
        sizeof(CvContour),CV_RETR_EXTERNAL);  
    // cvReleaseMemStorage(& storage);   
    printf("轮廓个数：%d",count);  
    int idx = 0;  
    char szName[56] = {0};  
//    int tempCount=0;
//    显示绘制的结果
    for (CvSeq* c = contours; c != NULL; c = c->h_next) {
         CvRect rc =cvBoundingRect(c,0);  
         cvDrawRect(imgSrc, cvPoint(rc.x, rc.y), cvPoint(rc.x + rc.width, rc.y + rc.height), CV_RGB(255, 0, 0));  
  
      // if () {这里可以根据轮廓的大小进行筛选,上面源图片有瑕疵可以用腐蚀，膨胀来祛除  
      //    continue;  
     //  }  
//          设置ROI并显示分割后的结果
        IplImage* imgNo = cvCreateImage(cvSize(rc.width, rc.height), IPL_DEPTH_8U, 3);
        cvSetImageROI(imgSrc, rc);  
        cvCopyImage(imgSrc, imgNo);  
        cvResetImageROI(imgSrc);  
        sprintf(szName, "wnd_%d", idx++);   
        cvNamedWindow(szName);  
        cvShowImage(szName, imgNo);  
        cvReleaseImage(&imgNo);
    }  
//      后续的处理
    cvNamedWindow("src");
    cvShowImage("src", imgSrc);  
    cvWaitKey(0);  
    cvReleaseMemStorage(&storage);  
    cvReleaseImage(&imgSrc);  
    cvReleaseImage(&img_gray);  
    cvDestroyAllWindows();  
    return 0;  
}  