#include <opencv/cv.h>
#include <opencv2/highgui.hpp>  
#include <opencv/cxcore.h> 
#include <sys/time.h>

#define IMAGE "/Users/W_littlewhite/Documents/Git/OCR-Cut/real_img2s.jpg"

using namespace cv;

bool IsDimodal(double HistGram[256]);

int FindThreshold(IplImage* imgSrc);

IplImage* rotate(IplImage* src,double angle);

double findAngle(IplImage* src);

long getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(int argc, char* argv[])
{
    
    printf("begin:%ld\n",getCurrentTime());

//    加载图像
    IplImage* imgSrc = cvLoadImage(IMAGE,CV_LOAD_IMAGE_COLOR);
//    将图像转换为灰度图
    IplImage* img_gray = cvCreateImage(cvGetSize(imgSrc), IPL_DEPTH_8U, 1);
    cvCvtColor(imgSrc, img_gray, CV_BGR2GRAY);
    
    int threshold = FindThreshold(img_gray);
    
//    二值化处理
    cvThreshold(img_gray, img_gray,threshold, 255,CV_THRESH_BINARY_INV);// CV_THRESH_BINARY_INV使得背景为黑色，字符为白色，这样找到的最外层才是字符的最外层
    
    //    局部自适应二值化处理
//    cvAdaptiveThreshold(img_gray, img_gray, 255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV);
    
    //膨胀操作
    cvDilate(img_gray, img_gray,NULL,2);
    
    double angle = findAngle(img_gray);
//    cvCanny( img_gray, img_gray, 50, 200, 3 );
    
    imgSrc = rotate(imgSrc, -angle*180/M_PI);
    img_gray = rotate(img_gray, -angle*180/M_PI);
    
    cvShowImage("ThresholdImg",img_gray);
    CvSeq* contours = NULL;  
    CvMemStorage* storage = cvCreateMemStorage(0);
//    检索轮廓并返回检测到的轮廓数量
    cvFindContours(img_gray, storage, &contours,
        sizeof(CvContour),CV_RETR_EXTERNAL);  
    // cvReleaseMemStorage(& storage);   
    int idx = 0;
    char szName[56] = {0};  
//    显示绘制的结果
    for (CvSeq* c = contours; c != NULL; c = c->h_next) {
         CvRect rc =cvBoundingRect(c,0);
        
         if (rc.height < 50||rc.width < 50) {    //这里可以根据轮廓的大小进行筛选,上面源图片有瑕疵可以用腐蚀，膨胀来祛除
            continue;
         }
//          设置ROI并显示分割后的结果
        cvDrawRect(imgSrc, cvPoint(rc.x, rc.y), cvPoint(rc.x + rc.width, rc.y + rc.height), CV_RGB(255, 0, 0));
        
        IplImage* imgNo = cvCreateImage(cvSize(rc.width, rc.height), IPL_DEPTH_8U, 3);
        cvSetImageROI(imgSrc, rc);  
        cvCopyImage(imgSrc, imgNo);  
        cvResetImageROI(imgSrc);  
        sprintf(szName, "wnd_%d", idx++);   
        cvNamedWindow(szName);  
        cvShowImage(szName, imgNo);  
        cvReleaseImage(&imgNo);
    }  
//      后续的处理
    cvNamedWindow("src");
    cvShowImage("src", imgSrc);
    
    printf("begin:%ld\n",getCurrentTime());

    cvWaitKey(0);  
    cvReleaseMemStorage(&storage);  
    cvReleaseImage(&imgSrc);  
    cvReleaseImage(&img_gray);  
    cvDestroyAllWindows();  
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

//通过Hough变换来检测直线并返回角度
double findAngle(IplImage* src) {
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    CvPoint* line;
    int linesLong[100] = {0};
    int i;
    double alpha = 0;
    
    IplImage* img_temp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvCopyImage(src, img_temp);
    
    cvCanny( img_temp, img_temp, 50, 200, 3 );
    
//    cvCvtColor( src, src, CV_GRAY2BGR );
    
    //累积概率hough变换
    lines = cvHoughLines2( img_temp, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 100, 50, 100 );
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
            alpha = atan(std::abs(1.0*(line[1].y - line[0].y)/(line[1].x - line[0].x)));
//            printf("%lf",alpha);
        }
    }
    return alpha;

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
