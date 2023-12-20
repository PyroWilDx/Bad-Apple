//
// Created by pyrow on 18/12/2023.
//

#ifndef BADAPPLE_UTILS_HPP
#define BADAPPLE_UTILS_HPP

#define BA_WIDTH 480
#define BA_HEIGHT 360
#define BA_FPS 30

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

typedef struct Rectangle {
    int x;
    int y;
    int w;
    int h;
} Rectangle;

class Utils {

public:
    static cv::Size targetSize;
    static const char *orglBAPath;
    static char displayC;

    static const std::vector<std::string> butBAPaths;

    static void myAssert(bool cond, const char *errMsg);

    static void swapInt(int *a, int *b);

    static bool isRectangle(bool **matrix, int x, int y, int w, int h);

    static Rectangle popBiggestRectangleInMatrix(bool **matrix, int w, int h);

    static void addImgToImg(cv::Mat &src, cv::Mat &addImg, int x, int y, int w, int h);

};

#endif
