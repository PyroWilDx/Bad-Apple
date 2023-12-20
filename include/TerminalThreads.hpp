//
// Created by pyrowildx on 20/12/23.
//

#ifndef BADAPPLE_TERMINALTHREADS_HPP
#define BADAPPLE_TERMINALTHREADS_HPP

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <opencv2/opencv.hpp>

#define MAX_BUFFER_SIZE 6

class TerminalThreads {

public:
    static void productStrImg();

    static void consumeStrImg();

private:
    static bool videoEnded;

    static std::string strImgBuffer[MAX_BUFFER_SIZE];
    static std::mutex mtx;
    static std::condition_variable condProd;
    static std::condition_variable condCons;
    static int iProd;
    static int iCons;
    static int nbReady;

    static void startProductStrImg();

    static void endProductStrImg();

    static void startConsumeStrImg();

    static void endConsumeStrImg();

};

#endif
