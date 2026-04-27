/**
 * @file        polygon.cpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#include <cescg/polygon.hpp>


cescg::Polygon::Polygon(const std::vector<glm::vec2> &Vertices)
{
    m_Verts = Vertices;
    m_Verts.reserve(10);
}

cescg::Polygon::Polygon(const cescg::Polygon &P)
{
    m_Verts = P.m_Verts;
    m_Verts.reserve(10);
}

cescg::Polygon::Polygon(cescg::Polygon &&P)
{
    m_Verts = std::move(P.m_Verts);
}

cescg::Polygon& cescg::Polygon::operator=(const cescg::Polygon &P)
{
    m_Verts = P.m_Verts;
    return *this;
}

cescg::Polygon& cescg::Polygon::operator=(cescg::Polygon &&P)
{
    m_Verts = std::move(P.m_Verts);
    return *this;
}

cescg::Polygon::~Polygon() { }

int cescg::Polygon::NumVertices() const { return m_Verts.size(); }
const glm::vec2 &cescg::Polygon::GetVertex(int i) const { return m_Verts[i]; }
void cescg::Polygon::SetVertex(int i, const glm::vec2 &v) { m_Verts[i] = v; }
const std::vector<glm::vec2> &cescg::Polygon::GetVertices() const { return m_Verts; }
void cescg::Polygon::SetVertices(const std::vector<glm::vec2> &NewVerts) { m_Verts = NewVerts; }

float cescg::Polygon::GetArea() const
{
    float Area = 0.0f;
    for (int i = 0; i < NumVertices(); ++i)
    {
        int ii = i + 1;
        if (ii >= NumVertices())
            ii = 0;
        Area += (m_Verts[i].x * m_Verts[ii].y - m_Verts[ii].x * m_Verts[i].y);
    }
    return 0.5f * Area;
}

glm::vec2 cescg::Polygon::GetCentroid() const
{
    glm::vec2 B(0.0f);
    for (int i = 0; i < NumVertices(); ++i)
        B += GetVertex(i);
    return B / (float)NumVertices();
}

glm::vec2 cescg::Polygon::GetCenterOfMass() const
{
    glm::vec2 C(0.0f);
    for (int i = 0; i < NumVertices(); ++i)
    {
        int ii = i + 1;
        if (ii >= NumVertices())
            ii = 0;
        float Cross = (m_Verts[i].x * m_Verts[ii].y - m_Verts[i].y * m_Verts[ii].x);
        C.x += (m_Verts[i].x + m_Verts[ii].x) * Cross;
        C.y += (m_Verts[i].y + m_Verts[ii].y) * Cross;
    }
    return C / (6.0f * GetArea());
}

std::pair<glm::vec2, glm::vec2> cescg::Polygon::GetEdge(int i) const
{
    return { GetVertex(i), GetVertex((i + 1) % NumVertices()) };
}

std::vector<std::pair<glm::vec2, glm::vec2>> cescg::Polygon::GetEdges() const
{
    std::vector<std::pair<glm::vec2, glm::vec2>> Edges;
    Edges.reserve(NumVertices());
    for (int i = 0; i < NumVertices(); ++i)
        Edges.emplace_back(GetEdge(i));
    return Edges;
}

void cescg::Polygon::CutInPlace(const cescg::HalfPlane &H)
{
    // Handle degenerate polygons
    if (NumVertices() == 0)
        return;
    
    if (NumVertices() == 1)
    {
        if (!H.IsInside(GetVertex(0)))
            m_Verts.clear();
        return;
    }

    std::vector<glm::vec2> NewVerts;
    NewVerts.reserve(NumVertices() + 1);
    for (int i = 0; i < NumVertices(); ++i)
    {
        // Get vertices
        glm::vec2 v1 = GetVertex(i);
        glm::vec2 v2 = GetVertex((i + 1) % NumVertices());

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

    m_Verts = std::move(NewVerts);
}

cescg::Polygon cescg::Polygon::Cut(const cescg::HalfPlane &H)
{
    cescg::Polygon P(*this);
    P.CutInPlace(H);
    return P;
}