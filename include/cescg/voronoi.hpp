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

typedef std::function<double(const cescg::Image&, const glm::ivec2&, const glm::ivec2&)> CellDistance;

cescg::Grid<int> VoronoiTexture(int Width, 
                                int Height, 
                                float Scale, 
                                float Randomness = 1.0f);

cescg::Grid<int> FrontPropagation(const cescg::Image& Img,
                                  const std::vector<glm::ivec2>& Sites,
                                  CellDistance DistFun);


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

    const cescg::VoronoiRegion& GetRegion(int i) const;
    const std::vector<cescg::VoronoiRegion>& GetRegions() const;
};

} // namespace cescg
