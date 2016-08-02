#include "opencv/cv.h"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/real_img2s.jpg"

/** @function main */
int main( int argc, char** argv )
{
    //    创建两个矩阵的三元数组
    CvPoint2D32f srcTri[3], dstTri[3];
    //    创建矩阵
    CvMat* rot_mat = cvCreateMat(2,3,CV_32FC1);
    CvMat* warp_mat = cvCreateMat(2,3,CV_32FC1);
    IplImage *src, *dst;
    src = cvLoadImage(IMAGE);
    //        复制图像
    dst = cvCloneImage(src);
    //        设置原点
    dst->origin = src->origin;
    //        初始化元素值
    cvZero(dst);
    
    //COMPUTE WARP MATRIX
    //        计算变换坐标
    srcTri[0].x = 0;          //src Top left 原矩阵左上角
    srcTri[0].y = 0;
    srcTri[1].x = src->width - 1;    //src Top right 原矩阵右上角
    srcTri[1].y = 0;
    srcTri[2].x = 0;          //src Bottom left  原矩阵左下角
    srcTri[2].y = src->height - 1;
    //- - - - - - - - - - - - - - -//
    dstTri[0].x = src->width*0.0;    //dst Top left 目标左上角
    dstTri[0].y = src->height*0.33;
    dstTri[1].x = src->width*0.85; //dst Top right  目标右上角
    dstTri[1].y = src->height*0.25;
    dstTri[2].x = src->width*0.15; //dst Bottom left  目标左下角
    dstTri[2].y = src->height*0.7;
    //        生成仿射映射矩阵 ，参数为：两个包含三个点的数组，需要进行仿射变换的矩阵
    cvGetAffineTransform(srcTri,dstTri,warp_mat);
    //        进行仿射变换
    cvWarpAffine(src,dst,warp_mat);
    cvCopy(dst,src);
    
    //COMPUTE ROTATION MATRIX
    //        创建旋转矩阵
    CvPoint2D32f center = cvPoint2D32f(src->width/2,
                                       src->height/2);
    //        设定旋转参数
    double angle = -50.0;
    double scale = 0.6;
    //        生成旋转的映射矩阵
    cv2DRotationMatrix(center,angle,scale,rot_mat);
    //        进行仿射变换
    cvWarpAffine(src,dst,rot_mat);    cvWarpAffine(src,dst,rot_mat);
    
    /// 显示结果
    cvNamedWindow( "Source image", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Source image", src );
    
    cvNamedWindow( "Rotated", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Rotated", dst );
    
    /// 等待用户按任意按键退出程序
    waitKey(0);
    
    return 0;
}