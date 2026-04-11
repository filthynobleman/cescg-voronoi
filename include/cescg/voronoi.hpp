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
#include <cescg/bbox.hpp>
#include <cescg/vororegion.hpp>


namespace cescg
{
    
// cescg::Grid<int> VoronoiPartitioning(const cescg::Image& Img,
//                                      const std::vector<glm::ivec2>& Samples);

// std::vector<std::vector<glm::vec2>> VoronoiPartitioning(const std::vector<glm::ivec2>& Samples,
//                                                         const glm::ivec2& BottomLeft,
//                                                         const glm::ivec2& TopRight);


// cescg::Grid<int> CentroidalVoronoi(const cescg::Image& Img,
//                                    std::vector<glm::ivec2>& Samples);

// std::vector<std::vector<glm::vec2>> CentroidalVoronoi(const std::vector<glm::ivec2>& Samples,
//                                                       const glm::ivec2& BottomLeft,
//                                                       const glm::ivec2& TopRight);


std::vector<glm::ivec2> PixelsFromConvexPolygon(const std::vector<glm::vec2>& Polygon);


cescg::Grid<int> VoronoiTexture(int Width, 
                                int Height, 
                                float Scale, 
                                float Randomness = 1.0f);


class VoronoiDiagram
{
private:
    std::vector<glm::vec2> m_Samples;
    std::vector<cescg::VoronoiRegion> m_Regions;
    cescg::BoundingBox m_BBox;
    bool m_UpToDate;

public:
    VoronoiDiagram(const cescg::BoundingBox& BB);
    VoronoiDiagram(const cescg::VoronoiDiagram& VD);
    VoronoiDiagram(cescg::VoronoiDiagram&& VD);
    cescg::VoronoiDiagram& operator=(const cescg::VoronoiDiagram& VD);
    cescg::VoronoiDiagram& operator=(cescg::VoronoiDiagram&& VD);
    ~VoronoiDiagram();

    const cescg::BoundingBox& GetBoundingBox() const;
    cescg::BoundingBox& GetBoundingBox();

    void AddSample(const glm::vec2& Sample);
    void AddSamples(const std::vector<glm::vec2>& Samples);

    bool IsUpToDate() const;

    void Compute();
    void MakeCentroidal(int MaxIter = -1);

    int NumSamples() const;
    const glm::vec2& GetSample(int i) const;
    const std::vector<glm::vec2>& GetSamples() const;

    cescg::VoronoiRegion GetRegion(int i) const;
    std::vector<cescg::VoronoiRegion> GetRegions() const;
};

} // namespace cescg
