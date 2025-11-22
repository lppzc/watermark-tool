#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

// 解析颜色字符串的辅助函数
cv::Scalar parseColor(const std::string& colorStr) {
    // 支持格式: "R,G,B" 或 "R G B" 或预定义颜色名
    if (colorStr == "white" || colorStr == "WHITE") {
        return cv::Scalar(255, 255, 255);
    } else if (colorStr == "black" || colorStr == "BLACK") {
        return cv::Scalar(0, 0, 0);
    } else if (colorStr == "red" || colorStr == "RED") {
        return cv::Scalar(0, 0, 255);
    } else if (colorStr == "green" || colorStr == "GREEN") {
        return cv::Scalar(0, 255, 0);
    } else if (colorStr == "blue" || colorStr == "BLUE") {
        return cv::Scalar(255, 0, 0);
    } else {
        // 解析 "R,G,B" 或 "R G B" 格式
        std::stringstream ss(colorStr);
        std::string item;
        std::vector<int> colors;
        
        while (std::getline(ss, item, ',')) {
            // 如果用空格分隔，再次分割
            std::stringstream ss2(item);
            std::string subItem;
            while (ss2 >> subItem && colors.size() < 3) {
                try {
                    colors.push_back(std::stoi(subItem));
                } catch (...) {
                    // 忽略无效值
                }
            }
        }
        
        // 确保有3个颜色值 (B, G, R)
        if (colors.size() >= 3) {
            return cv::Scalar(colors[0], colors[1], colors[2]); // BGR格式
        }
    }
    
    // 默认返回白色
    return cv::Scalar(255, 255, 255);
}

// 九宫格位置枚举
enum WatermarkPosition {
    TOP_LEFT = 1,
    TOP_CENTER = 2,
    TOP_RIGHT = 3,
    MIDDLE_LEFT = 4,
    CENTER = 5,
    MIDDLE_RIGHT = 6,
    BOTTOM_LEFT = 7,
    BOTTOM_CENTER = 8,
    BOTTOM_RIGHT = 9
};

// 解析位置参数
WatermarkPosition parsePosition(const std::string& posStr) {
    std::map<std::string, WatermarkPosition> posMap = {
        {"top-left", TOP_LEFT}, {"tl", TOP_LEFT},
        {"top-center", TOP_CENTER}, {"tc", TOP_CENTER},
        {"top-right", TOP_RIGHT}, {"tr", TOP_RIGHT},
        {"middle-left", MIDDLE_LEFT}, {"ml", MIDDLE_LEFT},
        {"center", CENTER}, {"c", CENTER},
        {"middle-right", MIDDLE_RIGHT}, {"mr", MIDDLE_RIGHT},
        {"bottom-left", BOTTOM_LEFT}, {"bl", BOTTOM_LEFT},
        {"bottom-center", BOTTOM_CENTER}, {"bc", BOTTOM_CENTER},
        {"bottom-right", BOTTOM_RIGHT}, {"br", BOTTOM_RIGHT}
    };
    
    auto it = posMap.find(posStr);
    if (it != posMap.end()) {
        return it->second;
    }
    
    // 尝试解析数字
    try {
        int pos = std::stoi(posStr);
        if (pos >= 1 && pos <= 9) {
            return static_cast<WatermarkPosition>(pos);
        }
    } catch (...) {
        // 忽略转换错误
    }
    
    // 默认返回右下角
    return BOTTOM_RIGHT;
}

// 根据位置计算水印坐标
cv::Point calculatePosition(WatermarkPosition position, cv::Size imageSize, cv::Size watermarkSize, int margin = 10) {
    int x = 0, y = 0;
    
    switch (position) {
        case TOP_LEFT:
            x = margin;
            y = margin;
            break;
        case TOP_CENTER:
            x = (imageSize.width - watermarkSize.width) / 2;
            y = margin;
            break;
        case TOP_RIGHT:
            x = imageSize.width - watermarkSize.width - margin;
            y = margin;
            break;
        case MIDDLE_LEFT:
            x = margin;
            y = (imageSize.height - watermarkSize.height) / 2;
            break;
        case CENTER:
            x = (imageSize.width - watermarkSize.width) / 2;
            y = (imageSize.height - watermarkSize.height) / 2;
            break;
        case MIDDLE_RIGHT:
            x = imageSize.width - watermarkSize.width - margin;
            y = (imageSize.height - watermarkSize.height) / 2;
            break;
        case BOTTOM_LEFT:
            x = margin;
            y = imageSize.height - watermarkSize.height - margin;
            break;
        case BOTTOM_CENTER:
            x = (imageSize.width - watermarkSize.width) / 2;
            y = imageSize.height - watermarkSize.height - margin;
            break;
        case BOTTOM_RIGHT:
            x = imageSize.width - watermarkSize.width - margin;
            y = imageSize.height - watermarkSize.height - margin;
            break;
    }
    
    return cv::Point(x, y);
}

class WatermarkAdder {
private:
    cv::Mat image;
    cv::Mat watermark;

public:
    // 加载图像和水印
    bool loadImage(const std::string& imagePath) {
        image = cv::imread(imagePath, cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cerr << "无法加载图像: " << imagePath << std::endl;
            return false;
        }
        return true;
    }

    // 加载水印图像
    bool loadWatermark(const std::string& watermarkPath) {
        watermark = cv::imread(watermarkPath, cv::IMREAD_UNCHANGED);
        if (watermark.empty()) {
            std::cerr << "无法加载水印: " << watermarkPath << std::endl;
            return false;
        }
        return true;
    }

    // 添加图像水印
    cv::Mat addImageWatermark(WatermarkPosition position, double alpha = 0.3, int maxWidth = 0, int maxHeight = 0) {
        if (image.empty()) {
            std::cerr << "未加载源图像" << std::endl;
            return cv::Mat();
        }
        
        if (watermark.empty()) {
            std::cerr << "未加载水印图像" << std::endl;
            return cv::Mat();
        }
        
        cv::Mat result = image.clone();
        
        // 调整水印大小
        cv::Mat resizedWatermark = watermark.clone();
        double scaleRatio = 1.0;
        
        if (maxWidth > 0 && maxHeight > 0) {
            // 按照最大尺寸限制调整水印大小
            double widthRatio = (double)maxWidth / watermark.cols;
            double heightRatio = (double)maxHeight / watermark.rows;
            scaleRatio = std::min(widthRatio, heightRatio);
            scaleRatio = std::min(scaleRatio, 1.0); // 不放大
            
            if (scaleRatio < 1.0) {
                cv::resize(watermark, resizedWatermark, cv::Size(0, 0), scaleRatio, scaleRatio);
            }
        } else {
            // 默认按图像大小的1/4调整
            double defaultMaxWidth = image.cols / 4.0;
            double defaultMaxHeight = image.rows / 4.0;
            double widthRatio = defaultMaxWidth / watermark.cols;
            double heightRatio = defaultMaxHeight / watermark.rows;
            scaleRatio = std::min(widthRatio, heightRatio);
            
            if (scaleRatio < 1.0) {
                cv::resize(watermark, resizedWatermark, cv::Size(0, 0), scaleRatio, scaleRatio);
            }
        }

        // 计算水印位置
        cv::Point pos = calculatePosition(position, image.size(), resizedWatermark.size());
        
        // 确保水印不会超出图像边界
        pos.x = std::max(0, std::min(pos.x, image.cols - resizedWatermark.cols));
        pos.y = std::max(0, std::min(pos.y, image.rows - resizedWatermark.rows));
        
        if (pos.x + resizedWatermark.cols > image.cols) {
            resizedWatermark = resizedWatermark(cv::Rect(0, 0, image.cols - pos.x, resizedWatermark.rows));
        }
        if (pos.y + resizedWatermark.rows > image.rows) {
            resizedWatermark = resizedWatermark(cv::Rect(0, 0, resizedWatermark.cols, image.rows - pos.y));
        }

        // 提取ROI区域
        cv::Mat roi = result(cv::Rect(pos.x, pos.y, resizedWatermark.cols, resizedWatermark.rows));

        // 处理透明通道
        if (resizedWatermark.channels() == 4) {
            std::vector<cv::Mat> channels;
            cv::split(resizedWatermark, channels);
            
            if (channels.size() >= 4) {
                cv::Mat bgr;
                cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, bgr);
                
                cv::Mat alphaChannel = channels[3];
                cv::Mat alphaFloat;
                alphaChannel.convertTo(alphaFloat, CV_32F, 1.0/255);
                
                // 逐像素处理透明度
                for (int i = 0; i < roi.rows; ++i) {
                    for (int j = 0; j < roi.cols; ++j) {
                        float a = alphaFloat.at<float>(i, j) * alpha;
                        cv::Vec3b& background = roi.at<cv::Vec3b>(i, j);
                        cv::Vec3b foreground = bgr.at<cv::Vec3b>(i, j);
                        
                        background[0] = cv::saturate_cast<uchar>(foreground[0] * a + background[0] * (1 - a));
                        background[1] = cv::saturate_cast<uchar>(foreground[1] * a + background[1] * (1 - a));
                        background[2] = cv::saturate_cast<uchar>(foreground[2] * a + background[2] * (1 - a));
                    }
                }
            }
        } else {
            // 简单叠加水印
            cv::addWeighted(roi, 1.0 - alpha, resizedWatermark, alpha, 0, roi);
        }

        return result;
    }

    // 添加文字水印
    cv::Mat addTextWatermark(const std::string& text, 
                            WatermarkPosition position,
                            double fontScale = 1.0,
                            cv::Scalar color = cv::Scalar(255, 255, 255),
                            int thickness = 2,
                            double alpha = 0.5) {
        if (image.empty()) {
            std::cerr << "未加载源图像" << std::endl;
            return cv::Mat();
        }
        
        cv::Mat result = image.clone();
        
        // 计算文字尺寸
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, &baseline);
        
        // 计算文字位置
        cv::Point pos = calculatePosition(position, image.size(), textSize);
        
        // 确保文字不会超出图像边界
        pos.x = std::max(0, std::min(pos.x, image.cols - textSize.width));
        pos.y = std::max(textSize.height, std::min(pos.y, image.rows - baseline));

        // 创建覆盖层
        cv::Mat overlay;
        result.copyTo(overlay);
        
        // 在覆盖层上绘制文字
        cv::putText(overlay, text, pos, cv::FONT_HERSHEY_SIMPLEX, 
                   fontScale, color, thickness);
        
        // 混合原图和覆盖层
        cv::addWeighted(result, 1.0 - alpha, overlay, alpha, 0, result);
        
        return result;
    }

    // 批量处理图像
    void batchProcess(const std::vector<std::string>& inputPaths,
                     const std::string& outputPath,
                     const std::string& watermarkText,
                     WatermarkPosition position = BOTTOM_RIGHT,
                     cv::Scalar color = cv::Scalar(255, 255, 255),
                     double fontScale = 1.0,
                     double alpha = 0.5) {
        for (const auto& path : inputPaths) {
            if (loadImage(path)) {
                cv::Mat watermarked = addTextWatermark(watermarkText, 
                                                     position,
                                                     fontScale,
                                                     color,
                                                     2,
                                                     alpha);
                
                if (!watermarked.empty()) {
                    // 构造输出文件名
                    size_t lastSlash = path.find_last_of("/\\");
                    std::string filename = path.substr(lastSlash + 1);
                    size_t lastDot = filename.find_last_of(".");
                    std::string name = filename.substr(0, lastDot);
                    std::string extension = filename.substr(lastDot);
                    
                    std::string outputFilename = outputPath + "/" + name + "_watermarked" + extension;
                    if (cv::imwrite(outputFilename, watermarked)) {
                        std::cout << "已处理: " << outputFilename << std::endl;
                    } else {
                        std::cerr << "保存失败: " << outputFilename << std::endl;
                    }
                } else {
                    std::cerr << "处理失败: " << path << std::endl;
                }
            }
        }
    }

    // 保存结果
    bool saveImage(const cv::Mat& img, const std::string& outputPath) {
        if (img.empty()) {
            std::cerr << "图像为空，无法保存" << std::endl;
            return false;
        }
        
        bool success = cv::imwrite(outputPath, img);
        if (!success) {
            std::cerr << "保存图像失败: " << outputPath << std::endl;
        }
        return success;
    }
    
    // 获取图像尺寸
    cv::Size getImageSize() const {
        return image.size();
    }
};

// 显示帮助信息
void showHelp(const char* programName) {
    std::cout << "用法: " << programName << " <输入图像> <输出图像> [选项]" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  文字水印: " << programName << " input.jpg output.jpg \"水印文字\" [参数...]" << std::endl;
    std::cout << "  图像水印: " << programName << " input.jpg output.jpg watermark.png [参数...]" << std::endl;
    std::cout << std::endl;
    std::cout << "参数:" << std::endl;
    std::cout << "  --text \"文字\"        设置文字水印内容" << std::endl;
    std::cout << "  --image 文件.png      设置图像水印文件" << std::endl;
    std::cout << "  --position 位置       设置水印位置 (1-9 或名称)" << std::endl;
    std::cout << "  --color 颜色          设置文字颜色" << std::endl;
    std::cout << "  --size 大小           设置文字大小" << std::endl;
    std::cout << "  --alpha 透明度        设置水印透明度 (0.0-1.0)" << std::endl;
    std::cout << "  --thickness 厚度      设置文字粗细" << std::endl;
    std::cout << std::endl;
    std::cout << "位置选项:" << std::endl;
    std::cout << "  数字: 1-9 (九宫格位置)" << std::endl;
    std::cout << "  名称: tl/tc/tr/ml/c/mr/bl/bc/br" << std::endl;
    std::cout << "  全名: top-left/top-center/top-right/middle-left/center/middle-right/bottom-left/bottom-center/bottom-right" << std::endl;
    std::cout << std::endl;
    std::cout << "颜色格式:" << std::endl;
    std::cout << "  预定义颜色: white, black, red, green, blue" << std::endl;
    std::cout << "  RGB值: \"255,0,0\" (红色), \"0,255,0\" (绿色), \"0,0,255\" (蓝色)" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << programName << " photo.jpg output.jpg --text \"版权所有\" --position br --color red --alpha 0.7" << std::endl;
    std::cout << "  " << programName << " photo.jpg output.jpg --image watermark.png --position 5 --alpha 0.5" << std::endl;
}

// 参数解析结构体
struct WatermarkParams {
    std::string text = "WATERMARK";
    std::string imageFile = "";
    WatermarkPosition position = BOTTOM_RIGHT;
    cv::Scalar color = cv::Scalar(255, 255, 255);
    double fontSize = 1.0;
    double alpha = 0.5;
    int thickness = 2;
    bool isImageMode = false;
};

// 解析命令行参数
WatermarkParams parseArguments(int argc, char* argv[]) {
    WatermarkParams params;
    
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--text" && i + 1 < argc) {
            params.text = argv[++i];
        } else if (arg == "--image" && i + 1 < argc) {
            params.imageFile = argv[++i];
            params.isImageMode = true;
        } else if (arg == "--position" && i + 1 < argc) {
            params.position = parsePosition(argv[++i]);
        } else if (arg == "--color" && i + 1 < argc) {
            params.color = parseColor(argv[++i]);
        } else if (arg == "--size" && i + 1 < argc) {
            try {
                params.fontSize = std::stod(argv[++i]);
            } catch (...) {
                std::cerr << "无效的字体大小值" << std::endl;
            }
        } else if (arg == "--alpha" && i + 1 < argc) {
            try {
                params.alpha = std::stod(argv[++i]);
                params.alpha = std::max(0.0, std::min(1.0, params.alpha)); // 限制在0-1之间
            } catch (...) {
                std::cerr << "无效的透明度值" << std::endl;
            }
        } else if (arg == "--thickness" && i + 1 < argc) {
            try {
                params.thickness = std::stoi(argv[++i]);
            } catch (...) {
                std::cerr << "无效的粗细值" << std::endl;
            }
        } else if (arg == "--help" || arg == "-h") {
            showHelp(argv[0]);
            exit(0);
        } else if (i == 3 && argv[i][0] != '-') {
            // 兼容旧版参数格式
            if (std::string(argv[i]).find(".png") != std::string::npos || 
                std::string(argv[i]).find(".jpg") != std::string::npos ||
                std::string(argv[i]).find(".jpeg") != std::string::npos) {
                params.imageFile = argv[i];
                params.isImageMode = true;
            } else {
                params.text = argv[i];
            }
        }
    }
    
    return params;
}

// 主函数
int main(int argc, char* argv[]) {
    // 检查基本参数
    if (argc < 3) {
        showHelp(argv[0]);
        return -1;
    }
    
    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    
    // 解析参数
    WatermarkParams params = parseArguments(argc, argv);
    
    // 创建水印处理器
    WatermarkAdder adder;
    
    // 加载主图像
    if (!adder.loadImage(inputPath)) {
        return -1;
    }
    
    cv::Mat result;
    
    if (params.isImageMode) {
        // 图像水印模式
        if (adder.loadWatermark(params.imageFile)) {
            std::cout << "添加图像水印..." << std::endl;
            result = adder.addImageWatermark(params.position, params.alpha);
        } else {
            std::cerr << "无法加载图像水印: " << params.imageFile << std::endl;
            return -1;
        }
    } else {
        // 文字水印模式
        std::cout << "添加文字水印: " << params.text << std::endl;
        result = adder.addTextWatermark(params.text, params.position, params.fontSize, params.color, params.thickness, params.alpha);
    }
    
    // 保存结果
    if (result.empty()) {
        std::cerr << "处理失败，结果图像为空" << std::endl;
        return -1;
    }
    
    if (adder.saveImage(result, outputPath)) {
        std::cout << "水印添加成功，保存至: " << outputPath << std::endl;
    } else {
        return -1;
    }
    
    return 0;
}