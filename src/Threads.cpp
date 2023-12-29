//
// Created by pyrowildx on 20/12/23.
//

#include <iostream>
#include "Threads.hpp"
#include "BadApple.hpp"

bool Threads::videoEnded = false;

std::string Threads::strImgBuffer[MAX_BUFFER_SIZE] = {};

std::condition_variable Threads::condProd;
std::condition_variable Threads::condCons;
int Threads::iProd = 0;
int Threads::iCons = 0;
int Threads::nbReady = 0;
std::mutex Threads::mtx;

void Threads::productStrImg() {
    cv::Mat imgOrglBA;
    std::vector<cv::Mat> imgButBAList = std::vector<cv::Mat>(BadApple::nbVideo);
    while (true) {
        if (!BadApple::updateImgs(imgOrglBA, imgButBAList, false)) {
            videoEnded = true;
            break;
        }

        startProduct();

        BadApple::updateStrImg(imgOrglBA, strImgBuffer[iProd]);

        endProduct();
    }
}

void Threads::consumeStrImg() {
    while (true) {
        if (nbReady == 0 && videoEnded) break;

        startConsume();

        BadApple::displayStrImg(strImgBuffer[iCons]);
        strImgBuffer[iCons] = "";

        endConsume();
    }
}

void Threads::startProduct() {
    std::unique_lock<std::mutex> lock(mtx);
    while (nbReady >= MAX_BUFFER_SIZE) {
        condProd.wait(lock);
        nbReady = nbReady; // To Brain CLion :/
    }
    mtx.unlock();
}

void Threads::endProduct() {
    std::unique_lock<std::mutex> lock(mtx);
    nbReady++;
    iProd = (iProd + 1) % MAX_BUFFER_SIZE;
    condCons.notify_one();
    mtx.unlock();
}

void Threads::startConsume() {
    std::unique_lock<std::mutex> lock(mtx);
    while (nbReady < 1) {
        condCons.wait(lock);
        nbReady = nbReady; // To Brain CLion :/
    }
    mtx.unlock();
}

void Threads::endConsume() {
    std::unique_lock<std::mutex> lock(mtx);
    nbReady--;
    iCons = (iCons + 1) % MAX_BUFFER_SIZE;
    condProd.notify_one();
    mtx.unlock();
}
