/**
 * @file        halfplane.cpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#include <cescg/halfplane.hpp>

cescg::HalfPlane::HalfPlane(const glm::vec2 &n, float s)
{
    float Len = glm::length(n);
    if (Len > 0.0f)
        m_Normal = n / Len;
    m_Shift = s;
}

cescg::HalfPlane::HalfPlane(const cescg::HalfPlane &HP)
{
    m_Normal = HP.m_Normal;
    m_Shift = HP.m_Shift;
}

cescg::HalfPlane::HalfPlane(cescg::HalfPlane &&HP)
{
    m_Normal = std::move(HP.m_Normal);
    m_Shift = std::move(HP.m_Shift);
}

cescg::HalfPlane& cescg::HalfPlane::operator=(const cescg::HalfPlane &HP)
{
    m_Normal = HP.m_Normal;
    m_Shift = HP.m_Shift;
    return *this;
}

cescg::HalfPlane& cescg::HalfPlane::operator=(cescg::HalfPlane &&HP)
{
    m_Normal = std::move(HP.m_Normal);
    m_Shift = std::move(HP.m_Shift);
    return *this;
}

cescg::HalfPlane::~HalfPlane() { }

const glm::vec2 &cescg::HalfPlane::GetNormal() const { return m_Normal; }
float cescg::HalfPlane::GetShift() const { return m_Shift; }
void cescg::HalfPlane::SetNormal(const glm::vec2 &n) { m_Normal = n; }
void cescg::HalfPlane::SetShift(float s) { m_Shift = s; }

void cescg::HalfPlane::Flip()
{
    m_Normal = -m_Normal;
    m_Shift = -m_Shift;
}

float cescg::HalfPlane::SignedDistance(const glm::vec2 &P) const
{
    return glm::dot(m_Normal, P) + m_Shift;
}

bool cescg::HalfPlane::IsInside(const glm::vec2 &P) const
{
    return SignedDistance(P) <= 0;
}

glm::vec2 cescg::HalfPlane::Specular(const glm::vec2 &P) const
{
    return P - 2.0f * glm::dot(m_Normal, P) * m_Normal;
}

cescg::HalfPlane cescg::PerpendicularBisector(const glm::vec2 &Inside, 
                                              const glm::vec2 &Outside)
{
    glm::vec2 Midpoint = 0.5f * (Inside + Outside);
    glm::vec2 Direction = Outside - Inside;

    float Len = glm::length(Direction);
    if (Len > 0.0f)
        Direction /= Len;

    float Shift = -glm::dot(Direction, Midpoint);
    cescg::HalfPlane PB(Direction, Shift);
    if (!PB.IsInside(Inside))
        PB.Flip();

    return PB;
}

cescg::Polygon cescg::CutPolygon(const cescg::HalfPlane &H, 
                                 const cescg::Polygon &P)
{
    // Handle degenerate polygons
    if (P.NumVertices() == 0)
        return P;
    
    if (P.NumVertices() == 1)
    {
        if (H.IsInside(P.GetVertex(0)))
            return P;
        return cescg::Polygon({ });
    }

    std::vector<glm::vec2> NewVerts;
    NewVerts.reserve(P.NumVertices());
    for (int i = 0; i < P.NumVertices(); ++i)
    {
        // Get vertices
        glm::vec2 v1 = P.GetVertex(i);
        glm::vec2 v2 = P.GetVertex((i + 1) % P.NumVertices());

        // Check signed distance
        float d1 = H.SignedDistance(v1);
        float d2 = H.SignedDistance(v2);

        // If really close to plane, project onto plane
        if (std::abs(d1) < 1e-6f)
            d1 = 0.0f;
        if (std::abs(d2) < 1e-6f)
            d2 = 0.0f;

        // Which inside?
        bool i1 = d1 <= 0.0f;
        bool i2 = d2 <= 0.0f;

        // Distinguish different cases
        if (i1 && i2)
        {
            // If both inside, keep right end of segment
            NewVerts.emplace_back(v2);
            continue;
        }
        else if (!i1 && !i2)
        {
            // Both outside, we skip the segment
            continue;
        }
        // Find intersection with halfplane
        float dt = d2 - d1;
        float t = -d1 / dt;
        t = glm::clamp(t, 0.0f, 1.0f);
        glm::vec2 Intersect = v1 + t * (v2 - v1);
        // Intersection is surely part of the clipped polygon
        NewVerts.emplace_back(Intersect);
        // If left inside and right outside, we are exiting from the halfplane
        // The right end vertex should be skipped
        if (!i1 && i2)
        {
            // Otherwise, we are entering in the halfplane
            // Keep the right vertex
            NewVerts.emplace_back(v2);
        }
    }

    return cescg::Polygon(NewVerts);
}