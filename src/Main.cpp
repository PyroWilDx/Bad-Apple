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
    if (argc > 4) {
        if (std::stoi(argv[4]) == 1) multiThreading = true;
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

        cv::Mat imgOrglBA;
        std::vector<cv::Mat> imgButBAList = std::vector<cv::Mat>(BadApple::nbVideo);
        while (true) {
            std::string strImg;
            if (!BadApple::updateStrImg(imgOrglBA, imgButBAList, strImg)) {
                break;
            }

            BadApple::displayStrImg(strImg);

            cv::imshow(wOrgl, imgOrglBA);

            if (cv::waitKey(30) == 27) {
                music.pause();
                while (true) {
                    if (cv::waitKey(30) == 27) break;
                }
                music.play();
            }
        }
    }

    return EXIT_SUCCESS;
}
