//
// Created by pyrow on 18/12/2023.
//

#ifndef BADAPPLE_UTILS_HPP
#define BADAPPLE_UTILS_HPP

//#define ALPHA
#define MODE 2

#define WAIT_TIME 1

//#define TARGET_WIDTH 480
//#define TARGET_HEIGHT 360
//#define TARGET_WIDTH 960
//#define TARGET_HEIGHT 720
//#define TARGET_WIDTH 1440
//#define TARGET_HEIGHT 1080
#define TARGET_WIDTH 2880
#define TARGET_HEIGHT 2160

//#define TARGET_WIDTH 2870
//#define TARGET_HEIGHT 2156

#define BA_FPS 30

#define QT_PRECISION_W 6
#define QT_PRECISION_H 6

#define MIN_INTENSITY 32
#define MAX_INTENSITY_GAP 6

#define MIN_ALPHA 32.

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

typedef struct VideoInfo {
    std::string path;
    int delay;
} VideoInfo;

typedef struct Rectangle {
    int x;
    int y;
    int w;
    int h;
} Rectangle;

typedef struct QuadTree {
    bool value;
    Rectangle rect;
    struct QuadTree *topLeft;
    struct QuadTree *topRight;
    struct QuadTree *botLeft;
    struct QuadTree *botRight;
} QuadTree;

class Utils {

public:
    static cv::Size targetSize;
    static const char *orglBAPath;
    static char displayC;
    static double changeProb;

    static const std::vector<VideoInfo> butBAPaths;

    static void myAssert(bool cond, const char *errMsg);

    static double getRandomFloat();

    static int getRandomInt(int maxExcluded);

    static void fillMatrixRandom(int **array, int n, int m, int maxExcluded);

    static bool isRectangleSameValue(uint8_t **matrix, int x, int y, int w, int h);

    static bool hasRectangleEnoughAlpha(uint8_t **matrix, int x, int y,
                                        int w, int h, double minAlpha);

    static void buildQuadTreeFromMatrixRec(QuadTree *quadTree, uint8_t **matrix,
                                           int x, int y, int w, int h);

    static QuadTree *getQuadTreeFromMatrix(uint8_t **matrix, int w, int h);

    static void destroyQuadTree(QuadTree *quadTree);

    static void addImgToImg(cv::Mat &src, cv::Mat &addImg, int x, int y, int w, int h);

    static void addImgToImgMapAlpha(cv::Mat &src, cv::Mat &addImg,
                                    int x, int y, int w, int h);

    static void addImgToImgSimple(cv::Mat &src, cv::Mat &addImg,
                                  int x, int y, int w, int h);

};

#endif
