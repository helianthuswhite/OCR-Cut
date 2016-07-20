#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <opencv/cxcore.h>

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/test_img4.png"

using namespace cv;

int* project(IplImage* src,int number);

void cut(int* h,IplImage* src,int number);

//计数器
int count = 0;

int main(int argc, char* argv[])
{
    //    加载图像
    IplImage* imgSrc = cvLoadImage(IMAGE,CV_LOAD_IMAGE_COLOR);
    
    //    将图像转换为灰度图
    IplImage* img_gray = cvCreateImage(cvGetSize(imgSrc), IPL_DEPTH_8U, 1);
    cvCvtColor(imgSrc, img_gray, CV_BGR2GRAY);
    
    //    二值化处理
    cvThreshold(img_gray, img_gray,100, 255,CV_THRESH_BINARY_INV);// CV_THRESH_BINARY_INV使得背景为黑色，字符为白色，这样找到的最外层才是字符的最外层
    cvShowImage("Threshold", img_gray);
    
//    执行行投影操作
    int *h = project(img_gray,img_gray->height);

//    执行分割操作
    cut(h,img_gray,img_gray->height);
    
    printf("找到的汉字数量为：%d",count);
    //显示
    cvWaitKey(0);
    cvReleaseImage(&imgSrc);
    cvReleaseImage(&img_gray);
    cvDestroyAllWindows();
    return 0;  
}


int* project(IplImage* src,int number) {
//    
    IplImage* img_temp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvCopyImage(src, img_temp);
    //膨胀操作
    cvDilate(img_temp, img_temp,NULL);
    //存储投影值的数组
    int* a = new int[number];
    for(int i = 0;i < number;i++) {
        a[i] = 0;
    }
    //对每一行计算投影值
    for(int y = 0;y < img_temp->height;y++)
    {
        uchar* ptr = (uchar*)(img_temp->imageData + y*img_temp->widthStep);
        //遍历这一行的每一个像素，如果是有效的，累加投影值
        for(int x = 0;x < img_temp->width;x++)
        {
            if(ptr[x] == 255) {
                if (number == img_temp->height) {
                    a[y]++;
                } else {
                    a[x]++;
                }
            }
        }
    }
    
    return a;
}


void cut(int* a,IplImage* src,int number) {
    bool inWord = false;
    //分割开始位置和结束位置
    int start = 0,end = 0;
    //    存储每一行的图片
    IplImage* imgNo;
    char szName[100] = {0};
    //存储
    for(int i = 0;i < number;i++)
    {
        if(!inWord && a[i] > 0) {
            inWord = true;
            start = i;
        }else if(inWord && a[i] == 0) {
            inWord = false;
            end = i;
            if(number == src->height) {
                imgNo = cvCreateImage(cvSize(src->width,end-start+1), IPL_DEPTH_8U, 1);
                cvSetImageROI(src, cvRect(0, start, imgNo->width, imgNo->height));
                cvCopyImage(src, imgNo);
                cvResetImageROI(src);
                //对每一行的图像进行列投影处理
                int* w = project(imgNo,imgNo->width);
                cut(w,imgNo,imgNo->width);

            }else {
                imgNo = cvCreateImage(cvSize(end-start+1,src->height), IPL_DEPTH_8U, 1);
                cvSetImageROI(src, cvRect(start, 0, imgNo->width, imgNo->height));
                cvCopyImage(src, imgNo);
                cvResetImageROI(src);
                //            显示每一行的结果
                sprintf(szName, "windowstart_%d", ++count);
                cvNamedWindow(szName);
                cvShowImage(szName, imgNo);
                cvReleaseImage(&imgNo);
            }
        }
    }
    return;
}
