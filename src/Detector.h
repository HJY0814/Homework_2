//
// Created by HeJingyuan on 2026/11/13.
//

#ifndef HOMEWORK_2_DETECTOR_H
#define HOMEWORK_2_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

class Detector {
public:
    Detector();
    ~Detector();

    // 用于识别 Apriltag
    void detect(Mat& frame);
    // 重载函数：用于识别魔方
    void detect(const string& video_path);

private:
    // 存储魔方颜色
    Vec3b cube_colors[6];
};

#endif //HOMEWORK_2_DETECTOR_H