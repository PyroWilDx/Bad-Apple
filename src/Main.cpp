#include <string>
#include <vector>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <SFML/Audio.hpp>
#include "Utils.hpp"


// ./BadApple 220 77 �
int main(int argc, char *argv[]) {
    std::string path = std::filesystem::current_path().string();
    std::filesystem::current_path(path + "/..");

    if (argc < 4) {
        std::cout << "Usage : ./BadApple <Width> <Height> <Character>" << std::endl;
        return EXIT_FAILURE;
    }

    Utils::targetSize = cv::Size(std::stoi(argv[1]), std::stoi(argv[2]));

    cv::VideoCapture orglBA = cv::VideoCapture(Utils::orglBAPath);
    Utils::myAssert(orglBA.isOpened(), "Failed to open video.");

    std::vector<cv::VideoCapture> butBAList = {};
    for (std::string butBAPath : Utils::butBAPaths) {
        cv::VideoCapture butBA = cv::VideoCapture(Utils::orglBAPath);
        butBAList.push_back(butBA);
        Utils::myAssert(butBA.isOpened(), "Failed to open video.");
    }

    sf::Music music;
    bool musicOpened = music.openFromFile("res/BadApple.ogg");
    Utils::myAssert(musicOpened, "Failed to open music.");
    music.play();

    const std::string wOrgl = "Original";
    const std::string wTarget = "Target";
    cv::namedWindow(wOrgl, cv::WINDOW_NORMAL);
    cv::moveWindow(wOrgl, 800, 0);
    cv::namedWindow(wTarget, cv::WINDOW_NORMAL);
    cv::moveWindow(wTarget, 1200, 400);

    cv::Mat imgOrglBA;
    std::vector<cv::Mat> imgButBAList = std::vector<cv::Mat>(butBAList.size());
    while (true) {
        orglBA >> imgOrglBA;
        if (imgOrglBA.empty()) break;

        cv::Mat imgResizedOrglBA;
        cv::resize(imgOrglBA, imgResizedOrglBA, Utils::targetSize);
        for (int i = 0; i < imgButBAList.size(); i++) {
            butBAList[i] >> imgButBAList[i];
        }

        cv::Mat imgGrayOrglBA;
        cv::cvtColor(imgResizedOrglBA, imgGrayOrglBA, cv::COLOR_BGR2GRAY);
        uint8_t intensityMatrix[imgGrayOrglBA.rows][imgGrayOrglBA.cols];
        std::string strImg;
        for (int i = 0; i < imgGrayOrglBA.rows; i++) {
            for (int j = 0; j < imgGrayOrglBA.cols; j++) {
                uint8_t intensity = imgGrayOrglBA.at<uchar>(i, j);
                intensityMatrix[i][j] = intensity;
                if (intensity > 32) strImg += argv[3];
                else strImg += " ";
            }
            strImg += "\n";
        }

        cv::Mat imgTarget;
        imgResizedOrglBA.copyTo(imgTarget);

        cv::imshow(wOrgl, imgOrglBA);
        cv::resize(imgTarget, imgTarget, cv::Size(480, 360));
        cv::imshow(wTarget, imgTarget);

        if (cv::waitKey(30) == 27) {
            music.pause();
            while (true) {
                if (cv::waitKey(30) == 27) break;
            }
            music.play();
        }

        std::cout << strImg;
    }

    return EXIT_SUCCESS;
}
