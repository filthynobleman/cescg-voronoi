/**
 * @file        voronoi_part.cpp
 * 
 * @brief       Implementation of cescg::VoronoiPartitioning()
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-31
 */
#include <cescg/voronoi.hpp>


cescg::Grid<int> cescg::VoronoiPartitioning(const cescg::Image &Img, 
                                            const std::vector<glm::ivec2> &Samples)
{
    cescg::Grid<int> G(Img.GetWidth(), Img.GetHeight());
    
    #pragma omp parallel for
    for (int j = 0; j < G.GetHeight(); ++j)
    {
        for (int i = 0; i < G.GetWidth(); ++i)
        {
            glm::ivec2 ij(i, j);
            double MinD = std::numeric_limits<double>::infinity();
            int Closest = -1;
            for (int k = 0; k < Samples.size(); ++k)
            {
                glm::vec2 Diff(Samples[k] - ij);
                double D = glm::length(Diff);
                if (D < MinD)
                {
                    MinD = D;
                    Closest = k;
                }
            }
            G(i, j) = Closest;
        }
    }

    return G;
}