/**
 * @file        image.hpp
 * 
 * @brief       A class for representing images.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-30
 */
#pragma once


#include <cescg/utils.hpp>


namespace cescg
{
    
class Image
{
private:
    // Image's width
    int m_Width;
    // Image's height
    int m_Height;
    // Image's number of channels
    int m_NCh;

    // Images's data, organized in row-column-channel order
    float* m_Data;

public:
    // Loads an image from file.
    // Only supports images with 1 channel (grayscale) or 3 channels (RGB).
    Image(const std::string& Filename);

    // Create an empty (black) image with given size and number of channels.
    // Number of channels must be either 1 (grayscale) or 3 (RGB). Alpha channel is not supported.
    Image(int Width, int Height, int NCh);

    // Copy constructor.
    Image(const cescg::Image& Img);

    // Move constructor.
    Image(cescg::Image&& Img);

    // Assigment-copy operator.
    cescg::Image& operator=(const cescg::Image& Img);

    // Assigment-move operator.
    cescg::Image& operator=(cescg::Image&& Img);

    // Destructor frees the memory for image data.
    ~Image();


    // Image width
    int GetWidth() const;
    // Image height
    int GetHeight() const;
    // Number of channels
    int GetChannels() const;

    // Value at pixel (i, j). No boundary checks are performed.
    // With single channel images, all entries have the same value (though, the use of GetLuminance() is preferred).
    glm::vec3 GetPixel(int i, int j) const;

    // Get luminance of pixel (i, j). No boundary checks are performed.
    // With single channel images, it returns the pixel's value.
    float GetLuminance(int i, int j) const;

    // Set value of pixel (i, j). No boundary or value checks are perfomed.
    // With single channel images, luminance is extracted from the color.
    void SetPixel(int i, int j, const glm::vec3& Value);

    // Set luminance of pixel (i, j). No boundary checks are performed.
    // With single channel images, it sets the pixel's value.
    void SetLuminance(int i, int j, float Value);

    // Converts the image to a grayscale version according to its luminance.
    // With single channel images, creates a copy of itself.
    cescg::Image ToGrayscale() const;


    // Set value of multiple pixels at once.
    // See SetPixel() for reference.
    void DrawPixels(const std::vector<glm::ivec2>& Pixels,
                    const glm::vec3& Color);

    // Set value of multiple pixels at once.
    // See SetPixel() for reference.
    void DrawPixels(const std::vector<glm::ivec2>& Pixels,
                    float Value);

    // Exports the image to the given filename, if allowed.
    // Directories are created, if needed. Pixels values are clamped to sane ranges.
    bool Export(const std::string& Filename) const;
};

glm::vec3 YUV2RGB(const glm::vec3& yuv);
glm::vec3 RGB2YUV(const glm::vec3& rgb);

} // namespace cescg
