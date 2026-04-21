/**
 * @file        mosaicking.cpp
 * 
 * @brief       Computes a Voronoi diagram over the image and uses it to
 *              apply a mosaicking effect.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-14
 */
#include <cescg/utils.hpp>
#include <cescg/voronoi.hpp>
#include <cescg/image.hpp>
#include <cescg/rasterizer.hpp>
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
    cescg::Image Img(CLIArgs.InFile);

    // Compute random sites
    std::vector<glm::vec2> Sites;
    Sites.reserve(CLIArgs.NumSites);
    cescg::RandGen& RNG = cescg::RandGen::GetInstance();
    for (int s = 0; s < CLIArgs.NumSites; ++s)
    {
        Sites.emplace_back(RNG.RandomReal() * (Img.GetWidth() - 1), 
                           RNG.RandomReal() * (Img.GetHeight() - 1));
    }

    // Compute Voronoi diagram
    cescg::BoundingBox BB(glm::vec2(0.0f), 
                          glm::vec2(Img.GetWidth() - 1, Img.GetHeight() - 1));
    cescg::VoronoiDiagram VD(BB);
    VD.AddSamples(Sites);
    VD.Compute();

    // Make it centroidal, if asked
    if (CLIArgs.Centroidal)
        VD.MakeCentroidal(20);

    
    // Create the output image
    cescg::Image ImgOut(Img.GetWidth(), Img.GetHeight(), Img.GetChannels());

    // Fill cells
    const std::vector<cescg::VoronoiRegion>& Regions = VD.GetRegions();
    cescg::Rasterizer Raster(Img.GetWidth(), Img.GetHeight());
    #pragma omp parallel for
    for (int s = 0; s < VD.NumSamples(); ++s)
    {
        // Get pixels of cell
        std::vector<glm::ivec2> Pixels;
        Raster.PixelsFromConvexPolygon(Regions[s].GetPolygon(), Pixels);
        // Get color of site
        glm::ivec2 si = Raster.PixelAtCoordinates(Sites[s]);
        glm::vec3 Color = Img.GetPixel(glm::round(Sites[s].x), glm::round(Sites[s].y));
        ImgOut.DrawPixels(Pixels, Color);
    }

    // If asked, draw edges
    if (CLIArgs.DrawEdges)
    {
        #pragma omp parallel for
        for (int s = 0; s < VD.NumSamples(); ++s)
        {
            auto Edges = VD.GetRegion(s).GetPolygon().GetEdges();
            for (const auto& e : Edges)
            {
                std::vector<glm::ivec2> Pixels;
                Raster.PixelsFromSegment(e.first, e.second, 1, Pixels);
                ImgOut.DrawPixels(Pixels, glm::vec3(1.0f));
            }
        }
    }

    // Export image
    ImgOut.Export(CLIArgs.OutFile);



    return EXIT_SUCCESS;
}


void ParseArgs(int argc, const char *const argv[])
{
    CLIArgs.InFile = CESCG_SAMPLES_DIR "/beach-1920x1080.jpg";
    CLIArgs.OutFile = CESCG_OUTPUT_DIR "/mosaicking.jpg";
    CLIArgs.NumSites = 5000;
    CLIArgs.Centroidal = false;
    CLIArgs.DrawEdges = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string argvi = argv[i];
        if (argvi[0] != '-')
        {
            CLIArgs.InFile = argvi;
            continue;
        }

        if (argvi == "-e" || argvi == "--draw-edges")
        {
            CLIArgs.DrawEdges = true;
            continue;
        }
        if (argvi == "-c" || argvi == "--centroidal")
        {
            CLIArgs.Centroidal = true;
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
    }
}