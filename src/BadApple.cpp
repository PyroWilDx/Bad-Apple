//
// Created by pyrowildx on 20/12/23.
//

#include <iostream>
#include "BadApple.hpp"

cv::VideoCapture BadApple::orglBA;
std::vector<cv::VideoCapture> BadApple::butBAList = {};
int BadApple::nbVideo = 0;

int BadApple::currFrame = 0;
cv::Mat BadApple::imgGrayOrglBA;

void BadApple::initBadApple() {
    BadApple::orglBA = cv::VideoCapture(Utils::orglBAPath);
    Utils::myAssert(BadApple::orglBA.isOpened(), "Failed to open video.");

    BadApple::nbVideo = (int) Utils::butBAPaths.size();
    cv::Mat tmpImg;
    for (int i = 0; i < BadApple::nbVideo; i++) {
        cv::VideoCapture butBA = cv::VideoCapture(Utils::butBAPaths[i].path);
        Utils::myAssert(butBA.isOpened(), "Failed to open video.");
        BadApple::butBAList.push_back(butBA);
        if (Utils::butBAPaths[i].delay < 0) {
            for (int j = 0; j < -Utils::butBAPaths[i].delay; j++) {
                butBA >> tmpImg;
            }
        }
    }
}

bool BadApple::updateImgs(cv::Mat &imgOrglBA, std::vector<cv::Mat> &imgButBAList) {
    BadApple::orglBA >> imgOrglBA;

    if (imgOrglBA.empty()) return false;

    for (int i = 0; i < BadApple::nbVideo; i++) {
        if (Utils::butBAPaths[i].delay > BadApple::currFrame) continue;
        BadApple::butBAList[i] >> imgButBAList[i];
    }

    BadApple::currFrame++;

    return true;
}

void BadApple::updateStrImg(cv::Mat &imgOrglBA, std::string &targetStrImg) {
    cv::cvtColor(imgOrglBA, BadApple::imgGrayOrglBA, cv::COLOR_BGR2GRAY);
    cv::resize(BadApple::imgGrayOrglBA, BadApple::imgGrayOrglBA, Utils::targetSize);
    for (int i = 0; i < BadApple::imgGrayOrglBA.rows; i++) {
        for (int j = 0; j < BadApple::imgGrayOrglBA.cols; j++) {
            uint8_t intensity = BadApple::imgGrayOrglBA.at<uchar>(i, j);
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

void BadApple::updateMatrix(cv::Mat &imgOrglBA, uint8_t **imgMatrix) {
    cv::cvtColor(imgOrglBA, BadApple::imgGrayOrglBA, cv::COLOR_BGR2GRAY);
    cv::resize(BadApple::imgGrayOrglBA, BadApple::imgGrayOrglBA, cv::Size(TARGET_WIDTH, TARGET_HEIGHT));
    for (int i = 0; i < TARGET_HEIGHT; i++) {
        for (int j = 0; j < TARGET_WIDTH; j++) {
            uint8_t intensity = BadApple::imgGrayOrglBA.at<uint8_t>(i, j);
            if (intensity > 0) imgMatrix[i][j] = intensity;
            else imgMatrix[i][j] = 0;
        }
    }
}

void BadApple::addImgToTargetImg(Rectangle *rect, cv::Mat &targetImg,
                                 std::vector<cv::Mat> &imgButBAList,
                                 int **rdIndexArray, int *rdI) {
    int i = rect->y;
    int j = rect->x;

    if (Utils::getRandomFloat() < Utils::changeProb) {
        rdIndexArray[i][j] = Utils::getRandomInt(BadApple::nbVideo);
    }

    while (imgButBAList[rdIndexArray[i][j]].empty()
           || Utils::butBAPaths[rdIndexArray[i][j]].delay > BadApple::currFrame) {
        if (imgButBAList.size() == 1) goto white; // For Debug a Single Video
        rdIndexArray[i][j] = Utils::getRandomInt(BadApple::nbVideo);
    }
    if (rect->w < SMALLEST_RECT_W || rect->h < SMALLEST_RECT_H) {
        white: // For Debug a Single Video
        targetImg(cv::Rect(rect->x, rect->y,
                           rect->w, rect->h))
                = cv::Scalar(UCHAR_MAX, UCHAR_MAX, UCHAR_MAX);
    } else {
        Utils::addImgToImg(targetImg, imgButBAList[rdIndexArray[i][j]],
                           rect->x, rect->y,
                           rect->w, rect->h);
        *rdI = *rdI + 1;
    }
}

void BadApple::iterateQT(QuadTree *quadTree, std::vector<cv::Mat> &imgButBAList,
                         std::vector<Rectangle *> &rects) {
    if (quadTree->value) {
        rects.push_back(&quadTree->rect);
        return;
    }
    if (quadTree->topLeft != nullptr) iterateQT(quadTree->topLeft, imgButBAList, rects);
    if (quadTree->topRight != nullptr) iterateQT(quadTree->topRight, imgButBAList, rects);
    if (quadTree->botLeft != nullptr) iterateQT(quadTree->botLeft, imgButBAList, rects);
    if (quadTree->botRight != nullptr) iterateQT(quadTree->botRight, imgButBAList, rects);
}

void BadApple::updateTargetImgQT(uint8_t **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                                 int **rdIndexArray, cv::Mat &targetImg) {
    QuadTree *quadTree = Utils::getQuadTreeFromMatrix(imgMatrix,
                                                      TARGET_WIDTH, TARGET_HEIGHT);

    std::vector<Rectangle *> rects = {};
    iterateQT(quadTree, imgButBAList, rects);

    int rdI = 0;
    for (Rectangle *rect: rects) {
        addImgToTargetImg(rect, targetImg, imgButBAList,
                          rdIndexArray, &rdI);
    }
    Utils::destroyQuadTree(quadTree);
}

void BadApple::updateVideo(uint8_t **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                           int **rdIndexArray, cv::Mat &targetImg,
                           cv::VideoWriter &video) {
    BadApple::updateTargetImgQT(imgMatrix, imgButBAList,
                                rdIndexArray, targetImg);

#ifndef ALPHA
    video.write(targetImg);
#endif
}

int BadApple::getBAWidthWithHeight(int h) {
    return (int) lround(((double) h * ((double) TARGET_WIDTH / (double) TARGET_HEIGHT)));
}
