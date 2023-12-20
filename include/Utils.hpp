//
// Created by pyrow on 18/12/2023.
//

#ifndef BADAPPLE_UTILS_HPP
#define BADAPPLE_UTILS_HPP

#define BA_WIDTH 480
#define BA_HEIGHT 360

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

class Utils {

public:
    static cv::Size targetSize;
    static const char *orglBAPath;
    static char displayC;

    static const std::vector<std::string> butBAPaths;

    static void myAssert(bool cond, const char *errMsg);

    static void addImgToImg(cv::Mat &src, cv::Mat &addImg, int x, int y, int w, int h);

};

#endif
