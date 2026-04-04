/**
 * @file        delaunay.hpp
 * 
 * @brief       Class for computing Delaunay triangulations of points inside the square [0, 1].
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-04
 */
#pragma once

#include <cescg/utils.hpp>
#include <cescg/voronoi.hpp>


namespace cescg
{
    
class Delaunay
{
private:
    std::vector<glm::vec2> m_Pts;
    std::vector<glm::ivec3> m_Tris;

public:
    Delaunay(const std::vector<glm::vec2>& Pts = { });
    Delaunay(const cescg::Delaunay& DT);
    Delaunay(cescg::Delaunay&& DT);
    cescg::Delaunay& operator=(const cescg::Delaunay& DT);
    cescg::Delaunay& operator=(cescg::Delaunay&& DT);
    ~Delaunay();

    int NumPoints() const;

    void AddPoint(const glm::vec2& p);
    void AddPoints(const std::vector<glm::vec2>& Pts);

    void GetTriangulation(std::vector<glm::vec2>& Pts,
                          std::vector<glm::ivec3>& Tris) const;
    std::vector<cescg::VoronoiRegion> GetVoronoiDiagram() const;
};

} // namespace cescg
