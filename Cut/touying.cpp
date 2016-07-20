#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <opencv/cxcore.h>

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/test_img4.png"

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
    //存储投影值的数组
    int* h = new int[img_gray->height];
    for(int y = 0;y < img_gray->height;y++) {
        h[y] = 0;
    }
    //对每一行计算投影值
    for(int y = 0;y < img_gray->height;y++)
    {
        uchar* ptr = (uchar*)(img_gray->imageData + y*img_gray->widthStep);
        //遍历这一行的每一个像素，如果是有效的，累加投影值
        for(int x = 0;x < img_gray->width;x++)
        {
            if(ptr[x] == 255)
                h[y]++;
        }
    }
    
    //准备一个图像用于画投影图
    IplImage* paintY = cvCreateImage(cvGetSize(img_gray), IPL_DEPTH_8U, 1);
    cvZero(paintY);
    
    //画图
//    CvScalar t= cvScalar(255,0,0,0);
//    是否进入投影区
    bool inWord = false;
    //分割开始位置和结束位置
    int start = 0,end = 0;
//    char szName[56] = {0};
//    获得行数
    int HEIGHT = paintY->height;
//    存储每一行的图片
    IplImage* imgNo[HEIGHT];
    int i = 0;
    //存储
    for(int y = 0;y < HEIGHT;y++)
    {
        if(!inWord && h[y] > 0) {
            inWord = true;
            start = y;
        }else if(inWord && h[y] == 0) {
            inWord = false;
            end = y;
            imgNo[i] = cvCreateImage(cvSize(img_gray->width,end-start+1), IPL_DEPTH_8U, 1);
            cvSetImageROI(img_gray, cvRect(0, start, imgNo[i]->width, imgNo[i]->height));
            cvCopyImage(img_gray, imgNo[i]);
            cvResetImageROI(img_gray);
            i++;
//            sprintf(szName, "windowstart_%d", start);
//            cvNamedWindow(szName);
//            cvShowImage(szName, imgNo[y]);
//            cvReleaseImage(&imgNo[y]);
        }
//        printf("%d\n",h[y]);
    }
    
    
//    准备对列进行投影
    //存储投影值的数组
    int* w = new int[imgNo[0]->width];
    for(int x = 0;x < imgNo[0]->width;x++) {
        w[x] = 0;
    }
    //对每一列计算投影值
    for(int y = 0;y < imgNo[0]->height;y++)
    {
        uchar* ptr = (uchar*)(imgNo[0]->imageData + y*imgNo[0]->widthStep);
        //遍历这一行的每一个像素，如果是有效的，累加投影值
        for(int x = 0;x < img_gray->width;x++)
        {
            if(ptr[x] == 255)
                w[x]++;
        }
    }
    
    //准备一个图像用于画投影图
    IplImage* paintX = cvCreateImage(cvGetSize(imgNo[0]), IPL_DEPTH_8U, 1);
    cvZero(paintX);
    
    
    //画图
    CvScalar t= cvScalar(255,0,0,0);
    //    是否进入投影区
    inWord = false;
    //分割开始位置和结束位置
    start = 0,end = 0;
//        char szName[56] = {0};
    //存储
    for(int x = 0;x < imgNo[0]->width;x++)
    {
        for(int y = 0;y < w[x];y++)
            cvSet2D(paintX, y, x, t);
    }
    
    //显示
    cvNamedWindow("PaintX");
    cvShowImage("PaintX", paintX);
    cvWaitKey(0);
    cvReleaseImage(&imgSrc);
    cvReleaseImage(&img_gray);
//    cvReleaseImage(&paintY);
    cvDestroyAllWindows();
    return 0;  
}