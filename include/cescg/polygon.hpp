/**
 * @file        polygon.hpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#pragma once

#include <cescg/utils.hpp>


namespace cescg
{
    
class Polygon
{
private:
    std::vector<glm::vec2> m_Verts;

public:
    Polygon(const std::vector<glm::vec2>& Vertices);
    Polygon(const cescg::Polygon& P);
    Polygon(cescg::Polygon&& P);
    cescg::Polygon& operator=(const cescg::Polygon& P);
    cescg::Polygon& operator=(cescg::Polygon&& P);
    ~Polygon();

    int NumVertices() const;
    const glm::vec2& GetVertex(int i) const;
    const std::vector<glm::vec2>& GetVertices() const;

    float GetArea() const;

    glm::vec2 GetCentroid() const;
    glm::vec2 GetCenterOfMass() const;
    std::pair<glm::vec2, glm::vec2> GetEdge(int i) const;
    std::vector<std::pair<glm::vec2, glm::vec2>> GetEdges() const;
};

} // namespace cescg
