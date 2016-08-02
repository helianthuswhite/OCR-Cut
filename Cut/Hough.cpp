#include "opencv/cv.h"
#include "opencv2/highgui.hpp"
#include "math.h"

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/real_img2s.jpg"

int main(int argc, char **argv)
{
    //    加载灰度图像
    IplImage* image = cvLoadImage(
                                  IMAGE,
                                  CV_LOAD_IMAGE_GRAYSCALE
                                  );
    //    变成更好看的彩色
    IplImage* src = cvLoadImage( IMAGE ); //Changed for prettier show in color
    //    创建存储区
    CvMemStorage* storage = cvCreateMemStorage(0);
    //    模糊处理，输入输出、模糊方法、水平和竖直方向sigma值
    cvSmooth(image, image, CV_GAUSSIAN, 5, 5 );
    //    进行霍夫圆变换
    CvSeq* results = cvHoughCircles(
                                    image,
                                    storage,
                                    CV_HOUGH_GRADIENT,
                                    4,
                                    image->width/10
                                    );
    //    遍历结果集
    for( int i = 0; i < results->total; i++ ) {
        //      返回索引制定的元素指针
        float* p = (float*) cvGetSeqElem( results, i );
        //      创建圆心
        CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );
        //      绘制圆
        cvCircle(
                 src,
                 pt, 
                 cvRound( p[2] ),
                 CV_RGB(0xff,0,0) 
                 );
    }
    //    显示结果
    cvNamedWindow( "cvHoughCircles", 1 );
    cvShowImage( "cvHoughCircles", src);
    cvWaitKey(0);

}