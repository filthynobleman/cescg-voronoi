/**
 * @file        voronoi.hpp
 * 
 * @brief       Functions for Voronoi diagrams.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-30
 */
#pragma once

#include <cescg/utils.hpp>
#include <cescg/image.hpp>
#include <cescg/grid.hpp>


namespace cescg
{
    
cescg::Grid<int> VoronoiPartitioning(const cescg::Image& Img,
                                     const std::vector<glm::ivec2>& Samples);


cescg::Grid<int> CentroidalVoronoi(const cescg::Image& Img,
                                   std::vector<glm::ivec2>& Samples);


cescg::Grid<int> VoronoiTexture(int Width, 
                                int Height, 
                                float Scale, 
                                float Randomness = 1.0f);

} // namespace cescg
