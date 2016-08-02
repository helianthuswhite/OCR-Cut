#include "opencv/cv.h"
#include "opencv2/highgui.hpp"
#include "math.h"

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/real_img2s.jpg"

int main(int argc, char **argv)
{
    IplImage* src = cvLoadImage( IMAGE, 0 );
    IplImage* dst;
    IplImage* color_dst;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    int i;
    
    if( !src )
    {
        printf("Load image error!\n");
        return -1;
    }
    
    dst = cvCreateImage( cvGetSize(src), 8, 1 );
    color_dst = cvCreateImage( cvGetSize(src), 8, 3 );
    
    cvCanny( src, dst, 50, 200, 3 );
    //cvThreshold(src, dst, 50, 255, CV_THRESH_BINARY_INV);
    
    cvCvtColor( dst, color_dst, CV_GRAY2BGR );
#if 0   //标准hough变换
    printf("标准hough变换\n");
    lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 100, 0, 0 );
    
    for( i = 0; i < MIN(lines->total,100); i++ )
    {
        float* line = (float*)cvGetSeqElem(lines,i);
        float rho = line[0];
        float theta = line[1];
        CvPoint pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0), 3, CV_AA, 0 );
    }
#else   //累积概率hough变换
    printf("累积概率hough变换\n");
    lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 50, 50, 10 );
    for( i = 0; i < lines->total; i++ )
    {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
        cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
    }
#endif
    cvNamedWindow( "Source", 1 );
    cvShowImage( "Source", src );
    
    cvNamedWindow( "Hough", 1 );
    cvShowImage( "Hough", color_dst );
    
    cvWaitKey(0);
    
    return 0;
}