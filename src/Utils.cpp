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
        "res/Apple.mp4", // OK ZOOM
        "res/Cs-Go.mp4", // OK ZOOM
        "res/GoogleMaps.mp4", // SHIT VIDEO
        "res/MinecraftEnderman.mp4", // SHIT VIDEO
        "res/MinecraftLava.mp4", // CV TO 30 FPS
        "res/MinecraftSheep.mp4", // LATE OF VERY LITTLE
        "res/MineSweeper.mp4", // CV TO 30 FPS & START TOO FAST
        "res/StopMotion.mp4", // OK
        "res/TerminalVLC.mp4", // IN ADVANCE OF VERY LITTLE
        "res/VsCode.mp4", // LATE OF 1 SEC
        "res/WindowsVirus.mp4" // LATE OF LITTLE
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

void Utils::fillArrayRandom(int *array, int n, int max) {
    for (int i = 0; i < n; i++) {
        array[i] = rand() % max;
    }
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

bool Utils::isRectangleSameValue(bool **matrix, int x, int y, int w, int h) {
    bool value = matrix[y][x];
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            if (matrix[j][i] != value) return false;
        }
    }
    return true;
}

void Utils::buildQuadTreeFromMatrixRec(QuadTree *quadTree, bool **matrix,
                                       int x, int y, int w, int h) {
    quadTree->rect.x = x;
    quadTree->rect.y = y;
    quadTree->rect.w = w;
    quadTree->rect.h = h;

    int wHalf = w / 2;
    int hHalf = h / 2;

    if (isRectangleSameValue(matrix, x, y, w, h)
        || wHalf < QT_PRECISION_W || hHalf < QT_PRECISION_H) {
        quadTree->value = matrix[y][x];
        quadTree->topLeft = nullptr;
        quadTree->topRight = nullptr;
        quadTree->botLeft = nullptr;
        quadTree->botRight = nullptr;
        return;
    }

    quadTree->value = false;
    quadTree->topLeft = new QuadTree;
    quadTree->topRight = new QuadTree;
    quadTree->botLeft = new QuadTree;
    quadTree->botRight = new QuadTree;
    int wCompensation = (w % 2 == 0) ? 0 : 1;
    int hCompensation = (h % 2 == 0) ? 0 : 1;
    buildQuadTreeFromMatrixRec(quadTree->topLeft, matrix,
                               x, y,
                               wHalf, hHalf);
    buildQuadTreeFromMatrixRec(quadTree->topRight, matrix,
                               x + wHalf, y,
                               wHalf + wCompensation, hHalf);
    buildQuadTreeFromMatrixRec(quadTree->botLeft, matrix,
                               x, y + hHalf,
                               wHalf, hHalf + hCompensation);
    buildQuadTreeFromMatrixRec(quadTree->botRight, matrix,
                               x + wHalf, y + hHalf,
                               wHalf + wCompensation, hHalf + hCompensation);
}

QuadTree *Utils::getQuadTreeFromMatrix(bool **matrix, int w, int h) {
    auto *quadTree = new QuadTree;
    buildQuadTreeFromMatrixRec(quadTree, matrix, 0, 0, w, h);
    return quadTree;
}

void Utils::destroyQuadTree(QuadTree *quadTree) {
    if (quadTree == nullptr) return;

    destroyQuadTree(quadTree->topLeft);
    destroyQuadTree(quadTree->topRight);
    destroyQuadTree(quadTree->botLeft);
    destroyQuadTree(quadTree->botRight);

    delete quadTree;
}

void Utils::addImgToImg(cv::Mat &src, cv::Mat &addImg, int x, int y, int w, int h) {
    cv::Mat cpyAddImg;
    cv::resize(addImg, cpyAddImg, cv::Size(w, h));
    cpyAddImg.copyTo(src(cv::Rect(cv::Point(x, y), cpyAddImg.size())));
}
