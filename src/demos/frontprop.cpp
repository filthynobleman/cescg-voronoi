/**
 * @file        frontprop.cpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-21
 */
#include <cescg/utils.hpp>
#include <cescg/image.hpp>
#include <cescg/voronoi.hpp>
#include <cescg/randgen.hpp>

#include <iostream>

struct {
    // Path to input image
    std::string InFile;
    // Path to output image
    std::string OutFile;

    // Number of sites for Voronoi diagram
    int NumSites;
    // Whether or not the diagram must be centroidal
    bool Centroidal;

    // Whether or not to draw edges between Voronoi cells
    bool DrawEdges;
} CLIArgs;



void ParseArgs(int argc, const char* const argv[]);

int main(int argc, const char* const argv[])
{
    ParseArgs(argc, argv);

    // Load the image
    cescg::Image Img(CESCG_SAMPLES_DIR "/beach-1920x1080.jpg");

    // Compute random sites
    std::vector<glm::ivec2> Sites;
    Sites.reserve(CLIArgs.NumSites);
    cescg::RandGen& RNG = cescg::RandGen::GetInstance();
    for (int s = 0; s < CLIArgs.NumSites; ++s)
    {
        Sites.emplace_back(RNG.RandomInteger(0, Img.GetWidth()), 
                           RNG.RandomInteger(0, Img.GetHeight()));
    }

    // Front propagate with euclidean/color distance
    auto distlambda = [] (const cescg::Image& Img, const glm::ivec2& a, const glm::ivec2& b) { 
        glm::ivec2 diff = a - b;
        return std::sqrt(diff.x * diff.x + diff.y * diff.y);
    };
    auto colorlambda = [] (const cescg::Image& Img, const glm::ivec2& a, const glm::ivec2& b) { 
        glm::vec3 c1 = Img.GetPixel(a.x, a.y);
        glm::vec3 c2 = Img.GetPixel(b.x, b.y);
        double d = glm::distance(c1, c2);
        return d;
    };

    for (int i = 0; i < 120; ++i)
    {
        double t = i / 119.0;
        t = std::pow(std::log1p(t) / std::log1p(1), 0.25);
        auto mixlambda = [t,distlambda,colorlambda] (const cescg::Image& Img, const glm::ivec2& a, const glm::ivec2& b) { 
            return (1.0 - t) * distlambda(Img, a, b) + t * colorlambda(Img, a, b);
        };
        cescg::Grid<int> VPart = cescg::FrontPropagation(Img, Sites, mixlambda);

        // Creat eoutput image and export
        cescg::Image ImgOut(Img.GetWidth(), Img.GetHeight(), Img.GetChannels());
        for (int j = 0; j < Img.GetHeight(); ++j)
        {
            for (int i = 0; i < Img.GetWidth(); ++i)
            {
                glm::ivec2 s = Sites[VPart(i, j)];
                ImgOut.SetPixel(i, j, Img.GetPixel(s.x, s.y));
            }
        }
        std::string OutFile = CESCG_OUTPUT_DIR;
        OutFile += "/frontprop-";
        if (i < 10)
            OutFile += '0';
        if (i < 100)
            OutFile += '0';
        OutFile += std::to_string(i) + ".jpg";
        ImgOut.Export(OutFile);

        std::cout << (i + 1) << '/' << 120 << std::endl;
    }
    



    return EXIT_SUCCESS;
}

void ParseArgs(int argc, const char *const argv[])
{
    CLIArgs.InFile = CESCG_SAMPLES_DIR "/beach-1920x1080.jpg";
    CLIArgs.OutFile = CESCG_OUTPUT_DIR "/mosaicking.jpg";
    CLIArgs.NumSites = 200;
    CLIArgs.Centroidal = false;
    CLIArgs.DrawEdges = false;
}