# Watermark Tool v1.0.0

基于OpenCV的C++图像水印工具，支持文字水印和图像水印，可用于批量处理图片。

## 功能特性

- ✅ 支持文字水印和PNG图像水印（支持透明通道）
- ✅ 支持单张图片处理和批量处理
- ✅ 自动处理水印尺寸适配
- ✅ 支持透明度调节
- ✅ 支持多种图像格式（JPG, PNG, BMP, TIFF等）
- ✅ 跨平台支持（Linux/Windows/macOS）

## 安装

### 编译要求

- C++11 或更高版本
- OpenCV 4.x
- pkg-config (Linux/macOS)
- CMake (推荐)

### Ubuntu/Debian 系统安装依赖

```bash
sudo apt update
sudo apt install build-essential
sudo apt install libopencv-dev
sudo apt install cmake
```

### 编译方法

```bash
mkdir build
cd build
cmake ..
make
```

或者直接使用 g++ 编译：

```bash
g++ -std=c++11 watermark.cpp -o watermark $(pkg-config --cflags --libs opencv4)
```

## 使用方法

### 单张图片处理

```bash
# 添加文字水印
./watermark input.jpg output.jpg "水印文字"

# 添加图像水印
./watermark input.jpg output.jpg watermark.png

# 使用默认水印
./watermark input.jpg output.jpg
```

### 批量处理

1. 将待处理的图片放入 `input/` 目录
2. 运行批处理脚本：

```bash
chmod +x batch_watermark.sh
./batch_watermark.sh
```

处理后的图片将保存在 `output/` 目录中。

## 配置选项

在 [batch_watermark.sh](file:///home/vscode/water_in/batch_watermark.sh) 脚本中可以修改以下配置：

```bash
INPUT_DIR="input"           # 输入目录
OUTPUT_DIR="output"         # 输出目录
WATERMARK_TEXT="© 2024"     # 文字水印内容
WATERMARK_IMAGE="watermark.png" # 图像水印文件
MODE="text"                 # 水印模式: "text" 或 "image"
```

## 许可证

MIT License