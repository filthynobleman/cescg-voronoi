/**
 * @file        vororegion.hpp
 * 
 * @brief       A class representing a Voronoi region.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#pragma once

#include <cescg/utils.hpp>
#include <cescg/polygon.hpp>

namespace cescg
{

class VoronoiRegion
{
private:
    glm::vec2 m_Site;
    cescg::Polygon m_Polygon;

public:
    VoronoiRegion(const glm::vec2& Site,
                  const cescg::Polygon& Polygon);
    VoronoiRegion(const glm::vec2& Site,
                  const std::vector<glm::vec2>& Polygon);
    VoronoiRegion(const cescg::VoronoiRegion& Reg);
    VoronoiRegion(cescg::VoronoiRegion&& Reg);
    cescg::VoronoiRegion& operator=(const cescg::VoronoiRegion& Reg);
    cescg::VoronoiRegion& operator=(cescg::VoronoiRegion&& Reg);
    ~VoronoiRegion();


    const glm::vec2& GetSite() const;
    void SetSite(const glm::vec2& Site);
    const cescg::Polygon& GetPolygon() const;
    cescg::Polygon& GetPolygon();
    void SetPolygon(const cescg::Polygon& Poly);
    void SetPolygon(const std::vector<glm::vec2>& Poly);
};
    
} // namespace cescg
