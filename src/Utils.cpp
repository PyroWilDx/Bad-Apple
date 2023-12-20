//
// Created by pyrow on 18/12/2023.
//

#include <iostream>
#include <cassert>
#include "Utils.hpp"
#include "BadApple.hpp"

cv::Size Utils::targetSize = cv::Size(0, 0);
const char *Utils::orglBAPath = "res/BadApple.mp4";
char Utils::displayC;

const std::vector<std::string> Utils::butBAPaths = {
        "res/Apple.mp4",
        "res/Cs-Go.mp4",
        "res/GoogleMaps.mp4",
        "res/MinecraftEnderman.mp4",
        "res/MinecraftLava.mp4",
        "res/MinecraftSheep.mp4",
        "res/MineSweeper.mp4",
        "res/StopMotion.mp4",
        "res/TerminalVLC.mp4",
        "res/VsCode.mp4",
        "res/WindowsVirus.mp4"
};

void Utils::myAssert(bool cond, const char *errMsg) {
    if (!cond) {
        std::cout << errMsg << std::endl;
        assert(cond);
    }
}

void Utils::swapInt(int *a, int *b) {
    int t = *b;
    *b = *a;
    *a = t;
}

bool Utils::isRectangle(bool **matrix, int x, int y, int w, int h) {
    for (int i = x; i < x + h; i++) {
        for (int j = y; j < y + w; j++) {
            if (!matrix[i][j]) return false;
        }
    }
    return true;
}

Rectangle Utils::popBiggestRectangleInMatrix(bool **matrix, int w, int h) {
    Rectangle biggestRect = {0, 0, 0, 0};
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int hCurr = biggestRect.h + 1;
            int wCurr = BadApple::getBAWidthWithHeight(hCurr);
            while ((i + hCurr < h) && (j + wCurr < w) && isRectangle(matrix, i, j, wCurr, hCurr)) {
                biggestRect.x = i;
                biggestRect.y = j;
                biggestRect.w = wCurr;
                biggestRect.h = hCurr;
                hCurr++;
                wCurr = BadApple::getBAWidthWithHeight(hCurr);
            }
        }
    }
    for (int i = biggestRect.x; i < biggestRect.x + biggestRect.h; i++) {
        for (int j = biggestRect.y; j < biggestRect.y + biggestRect.w; j++) {
            matrix[i][j] = false;
        }
    }
    return biggestRect;
}

void Utils::addImgToImg(cv::Mat &src, cv::Mat &addImg, int x, int y, int w, int h) {
    cv::Mat cpyAddImg;
    cv::resize(addImg, cpyAddImg, cv::Size(w, h));
    cpyAddImg.copyTo(src(cv::Rect(cv::Point(x, y), cpyAddImg.size())));

}
