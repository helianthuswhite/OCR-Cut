#include <opencv/cv.hpp> 
#include <opencv2/highgui.hpp>  
#include <opencv/cxcore.h> 
using namespace cv;  
  
  
int main(int argc, char* argv[])  
{  
    IplImage* imgSrc = cvLoadImage("test_img.png",CV_LOAD_IMAGE_COLOR);  
    IplImage* img_gray = cvCreateImage(cvGetSize(imgSrc), IPL_DEPTH_8U, 1);  
    cvCvtColor(imgSrc, img_gray, CV_BGR2GRAY);  
    cvThreshold(img_gray, img_gray,100, 255,CV_THRESH_BINARY_INV);// CV_THRESH_BINARY_INV使得背景为黑色，字符为白色，这样找到的最外层才是字符的最外层  
    cvShowImage("ThresholdImg",img_gray);  
    CvSeq* contours = NULL;  
    CvMemStorage* storage = cvCreateMemStorage(0);    
    int count = cvFindContours(img_gray, storage, &contours,  
        sizeof(CvContour),CV_RETR_EXTERNAL);  
    // cvReleaseMemStorage(& storage);   
    printf("轮廓个数：%d",count);  
    int idx = 0;  
    char szName[56] = {0};  
    int tempCount=0;  
    for (CvSeq* c = contours; c != NULL; c = c->h_next) {  
         CvRect rc =cvBoundingRect(c,0);  
         cvDrawRect(imgSrc, cvPoint(rc.x, rc.y), cvPoint(rc.x + rc.width, rc.y + rc.height), CV_RGB(255, 0, 0));  
  
      // if () {这里可以根据轮廓的大小进行筛选,上面源图片有瑕疵可以用腐蚀，膨胀来祛除  
      //    continue;  
     //  }  
          
        IplImage* imgNo = cvCreateImage(cvSize(rc.width, rc.height), IPL_DEPTH_8U, 3);  
        cvSetImageROI(imgSrc, rc);  
        cvCopyImage(imgSrc, imgNo);  
        cvResetImageROI(imgSrc);  
        sprintf(szName, "wnd_%d", idx++);   
        cvNamedWindow(szName);  
        cvShowImage(szName, imgNo);  
        cvReleaseImage(&imgNo);
    }  
      
    cvNamedWindow("src");  
    cvShowImage("src", imgSrc);  
    cvWaitKey(0);  
    cvReleaseMemStorage(&storage);  
    cvReleaseImage(&imgSrc);  
    cvReleaseImage(&img_gray);  
    cvDestroyAllWindows();  
    return 0;  
}  