/**
 * @file        test_voronoi.cpp
 * 
 * @brief       Simple application for testing Voronoi partitionings.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-31
 */
#include <cescg/utils.hpp>
#include <cescg/image.hpp>
#include <cescg/voronoi.hpp>
#include <cescg/randgen.hpp>

#include <iostream>

#define OUT_DIR "./test-voronoi-output/"


int main(int argc, const char* const argv[])
{
    std::string ImgFile = SAMPLES_DIR "/camera-1920x1080.jpg";
    if (argc > 1)
        ImgFile = argv[1];

    int NumSamples = 5000;
    if (argc > 2)
        NumSamples = std::stoi(argv[2]);

    cescg::Image Img(ImgFile);

    std::vector<glm::ivec2> Samples;
    Samples.reserve(NumSamples);
    for (int s = 0; s < NumSamples; ++s)
        Samples.emplace_back(cescg::RandGen::GetInstance().RandomInteger(0, Img.GetWidth()),
                             cescg::RandGen::GetInstance().RandomInteger(0, Img.GetHeight()));

    std::vector<std::vector<glm::ivec2>> VPolys = cescg::VoronoiPartitioning(Samples, glm::ivec2(0, 0), glm::ivec2(Img.GetWidth(), Img.GetHeight()));
    std::cout << "Voronoi computed" << std::endl;
    std::vector<std::vector<glm::ivec2>> Pixels;
    Pixels.resize(Samples.size());
    for (int s = 0; s < Samples.size(); ++s)
        Pixels[s] = cescg::PixelsFromConvexPolygon(VPolys[s]);

    // cescg::Grid<int> VPart = cescg::VoronoiPartitioning(Img, Samples);

    std::vector<glm::vec3> Colors;
    Colors.reserve(NumSamples);
    for (int s = 0; s < NumSamples; ++s)
        Colors.emplace_back(Img.GetPixel(Samples[s].x, Samples[s].y));
    
    cescg::Image VoroImg(Img.GetWidth(), Img.GetHeight(), 3);
    for (int s = 0; s < Samples.size(); ++s)
    {
        for (const auto& p : Pixels[s])
            VoroImg.SetPixel(p.x, p.y, Colors[s]);
    }
    // for (int j = 0; j < VoroImg.GetHeight(); ++j)
    // {
    //     for (int i = 0; i < VoroImg.GetWidth(); ++i)
    //         VoroImg.SetPixel(i, j, Colors[VPart(i, j)]);
    // }

    VoroImg.Export(OUT_DIR "/voronoi.jpg");


    return EXIT_SUCCESS;
}