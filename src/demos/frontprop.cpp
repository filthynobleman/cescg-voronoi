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
    
    // Linear interpolation factor between Euclidean and color distance
    double InterpFactor;
} CLIArgs;



void ParseArgs(int argc, const char* const argv[]);
double EuclideanDistance(const cescg::Image& Img,
                         const glm::ivec2& a,
                         const glm::ivec2& b);
double ColorDistance(const cescg::Image& Img,
                     const glm::ivec2& a,
                     const glm::ivec2& b);

int main(int argc, const char* const argv[])
{
    ParseArgs(argc, argv);

    // Load the image
    cescg::Image Img(CLIArgs.InFile);

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
    double t = CLIArgs.InterpFactor;
    auto mixlambda = [t] (const cescg::Image& Img, const glm::ivec2& a, const glm::ivec2& b) { 
        return (1.0 - t) * EuclideanDistance(Img, a, b) + t * ColorDistance(Img, a, b);
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
    ImgOut.Export(CLIArgs.OutFile);


    return EXIT_SUCCESS;
}

double EuclideanDistance(const cescg::Image &Img, 
                         const glm::ivec2 &a, 
                         const glm::ivec2 &b)
{
    glm::vec2 Diff = glm::vec2(a - b);
    return glm::length(Diff);
}

double ColorDistance(const cescg::Image &Img, 
                     const glm::ivec2 &a, 
                     const glm::ivec2 &b)
{
    glm::vec3 ca = Img.GetPixel(a.x, a.y);
    glm::vec3 cb = Img.GetPixel(b.x, b.y);
    return glm::distance(ca, cb);
}

void ParseArgs(int argc, const char *const argv[])
{
    CLIArgs.InFile = CESCG_SAMPLES_DIR "/beach-1920x1080.jpg";
    CLIArgs.OutFile = CESCG_OUTPUT_DIR "/frontprop.jpg";
    CLIArgs.NumSites = 200;
    CLIArgs.InterpFactor = 0.0;

    for (int i = 1; i < argc; ++i)
    {
        std::string argvi = argv[i];
        if (argvi[0] != '-')
        {
            CLIArgs.InFile = argvi;
            continue;
        }

        if (i >= argc - 1)
        {
            std::cerr << "Option " << argvi << " requires an argument." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (argvi == "-n" || argvi == "--num-sites")
        {
            CLIArgs.NumSites = std::stoi(argv[++i]);
            continue;
        }
        if (argvi == "-o" || argvi == "--output")
        {
            CLIArgs.OutFile = argv[++i];
            continue;
        }
        if (argvi == "-f" || argvi == "--interp-factor")
        {
            CLIArgs.InterpFactor = std::stod(argv[++i]);
            if (CLIArgs.InterpFactor < 0.0 || CLIArgs.InterpFactor > 1.0)
            {
                std::cerr << "WARNING: interpolation factor provided (" << CLIArgs.InterpFactor;
                std::cerr << ") exceeds bounds [0, 1]." << std::endl;
                std::cerr << "It will be clamped to the closest bound." << std::endl;
            }
            CLIArgs.InterpFactor = glm::clamp(CLIArgs.InterpFactor, 0.0, 1.0);
            continue;
        }
    }
}