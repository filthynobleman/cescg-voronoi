/**
 * @file        test_voronoi.cpp
 * 
 * @brief       Simple application for testing centroidal Voronoi partitionings.
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

#define OUT_DIR "./test-voronoi-centroidal-output/"


int main(int argc, const char* const argv[])
{
    std::string ImgFile = SAMPLES_DIR "/beach-1920x1080.jpg";
    if (argc > 1)
        ImgFile = argv[1];

    int NumSamples = 15000;
    if (argc > 2)
        NumSamples = std::stoi(argv[2]);

    cescg::Image Img(ImgFile);
    Img = Img.ToGrayscale();

    std::vector<glm::ivec2> Samples;
    Samples.reserve(NumSamples);
    for (int s = 0; s < NumSamples; ++s)
    {
        glm::ivec2 NewS(cescg::RandGen::GetInstance().RandomInteger(0, Img.GetWidth()),
                        cescg::RandGen::GetInstance().RandomInteger(0, Img.GetHeight()));
        if (cescg::RandGen::GetInstance().RandomReal(0.0f, 0.25f) > Img.GetLuminance(NewS.x, NewS.y))
            Samples.emplace_back(NewS);
        else
            --s;
    }

    cescg::Grid<int> VPart = cescg::CentroidalVoronoi(Img, Samples);
    std::vector<float> Intensities;
    Intensities.resize(Samples.size(), 0.0f);
    std::vector<int> CellSizes;
    CellSizes.resize(Samples.size(), 0);
    for (int j = 0; j < Img.GetHeight(); ++j)
    {
        for (int i = 0; i < Img.GetWidth(); ++i)
        {
            Intensities[VPart(i, j)] += Img.GetLuminance(i, j);
            CellSizes[VPart(i, j)]++;
        }
    }
    
    cescg::Image VoroImg(Img.GetWidth(), Img.GetHeight(), 1);
    for (int j = 0; j < VoroImg.GetHeight(); ++j)
    {
        for (int i = 0; i < VoroImg.GetWidth(); ++i)
            VoroImg.SetLuminance(i, j, 1.0f);
    }
    
    for (int s = 0; s < Samples.size(); ++s)
    {
        Intensities[s] /= CellSizes[s];
        VoroImg.DrawDot(Samples[s].x, Samples[s].y, 6, 0.0f);
    }



    VoroImg.Export(OUT_DIR "/stippled.jpg");


    return EXIT_SUCCESS;
}