/**
 * @file        test_image.cpp
 * 
 * @brief       Small application for testing functionalities of class cescg::Image.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-31
 */
#include <cescg/utils.hpp>
#include <cescg/image.hpp>

#include <iostream>

#define OUT_DIR CESCG_OUTPUT_DIR "/test-image/"

int main(int argc, const char* const argv[])
{
    std::string ImgFile = CESCG_SAMPLES_DIR "/plant-1920x1080.jpg";
    if (argc > 1)
        ImgFile = argv[1];

    cescg::Image Img(ImgFile);

    // Save image and its grayscale
    Img.Export(OUT_DIR "/input-rgb.jpg");
    Img.ToGrayscale().Export(OUT_DIR "/input-grayscale.jpg");

    // Iteratively increase luminance and output
    for (int Iter = 0; Iter < 5; ++Iter)
    {
        std::cout << "Iteration " << Iter << "... " << std::flush;
        #pragma omp parallel for
        for (int j = 0; j < Img.GetHeight(); ++j)
        {
            for (int i = 0; i < Img.GetWidth(); ++i)
            {
                float Luma = Img.GetLuminance(i, j);
                Luma += 0.1f * (1.0f - Luma);
                Img.SetLuminance(i, j, Luma);
            }
        }

        std::string OutRGB = OUT_DIR;
        OutRGB += "/rgb-iter-" + std::to_string(Iter) + ".jpg";
        Img.Export(OutRGB);

        std::string OutGS = OUT_DIR;
        OutGS += "/grayscale-iter-" + std::to_string(Iter) + ".jpg";
        Img.ToGrayscale().Export(OutGS);

        std::cout << "Done." << std::endl;
    }



    return EXIT_SUCCESS;
}