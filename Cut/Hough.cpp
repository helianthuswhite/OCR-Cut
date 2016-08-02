#include "opencv/cv.h"
#include "opencv2/highgui.hpp"
#include "math.h"

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/real_img2s.jpg"

// clockwise 为true则顺时针旋转，否则为逆时针旋转
IplImage* rotateImage(IplImage* src, int angle, bool clockwise)
{
    angle = abs(angle) % 180;
    if (angle > 90)
    {
        angle = 90 - (angle % 90);
    }
    IplImage* dst = NULL;
    int width =
    (double)(src->height * sin(angle * CV_PI / 180.0)) +
    (double)(src->width * cos(angle * CV_PI / 180.0 )) + 1;
    int height =
    (double)(src->height * cos(angle * CV_PI / 180.0)) +
    (double)(src->width * sin(angle * CV_PI / 180.0 )) + 1;
    int tempLength = sqrt((double)src->width * src->width + src->height * src->height) + 10;
    int tempX = (tempLength + 1) / 2 - src->width / 2;
    int tempY = (tempLength + 1) / 2 - src->height / 2;
    int flag = -1;
    
    dst = cvCreateImage(cvSize(width, height), src->depth, src->nChannels);
    cvZero(dst);
    IplImage* temp = cvCreateImage(cvSize(tempLength, tempLength), src->depth, src->nChannels);
    cvZero(temp);
    
    cvSetImageROI(temp, cvRect(tempX, tempY, src->width, src->height));
    cvCopy(src, temp, NULL);
    cvResetImageROI(temp);
    
    if (clockwise)
        flag = 1;
    
    float m[6];
    int w = temp->width;
    int h = temp->height;
    m[0] = (float) cos(flag * angle * CV_PI / 180.);
    m[1] = (float) sin(flag * angle * CV_PI / 180.);
    m[3] = -m[1];
    m[4] = m[0];
    // 将旋转中心移至图像中间
    m[2] = w * 0.5f;
    m[5] = h * 0.5f;
    //
    CvMat M = cvMat(2, 3, CV_32F, m);
    cvGetQuadrangleSubPix(temp, dst, &M);
    cvReleaseImage(&temp);
    return dst;
}

int main(int argc, char **argv)
{
    IplImage *src = 0;
    IplImage *dst = 0;
    // 旋转角度
    int angle = 90;
    
    src = cvLoadImage(IMAGE,CV_LOAD_IMAGE_COLOR);
    cvNamedWindow("src", 1);
    cvShowImage("src", src);
    
    dst = rotateImage(src, angle, false);
    cvNamedWindow("dst", 2);
    cvShowImage("dst", dst);
    cvWaitKey(0);
    
    cvReleaseImage(&src);
    cvReleaseImage(&dst);
    return 0;
}