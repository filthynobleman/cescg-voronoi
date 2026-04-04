/**
 * @file        delaunay.cpp
 * 
 * @brief       Implementation of Delaunay triangulation interface.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-04
 */
#include <cescg/delaunay.hpp>

#include <map>

std::pair<glm::vec2, float> DelaunayCircumcircle(const glm::vec2& a, 
                                                 const glm::vec2& b, 
                                                 const glm::vec2& c)
{
    float d = 2 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
    float a2 = glm::dot(a, a);
    float b2 = glm::dot(b, b);
    float c2 = glm::dot(c, c);
    glm::vec2 u{    (a2 * (b.y - c.y) + b2 * (c.y - a.y) + c2 * (a.y - b.y)) / d,
                    (a2 * (c.x - b.x) + b2 * (a.x - c.x) + c2 * (b.x - a.x)) / d
                };
    return std::pair<glm::vec2, float>{ u, glm::distance(a, u) };
    
}

bool DelaunayAngleCompare(const glm::vec2& a, const glm::vec2& b)
{
    return std::atan2(a.y, a.x) < std::atan2(b.y, b.x);
}

cescg::Delaunay::Delaunay(const std::vector<glm::vec2> &Pts)
{
    m_Pts.emplace_back(-1.0f, -1.0f);
    m_Pts.emplace_back(4.0f, -1.0f);
    m_Pts.emplace_back(-1.0f, 4.0f);
    m_Tris.emplace_back(0, 1, 2);
    AddPoints(Pts);
}

cescg::Delaunay::Delaunay(const cescg::Delaunay &DT)
{
    m_Pts = DT.m_Pts;
    m_Tris = DT.m_Tris;
}

cescg::Delaunay::Delaunay(cescg::Delaunay &&DT)
{
    m_Pts = std::move(DT.m_Pts);
    m_Tris = std::move(DT.m_Tris);
}

cescg::Delaunay& cescg::Delaunay::operator=(const cescg::Delaunay &DT)
{
    m_Pts = DT.m_Pts;
    m_Tris = DT.m_Tris;
    return *this;
}

cescg::Delaunay& cescg::Delaunay::operator=(cescg::Delaunay &&DT)
{
    m_Pts = std::move(DT.m_Pts);
    m_Tris = std::move(DT.m_Tris);
    return *this;
}

cescg::Delaunay::~Delaunay() { }

void cescg::Delaunay::AddPoints(const std::vector<glm::vec2> &Pts)
{
    for (const auto& p : Pts)
        AddPoint(p);
}

void cescg::Delaunay::GetTriangulation(std::vector<glm::vec2> &Pts, 
                                       std::vector<glm::ivec3> &Tris) const
{
    Pts.clear();
    Tris.clear();
    if (m_Pts.size() == 3)
        return;
    Pts.reserve(m_Pts.size() - 3);
    Pts.insert(Pts.begin(), m_Pts.begin() + 3, m_Pts.end());
    Tris.reserve(m_Tris.size());
    for (const auto& t : m_Tris)
    {
        if (t.x < 3)
            continue;
        if (t.y < 3)
            continue;
        if (t.z < 3)
            continue;
        Tris.emplace_back(t - glm::ivec3(3));
    }
}

std::vector<cescg::VoronoiRegion> cescg::Delaunay::GetVoronoiDiagram() const
{
    std::vector<cescg::VoronoiRegion> Regions;
    Regions.resize(NumPoints());
    std::vector<glm::vec2> Pts;
    std::vector<glm::ivec3> Tris;
    GetTriangulation(Pts, Tris);
    for (int p = 0; p < Pts.size(); ++p)
        Regions[p].Site = Pts[p];
    for (const auto& t : Tris)
    {
        const glm::vec2& a = Pts[t.x];
        const glm::vec2& b = Pts[t.y];
        const glm::vec2& c = Pts[t.z];

        glm::vec2 u;
        float d;
        std::tie(u, d) = DelaunayCircumcircle(a, b, c);

        Regions[t.x].Polygon.emplace_back(u);
        Regions[t.y].Polygon.emplace_back(u);
        Regions[t.z].Polygon.emplace_back(u);
    }

    for (int p = 0; p < Regions.size(); ++p)
    {
        const glm::vec2& CurP = Regions[p].Site;
        std::sort(Regions[p].Polygon.begin(), Regions[p].Polygon.end(),
                  [CurP] (const glm::vec2& a, const glm::vec2& b) { return DelaunayAngleCompare(a - CurP, b - CurP); });
    }

    return Regions;
}

int cescg::Delaunay::NumPoints() const { return m_Pts.size() - 3; }


void cescg::Delaunay::AddPoint(const glm::vec2 &p)
{
    // Find triangles whose circumcircle contain the point
    std::vector<int> BadTriangles;
    for (int tt = 0; tt < m_Tris.size(); ++tt)
    {
        const auto& t = m_Tris[tt];
        const glm::vec2& a = m_Pts[t.x];
        const glm::vec2& b = m_Pts[t.y];
        const glm::vec2& c = m_Pts[t.z];

        glm::vec2 u;
        float d;
        std::tie(u, d) = DelaunayCircumcircle(a, b, c);

        if (glm::distance(u, p) <= d)
            BadTriangles.emplace_back(tt);
    }

    // Find boundary edges of the polygon made by bad triangles
    std::map<std::pair<int, int>, int> EMap;
    for (int tt : BadTriangles)
    {
        const auto& t = m_Tris[tt];

        std::pair<int, int> e12{ std::min(t.x, t.y), std::max(t.x, t.y) };
        if (EMap.find(e12) == EMap.end())
            EMap.emplace(e12, 0);
        EMap[e12]++;

        std::pair<int, int> e23{ std::min(t.y, t.z), std::max(t.y, t.z) };
        if (EMap.find(e23) == EMap.end())
            EMap.emplace(e23, 0);
        EMap[e23]++;

        std::pair<int, int> e31{ std::min(t.z, t.x), std::max(t.z, t.x) };
        if (EMap.find(e31) == EMap.end())
            EMap.emplace(e31, 0);
        EMap[e31]++;
    }

    std::vector<int> Polygon;
    for (auto it : EMap)
    {
        if (it.second == 1)
        {
            Polygon.emplace_back(it.first.first);
            Polygon.emplace_back(it.first.second);
        }
    }

    std::sort(Polygon.begin(), Polygon.end());
    auto PEnd = std::unique(Polygon.begin(), Polygon.end());
    Polygon.erase(PEnd, Polygon.end());
    std::sort(Polygon.begin(), Polygon.end(),
              [m_Pts=m_Pts,p] (int a, int b) { return DelaunayAngleCompare(m_Pts[a] - p, m_Pts[b] - p); });

    // Remove bad triangles
    for (int t : BadTriangles)
        m_Tris.erase(m_Tris.begin() + t);
    
    // Add new triangles by triangulating the polygon
    for (int k0 = 0; k0 < Polygon.size(); ++k0)
    {
        int k1 = (k0 + 1) % Polygon.size();
        m_Tris.emplace_back(Polygon[k0], Polygon[k1], m_Pts.size());
    }

    // Add the point
    m_Pts.emplace_back(p);
}