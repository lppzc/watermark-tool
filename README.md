```markdown
# 图像水印添加工具

一个基于OpenCV的C++图像水印添加工具，支持文字水印和图像水印，可用于批量处理图片。

## 功能特性

- ✅ 支持文字水印和PNG图像水印（支持透明通道）
- ✅ 支持单张图片处理和批量处理
- ✅ 自动处理水印尺寸适配
- ✅ 支持透明度调节
- ✅ 支持多种图像格式（JPG, PNG, BMP, TIFF等）
- ✅ 跨平台支持（Linux/Windows/macOS）

## 目录结构

```
.
├── watermark.cpp          # 主程序源代码
├── batch_watermark.sh     # 批量处理脚本
├── README.md             # 本说明文档
├── input/                # 待处理图片存放目录
├── output/               # 处理后图片输出目录
└── watermark.png         # 示例水印图片（可选）
```

## 编译要求

- C++11 或更高版本
- OpenCV 4.x
- pkg-config (Linux/macOS)

### Ubuntu/Debian 系统安装依赖

sudo apt update
sudo apt install build-essential
sudo apt install libopencv-dev
```

### macOS 系统安装依赖

```bash
brew install opencv
```

## 编译方法

### Linux/macOS:

```bash
g++ -std=c++11 watermark.cpp -o watermark $(pkg-config --cflags --libs opencv4)
```

### Windows:

使用Visual Studio或MinGW进行编译，需要链接OpenCV库。

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

## 脚本配置

在 [batch_watermark.sh](file:///home/vscode/water_in/batch_watermark.sh) 脚本中可以修改以下配置：

```bash
INPUT_DIR="input"           # 输入目录
OUTPUT_DIR="output"         # 输出目录
WATERMARK_TEXT="© 2024"     # 文字水印内容
WATERMARK_IMAGE="watermark.png" # 图像水印文件
MODE="text"                 # 水印模式: "text" 或 "image"
```

## 程序参数说明

### 命令行参数

```bash
./watermark <输入图像> <输出图像> [水印内容]
```

- `<输入图像>`: 源图像文件路径
- `<输出图像>`: 添加水印后的图像保存路径
- `[水印内容]`: 
  - 如果提供的是有效图像文件，则作为图像水印处理
  - 否则作为文字水印内容处理
  - 如果省略，则使用默认文字水印"WATERMARK"

## 支持的图像格式

- JPEG/JPG
- PNG (支持透明通道)
- BMP
- TIFF

## 注意事项

1. 确保输入图像文件存在且可读
2. 确保有足够的磁盘空间存储输出图像
3. 图像水印支持PNG格式的透明通道
4. 批量处理时，脚本会自动创建输出目录
5. 在Windows系统下需要适当调整编译参数

## 错误处理

程序会在以下情况下输出错误信息：

- 输入文件不存在或无法读取
- 水印文件无法加载
- 输出文件无法保存
- OpenCV库未正确安装

## 许可证

MIT License

## 贡献

欢迎提交Issue和Pull Request来改进这个工具。
```