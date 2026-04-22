/**
 * @file        stippling.cpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-22
 */
#include <cescg/utils.hpp>
#include <cescg/image.hpp>
#include <cescg/voronoi.hpp>
#include <cescg/rasterizer.hpp>
#include <cescg/randgen.hpp>

#include <iostream>


struct {
    // Input image
    std::string InFile;

    // Output image
    std::string OutFile;

    // Number of stippled points
    int NumPoints;

    // Size of points (diameter in pixels)
    int PointSize;

    // Luminance threshold to ignore the pixel when sampling
    float LumaThreshold;

    // Use luminance of inverted image
    bool Inverse;
} CLIArgs;



void ParseArgs(int argc, const char* const argv[]);


int main(int argc, const char* const argv[])
{
    ParseArgs(argc, argv);

    // Read the image
    cescg::Image Img(CLIArgs.InFile);
    Img = Img.ToGrayscale();
    if (CLIArgs.Inverse)
    {
        #pragma omp parallel for
        for (int j = 0; j < Img.GetHeight(); ++j)
        {
            for (int i = 0; i < Img.GetWidth(); ++i)
            {
                Img.SetLuminance(i, j, 1.0f - Img.GetLuminance(i, j));
            }
        }
    }

    // Get array of pixels under the luma threshold
    // (this is much more efficient than direct rejection sampling)
    std::vector<glm::ivec2> DarkPixels;
    DarkPixels.reserve(Img.GetWidth() * Img.GetHeight());
    for (int j = 0; j < Img.GetHeight(); ++j)
    {
        for (int i = 0; i < Img.GetWidth(); ++i)
        {
            if (Img.GetLuminance(i, j) > CLIArgs.LumaThreshold + 1e-6)
                continue;
            DarkPixels.emplace_back(i, j);
        }
    }
    // Sample the sites, prioritizing areas with darker values
    std::vector<glm::vec2> Sites;
    Sites.reserve(CLIArgs.NumPoints);
    for (int s = 0; s < CLIArgs.NumPoints; ++s)
    {
        float r;
        glm::ivec2 p;
        do
        {
            r = cescg::RandGen::GetInstance().RandomReal(0.0, CLIArgs.LumaThreshold);
            p = DarkPixels[cescg::RandGen::GetInstance().RandomInteger(0, DarkPixels.size())];
            // p.x = cescg::RandGen::GetInstance().RandomInteger(0, Img.GetWidth());
            // p.y = cescg::RandGen::GetInstance().RandomInteger(0, Img.GetHeight());
        } while (r <= Img.GetLuminance(p.x, p.y));
        Sites.emplace_back(p);
    }

    // Create rasterizer for later
    cescg::Rasterizer Raster(Img.GetWidth(), Img.GetHeight());

    // Create output image
    cescg::Image ImgOut(Img.GetWidth(), Img.GetHeight(), 1);
    #pragma omp parallel for
    for (int j = 0; j < ImgOut.GetHeight(); ++j)
    {
        for (int i = 0; i < ImgOut.GetWidth(); ++i)
            ImgOut.SetLuminance(i, j, 1.0f);
    }
    // Create bounding box
    cescg::BoundingBox BB(glm::vec2(0.0f), 
                          glm::vec2(Img.GetWidth() - 1, Img.GetHeight() - 1));
    // Iterate for weighted centroidal Voronoi diagram
    const int NumIters = 20;
    for (int Iter = 0; Iter < NumIters; ++Iter)
    {
        // Compute diagram from curret set of sites
        cescg::VoronoiDiagram VD(BB);
        VD.AddSamples(Sites);
        VD.Compute();
        // Update sites
        #pragma omp parallel for
        for (int s = 0; s < VD.NumSamples(); ++s)
        {
            // Get pixels inside site
            std::vector<glm::ivec2> Texel;
            Raster.PixelsFromConvexPolygon(VD.GetRegion(s).GetPolygon(), Texel);
            // Compute center of mass using inverse luminance as density
            glm::vec2 Center(0.0f);
            float Mass = 0.0f;
            for (const auto& p : Texel)
            {
                float m = 1.0f - Img.GetLuminance(p.x, p.y);
                if (m < 1e-6f)
                    continue;
                Center += m * glm::vec2(p);
                Mass += m;
            }
            if (Mass >= 1e-3f)
            {
                Center /= Mass;
                Sites[s] = Center;
            }
        }
    }
    // Put a dot at each recentered site
    std::vector<glm::ivec2> Pixels;
    float StippleRadius = CLIArgs.PointSize / 2.0f;
    for (int s = 0; s < CLIArgs.NumPoints; ++s)
    {
        glm::ivec2 p = Raster.PixelAtCoordinates(Sites[s]);
        Raster.PixelsFromCircle(p, StippleRadius, Pixels);
        ImgOut.DrawPixels(Pixels, 0.0f);
    }
    ImgOut.Export(CLIArgs.OutFile);



    return EXIT_SUCCESS;
}


void ParseArgs(int argc, const char* const argv[])
{
    CLIArgs.InFile = CESCG_SAMPLES_DIR "/kid-1024x1024.png";
    CLIArgs.OutFile = CESCG_OUTPUT_DIR "/stippling.jpg";
    CLIArgs.NumPoints = 10000;
    CLIArgs.PointSize = 2;
    CLIArgs.LumaThreshold = 1.0f;
    CLIArgs.Inverse = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string argvi = argv[i];
        if (argvi[0] != '-')
        {
            CLIArgs.InFile = argvi;
            continue;
        }

        if (argvi == "-i" || argvi == "--inverse")
        {
            CLIArgs.Inverse = true;
            continue;
        }

        if (i >= argc - 1)
        {
            std::cerr << "Option " << argvi << " requires an argument." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (argvi == "-o" || argvi == "--output")
        {
            CLIArgs.OutFile = argv[++i];
            continue;
        }
        if (argvi == "-n" || argvi == "--num-points")
        {
            CLIArgs.NumPoints = std::stoi(argv[++i]);
            continue;
        }
        if (argvi == "-s" || argvi == "--point-size")
        {
            CLIArgs.PointSize = std::stoi(argv[++i]);
            continue;
        }
        if (argvi == "-t" || argvi == "--threshold")
        {
            CLIArgs.LumaThreshold = std::stof(argv[++i]);
            if (CLIArgs.LumaThreshold < 0.0f || CLIArgs.LumaThreshold > 1.0f)
            {
                std::cerr << "WARNING: provided luminance threshold (" << CLIArgs.LumaThreshold;
                std::cerr << ") exceeds bounds [0, 1]." << std::endl;
                std::cerr << "It will be clamped to the closest bound." << std::endl;
            }
            CLIArgs.LumaThreshold = glm::clamp(CLIArgs.LumaThreshold, 0.0f, 1.0f);
            continue;
        }
    }
}