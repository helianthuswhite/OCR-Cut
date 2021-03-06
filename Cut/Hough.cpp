#include "opencv/cv.h"
#include "opencv2/highgui.hpp"
#include "math.h"

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/real_img2s.jpg"

bool IsDimodal(double HistGram[256]);

int FindThreshold(IplImage* imgSrc);

IplImage* rotate(IplImage* src,double angle);

int main(int argc, char **argv)
{
    IplImage* src = cvLoadImage( IMAGE, 0 );
    IplImage* dst;
    IplImage* color_dst;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    CvPoint* line;
    int linesLong[100] = {0};
    int i;
    double alpha = 0;
    
    if( !src )
    {
        printf("Load image error!\n");
        return -1;
    }
    
    dst = cvCreateImage( cvGetSize(src), 8, 1 );
    color_dst = cvCreateImage( cvGetSize(src), 8, 3 );
    
    int threshold = FindThreshold(src);
    //    二值化处理
    cvThreshold(src, dst,threshold, 255,CV_THRESH_BINARY_INV);
    
    //膨胀操作
    cvDilate(dst, dst,NULL,4);
    
    cvCanny( dst, dst, 50, 200, 3 );
    //cvThreshold(src, dst, 50, 255, CV_THRESH_BINARY_INV);
    
    cvCvtColor( dst, color_dst, CV_GRAY2BGR );
    
    //累积概率hough变换
    lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 100, 50, 100 );
    for( i = 0; i < lines->total; i++ )
    {
//        获取到每一条直线的长度
        line = (CvPoint*)cvGetSeqElem(lines,i);
        linesLong[i] = (int)sqrt((line[1].x - line[0].x) * (line[1].x - line[0].x) + (line[1].y - line[0].y) * (line[1].y - line[0].y));

    }
//    冒泡排序找到最长直线
    for (int m = 0; m<i-1; m++) {
        for (int n = 1; n<i; n++) {
            if (linesLong[m]<=linesLong[n]) {
                int temp = linesLong[m];
                linesLong[m] = linesLong[n];
                linesLong[n] = temp;
            }
        }
    }
//    选取最长直线为标准求角度
    for( i = 0; i < lines->total; i++ )
    {
        line = (CvPoint*)cvGetSeqElem(lines,i);
        if ((int)sqrt((line[1].x - line[0].x) * (line[1].x - line[0].x) + (line[1].y - line[0].y) * (line[1].y - line[0].y)) == linesLong[0]) {
            cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
            alpha = atan(std::abs(1.0*(line[1].y - line[0].y)/(line[1].x - line[0].x)));
            printf("%lf",alpha);
        }
    }
    
    cvNamedWindow( "Source_1", 1 );
    cvShowImage( "Source_1", src );
    
    cvNamedWindow( "Hough_1", 1 );
    cvShowImage( "Hough_1", color_dst );

//    旋转变换
    src = rotate(src, -alpha*180/M_PI);
    
    color_dst = rotate(color_dst, -alpha*180/M_PI);
    
    cvNamedWindow( "Source_2", 1 );
    cvShowImage( "Source_2", src );
    
    cvNamedWindow( "Hough_2", 1 );
    cvShowImage( "Hough_2", color_dst );
    
    cvWaitKey(0);
    
    return 0;
}


//双峰波谷法查找二值化阈值
int FindThreshold(IplImage* imgSrc) {
    int Iter = 0;
    double HistGram[256] = {0};   // 基于精度问题，一定要用浮点数来处理，否则得不到正确的结果
    double HistGramB[256] = {0};        // 求均值的过程会破坏前面的数据，因此需要两份数据
    //获得像素值直方图
    for(int y = 0;y < imgSrc->height;y++)
    {
        uchar* ptr = (uchar*)(imgSrc->imageData + y*imgSrc->widthStep);
        for(int x = 0;x < imgSrc->width;x++)
        {
            for(int num = 0;num<256;num++) {
                if(ptr[x] == num) HistGram[num]++;
            }
        }
    }
    // 通过三点求均值来平滑直方图
    while (IsDimodal(HistGram) == false)            // 判断是否已经是双峰的图像了
    {
        HistGramB[0] = (HistGram[0] + HistGram[0] + HistGram[1]) / 3;                 // 第一点
        for (int x = 1; x < 255; x++)
            HistGramB[x] = (HistGram[x - 1] + HistGram[x] + HistGram[x + 1]) / 3;     // 中间的点
        HistGramB[255] = (HistGram[254] + HistGram[255] + HistGram[255]) / 3;       //最后一点
        for (int x = 0; x < 256; x++)
        {
            HistGram[x] = HistGramB[x];
        }
        Iter++;
        if (Iter >= 1000) return -1;        // 直方图无法平滑为双峰的，返回错误代码
    }
    // 阈值极为两峰之间的最小值
    bool Peakfound = false;
    for (int x = 1; x < 255; x++)
    {
        if (HistGram[x - 1] < HistGram[x] && HistGram[x + 1] < HistGram[x]) Peakfound = true;
        if (Peakfound == true && HistGram[x - 1] >= HistGram[x] && HistGram[x + 1] >= HistGram[x])
            return x - 1;
    }
    return -1;
}

bool IsDimodal(double HistGram[256]) {
    // 对直方图的峰进行计数，只有峰数位2才为双峰
    int Count = 0;
    for (int x = 1; x < 256; x++)
    {
        if (HistGram[x - 1] < HistGram[x] && HistGram[x + 1] < HistGram[x])
        {
            Count++;
            if (Count > 2) return false;
        }
    }
    if (Count == 2)
        return true;
    else
        return false;
}

//传入角度对图像进行旋转
IplImage* rotate(IplImage* src,double angle) {
    //    创建矩阵
    CvMat* rot_mat = cvCreateMat(2,3,CV_32FC1);
    IplImage *dst;
    //        复制图像
    dst = cvCloneImage(src);
    //        设置原点
    dst->origin = src->origin;
    //        初始化元素值
    cvZero(dst);
    
    //COMPUTE ROTATION MATRIX
    //        创建旋转矩阵
    CvPoint2D32f center = cvPoint2D32f(src->width/2,
                                       src->height/2);
    double scale = 1;
    //        生成旋转的映射矩阵
    cv2DRotationMatrix(center,angle,scale,rot_mat);
    //        进行仿射变换
    cvWarpAffine(src,dst,rot_mat);

    return dst;

}
