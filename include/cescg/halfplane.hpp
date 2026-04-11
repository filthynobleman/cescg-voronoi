/**
 * @file        halfplane.hpp
 * 
 * @brief       Class representing halfplanes.
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
    
class HalfPlane
{
private:
    glm::vec2 m_Normal;
    float m_Shift;

public:
    HalfPlane(const glm::vec2& n, float s);
    HalfPlane(const cescg::HalfPlane& HP);
    HalfPlane(cescg::HalfPlane&& HP);
    cescg::HalfPlane& operator=(const cescg::HalfPlane& HP);
    cescg::HalfPlane& operator=(cescg::HalfPlane&& HP);
    ~HalfPlane();

    const glm::vec2& GetNormal() const;
    void SetNormal(const glm::vec2& n);

    float GetShift() const;
    void SetShift(float s);

    void Flip();

    float SignedDistance(const glm::vec2& P) const;
    bool IsInside(const glm::vec2& P) const;
    glm::vec2 Specular(const glm::vec2& P) const;
};


cescg::HalfPlane PerpendicularBisector(const glm::vec2& Inside,
                                       const glm::vec2& Outside);

cescg::Polygon CutPolygon(const cescg::HalfPlane& H,
                          const cescg::Polygon& P);

} // namespace cescg
