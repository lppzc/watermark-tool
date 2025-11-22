#!/bin/bash

# 批量水印处理脚本
# 用途: 自动处理input文件夹中的所有图片，添加水印后保存到output文件夹

# 配置区域 - 可根据需要修改
INPUT_DIR="input"
OUTPUT_DIR="output"
WATERMARK_TEXT="lpz blog all rights reserved"
WATERMARK_IMAGE="watermark.png"  # 如果使用图像水印，请确保此文件存在
WATERMARK_COLOR="black"          # 水印颜色: white, black, red, green, blue 或 "B,G,R"
WATERMARK_POSITION="br"          # 水印位置: tl,tc,tr,ml,c,mr,bl,bc,br 或 1-9
WATERMARK_SIZE="1.5"             # 字体大小
WATERMARK_ALPHA="0.6"            # 透明度 (0.0-1.0)
MODE="text"                      # 可选: "text"(文字水印) 或 "image"(图像水印)
EXECUTABLE="./watermark"         # 水印程序可执行文件路径

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 检查必需的文件和目录
echo -e "${BLUE}=== 批量水印处理工具 ===${NC}"

# 检查可执行文件是否存在
if [ ! -f "$EXECUTABLE" ]; then
    echo -e "${RED}错误: 可执行文件 $EXECUTABLE 不存在${NC}"
    echo -e "${YELLOW}请先编译watermark程序:${NC}"
    echo "  g++ -std=c++11 watermark.cpp -o watermark \$(pkg-config --cflags --libs opencv4)"
    exit 1
fi

# 检查输入目录
if [ ! -d "$INPUT_DIR" ]; then
    echo -e "${RED}错误: 输入目录 $INPUT_DIR 不存在${NC}"
    echo "请创建 $INPUT_DIR 目录并将需要处理的图片放入其中"
    exit 1
fi

# 创建输出目录（如果不存在）
mkdir -p "$OUTPUT_DIR"

# 检查图像水印文件（如果使用图像模式）
if [ "$MODE" = "image" ] && [ ! -f "$WATERMARK_IMAGE" ]; then
    echo -e "${YELLOW}警告: 图像水印文件 $WATERMARK_IMAGE 不存在${NC}"
    echo -e "${YELLOW}切换到文字水印模式${NC}"
    MODE="text"
fi

# 统计文件数量
IMAGE_COUNT=$(find "$INPUT_DIR" -type f \( -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.png" -o -iname "*.bmp" -o -iname "*.tiff" \) | wc -l)
if [ "$IMAGE_COUNT" -eq 0 ]; then
    echo -e "${YELLOW}警告: $INPUT_DIR 目录中没有找到支持的图片文件${NC}"
    exit 0
fi

echo -e "${GREEN}找到 $IMAGE_COUNT 个图片文件待处理${NC}"
echo -e "${BLUE}输入目录: $INPUT_DIR${NC}"
echo -e "${BLUE}输出目录: $OUTPUT_DIR${NC}"
echo -e "${BLUE}水印模式: $MODE${NC}"
if [ "$MODE" = "text" ]; then
    echo -e "${BLUE}水印文字: $WATERMARK_TEXT${NC}"
    echo -e "${BLUE}水印颜色: $WATERMARK_COLOR${NC}"
    echo -e "${BLUE}水印位置: $WATERMARK_POSITION${NC}"
    echo -e "${BLUE}字体大小: $WATERMARK_SIZE${NC}"
    echo -e "${BLUE}透明度: $WATERMARK_ALPHA${NC}"
else
    echo -e "${BLUE}水印图像: $WATERMARK_IMAGE${NC}"
    echo -e "${BLUE}水印位置: $WATERMARK_POSITION${NC}"
    echo -e "${BLUE}透明度: $WATERMARK_ALPHA${NC}"
fi
echo "------------------------"

# 初始化计数器
PROCESSED=0
FAILED=0

# 使用数组存储文件列表，避免管道导致的子shell问题
mapfile -t files < <(find "$INPUT_DIR" -type f \( -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.png" -o -iname "*.bmp" -o -iname "*.tiff" \))

# 处理每个图片文件
for file in "${files[@]}"; do
    # 获取文件名和扩展名
    filename=$(basename "$file")
    extension="${filename##*.}"
    name="${filename%.*}"
    
    # 构建输出文件路径
    output_file="$OUTPUT_DIR/${name}_watermarked.$extension"
    
    echo -e "${BLUE}正在处理: $filename${NC}"
    
    # 根据模式调用不同的处理方式
    if [ "$MODE" = "image" ]; then
        # 使用图像水印
        if ./"$EXECUTABLE" "$file" "$output_file" --image "$WATERMARK_IMAGE" \
            --position "$WATERMARK_POSITION" --alpha "$WATERMARK_ALPHA"; then
            echo -e "${GREEN}✓ 成功: $filename -> ${output_file}${NC}"
            ((PROCESSED++))
        else
            echo -e "${RED}✗ 失败: $filename${NC}"
            ((FAILED++))
        fi
    else
        # 使用文字水印和指定参数
        if ./"$EXECUTABLE" "$file" "$output_file" --text "$WATERMARK_TEXT" \
            --position "$WATERMARK_POSITION" --color "$WATERMARK_COLOR" \
            --size "$WATERMARK_SIZE" --alpha "$WATERMARK_ALPHA"; then
            echo -e "${GREEN}✓ 成功: $filename -> ${output_file}${NC}"
            ((PROCESSED++))
        else
            echo -e "${RED}✗ 失败: $filename${NC}"
            ((FAILED++))
        fi
    fi
done

echo "------------------------"
echo -e "${GREEN}处理完成!${NC}"
echo -e "${GREEN}成功处理: $PROCESSED 个文件${NC}"
if [ "$FAILED" -gt 0 ]; then
    echo -e "${RED}处理失败: $FAILED 个文件${NC}"
fi
echo -e "${BLUE}输出文件位于: $OUTPUT_DIR${NC}"