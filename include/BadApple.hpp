//
// Created by pyrowildx on 20/12/23.
//

#ifndef BADAPPLE_BADAPPLE_HPP
#define BADAPPLE_BADAPPLE_HPP

#include <string>
#include <opencv2/opencv.hpp>

class BadApple {

public:
    static cv::VideoCapture orglBA;
    static std::vector<cv::VideoCapture> butBAList;
    static size_t nbVideo;

    static void initBadApple();

    static bool updateStrImg(cv::Mat &imgOrglBA, std::vector<cv::Mat> &imgButBAList,
                             std::string &targetStrImg);

    static void displayStrImg(std::string &strImg);

};

#endif //BADAPPLE_BADAPPLE_HPP
