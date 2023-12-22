//
// Created by pyrow on 18/12/2023.
//

#ifndef BADAPPLE_UTILS_HPP
#define BADAPPLE_UTILS_HPP

#define WAIT_TIME 1

#define BA_WIDTH 480
#define BA_HEIGHT 360
#define BA_FPS 30

#define R_LIST_SIZE 8192

#define QT_PRECISION_W 1
#define QT_PRECISION_H 1

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

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

    static const std::vector<std::string> butBAPaths;

    static void myAssert(bool cond, const char *errMsg);

    static void swapInt(int *a, int *b);

    static void fillArrayRandom(int array[], int n, int max);

    static bool isRectangle(bool **matrix, int x, int y, int w, int h);

    static Rectangle popBiggestRectangleInMatrix(bool **matrix, int w, int h);

    static bool isRectangleSameValue(bool **matrix, int x, int y, int w, int h);

    static void buildQuadTreeFromMatrixRec(QuadTree *quadTree, bool **matrix,
                                           int x, int y, int w, int h);

    static QuadTree *getQuadTreeFromMatrix(bool **matrix, int w, int h);

    static void destroyQuadTree(QuadTree *quadTree);

    static void addImgToImg(cv::Mat &src, cv::Mat &addImg, int x, int y, int w, int h);

};

#endif
