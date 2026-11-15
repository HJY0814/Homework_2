//
// Created by HeJingyuan on 2026/11/13.
//

#ifndef HOMEWORK_2_DETECTOR_H
#define HOMEWORK_2_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

class Detector {
public:
    // 构造函数与析构函数
    Detector();
    ~Detector();

    // 1. 用于识别 Apriltag (传入摄像头抓到的单帧图像)
    void detect(Mat& frame);

    // 2. 重载函数：用于识别魔方 (传入本地视频路径，自动播放并识别)
    void detect(const string& video_path);

private:
    // 存储魔方标准的 6 种颜色
    Vec3b std_colors[6];
};

#endif //HOMEWORK_2_DETECTOR_H