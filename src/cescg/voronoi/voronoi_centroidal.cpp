/**
 * @file        voronoi_centroidal.cpp
 * 
 * @brief       Implementation of cescg::CentroidalVoronoi()
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-31
 */
#include <cescg/voronoi.hpp>


cescg::Grid<int> cescg::CentroidalVoronoi(const cescg::Image &Img, 
                                          std::vector<glm::ivec2> &Samples)
{
    cescg::Grid<int> G = VoronoiPartitioning(Img, Samples);
    bool Converged = false;
    std::vector<glm::dvec2> NewSamples;
    NewSamples.resize(Samples.size(), glm::dvec2(0.0));
    std::vector<double> CellSize;
    CellSize.resize(Samples.size(), 0);
    for (int Iter = 0; Iter < 20 && !Converged; ++Iter)
    {
        Converged = true;
        
        NewSamples.clear();
        NewSamples.resize(Samples.size(), glm::dvec2(0.0));
        CellSize.clear();
        CellSize.resize(Samples.size(), 0);
        
        for (int j = 0; j < G.GetHeight(); ++j)
        {
            for (int i = 0; i < G.GetWidth(); ++i)
            {
                double rho = 1.0f - Img.GetLuminance(i, j);
                NewSamples[G(i, j)] += rho * glm::dvec2(i, j);
                CellSize[G(i, j)] += rho;
            }
        }

        for (int s = 0; s < Samples.size(); ++s)
        {
            if (CellSize[s] < 1e-6)
                continue;
            glm::dvec2 NewS = NewSamples[s] / CellSize[s];
            NewS = glm::round(NewS);
            NewS.x = glm::clamp(NewS.x, 0.0, G.GetWidth() - 1.0);
            NewS.y = glm::clamp(NewS.y, 0.0, G.GetHeight() - 1.0);
            glm::ivec2 NewSI(NewS.x, NewS.y);
            glm::vec2 Diff = NewSI - Samples[s];
            if (NewSI != Samples[s])
                Converged = false;
            Samples[s] = NewSI;
        }

        G = VoronoiPartitioning(Img, Samples);
    }

    return G;
}