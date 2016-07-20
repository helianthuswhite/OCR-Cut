#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <opencv/cxcore.h>

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/test_img4.png"

using namespace cv;

int* projectH(IplImage* src);

int* projectW(IplImage* src);

void cutH(int* h,IplImage* src);

void cutW(int* h,IplImage* src);

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
    cvShowImage("ThresholdImg",img_gray);
    
//    执行投影操作
    int *h = projectH(img_gray);
    
//    将图片切割分行
    cutH(h,img_gray);
    
    printf("找到的汉字数量为：%d",count);
    //显示
    cvWaitKey(0);
    cvReleaseImage(&imgSrc);
    cvReleaseImage(&img_gray);
    cvDestroyAllWindows();
    return 0;  
}


int* projectH(IplImage* src) {
    //存储投影值的数组
    int* h = new int[src->height];
    for(int y = 0;y < src->height;y++) {
        h[y] = 0;
    }
    //对每一行计算投影值
    for(int y = 0;y < src->height;y++)
    {
        uchar* ptr = (uchar*)(src->imageData + y*src->widthStep);
        //遍历这一行的每一个像素，如果是有效的，累加投影值
        for(int x = 0;x < src->width;x++)
        {
            if(ptr[x] == 255)
                h[y]++;
        }
    }
    
    return h;
}

int* projectW(IplImage* src) {
    //存储投影值的数组
    int* w = new int[src->width];
    for(int x = 0;x < src->width;x++) {
        w[x] = 0;
    }
    //对每一列计算投影值
    for(int y = 0;y < src->height;y++)
    {
        uchar* ptr = (uchar*)(src->imageData + y*src->widthStep);
        //遍历这一行的每一个像素，如果是有效的，累加投影值
        for(int x = 0;x < src->width;x++)
        {
            if(ptr[x] == 255)
                w[x]++;
        }
    }
    return w;
}

void cutH(int* h,IplImage* src) {
    bool inWord = false;
    //分割开始位置和结束位置
    int start = 0,end = 0;
    //    获得行数
    int HEIGHT = src->height;
    //    存储每一行的图片
    IplImage* imgNo;
    //存储
    for(int y = 0;y < HEIGHT;y++)
    {
        if(!inWord && h[y] > 0) {
            inWord = true;
            start = y;
        }else if(inWord && h[y] == 0) {
            inWord = false;
            end = y;
            imgNo = cvCreateImage(cvSize(src->width,end-start+1), IPL_DEPTH_8U, 1);
            cvSetImageROI(src, cvRect(0, start, imgNo->width, imgNo->height));
            cvCopyImage(src, imgNo);
            cvResetImageROI(src);
            //对每一行的图像进行列投影处理
            int* w = projectW(imgNo);
            cutW(w,imgNo);
        }
    }

}

void cutW(int* w,IplImage* src) {
    bool inWord = false;
    //分割开始位置和结束位置
    int start = 0,end = 0;
    //    获得列数
    int WIDTH = src->width;
    //    存储每一列的图片
    IplImage* imgNo;
    char szName[100] = {0};
    //存储
    for(int x = 0;x < WIDTH;x++)
    {
        if(!inWord && w[x] > 0) {
            inWord = true;
            start = x;
        }else if(inWord && w[x] == 0) {
            inWord = false;
            end = x;
            imgNo = cvCreateImage(cvSize(end-start+1,src->height), IPL_DEPTH_8U, 1);
            cvSetImageROI(src, cvRect(start-1, 0, imgNo->width, imgNo->height));
            cvCopyImage(src, imgNo);
            cvResetImageROI(src);
//            显示每一行的结果
            sprintf(szName, "windowstart_%d", start);
            cvNamedWindow(szName);
            cvShowImage(szName, imgNo);
            cvReleaseImage(&imgNo);
            count++;
        }
    }

}