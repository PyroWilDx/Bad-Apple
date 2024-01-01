//
// Created by pyrowildx on 20/12/23.
//

#ifndef BADAPPLE_BADAPPLE_HPP
#define BADAPPLE_BADAPPLE_HPP

#include <string>
#include <opencv2/opencv.hpp>
#include "Utils.hpp"

#define SMALLEST_RECT_W 1
#define SMALLEST_RECT_H 1

class BadApple {

public:
    static cv::VideoCapture orglBA;
    static std::vector<cv::VideoCapture> butBAList;
    static int nbVideo;

    static int currFrame;
    static cv::Mat imgGrayOrglBA;

    static void initBadApple();

    static bool updateImgs(cv::Mat &imgOrglBA, std::vector<cv::Mat> &imgButBAList,
                           bool updateButBA);

    static void updateStrImg(cv::Mat &imgOrglBA, std::string &targetStrImg);

    static void displayStrImg(std::string &strImg);

    static void updateMatrix(cv::Mat &imgOrglBA, uint8_t **imgMatrix);

    static void addImgToTargetImg(Rectangle *rect, cv::Mat &targetImg,
                                  std::vector<cv::Mat> &imgButBAList,
                                  int **rdIndexArray, int *rdI);

    static void iterateQT(QuadTree *quadTree, std::vector<cv::Mat> &imgButBAList,
                          std::vector<Rectangle *> &rects);

    static void updateTargetImgQT(uint8_t **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                                  int **rdIndexArray, cv::Mat &targetImg);

    static void updateTargetImgMode1(uint8_t **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                                     cv::Mat &targetImg);

    static void updateVideo(uint8_t **imgMatrix, std::vector<cv::Mat> &imgButBAList,
                            int **rdIndexArray, cv::Mat &targetImg,
                            cv::VideoWriter &video);

    static int getBAWidthWithHeight(int h);

};

#endif
