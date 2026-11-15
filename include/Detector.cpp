//
// Created by HeJingyuan on 2026/11/13.
//

#include "../src/Detector.h"
#include <opencv2/aruco.hpp>
using namespace std;
using namespace cv;

// 构造函数：初始化标准颜色
Detector::Detector() {
    std_colors[0] = Vec3b(255, 255, 255); // 0: 白
    std_colors[1] = Vec3b(0, 255, 255); // 1: 黄
    std_colors[2] = Vec3b(0, 0, 255); // 2: 红
    std_colors[3] = Vec3b(0, 165, 255); // 3: 橙
    std_colors[4] = Vec3b(255, 0, 0); // 4: 蓝
    std_colors[5] = Vec3b(0, 255, 0); // 5: 绿
}

Detector::~Detector() = default;

// 1. 用于识别 Apriltag (传入摄像头抓到的单帧图像)
void Detector::detect(Mat& frame) {
    if (frame.empty()) return;

    // 1. 转灰度图
    Mat gray;
    cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // 2. 获取预定义字典
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_APRILTAG_36h11);

    // 3. 准备检测参数
    Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();

    // 4. 准备输出变量
    vector<vector<Point2f>> corners;
    vector<int> ids;
    vector<vector<Point2f>> rejected;

    // 5. 直接调用全局函数进行检测
    aruco::detectMarkers(gray, dictionary, corners, ids, parameters, rejected);

    // 6. 如果检测到，就在原图上画框和 ID
    if (!ids.empty()) {
        aruco::drawDetectedMarkers(frame, corners, ids);
    }
}

// 2. 重载函数：用于识别魔方
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

        // ==========================================
        // 魔方识别核心算法（原 processCubeFrame 的内容）
        // ==========================================

        // 1. 灰度 + 高斯模糊
        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
        GaussianBlur(gray_frame, gaussian_frame, Size(5, 5), 0);

        // 2. Canny 边缘检测
        Canny(gaussian_frame, edges_frame, 50, 150);

        // 3. 闭运算填补断线（解决深色魔方找不到边缘的问题）
        Mat kernel = getStructuringElement(MORPH_RECT, Size(9, 9));
        morphologyEx(edges_frame, edges_frame, MORPH_CLOSE, kernel);

        // 4. 找轮廓
        vector<vector<Point> > contours;
        findContours(edges_frame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // 准备画布
        result_frame = frame.clone();

        // 5. 找面积最大的轮廓（定位魔方）
        double max_area = 0;
        Rect best_rect;

        for (size_t i = 0; i < contours.size(); i++) {
            double area = contourArea(contours[i]);
            if (area > 5000 && area > max_area) {
                max_area = area;
                best_rect = boundingRect(contours[i]);
            }
        }

        // 6. 识别颜色并画框
        if (max_area > 0) {
            // 取框正中心 20x20 的区域
            int cx = best_rect.x + best_rect.width / 2;
            int cy = best_rect.y + best_rect.height / 2;
            Rect center_roi(cx - 10, cy - 10, 20, 20);

            // 边界保护
            if (center_roi.x >= 0 && center_roi.y >= 0 &&
                center_roi.x + center_roi.width <= frame.cols &&
                center_roi.y + center_roi.height <= frame.rows) {
                Mat roi_mat = frame(center_roi);
                Mat roi_hsv;
                cvtColor(roi_mat, roi_hsv, COLOR_BGR2HSV);

                // 计算平均 HSV
                Scalar mean_hsv = mean(roi_hsv);
                int H = mean_hsv[0];
                int S = mean_hsv[1];
                int V = mean_hsv[2];

                // 匹配标准颜色
                Vec3b draw_color;
                if (V < 40) {
                    draw_color = Vec3b(0, 0, 0); // 黑色（极暗情况）
                } else if (S < 60 && V > 100) {
                    draw_color = std_colors[0]; // 白
                } else {
                    if (H < 10 || H > 160) draw_color = std_colors[2]; // 红
                    else if (H < 25) draw_color = std_colors[3]; // 橙
                    else if (H < 35) draw_color = std_colors[1]; // 黄
                    else if (H < 85) draw_color = std_colors[5]; // 绿
                    else if (H < 130) draw_color = std_colors[4]; // 蓝
                    else draw_color = std_colors[2]; // 兜底
                }

                // 用标准色画框
                rectangle(result_frame, best_rect, Scalar(draw_color[0], draw_color[1], draw_color[2]), 3);
            }
        }
        // ==========================================

        // 显示窗口
        imshow("Image_raw", frame);
        if (!result_frame.empty()) {
            imshow("result", result_frame);
        }

        // 按 ESC 退出
        if (waitKey(30) == 27) break;
    }

    cap.release();
    destroyAllWindows();
}
