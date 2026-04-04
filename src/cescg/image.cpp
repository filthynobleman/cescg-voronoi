/**
 * @file        image.cpp
 * 
 * @brief       Implmentation of class cescg::Image.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-31
 */
#include <cescg/image.hpp>

// Include STB with implementation
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// Luminance conversion red coefficient according to ITU-R Recommendation BT.2020
#define Y2020KR 0.2627f
// Luminance conversion green coefficient according to ITU-R Recommendation BT.2020
#define Y2020KG 0.6780f
// Luminance conversion blue coefficient according to ITU-R Recommendation BT.2020
#define Y2020KB 0.0593f


// JPEG extension "jpeg"
#define JPEG_EXTENSION ".jpeg"
// JPEG extension "jpg"
#define JPG_EXTENSION ".jpg"
// PNG extension
#define PNG_EXTENSION ".png"
// BMP extension
#define BMP_EXTENSION ".bmp"

// Vector of valid extensions supported for writing images
const inline std::vector<std::string> ValidImgExtensions = {    JPEG_EXTENSION, 
                                                                JPG_EXTENSION, 
                                                                PNG_EXTENSION, 
                                                                BMP_EXTENSION
                                                            };


// Factor of gamma correction
#define GAMMA_FACTOR 2.2f


cescg::Image::Image(const std::string &Filename)
{
    // This ensures the Y coordinates is 0 at the bottom
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(Filename.c_str(), &m_Width, &m_Height, &m_NCh, 0);
    if (data == nullptr)
    {
        std::stringstream ss;
        ss << "Cannot read image file " << Filename;
        throw std::runtime_error(ss.str());
    }
    if (m_NCh != 1 && m_NCh != 3)
    {
        std::stringstream ss;
        ss << "Images with " << m_NCh << " channels are not supported. Only 1 or 3 channels.";
        throw std::runtime_error(ss.str());
    }

    m_Data = (float*)std::malloc(m_Width * m_Height * m_NCh * sizeof(float));
    if (m_Data == nullptr)
        throw std::runtime_error("Cannot allocate image data.");

    for (int j = 0; j < m_Height; ++j)
    {
        for (int i = 0; i < m_Width; ++i)
        {
            for (int k = 0; k < m_NCh; ++k)
            {
                size_t Idx = (j * m_Width + i) * m_NCh + k;
                m_Data[Idx] = data[Idx] / 255.0f;
            }
        }
    }
}

cescg::Image::Image(int Width, int Height, int NCh)
{
    m_Width = Width;
    m_Height = Height;
    m_NCh = NCh;
    if (m_NCh != 1 && m_NCh != 3)
    {
        std::stringstream ss;
        ss << "Images with " << m_NCh << " channels are not supported. Only 1 or 3 channels.";
        throw std::runtime_error(ss.str());
    }

    m_Data = (float*)std::calloc(m_Width * m_Height * m_NCh, sizeof(float));
    if (m_Data == nullptr)
        throw std::runtime_error("Cannot allocate image data.");
}

cescg::Image::Image(const cescg::Image &Img)
{
    m_Width = Img.m_Width;
    m_Height = Img.m_Height;
    m_NCh = Img.m_NCh;

    m_Data = (float*)std::malloc(m_Width * m_Height * m_NCh * sizeof(float));
    if (m_Data == nullptr)
        throw std::runtime_error("Cannot allocate image data.");

    m_Data = (float*)std::memcpy(m_Data, Img.m_Data, m_Width * m_Height * m_NCh * sizeof(float));
    if (m_Data == nullptr)
        throw std::runtime_error("Cannot copy image data.");
}

cescg::Image::Image(cescg::Image &&Img)
{
    m_Width = Img.m_Width;
    m_Height = Img.m_Height;
    m_NCh = Img.m_NCh;
    m_Data = Img.m_Data;
    Img.m_Data = nullptr;
}

cescg::Image& cescg::Image::operator=(const cescg::Image &Img)
{
    m_Width = Img.m_Width;
    m_Height = Img.m_Height;
    m_NCh = Img.m_NCh;

    m_Data = (float*)std::malloc(m_Width * m_Height * m_NCh * sizeof(float));
    if (m_Data == nullptr)
        throw std::runtime_error("Cannot allocate image data.");

    m_Data = (float*)std::memcpy(m_Data, Img.m_Data, m_Width * m_Height * m_NCh * sizeof(float));
    if (m_Data == nullptr)
        throw std::runtime_error("Cannot copy image data.");

    return *this;
}

cescg::Image& cescg::Image::operator=(cescg::Image &&Img)
{
    m_Width = Img.m_Width;
    m_Height = Img.m_Height;
    m_NCh = Img.m_NCh;
    m_Data = Img.m_Data;
    Img.m_Data = nullptr;

    return *this;
}

cescg::Image::~Image()
{
    delete m_Data;
}

int cescg::Image::GetWidth() const { return m_Width; }
int cescg::Image::GetHeight() const { return m_Height; }
int cescg::Image::GetChannels() const { return m_NCh; }

glm::vec3 cescg::Image::GetPixel(int i, int j) const
{
    if (i < 0 || i >= GetWidth() || j < 0 || j >= GetHeight())
        return glm::vec3(-1.0f);

    if (m_NCh == 1)
        return glm::vec3(m_Data[j * m_Width + i]);
    size_t Idx = (j * m_Width + i) * m_NCh;
    return glm::vec3(m_Data[Idx], m_Data[Idx + 1], m_Data[Idx + 2]);
}

float cescg::Image::GetLuminance(int i, int j) const
{
    if (i < 0 || i >= GetWidth() || j < 0 || j >= GetHeight())
        return -1.0f;

    if (m_NCh == 1)
        return m_Data[j * m_Width + i];
    glm::vec3 p = GetPixel(i, j);
    p = glm::pow(p, glm::vec3(GAMMA_FACTOR));
    // Conversion to luminance according to ITU-R Recommendation BT.2020
    return glm::pow(Y2020KR * p.r + Y2020KG * p.g + Y2020KB * p.b, 1.0f / GAMMA_FACTOR);
}

void cescg::Image::SetPixel(int i, int j, const glm::vec3 &Value)
{
    if (i < 0 || i >= GetWidth() || j < 0 || j >= GetHeight())
        return;

    if (m_NCh == 1)
    {
        float Luma = Y2020KR * glm::pow(Value.r, GAMMA_FACTOR) + 
                     Y2020KG * glm::pow(Value.g, GAMMA_FACTOR) + 
                     Y2020KB * glm::pow(Value.b, GAMMA_FACTOR);
        m_Data[j * m_Width + i] = glm::pow(Luma, 1.0f / GAMMA_FACTOR);
        return;
    }

    size_t Idx = (j * m_Width + i) * m_NCh;
    m_Data[Idx] = Value.r;
    m_Data[Idx + 1] = Value.g;
    m_Data[Idx + 2] = Value.b;
}

void cescg::Image::SetLuminance(int i, int j, float Value)
{
    if (i < 0 || i >= GetWidth() || j < 0 || j >= GetHeight())
        return;

    if (m_NCh == 1)
    {
        m_Data[j * m_Width + i] = Value;
        return;
    }

    glm::vec3 p = GetPixel(i, j);
    p = RGB2YUV(p);
    p.x = Value;
    SetPixel(i, j, YUV2RGB(p));
}

cescg::Image cescg::Image::ToGrayscale() const
{
    if (GetChannels() == 1)
        return cescg::Image(*this);

    cescg::Image Img(m_Width, m_Height, 1);
    for (int j = 0; j < m_Height; ++j)
    {
        for (int i = 0; i < m_Width; ++i)
            Img.SetLuminance(i, j, GetLuminance(i, j));
    }

    return Img;
}

void cescg::Image::DrawDot(int i, int j, int Radius, float Luminance)
{
    DrawDot(i, j, Radius, glm::vec3(Luminance));
}

void cescg::Image::DrawDot(int i, int j, int Radius, const glm::vec3 &Color)
{
    double r_sqr = (Radius + 0.5) * (Radius + 0.5);
    for (int dj = - Radius; dj <= Radius; ++dj)
    {
        int jj = j + dj;
        if (jj < 0 || jj >= GetHeight())
            continue;
        for (int di = -Radius; di <= Radius; ++di)
        {
            int ii = i + di;
            if (ii < 0 || ii >= GetWidth())
                continue;
            
            double d_sqr = di * di + dj * dj;
            if (d_sqr > r_sqr)
                continue;

            SetPixel(ii, jj, Color);
        }
    }
}

void cescg::Image::DrawLine(const glm::ivec2 &Start, const glm::ivec2 &End, int Width, const glm::vec3 &Color)
{
    glm::ivec2 Diff = End - Start;
    // if (Diff.x == 0)
    // {
    //     for (int y = std::min(Start.y, End.y); y <= std::max(Start.y, End.y); ++y)
    //     {
    //         for (int x = Start.x - Width / 2; x <= Start.x + Width / 2; ++x)
    //             SetPixel(x, y, Color);
    //     }
    // }
    float XDiff = Diff.x;
    float YDiff = Diff.y;
    if (std::abs(XDiff) > std::abs(YDiff))
    {
        float Slope = YDiff / XDiff;
        for (int x = std::min(Start.x, End.x); x <= std::max(Start.x, End.x); ++x)
        {
            int y = Start.y + ((x - Start.x) * Slope);
            for (int yy = y - Width / 2; yy <= y + Width / 2; ++yy)
                SetPixel(x, yy, Color);
        }
    }
    else
    {
        float Slope = XDiff / YDiff;
        for (int y = std::min(Start.y, End.y); y <= std::max(Start.y, End.y); ++y)
        {
            int x = Start.x + ((y - Start.y) * Slope);
            for (int xx = x - Width / 2; xx <= x + Width / 2; ++xx)
                SetPixel(xx, y, Color);
        }
    }
}

void cescg::Image::DrawLine(const glm::ivec2 &Start, const glm::ivec2 &End, int Width, float Luminance)
{
    DrawLine(Start, End, Width, glm::vec3(Luminance));
}

bool cescg::Image::Export(const std::string &Filename) const
{
    // Check validity of extension
    std::filesystem::path Filepath(Filename);
    std::string Ext = Filepath.extension().string();
    std::transform(Ext.begin(), Ext.end(), Ext.begin(), [](char c) { return (char)std::tolower((int)c); });
    if (std::find(ValidImgExtensions.begin(), ValidImgExtensions.end(), Ext) == ValidImgExtensions.end())
    {
        std::stringstream ss;
        ss << "Extension " << Ext << " is not supported for writing.";
        throw std::runtime_error(ss.str());
    }

    // Make directory, if it does not exists
    std::filesystem::path Directory = Filepath.parent_path();
    if (!std::filesystem::exists(Directory))
    {
        if (!std::filesystem::create_directories(Directory))
        {
            std::stringstream ss;
            ss << "Failed to create directory " << Directory.string();
            throw std::runtime_error(ss.str());
        }
    }
    else if (!std::filesystem::is_directory(Directory))
    {
        std::stringstream ss;
        ss << Directory.string() << " already exists and it\'s not a directory.";
        throw std::runtime_error(ss.str());
    }


    // This ensures the Y coordinates is 0 at the bottom
    stbi_flip_vertically_on_write(true);

    unsigned char* data = (unsigned char*)std::malloc(m_Width * m_Height * m_NCh * sizeof(unsigned char));
    if (data == nullptr)
        throw std::runtime_error("Failed to allocate data for writing.");
    
    for (int j = 0; j < m_Height; ++j)
    {
        for (int i = 0; i < m_Width; ++i)
        {
            for (int k = 0; k < m_NCh; ++k)
            {
                size_t Idx = (j * m_Width + i) * m_NCh + k;
                data[Idx] = (unsigned char)std::roundf(255.0f * m_Data[Idx]);
            }
        }
    }
    
    bool Success = false;
    if (Ext == JPEG_EXTENSION || Ext == JPG_EXTENSION)
        Success = stbi_write_jpg(Filename.c_str(), m_Width, m_Height, m_NCh, data, 90);
    else if (Ext == PNG_EXTENSION)
        Success = stbi_write_png(Filename.c_str(), m_Width, m_Height, m_NCh, data, m_Width * m_NCh * sizeof(unsigned char));
    else if (Ext == BMP_EXTENSION)
        Success = stbi_write_bmp(Filename.c_str(), m_Width, m_Height, m_NCh, data);
    
    if (!Success)
    {
        std::stringstream ss;
        ss << "Cannot write image " << Filename;
        throw std::runtime_error(ss.str());
    }

    return Success;
}

glm::vec3 cescg::YUV2RGB(const glm::vec3 &yuv)
{
    static glm::mat3 RGBMat(1.0f, 1.0f, 1.0f,
                            0.0f, - Y2020KB / Y2020KG * (2 - 2 * Y2020KB), 2 - 2* Y2020KB,
                            2 - 2 * Y2020KR, - Y2020KR / Y2020KG * (2 - 2 * Y2020KR), 0.0f);
    glm::vec3 rgb = yuv;
    rgb.x = glm::pow(rgb.x, GAMMA_FACTOR);
    return glm::pow(RGBMat * rgb, glm::vec3(1.0f / GAMMA_FACTOR));
    // float h_prime = 6 * hsl.x; // 360  * hsl.x / 60
    // float x = hsl.y * (1 - std::abs(std::remainderf(h_prime, 2.0f) - 1));
    // glm::vec3 rgb{ 0.0f, 0.0f, 0.0f };
    // if (h_prime < 1)
    //     rgb = { hsl.y, x, 0.0f };
    // else if (h_prime < 2)
    //     rgb = { x, hsl.y, 0.0f };
    // else if (h_prime < 3)
    //     rgb = { 0.0f, hsl.y, x };
    // else if (h_prime < 4)
    //     rgb = { 0.0f, x, hsl.y };
    // else if (h_prime < 5)
    //     rgb = { x, 0.0f, hsl.y };
    // else if (h_prime < 6)
    //     rgb = { hsl.y, 0.0f, x };
    
    // float m = hsl.z - (Y2020KR * rgb.r + Y2020KG * rgb.g + Y2020KB * rgb.b);
    // rgb += m;
    // return rgb;
}

glm::vec3 cescg::RGB2YUV(const glm::vec3 &rgb)
{
    static glm::mat3 YUVMat(Y2020KR, -0.5f * Y2020KR / (1 - Y2020KB), 0.5f,
                            Y2020KG, -0.5f * Y2020KG / (1 - Y2020KB), -0.5f * Y2020KG / (1 - Y2020KR),
                            Y2020KB, 0.5f, -0.5f * Y2020KB / (1 - Y2020KR));
    glm::vec3 yuv = YUVMat * glm::pow(rgb, glm::vec3(GAMMA_FACTOR));
    yuv.x = glm::pow(yuv.x, 1.0f / GAMMA_FACTOR);
    return yuv;
    // float luma = Y2020KR * rgb.r + Y2020KG * rgb.g + Y2020KB * rgb.b;
    // float alpha = 0.5f * (2.0f * rgb.r - rgb.g - rgb.b);
    // float beta = 0.5f * std::sqrtf(3.0f) * (rgb.g - rgb.b);
    // return glm::vec3(std::atan2f(beta, alpha), std::sqrtf(alpha * alpha + beta * beta), luma);
}