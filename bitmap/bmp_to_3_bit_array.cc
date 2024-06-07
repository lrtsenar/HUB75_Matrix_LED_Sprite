#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iomanip>

// to chatGPT
// write a c++ program which convert a bitmap file (32, 24 ou 16 bits color depth) into a 2D "C" array (const uint8_t image[y][x] = { {...}, ... };) where pixels (converted to 8 bits color depth) and organized in y,x starting upper left, ending lower right. Print the result on console
//-------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
struct BMPFileHeader { uint16_t bfType; uint32_t bfSize; uint16_t bfReserved1; uint16_t bfReserved2; uint32_t bfOffBits; };
struct BMPInfoHeader { uint32_t biSize; int32_t biWidth; int32_t biHeight; uint16_t biPlanes; uint16_t biBitCount;
    uint32_t biCompression; uint32_t biSizeImage; int32_t biXPelsPerMeter; int32_t biYPelsPerMeter; uint32_t biClrUsed; uint32_t biClrImportant; };
#pragma pack(pop)

struct Pixel { uint8_t b, g, r; };
//-------------------------------------------------------------------------------------------------
uint8_t convertTo3Bit(const Pixel& p) 
{   //return (uint8_t)(((uint8_t)(pixel.r >> 5) << 5) | ((uint8_t)(pixel.g >> 5) << 2) | (uint8_t)(pixel.b >> 6));
    return (uint8_t)((((p.r > 126) ? 1 : 0) << 2) | ((p.g > 126 ? 1: 0) << 1) | ((p.b > 126 ? 1: 0)));
}
//-------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
    std::string filename = argv[1] ? argv[1] : "";
    if (filename.empty()) { std::cerr << "Unable to open file " << filename << "\n"; return 1; }
    std::ifstream file(filename, std::ios::binary);

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    if (fileHeader.bfType != 0x4D42) { std::cerr << "Not a BMP file\n"; return 1; }

    int32_t width = infoHeader.biWidth;
    int32_t height = infoHeader.biHeight;
    uint16_t bitCount = infoHeader.biBitCount;

    std::cerr << "color depth=" << bitCount << "\n";
    if (/*bitCount != 32 &&*/ bitCount != 24 /*&& bitCount != 16*/) { std::cerr << "Only 24 bits image supported, current: " << bitCount << "\n"; return 1; }

    std::vector<std::vector<uint8_t>> image(height, std::vector<uint8_t>(width / 2));
    file.seekg(fileHeader.bfOffBits, std::ios::beg);

    bool bFirst = true;
    for (int32_t y = height - 1; y >= 0; --y) 
    {
        int32_t xx = 0;
        for (int32_t x = 0; x < width; ++x)
        {
            Pixel pixel;
            if (bitCount == 24) { file.read(reinterpret_cast<char*>(&pixel), 3); } 
            else if (bitCount == 32) { file.read(reinterpret_cast<char*>(&pixel), 4); } 
            else if (bitCount == 16) { uint16_t temp;
                                       file.read(reinterpret_cast<char*>(&temp), 2);
                                       pixel.r = ((temp & 0xF800) >> 8);
                                       pixel.g = ((temp & 0x07E0) >> 3);
                                       pixel.b = ((temp & 0x001F) << 3);
            }
            //printf("rgb(%d,%d,%d)\n", pixel.r, pixel.g, pixel.b);
            uint8_t pix = convertTo3Bit(pixel);
            image[y][xx] |= (bFirst ? pix << 4 : pix);
            bFirst = !bFirst;
            if (bFirst) { ++xx; }
        }

        if (bitCount == 24 || bitCount == 16) { file.ignore((4 - (width * (bitCount / 8)) % 4) % 4); }
    }

    filename.resize(filename.find("."));
    std::cout << "const uint8_t " << filename << "[] PROGMEM = { // height=" << height << ", width(real image)=" << width << " , width(buffer)=" << width/2 << "\n";
    int32_t i = 0;
    for (const auto& row : image) 
    {   std::cout << ((i == 0) ? "     " : "   , ");
        for (size_t x = 0; x < row.size(); ++x) 
        {   std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(row[x]); 
            if (x < row.size() - 1) { std::cout << ", "; }
        }
        if (++i < image.size()) { std::cout << "\n"; }
    }
    std::cout << " };\n";
    for (auto & c: filename) c = toupper(c);
    std::cout << "#define " << filename << "(x, y, buf) { " << std::dec << height << ", " << width << ", x, y, 0, 0, 0, buf }\n\n";
    return 0;
}
