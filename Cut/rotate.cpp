#include "opencv/cv.h"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/real_img2s.jpg"

/** @function main */
int main( int argc, char** argv )
{
    //    创建矩阵
    CvMat* rot_mat = cvCreateMat(2,3,CV_32FC1);
    IplImage *src, *dst;
    src = cvLoadImage(IMAGE);
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
    //        设定旋转参数
    double angle = -5.0;
    double scale = 1;
    //        生成旋转的映射矩阵
    cv2DRotationMatrix(center,angle,scale,rot_mat);
    //        进行仿射变换
    cvWarpAffine(src,dst,rot_mat);
    /// 显示结果
    cvNamedWindow( "Source image", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Source image", src );
    
    cvNamedWindow( "Rotated", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Rotated", dst );
    
    /// 等待用户按任意按键退出程序
    waitKey(0);
    
    return 0;
}