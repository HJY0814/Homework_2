//
// Created by HeJingyuan on 2026/11/13.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "../src/Detector.h"

using namespace std;
using namespace cv;

int main() {
    Detector detector;
    string cube_video_path = "/home/croissant/Homework_2/cube.mp4";

    // detector.detect(cube_video_path);

    VideoCapture cap(0); // 打开默认摄像头 (0)
    if (cap.isOpened()) {
        Mat frame;
        while (cap.isOpened()) {
            cap >> frame;
            if (frame.empty()) break;

            detector.detect(frame);
            imshow("Apriltag Camera", frame);
            if (waitKey(30) == 27) break;
        }
        cap.release();
    }

    return 0;
}