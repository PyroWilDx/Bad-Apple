//
// Created by pyrow on 18/12/2023.
//

#include <iostream>
#include <cassert>
#include "Utils.hpp"

// w = 480 / (360 / h);
cv::Size Utils::targetSize = cv::Size(0, 0);
const char *Utils::orglBAPath = "res/BadApple.mp4";

const std::vector<std::string> Utils::butBAPaths = {"res/BadApple.mp4"};

void Utils::myAssert(bool cond, const char *errMsg) {
    if (!cond) {
        std::cout << errMsg << std::endl;
        assert(cond);
    }
}

void Utils::addImgToImg(cv::Mat &src, cv::Mat &addImg, int x, int y, int w, int h) {
    cv::Mat cpyAddImg;
    cv::resize(addImg, cpyAddImg, cv::Size(w, h));
    cpyAddImg.copyTo(src(cv::Rect(cv::Point(x, y), cpyAddImg.size())));

}
