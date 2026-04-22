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
}

cescg::Polygon::Polygon(const cescg::Polygon &P)
{
    m_Verts = P.m_Verts;
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