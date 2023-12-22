#include <cstdlib>
#include <ctime>
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
    time_t currTime;
    srand(time(&currTime));

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

    bool useQuadTree = true;

    BadApple::initBadApple();

    sf::Music music;
    if (terminalMode) {
        bool musicOpened = music.openFromFile("res/BadApple.ogg");
        Utils::myAssert(musicOpened, "Failed to open music.");
        music.play();
    }

    cv::VideoWriter generatedVideo;
    if (generateVideo) {
        char vidName[256];
        sprintf(vidName, "res/%ld.avi", currTime);
        generatedVideo = cv::VideoWriter(
                vidName,
                cv::VideoWriter::fourcc('X', 'V', 'I', 'D'),
                BA_FPS, cv::Size(BA_WIDTH, BA_HEIGHT)
        );
    }

    if (multiThreading) {
        std::thread productThread(TerminalThreads::productStrImg);
        std::thread consumeThread(TerminalThreads::consumeStrImg);

        consumeThread.join();

        return EXIT_SUCCESS;
    }

    const std::string wOrgl = "Original";
    cv::namedWindow(wOrgl, cv::WINDOW_NORMAL);
    cv::moveWindow(wOrgl, 800, 0);

    const std::string wTarget = "Target";
    if (generateVideo) {
        cv::namedWindow(wTarget, cv::WINDOW_NORMAL);
        cv::moveWindow(wTarget, 800, 400);
    }

    cv::Mat imgOrglBA;
    std::vector<cv::Mat> imgButBAList = std::vector<cv::Mat>(BadApple::nbVideo);

    std::string strImg;

    bool **imgMatrix;
    int rdIndexArray[R_LIST_SIZE];
    int rdUpdateCpt = 0;
    cv::Mat targetImg;

    if (generateVideo) {
        imgMatrix = (bool **) malloc(sizeof(bool *) * BA_HEIGHT);
        for (int i = 0; i < BA_HEIGHT; i++) {
            imgMatrix[i] = (bool *) malloc(sizeof(bool) * BA_WIDTH);
        }
        Utils::fillArrayRandom(rdIndexArray, R_LIST_SIZE, BadApple::nbVideo);
    }

    while (true) {
        if (!BadApple::updateImgs(imgOrglBA, imgButBAList)) break;

        cv::imshow(wOrgl, imgOrglBA);

        if (terminalMode) {
            BadApple::updateStrImg(imgOrglBA, strImg);
            BadApple::displayStrImg(strImg);
        }

        if (generateVideo) {
            BadApple::updateMatrix(imgOrglBA, imgMatrix);

            targetImg = cv::Mat::zeros(BA_HEIGHT, BA_WIDTH, CV_8UC3);

            BadApple::updateVideo(imgMatrix, imgButBAList,
                                  rdIndexArray, targetImg,
                                  generatedVideo, useQuadTree);

            cv::imshow(wTarget, targetImg);

            rdUpdateCpt++;
            if (rdUpdateCpt == R_UPDATE_MAX) {
                Utils::fillArrayRandom(rdIndexArray, R_LIST_SIZE, BadApple::nbVideo);
                rdUpdateCpt = 0;
            }
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
        for (int i = 0; i < BA_HEIGHT; i++) {
            free(imgMatrix[i]);
        }
        free(imgMatrix);
    }

    return EXIT_SUCCESS;
}
