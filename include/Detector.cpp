//
// Created by HeJingyuan on 2026/11/13.
//

#include "../src/Detector.h"
#include <opencv2/aruco.hpp>
using namespace std;
using namespace cv;

// 构造函数：初始化标准颜色
Detector::Detector() {
    cube_colors[0] = Vec3b(255, 255, 255); // 0: 白
    cube_colors[1] = Vec3b(0, 255, 255); // 1: 黄
    cube_colors[2] = Vec3b(0, 0, 255); // 2: 红
    cube_colors[3] = Vec3b(0, 165, 255); // 3: 橙
    cube_colors[4] = Vec3b(255, 0, 0); // 4: 蓝
    cube_colors[5] = Vec3b(0, 255, 0); // 5: 绿
}

Detector::~Detector() = default;

// 用于识别 Apriltag
void Detector::detect(Mat& frame) {
    if (frame.empty()) return;

    Mat gray;
    cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    const auto dict = aruco::getPredefinedDictionary(aruco::DICT_APRILTAG_36h11);
    const auto parameters = aruco::DetectorParameters::create();

    vector<vector<Point2f>> corners;
    vector<int> ids;
    vector<vector<Point2f>> rejected;

    aruco::detectMarkers(gray, dict, corners, ids, parameters, rejected);

    if (!ids.empty()) {
        aruco::drawDetectedMarkers(frame, corners, ids);
    }
}

// 重载函数：用于识别魔方
void Detector::detect(const string &video_path) {
    VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        cout << "魔方视频获取失败" << endl;
        return;
    }

    Mat frame, gray_frame, edges_frame, gaussian_frame, result_frame;

    while (cap.isOpened()) {
        cap >> frame;
        if (frame.empty()) break;

        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
        GaussianBlur(gray_frame, gaussian_frame, Size(5, 5), 0);
        Canny(gaussian_frame, edges_frame, 50, 150);

        Mat kernel = getStructuringElement(MORPH_RECT, Size(9, 9));
        morphologyEx(edges_frame, edges_frame, MORPH_CLOSE, kernel);

        vector<vector<Point>> outline;
        findContours(edges_frame, outline, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        result_frame = frame.clone();

        double max_area = 0;
        Rect cube_rect;

        for (size_t i = 0; i < outline.size(); i++) {
            double area = contourArea(outline[i]);
            if (area > 5000 && area > max_area) {
                max_area = area;
                cube_rect = boundingRect(outline[i]);
            }
        }

        if (max_area > 0) {
            int cx = cube_rect.x + cube_rect.width / 2;
            int cy = cube_rect.y + cube_rect.height / 2;
            Rect center_box(cx - 10, cy - 10, 20, 20);

            Mat box_mat = frame(center_box);
            Mat box_hsv;
            cvtColor(box_mat, box_hsv, COLOR_BGR2HSV);

            Scalar mean_hsv = mean(box_hsv);
            int H = mean_hsv[0];
            int S = mean_hsv[1];
            int V = mean_hsv[2];

            Vec3b draw_color;
            if (S < 60 && V > 100) {
                draw_color = cube_colors[0]; // 白
            } else {
                if (H < 10 || H > 160) draw_color = cube_colors[2]; // 红
                else if (H < 25) draw_color = cube_colors[3]; // 橙
                else if (H < 35) draw_color = cube_colors[1]; // 黄
                else if (H < 85) draw_color = cube_colors[5]; // 绿
                else if (H < 130) draw_color = cube_colors[4]; // 蓝
                else draw_color = Vec3b(0, 0, 0);
            }

            rectangle(result_frame, cube_rect,
                Scalar(draw_color[0], draw_color[1], draw_color[2]), 3);

        }

        imshow("Image_raw", frame);
        imshow("Image_binary", gray_frame);
        imshow("result", result_frame);

        if (waitKey(30) == 27) break;
    }

    cap.release();
    destroyAllWindows();
}
