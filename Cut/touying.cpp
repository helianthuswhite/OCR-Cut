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
    CvScalar t= cvScalar(255,0,0,0);
    for(int y = 0;y < paintY->height;y++)
    {
        for(int x = 0;x < h[y];x++)
            cvSet2D(paintY, y, x, t);
//        printf("%d\n",h[y]);
    }
    
    //显示
    cvNamedWindow("PaintY");
    cvShowImage("PaintY", paintY);
    cvWaitKey(0);
    cvReleaseImage(&imgSrc);
    cvReleaseImage(&img_gray);
    cvReleaseImage(&paintY);
    cvDestroyAllWindows();
    return 0;  
}