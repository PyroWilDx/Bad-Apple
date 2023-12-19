#include <string>
#include <vector>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "Utils.hpp"

int main(int argc, char *argv[]) {
    std::string path = std::filesystem::current_path().string();
    std::filesystem::current_path(path + "/..");

    if (argc > 1) {
        Utils::targetSize = cv::Size(std::stoi(argv[1]), std::stoi(argv[2]));
    }

    cv::VideoCapture orglBA = cv::VideoCapture(Utils::orglBAPath);
    Utils::myAssert(orglBA.isOpened(), "Failed to open video.");

    std::vector<cv::VideoCapture> butBAList = {};
    for (std::string butBAPath : Utils::butBAPaths) {
        cv::VideoCapture butBA = cv::VideoCapture(Utils::orglBAPath);
        butBAList.push_back(butBA);
        Utils::myAssert(butBA.isOpened(), "Failed to open video.");
    }

    cv::Mat imgOrglBA;
    std::vector<cv::Mat> imgButBAList = std::vector<cv::Mat>(butBAList.size());
    while (true) {
        orglBA >> imgOrglBA;
        cv::resize(imgOrglBA, imgOrglBA, Utils::targetSize);
        for (int i = 0; i < imgButBAList.size(); i++) {
            butBAList[i] >> imgButBAList[i];
        }

        if (imgOrglBA.empty()) break;

        cv::Mat imgGrayOrglBA;
        cv::cvtColor(imgOrglBA, imgGrayOrglBA, cv::COLOR_BGR2GRAY);
        uint8_t intensityMatrix[imgGrayOrglBA.rows][imgGrayOrglBA.cols];
        std::string strImg;
        for (int i = 0; i < imgGrayOrglBA.rows; i++) {
            for (int j = 0; j < imgGrayOrglBA.cols; j++) {
                intensityMatrix[i][j] = imgGrayOrglBA.at<uchar>(i, j);
                if (intensityMatrix[i][j] != 0) strImg += (char) intensityMatrix[i][j];
                else strImg += " ";
            }
            strImg += "\n";
        }

        cv::Mat img;
        imgOrglBA.copyTo(img);

        cv::resize(img, img, Utils::targetSize);

        cv::resize(img, img, cv::Size(480, 360));
        cv::imshow("Test", img);

        if (cv::waitKey(30) == 27) {
            std::string stop;
            std::cin >> stop;
        }

        std::cout << strImg;
    }

    return EXIT_SUCCESS;
}
