# Watermark Tool v1.0.0

Initial release of the Watermark Tool - a C++ image watermarking tool based on OpenCV, supporting text and image watermarks for batch processing images.

## Features
- Support for text watermarks and PNG image watermarks (with alpha channel)
- Single image processing and batch processing
- Automatic watermark size adaptation
- Transparency adjustment support
- Support for multiple image formats (JPG, PNG, BMP, TIFF, etc.)
- Cross-platform support (Linux/Windows/macOS)

## Installation

### Requirements

- C++11 or higher
- OpenCV 4.x
- pkg-config (Linux/macOS)
- CMake (recommended)

### Installing dependencies on Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential
sudo apt install libopencv-dev
sudo apt install cmake
```

### Building

```bash
mkdir build
cd build
cmake ..
make
```

Or compile directly with g++:

```bash
g++ -std=c++11 watermark.cpp -o watermark $(pkg-config --cflags --libs opencv4)
```

## Usage

### Processing a single image

```bash
# Add text watermark
./watermark input.jpg output.jpg "Watermark Text"

# Add image watermark
./watermark input.jpg output.jpg watermark.png

# Use default watermark
./watermark input.jpg output.jpg
```

### Batch processing

1. Put images to be processed in the `input/` directory
2. Run the batch processing script:

```bash
chmod +x batch_watermark.sh
./batch_watermark.sh
```

Processed images will be saved in the `output/` directory.