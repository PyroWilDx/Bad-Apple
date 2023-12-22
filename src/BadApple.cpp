//
// Created by pyrowildx on 20/12/23.
//

#include <iostream>
#include "BadApple.hpp"

cv::VideoCapture BadApple::orglBA;
std::vector<cv::VideoCapture> BadApple::butBAList = {};
int BadApple::nbVideo = 0;

void BadApple::initBadApple() {
    BadApple::orglBA = cv::VideoCapture(Utils::orglBAPath);

    for (const std::string &butBAPath: Utils::butBAPaths) {
        cv::VideoCapture butBA = cv::VideoCapture(butBAPath);
        BadApple::butBAList.push_back(butBA);
        Utils::myAssert(butBA.isOpened(), "Failed to open video.");
    }
    nbVideo = (int) BadApple::butBAList.size();
}

bool BadApple::updateImgs(cv::Mat &imgOrglBA, std::vector<cv::Mat> &imgButBAList) {
    BadApple::orglBA >> imgOrglBA;

    if (imgOrglBA.empty()) return false;

    for (int i = 0; i < BadApple::nbVideo; i++) {
        BadApple::butBAList[i] >> imgButBAList[i];
    }

    return true;
}

void BadApple::updateStrImg(cv::Mat &imgOrglBA, std::string &targetStrImg) {
    cv::Mat imgGrayOrglBA;
    cv::cvtColor(imgOrglBA, imgGrayOrglBA, cv::COLOR_BGR2GRAY);
    cv::resize(imgGrayOrglBA, imgGrayOrglBA, Utils::targetSize);
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
}

void BadApple::displayStrImg(std::string &strImg) {
    std::cout << strImg << std::flush;
    strImg.clear();
}

void BadApple::updateMatrix(cv::Mat &imgOrglBA, bool **imgMatrix) {
    cv::Mat imgGrayOrglBA;
    cv::cvtColor(imgOrglBA, imgGrayOrglBA, cv::COLOR_BGR2GRAY);
    cv::resize(imgGrayOrglBA, imgGrayOrglBA, cv::Size(BA_WIDTH, BA_HEIGHT));
    cv::Mat targetImg = cv::Mat::zeros(BA_HEIGHT, BA_WIDTH, CV_8UC3);
    for (int i = 0; i < BA_HEIGHT; i++) {
        for (int j = 0; j < BA_WIDTH; j++) {
            uint8_t intensity = imgGrayOrglBA.at<uchar>(i, j);
            if (intensity > 32) imgMatrix[i][j] = true;
            else imgMatrix[i][j] = false;
        }
    }
}

void BadApple::addImgToTargetImg(Rectangle &rect, cv::Mat &targetImg,
                                 std::vector<cv::Mat> &imgButBAList,
                                 int *rdIndexArray, int *rdI) {
    while (imgButBAList[rdIndexArray[*rdI]].empty()) {
        rdIndexArray[*rdI] = rand() % BadApple::nbVideo;
    }
    if (rect.w < SMALLEST_RECT_W || rect.h < SMALLEST_RECT_H) {
        targetImg(cv::Rect(rect.x, rect.y,
                           rect.w, rect.h))
                = cv::Scalar(UCHAR_MAX, UCHAR_MAX, UCHAR_MAX);
    } else {
        Utils::addImgToImg(targetImg, imgButBAList[rdIndexArray[*rdI]],
                           rect.x, rect.y,
                           rect.w, rect.h);
        *rdI = *rdI + 1;
    }
}

void BadApple::updateTargetImgBR(bool **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                                 int *rdIndexArray, cv::Mat &targetImg) {
    int rdI = 0;
    while (true) {
        Rectangle biggestRect = Utils::popBiggestRectangleInMatrix(
                imgMatrix,
                BA_WIDTH, BA_HEIGHT
        );
        if (biggestRect.h == 0) break;

        Utils::swapInt(&biggestRect.x, &biggestRect.y);

        addImgToTargetImg(biggestRect, targetImg, imgButBAList,
                          rdIndexArray, &rdI);
    }
}

void BadApple::iterateQT(QuadTree *quadTree, std::vector<cv::Mat> &imgButBAList,
                         int *rdIndexArray, cv::Mat &targetImg, int *rdI) {
    if (quadTree->value) {
        addImgToTargetImg(quadTree->rect, targetImg, imgButBAList,
                          rdIndexArray, rdI);
        return;
    }
    if (quadTree->topLeft != nullptr) {
        iterateQT(quadTree->topLeft, imgButBAList,
                  rdIndexArray, targetImg, rdI);
    }
    if (quadTree->topRight != nullptr) {
        iterateQT(quadTree->topRight, imgButBAList,
                  rdIndexArray, targetImg, rdI);
    }
    if (quadTree->botLeft != nullptr) {
        iterateQT(quadTree->botLeft, imgButBAList,
                  rdIndexArray, targetImg, rdI);
    }
    if (quadTree->botRight != nullptr) {
        iterateQT(quadTree->botRight, imgButBAList,
                  rdIndexArray, targetImg, rdI);
    }
}

void BadApple::updateTargetImgQT(bool **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                                 int *rdIndexArray, cv::Mat &targetImg) {
    QuadTree *quadTree = Utils::getQuadTreeFromMatrix(imgMatrix,
                                                      BA_WIDTH, BA_HEIGHT);

    int rdI = 0;
    iterateQT(quadTree, imgButBAList, rdIndexArray, targetImg, &rdI);

    Utils::destroyQuadTree(quadTree);
}

void BadApple::updateVideo(bool **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                           int *rdIndexArray, cv::Mat &targetImg,
                           cv::VideoWriter &video, bool quadTree) {
    if (quadTree) {
        BadApple::updateTargetImgQT(imgMatrix, imgButBAList,
                                    rdIndexArray, targetImg);
    } else {
        BadApple::updateTargetImgBR(imgMatrix, imgButBAList,
                                    rdIndexArray, targetImg);
    }

    video.write(targetImg);
}

int BadApple::getBAWidthWithHeight(int h) {
    return (int) lround(((double) h * ((double) BA_WIDTH / (double) BA_HEIGHT)));
}
