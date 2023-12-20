//
// Created by pyrowildx on 20/12/23.
//

#include <iostream>
#include "TerminalThreads.hpp"
#include "Utils.hpp"
#include "BadApple.hpp"

bool TerminalThreads::videoEnded = false;

std::string TerminalThreads::strImgBuffer[MAX_BUFFER_SIZE] = {};
std::condition_variable TerminalThreads::condProd;
std::condition_variable TerminalThreads::condCons;
int TerminalThreads::iProd = 0;
int TerminalThreads::iCons = 0;
int TerminalThreads::nbReady = 0;
std::mutex TerminalThreads::mtx;

void TerminalThreads::productStrImg() {
    cv::Mat imgOrglBA;
    std::vector<cv::Mat> imgButBAList = std::vector<cv::Mat>(BadApple::nbVideo);
    while (true) {
        startProductStrImg();

        if (!BadApple::updateStrImg(imgOrglBA, imgButBAList, strImgBuffer[iProd])) {
            videoEnded = true;
            break;
        }

        endProductStrImg();
    }
}

void TerminalThreads::consumeStrImg() {
    while (nbReady > 0 || !videoEnded) {
        startConsumeStrImg();

        BadApple::displayStrImg(strImgBuffer[iCons]);
        strImgBuffer[iCons] = "";

        endConsumeStrImg();
    }
}

void TerminalThreads::startProductStrImg() {
    std::unique_lock<std::mutex> lock(mtx);
    while (nbReady >= MAX_BUFFER_SIZE) {
        condProd.wait(lock);
    }
    mtx.unlock();
}

void TerminalThreads::endProductStrImg() {
    std::unique_lock<std::mutex> lock(mtx);
    nbReady++;
    iProd = (iProd + 1) % MAX_BUFFER_SIZE;
    condCons.notify_one();
    mtx.unlock();
}

void TerminalThreads::startConsumeStrImg() {
    std::unique_lock<std::mutex> lock(mtx);
    while (nbReady < 1) {
        condCons.wait(lock);
    }
    mtx.unlock();
}

void TerminalThreads::endConsumeStrImg() {
    std::unique_lock<std::mutex> lock(mtx);
    nbReady--;
    iCons = (iCons + 1) % MAX_BUFFER_SIZE;
    condProd.notify_one();
    mtx.unlock();
}