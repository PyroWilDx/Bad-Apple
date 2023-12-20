//
// Created by pyrowildx on 20/12/23.
//

#include <iostream>
#include "BadApple.hpp"
#include "Utils.hpp"

cv::VideoCapture BadApple::orglBA;
std::vector<cv::VideoCapture> BadApple::butBAList = {};
size_t BadApple::nbVideo = 0;

void BadApple::initBadApple() {
    BadApple::orglBA = cv::VideoCapture(Utils::orglBAPath);

    for (std::string butBAPath : Utils::butBAPaths) {
        cv::VideoCapture butBA = cv::VideoCapture(Utils::orglBAPath);
        BadApple::butBAList.push_back(butBA);
        Utils::myAssert(butBA.isOpened(), "Failed to open video.");
    }
    nbVideo = BadApple::butBAList.size();
}

bool BadApple::updateStrImg(cv::Mat &imgOrglBA, std::vector<cv::Mat> &imgButBAList,
                            std::string &targetStrImg) {
    BadApple::orglBA >> imgOrglBA;
    if (imgOrglBA.empty()) {
        return false;
    }

    cv::Mat imgResizedOrglBA;
    cv::resize(imgOrglBA, imgResizedOrglBA, Utils::targetSize);
    for (int i = 0; i < imgButBAList.size(); i++) {
        BadApple::butBAList[i] >> imgButBAList[i];
    }

    cv::Mat imgGrayOrglBA;
    cv::cvtColor(imgResizedOrglBA, imgGrayOrglBA, cv::COLOR_BGR2GRAY);
    uint8_t intensityMatrix[imgGrayOrglBA.rows][imgGrayOrglBA.cols];
    std::string strImg;
    for (int i = 0; i < imgGrayOrglBA.rows; i++) {
        for (int j = 0; j < imgGrayOrglBA.cols; j++) {
            uint8_t intensity = imgGrayOrglBA.at<uchar>(i, j);
            intensityMatrix[i][j] = intensity;
            if (intensity > 32) targetStrImg += Utils::displayC;
            else targetStrImg += " ";
        }
        targetStrImg += "\n";
    }

    return true;
}

void BadApple::displayStrImg(std::string &strImg) {
    std::cout << strImg << std::flush;
}
