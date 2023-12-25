//
// Created by pyrowildx on 20/12/23.
//

#ifndef BADAPPLE_THREADS_HPP
#define BADAPPLE_THREADS_HPP

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <opencv2/opencv.hpp>

#define MAX_BUFFER_SIZE 6

class Threads {

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

    static void startProduct();

    static void endProduct();

    static void startConsume();

    static void endConsume();

};

#endif
