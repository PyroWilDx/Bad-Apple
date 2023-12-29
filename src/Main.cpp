#include <cstdlib>
#include <ctime>
#include <string>
#include <filesystem>
#include <thread>
#include <opencv2/opencv.hpp>
#include <SFML/Audio.hpp>
#include "Utils.hpp"
#include "Threads.hpp"
#include "BadApple.hpp"

// ./BadApple 220 77 X
int main(int argc, char *argv[]) {
    time_t currTime;
    time(&currTime);

    std::string path = std::filesystem::current_path().string();
    std::filesystem::current_path(path + "/..");

    if (argc < 4) {
        std::cout << "Usage : ./BadApple <Width> <Height> <Character> [Mode]" << std::endl;
        return EXIT_FAILURE;
    }

    Utils::targetSize = cv::Size(std::stoi(argv[1]), std::stoi(argv[2]));
    Utils::displayC = argv[3][0];

    int value = 0;
    if (argc > 4) value = std::stoi(argv[4]);
    bool terminalMode = (value == 0);
    bool generateVideo = (value == 1);
    bool multiThreading = (value == 2);

    BadApple::initBadApple();

    sf::Music music;
    if (terminalMode) {
        bool musicOpened = music.openFromFile("res/BadApple.ogg");
        Utils::myAssert(musicOpened, "Failed to open music.");
        music.play();
    }

    cv::VideoWriter generatedVideo;
#ifndef ALPHA
    if (generateVideo) {
        char vidName[256];
        sprintf(vidName, "output/%ld.avi", currTime);
        generatedVideo = cv::VideoWriter(
                vidName,
                cv::VideoWriter::fourcc('X', 'V', 'I', 'D'),
                BA_FPS, cv::Size(TARGET_WIDTH, TARGET_HEIGHT),
                true
        );
        std::cout << "Filename : " << vidName << std::endl;
    }
#endif

    if (multiThreading) {
        std::thread productThread(Threads::productStrImg);
        std::thread consumeThread(Threads::consumeStrImg);

        consumeThread.join();

        return EXIT_SUCCESS;
    }

    const std::string wOrgl = "Original";
    if (terminalMode) {
        cv::namedWindow(wOrgl, cv::WINDOW_NORMAL);
        cv::moveWindow(wOrgl, 800, 0);
    }

    const std::string wTarget = "Target";
    if (generateVideo) {
        cv::namedWindow(wTarget, cv::WINDOW_NORMAL);
        cv::moveWindow(wTarget, 800, 400);
    }

    cv::Mat imgOrglBA;
    std::vector<cv::Mat> imgButBAList = std::vector<cv::Mat>(BadApple::nbVideo);

    std::string strImg;

    uint8_t **imgMatrix;
    int **rdIndexMatrix;
    cv::Mat targetImg;

    if (generateVideo) {
        imgMatrix = (uint8_t **) malloc(sizeof(uint8_t *) * TARGET_HEIGHT);
        rdIndexMatrix = (int **) malloc(sizeof(int *) * TARGET_HEIGHT);
        for (int i = 0; i < TARGET_HEIGHT; i++) {
            imgMatrix[i] = (uint8_t *) malloc(sizeof(uint8_t) * TARGET_WIDTH);
            rdIndexMatrix[i] = (int *) malloc(sizeof(int) * TARGET_WIDTH);
        }
        Utils::fillMatrixRandom(rdIndexMatrix, TARGET_HEIGHT, TARGET_WIDTH, BadApple::nbVideo);
    }

    while (true) {
        if (!BadApple::updateImgs(imgOrglBA, imgButBAList, generateVideo)) break;

        std::cout << "Frame : " << BadApple::currFrame << std::endl;

        if (terminalMode) {
            cv::imshow(wOrgl, imgOrglBA);

            BadApple::updateStrImg(imgOrglBA, strImg);
            BadApple::displayStrImg(strImg);
        }

        if (generateVideo) {
            BadApple::updateMatrix(imgOrglBA, imgMatrix);

#ifndef ALPHA
            targetImg = cv::Mat::zeros(TARGET_HEIGHT, TARGET_WIDTH, CV_8UC3);
#else
            targetImg = cv::Mat::zeros(TARGET_HEIGHT, TARGET_WIDTH, CV_8UC4); // Alpha
#endif

            BadApple::updateVideo(imgMatrix, imgButBAList,
                                  rdIndexMatrix, targetImg,
                                  generatedVideo);

            cv::imshow(wTarget, targetImg);

#ifdef ALPHA
            cv::imwrite("output/frame" + std::to_string(BadApple::currFrame) + ".png", targetImg); // Alpha
#endif
        }

        if (terminalMode) {
            if (cv::waitKey(30) == 27) {
                music.pause();
                while (true) {
                    if (cv::waitKey(30) == 27) break;
                }
                music.play();
            }
        } else {
            if (cv::waitKey(WAIT_TIME) == 27) {
                while (true) {
                    if (cv::waitKey(30) == 27) break;
                }
            }
        }
    }

    if (generateVideo) {
        generatedVideo.release();
        for (int i = 0; i < TARGET_HEIGHT; i++) {
            free(imgMatrix[i]);
        }
        free(imgMatrix);
        free(rdIndexMatrix);
    }

    return EXIT_SUCCESS;
}
