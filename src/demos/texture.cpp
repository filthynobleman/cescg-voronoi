/**
 * @file        texture.cpp
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
#include <cescg/randgen.hpp>

#include <iostream>


struct {
    // Name of output file
    std::string OutFile;

    // Image resolution
    int Resolution;

    // Scale of the pattern
    float Scale;

    // Randomness of site positions
    float Randomness;
} CLIArgs;


void ParseArgs(int argc, const char* const argv[]);


int main(int argc, const char* const argv[])
{
    ParseArgs(argc, argv);

    // Create an image
    cescg::Image Img(CLIArgs.Resolution, CLIArgs.Resolution, 3);

    // Compute Voronoi texture
    cescg::Grid<int> Texture = cescg::VoronoiTexture(CLIArgs.Resolution, 
                                                     CLIArgs.Resolution,
                                                     CLIArgs.Scale,
                                                     CLIArgs.Randomness);
    
    // Fill the image
    // #pragma omp parallel for
    for (int j = 0; j < Img.GetHeight(); ++j)
    {
        for (int i = 0; i < Img.GetWidth(); ++i)
            Img.SetPixel(i, j, cescg::RandGen::GetInstance().HashColor(Texture(i, j)));
    }

    // Save image
    Img.Export(CLIArgs.OutFile);


    return EXIT_SUCCESS;
}

void ParseArgs(int argc, const char* const argv[])
{
    CLIArgs.OutFile = CESCG_OUTPUT_DIR "/texture.jpg";
    CLIArgs.Resolution = 1024;
    CLIArgs.Scale = 5.0f;
    CLIArgs.Randomness = 1.0f;

    for (int i = 1; i < argc; ++i)
    {
        std::string argvi = argv[i];
        if (argvi[0] != '-')
        {
            CLIArgs.OutFile = argvi;
            continue;
        }

        if (i >= argc - 1)
        {
            std::cerr << "Option " << argvi << " requires an argument." << std::endl;
            exit(EXIT_FAILURE);
        }
        
        if (argvi == "-d" || argvi == "--dimension")
        {
            CLIArgs.Resolution = std::stoi(argv[++i]);
            continue;
        }
        if (argvi == "-s" || argvi == "--scale")
        {
            CLIArgs.Scale = std::stof(argv[++i]);
            continue;
        }
        if (argvi == "-r" || argvi == "--randomness")
        {
            CLIArgs.Randomness = std::stof(argv[++i]);
            if (CLIArgs.Randomness < 0.0f || CLIArgs.Randomness > 1.0f)
            {
                std::cerr << "WARNING: provided randomness value (" << CLIArgs.Randomness;
                std::cerr << ") exceeds bounds [0, 1]." << std::endl;
                std::cerr << "It will be clamped to the closest bound." << std::endl;
            }
            CLIArgs.Randomness = glm::clamp(CLIArgs.Randomness, 0.0f, 1.0f);
            continue;
        }
    }
}