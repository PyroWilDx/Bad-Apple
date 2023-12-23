//
// Created by pyrowildx on 20/12/23.
//

#include <iostream>
#include "BadApple.hpp"

cv::VideoCapture BadApple::orglBA;
std::vector<cv::VideoCapture> BadApple::butBAList = {};
int BadApple::nbVideo = 0;

int BadApple::currFrame = 0;

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
    cv::resize(imgGrayOrglBA, imgGrayOrglBA, cv::Size(TARGET_WIDTH, TARGET_HEIGHT));
    cv::Mat targetImg = cv::Mat::zeros(TARGET_HEIGHT, TARGET_WIDTH, CV_8UC3);
    for (int i = 0; i < TARGET_HEIGHT; i++) {
        for (int j = 0; j < TARGET_WIDTH; j++) {
            uint8_t intensity = imgGrayOrglBA.at<uchar>(i, j);
            if (intensity > 32) imgMatrix[i][j] = true;
            else imgMatrix[i][j] = false;
        }
    }
}

void BadApple::addImgToTargetImg(Rectangle *rect, cv::Mat &targetImg,
                                 std::vector<cv::Mat> &imgButBAList,
                                 int *rdIndexArray, int *rdI) {
    while (imgButBAList[rdIndexArray[*rdI]].empty()
           || Utils::butBAPaths[rdIndexArray[*rdI]].delay > BadApple::currFrame) {
        if (imgButBAList.size() == 1) goto white; // For Debug a Single Video
        rdIndexArray[*rdI] = Utils::getRandomInt(BadApple::nbVideo);
    }
    if (rect->w < SMALLEST_RECT_W || rect->h < SMALLEST_RECT_H) {
        white: // For Debug a Single Video
        targetImg(cv::Rect(rect->x, rect->y,
                           rect->w, rect->h))
                = cv::Scalar(UCHAR_MAX, UCHAR_MAX, UCHAR_MAX);
    } else {
        Utils::addImgToImg(targetImg, imgButBAList[rdIndexArray[*rdI]],
                           rect->x, rect->y,
                           rect->w, rect->h);
        *rdI = *rdI + 1;
    }
}

void BadApple::updateTargetImgBR(bool **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                                 int *rdIndexArray, cv::Mat &targetImg) {
    int rdI = 0;
    while (true) {
        Rectangle biggestRect = Utils::popBiggestRectangleInMatrix(
                imgMatrix,
                TARGET_WIDTH, TARGET_HEIGHT
        );
        if (biggestRect.h == 0) break;

        Utils::swapInt(&biggestRect.x, &biggestRect.y);

        addImgToTargetImg(&biggestRect, targetImg, imgButBAList,
                          rdIndexArray, &rdI);
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

void BadApple::updateTargetImgQT(bool **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                                 int *rdIndexArray, cv::Mat &targetImg) {
    QuadTree *quadTree = Utils::getQuadTreeFromMatrix(imgMatrix,
                                                      TARGET_WIDTH, TARGET_HEIGHT);

    std::vector<Rectangle *> rects = {};
    iterateQT(quadTree, imgButBAList, rects);
    std::sort(rects.begin(), rects.end(), &Utils::compareRectangle);

    int rdI = 0;
    for (Rectangle *rect: rects) {
        addImgToTargetImg(rect, targetImg, imgButBAList,
                          rdIndexArray, &rdI);
    }
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
    return (int) lround(((double) h * ((double) TARGET_WIDTH / (double) TARGET_HEIGHT)));
}
