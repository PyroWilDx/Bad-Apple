//
// Created by pyrow on 18/12/2023.
//

#include <iostream>
#include <cassert>
#include <random>
#include "Utils.hpp"
#include "BadApple.hpp"

std::random_device rd;
std::mt19937 gen(rd());

cv::Size Utils::targetSize = cv::Size(0, 0);
const char *Utils::orglBAPath = "res/BadApple.mp4";
char Utils::displayC;
double Utils::changeProb = 1. / BA_FPS;

// Every Video : 30 FPS
const std::vector<VideoInfo> Utils::butBAPaths = {
        {"res/BadApple.mp4",                    0}, // For Mode 1
        {"res/50Windows.mp4",                   -35},
        {"res/80SynthWave.mp4",                 8},
        {"res/ActuallyPlayable.mp4",            -97},
        {"res/AiGenerated.mp4",                 0},
//        {"res/AiGeneratedCharacters.mp4",       1}, // NO
//        {"res/Apple.mp4",                       0}, // NO
        {"res/AppleEmoji.mp4",                  1},
        {"res/BlackMIDI.mp4",                   2},
        {"res/C++.mp4",                         -3},
//        {"res/C1-65A.mp4",                      38}, // NO
        {"res/Chess.mp4",                       3},
        {"res/ColorChannels.mp4",               0}, // For Mode 1
        {"res/ConwaysLifeGame.mp4",             -553},
        {"res/Cs-Go.mp4",                       3},
        {"res/Desmos.mp4",                      0},
//        {"res/Discord.mp4",                     -114}, // NO
        {"res/Fire.mp4",                        5},
        {"res/Flick.mp4",                       5}, // For Mode 1
        {"res/FourierSeries.mp4",               -774},
        {"res/FourierTransform.mp4",            -362},
        {"res/Gameboy.mp4",                     -20},
//        {"res/HyperbolicPlane.mp4",             0}, // NO
        {"res/Laser.mp4",                       -36},
//        {"res/LaserShow.mp4",                   -284}, // NO
        {"res/LaserTeslaCoil.mp4",              8},
//        {"res/MarioCoins.mp4",                  -24}, // NO
        {"res/MiddleEurope.mp4",                17},
//        {"res/MinecraftApple.mp4",              -2}, // NO
        {"res/MinecraftChiseledBookshelf.mp4",  16},
//        {"res/MinecraftCopper.mp4",             162}, // For Mode 0
        {"res/MinecraftLava.mp4",               -3},
        {"res/MinecraftPinkPetal.mp4",          2},
        {"res/MinecraftSheep.mp4",              0},
        {"res/MinecraftTrapdoor.mp4",           -479},
        {"res/MineSweeper.mp4",                 3},
        {"res/MK-90.mp4",                       -263},
        {"res/MolecularDynamicsSimulation.mp4", -30},
        {"res/OnlyMotion.mp4",                  0},
        {"res/Oscilloscope.mp4",                2},
//        {"res/Nes8Bit.mp4",                     -925}, // NO
//        {"res/PauseCantSee.mp4",                0}, // NO
//        {"res/PeoplePlayground.mp4",            46}, // NO
        {"res/PhosphorScreen.mp4",              0},
        {"res/PrimeNumber.mp4",                 0},
        {"res/QrCode.mp4",                      0},
        {"res/RPlace.mp4",                      26},
        {"res/RocketLeague.mp4",                -264}, // For Mode 1
        {"res/Scratch.mp4",                     -53},
        {"res/SingleLine.mp4",                  0},
        {"res/Spaghetti.mp4",                   0},
        {"res/StopMotion.mp4",                  0},
        {"res/StraightLines.mp4",               2},
        {"res/SuperMarioBrosX.mp4",             1},
        {"res/TerminalVLC.mp4",                 -2},
//        {"res/Terraria.mp4",                    -1601}, // NO
        {"res/TimeSeries.mp4",                  4},
        {"res/VolumeSlice.mp4",                 0},
        {"res/Water.mp4",                       2},
        {"res/WindowsScreensaver.mp4",          -24},
        {"res/WindowsTaskManager.mp4",          0},
        {"res/WindowsVirus.mp4",                0},
//        {"res/YoutubeHomepage.mp4",             791}, // For Mode 0
};

void Utils::myAssert(bool cond, const char *errMsg) {
    if (!cond) {
        std::cout << errMsg << std::endl;
        assert(cond);
    }
}

double Utils::getRandomFloat() {
    std::uniform_real_distribution<double> randReal(0., 1.);
    return randReal(gen);
}

int Utils::getRandomInt(int maxExcluded) {
    std::uniform_int_distribution<> randInt(0, maxExcluded - 1);
    return randInt(gen);
}

void Utils::fillMatrixRandom(int **array, int n, int m, int maxExcluded) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            array[i][j] = getRandomInt(maxExcluded);
        }
    }
}

bool Utils::isRectangleSameValue(uint8_t **matrix, int x, int y, int w, int h) {
//    bool valueGtMin = (matrix[y][x] > MIN_INTENSITY);
//    for (int i = x; i < x + w; i++) {
//        for (int j = y; j < y + h; j++) {
//            if (matrix[j][i] > MIN_INTENSITY) {
//                if (!valueGtMin) return false;
//            } else {
//                if (valueGtMin) return false;
//            }
//        }
//    }
//    return true;
    int min = 256;
    int max = 0;
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            if (matrix[j][i] > max) max = matrix[j][i];
            if (matrix[j][i] < min) min = matrix[j][i];
        }
    }
    int maxGap = MAX_INTENSITY_GAP;
    if (BadApple::currFrame >= 1620 && BadApple::currFrame < 1700) {
        maxGap = 2;
    }
    return ((max - min) < maxGap);
}

bool Utils::hasRectangleEnoughAlpha(uint8_t **matrix, int x, int y,
                                    int w, int h, double minAlpha) {
    int totalAlpha = 0;
    int nAlpha = 0;
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            totalAlpha += matrix[j][i];
            nAlpha++;
        }
    }
    double meanAlpha = (double) totalAlpha / (double) nAlpha;
    return (meanAlpha > minAlpha);
}

void Utils::buildQuadTreeFromMatrixRec(QuadTree *quadTree, uint8_t **matrix,
                                       int x, int y, int w, int h) {
    quadTree->rect.x = x;
    quadTree->rect.y = y;
    quadTree->rect.w = w;
    quadTree->rect.h = h;

    int wHalf = w / 2;
    int hHalf = h / 2;

    bool isSameValue = isRectangleSameValue(matrix, x, y, w, h);
    if (isSameValue || wHalf < QT_PRECISION_W || hHalf < QT_PRECISION_H) {
        quadTree->value = hasRectangleEnoughAlpha(matrix, x, y, w, h, MIN_ALPHA);
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

QuadTree *Utils::getQuadTreeFromMatrix(uint8_t **matrix, int w, int h) {
    auto *quadTree = new QuadTree;

    int minAlpha = 254.;
    if (BadApple::currFrame >= 2752 && BadApple::currFrame < 2790) {
        minAlpha = 252.;
    } else if (BadApple::currFrame >= 5320 && BadApple::currFrame < 5370) {
        minAlpha = 256.;
    }
    if (hasRectangleEnoughAlpha(matrix, 0, 0, w, h, minAlpha)) {
        quadTree->value = true;
        quadTree->rect = {0, 0, w, h};
        quadTree->topLeft = nullptr;
        quadTree->topRight = nullptr;
        quadTree->botLeft = nullptr;
        quadTree->botRight = nullptr;
        return quadTree;
    }

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

#ifdef ALPHA
    cv::cvtColor(cpyAddImg, cpyAddImg, cv::COLOR_BGR2BGRA); // Alpha
    int totalAlpha = 0;
    int nAlpha = 0;
    for (int j = 0; j < cpyAddImg.rows; j++) {
        for (int i = 0; i < cpyAddImg.cols; i++) {
            totalAlpha += BadApple::imgGrayOrglBA.at<uint8_t>(y + j, x + i);
            nAlpha++;
        }
    }
    double meanAlpha = (double) totalAlpha / (double) nAlpha;

    std::vector<cv::Mat> channels;
    cv::split(cpyAddImg, channels);
    cv::Mat alpha = channels[3];
    alpha.setTo((int) meanAlpha);
    cv::merge(channels, cpyAddImg);
#else
    int totalAlpha = 0;
    int nAlpha = 0;
    for (int j = 0; j < cpyAddImg.rows; j++) {
        for (int i = 0; i < cpyAddImg.cols; i++) {
            totalAlpha += BadApple::imgGrayOrglBA.at<uint8_t>(y + j, x + i);
            nAlpha++;
        }
    }
    double meanAlpha = (double) totalAlpha / (double) nAlpha;
    double factAlpha = meanAlpha / 255.;

    std::vector<cv::Mat> channels;
    cv::split(cpyAddImg, channels);
    for (auto &channel: channels) {
        channel = channel * factAlpha;
    }
    cv::merge(channels, cpyAddImg);
#endif

    cpyAddImg.copyTo(src(cv::Rect(cv::Point(x, y), cpyAddImg.size())));
}

void Utils::addImgToImgMapAlpha(cv::Mat &src, cv::Mat &addImg, int x, int y, int w, int h) {
    cv::Mat cpyAddImg;
    cv::resize(addImg, cpyAddImg, cv::Size(w, h));

    for (int j = 0; j < cpyAddImg.rows; j++) {
        for (int i = 0; i < cpyAddImg.cols; i++) {
            uint8_t intensity = BadApple::imgGrayOrglBA.at<uint8_t>(y + j, x + i);
            auto &pixel = cpyAddImg.at<cv::Vec3b>(j, i);
            pixel[0] = uint8_t((double) pixel[0] * ((double) intensity / 255.));
            pixel[1] = uint8_t((double) pixel[1] * ((double) intensity / 255.));
            pixel[2] = uint8_t((double) pixel[2] * ((double) intensity / 255.));
        }
    }

    cpyAddImg.copyTo(src(cv::Rect(cv::Point(x, y), cpyAddImg.size())));
}
