#include <string>
#include <filesystem>
#include <thread>
#include <opencv2/opencv.hpp>
#include <SFML/Audio.hpp>
#include "Utils.hpp"
#include "TerminalThreads.hpp"
#include "BadApple.hpp"

// ./BadApple 220 77 X
int main(int argc, char *argv[]) {
    std::string path = std::filesystem::current_path().string();
    std::filesystem::current_path(path + "/..");

    if (argc < 4) {
        std::cout << "Usage : ./BadApple <Width> <Height> <Character> [Multi-Threading (1)]" << std::endl;
        return EXIT_FAILURE;
    }

    Utils::targetSize = cv::Size(std::stoi(argv[1]), std::stoi(argv[2]));
    Utils::displayC = argv[3][0];

    bool multiThreading = false;
    bool generateVideo = false;
    if (argc > 4) {
        int value = std::stoi(argv[4]);
        if (value == 1) multiThreading = true;
        if (value == 2) generateVideo = true;
    }

    BadApple::initBadApple();

    sf::Music music;
    bool musicOpened = music.openFromFile("res/BadApple.ogg");
    Utils::myAssert(musicOpened, "Failed to open music.");
    music.play();

    if (multiThreading) {
        std::thread productThread(TerminalThreads::productStrImg);
        std::thread consumeThread(TerminalThreads::consumeStrImg);

        consumeThread.join();
    } else {
        const std::string wOrgl = "Original";
        cv::namedWindow(wOrgl, cv::WINDOW_NORMAL);
        cv::moveWindow(wOrgl, 800, 0);
        const std::string wTarget = "Target";
        cv::namedWindow(wTarget, cv::WINDOW_NORMAL);
        cv::moveWindow(wTarget, 800, 400);

        cv::Mat imgOrglBA;
        std::vector<cv::Mat> imgButBAList = std::vector<cv::Mat>(BadApple::nbVideo);
        bool **imgMatrix = (bool **) malloc(sizeof(bool *) * BA_HEIGHT);
        for (int i = 0; i < BA_HEIGHT; i++) {
            imgMatrix[i] = (bool *) malloc(sizeof(bool) * BA_WIDTH);
        }
        while (true) {
            std::string strImg;
            if (!BadApple::updateStrImg(imgOrglBA, imgButBAList, strImg)) {
                break;
            }
//            BadApple::displayStrImg(strImg);
            cv::imshow(wOrgl, imgOrglBA);

            if (generateVideo) {
                cv::Mat imgGrayOrglBA;
                cv::cvtColor(imgOrglBA, imgGrayOrglBA, cv::COLOR_BGR2GRAY);
                cv::resize(imgGrayOrglBA, imgGrayOrglBA, cv::Size(BA_WIDTH, BA_HEIGHT));
                cv::Mat targetImg = cv::Mat::zeros(BA_HEIGHT, BA_WIDTH, CV_8UC3);
                for (int i = 0; i < BA_HEIGHT; i++) {
                    for (int j = 0; j < BA_WIDTH; j++) {
                        uint8_t intensity = imgGrayOrglBA.at<uchar>(i, j);
                        if (intensity > 32) imgMatrix[i][j] = true;
                        else imgMatrix[i][j] = false;
                    }
                }

                while (true) {
                    Rectangle biggestRect = Utils::popBiggestRectangleInMatrix(imgMatrix,
                                                                               BA_WIDTH, BA_HEIGHT);
                    if (biggestRect.h == 0) break;

                    Utils::swapInt(&biggestRect.x, &biggestRect.y);
                    if (biggestRect.h < 6) {
                        targetImg(cv::Rect(biggestRect.x, biggestRect.y,
                                           biggestRect.w, biggestRect.h))
                                           = cv::Scalar(255, 255, 255);
                    } else {
                        Utils::addImgToImg(targetImg, imgButBAList[0],
                                           biggestRect.x, biggestRect.y,
                                           biggestRect.w, biggestRect.h);
                    }
                }
                cv::imshow(wTarget, targetImg);
            }

            if (cv::waitKey(30) == 27) {
                music.pause();
                while (true) {
                    if (cv::waitKey(30) == 27) break;
                }
                music.play();
            }
        }
        for (int i = 0; i < BA_HEIGHT; i++) {
            free(imgMatrix[i]);
        }
        free(imgMatrix);
    }

    return EXIT_SUCCESS;
}
