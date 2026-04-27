/**
 * @file        voronoi.cpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#include <cescg/voronoi.hpp>
#include <cescg/halfplane.hpp>
#include <cescg/randgen.hpp>

#include <queue>


cescg::VoronoiDiagram::VoronoiDiagram(const cescg::BoundingBox &BB)
{
    m_BBox = BB;
    m_UpToDate = false;
}

cescg::VoronoiDiagram::VoronoiDiagram(const cescg::VoronoiDiagram &VD)
{
    m_Samples = VD.m_Samples;
    m_Regions = VD.m_Regions;
    m_BBox = VD.m_BBox;
    m_UpToDate = VD.m_UpToDate;
}

cescg::VoronoiDiagram::VoronoiDiagram(cescg::VoronoiDiagram &&VD)
{
    m_Samples = std::move(VD.m_Samples);
    m_Regions = std::move(VD.m_Regions);
    m_BBox = std::move(VD.m_BBox);
    m_UpToDate = std::move(VD.m_UpToDate);
}

cescg::VoronoiDiagram& cescg::VoronoiDiagram::operator=(const cescg::VoronoiDiagram &VD)
{
    m_Samples = VD.m_Samples;
    m_Regions = VD.m_Regions;
    m_BBox = VD.m_BBox;
    m_UpToDate = VD.m_UpToDate;
    return *this;
}

cescg::VoronoiDiagram& cescg::VoronoiDiagram::operator=(cescg::VoronoiDiagram &&VD)
{
    m_Samples = std::move(VD.m_Samples);
    m_Regions = std::move(VD.m_Regions);
    m_BBox = std::move(VD.m_BBox);
    m_UpToDate = std::move(VD.m_UpToDate);
    return *this;
}

cescg::VoronoiDiagram::~VoronoiDiagram() { }

const cescg::BoundingBox &cescg::VoronoiDiagram::GetBoundingBox() const { return m_BBox; }
cescg::BoundingBox &cescg::VoronoiDiagram::GetBoundingBox() { return m_BBox; }

void cescg::VoronoiDiagram::AddSample(const glm::vec2 &Sample)
{
    m_Samples.emplace_back(Sample);
    m_UpToDate = false;
}

void cescg::VoronoiDiagram::AddSamples(const std::vector<glm::vec2> &Samples)
{
    for (const auto& s : Samples)
        AddSample(s);
}

bool cescg::VoronoiDiagram::IsUpToDate() const { return m_UpToDate; }

int cescg::VoronoiDiagram::NumSamples() const { return m_Samples.size(); }
const glm::vec2 &cescg::VoronoiDiagram::GetSample(int i) const { return m_Samples[i]; }
const std::vector<glm::vec2> &cescg::VoronoiDiagram::GetSamples() const { return m_Samples; }
const cescg::VoronoiRegion& cescg::VoronoiDiagram::GetRegion(int i) const { return m_Regions[i]; }
const std::vector<cescg::VoronoiRegion>& cescg::VoronoiDiagram::GetRegions() const { return m_Regions; }

bool AngleCompare(const glm::vec2& a, const glm::vec2& b)
{
    return std::atan2(a.y, a.x) < std::atan2(b.y, b.x);
}

void cescg::VoronoiDiagram::Compute()
{
    m_Regions.clear();
    m_Regions.reserve(NumSamples());
    cescg::Polygon StartBB({m_BBox.GetBottomLeft(),
                            m_BBox.GetBottomRight(),
                            m_BBox.GetTopRight(),
                            m_BBox.GetTopLeft()});
    for (int i = 0; i < NumSamples(); ++i)
        m_Regions.emplace_back(m_Samples[i], StartBB);

    #pragma omp parallel for
    for (int i = 0; i < NumSamples(); ++i)
    {
        // Get smaller ball centered at barycenter that contains the polygon
        float MaxDist = 0.0;
        glm::vec2 CoM = m_Regions[i].GetPolygon().GetCenterOfMass();
        for (int k = 0; k < m_Regions[i].GetPolygon().NumVertices(); ++k)
        {
            glm::vec2 d = CoM - m_Regions[i].GetPolygon().GetVertex(k);
            MaxDist = std::max(MaxDist, glm::dot(d, d));
        }
        for (int j = 0; j < NumSamples(); ++j)
        {
            if (i == j)
                continue;

            // If distance is larger than twice the ball, polygon is completely
            // inside the halfplane determined by the perpendicular bisector
            // D > 2 * MaxDist -> D^2 > 4 * MaxDist^2
            glm::vec2 Diff = m_Samples[i] - m_Samples[j];
            if (glm::dot(Diff, Diff) >= 4 * MaxDist)
                continue;

            // Compute halfplane determined by perpendicular bisector
            // and execute the cut
            cescg::HalfPlane Hij = cescg::PerpendicularBisector(m_Samples[i], m_Samples[j]);
            m_Regions[i].GetPolygon().CutInPlace(Hij);

            // Recompute ball
            CoM = m_Regions[i].GetPolygon().GetCenterOfMass();
            MaxDist = 0.0;
            for (int k = 0; k < m_Regions[i].GetPolygon().NumVertices(); ++k)
            {
                glm::vec2 d = CoM - m_Regions[i].GetPolygon().GetVertex(k);
                MaxDist = std::max(MaxDist, glm::dot(d, d));
            }

            // Handle degenerate cases
            if (m_Regions[i].GetPolygon().NumVertices() == 0)
                break;
        }
    }

    m_UpToDate = true;
}

void cescg::VoronoiDiagram::MakeCentroidal(int MaxIter)
{
    if (MaxIter < 0)
        MaxIter = std::numeric_limits<int>::max();
    
    if (!IsUpToDate())
        Compute();

    for (int Iter = 0; Iter < MaxIter; ++Iter)
    {
        float Delta = 0.0f;
        for (int i = 0; i < NumSamples(); ++i)
        {
            glm::vec2 NewSite = m_Regions[i].GetPolygon().GetCenterOfMass();
            Delta += glm::distance(m_Samples[i], NewSite);
            m_Samples[i] = NewSite;
        }
        Delta /= (float)NumSamples();
        
        Compute();

        if (Delta <= 1e-3f)
            break;
    }
}


struct FrontPropGreater
{
    bool operator()(const std::pair<double, glm::ivec2>& a,
                    const std::pair<double, glm::ivec2>& b) const
    {
        if (a.first != b.first)
            return a.first > b.first;
        if (a.second.x != b.second.x)
            return a.second.x > b.second.x;
        return a.second.y > b.second.y;
    }
};

cescg::Grid<int> cescg::FrontPropagation(const cescg::Image &Img, 
                                         const std::vector<glm::ivec2> &Sites, 
                                         CellDistance DistFun)
{
    cescg::Grid<int> VPart(Img.GetWidth(), Img.GetHeight(), -1);
    cescg::Grid<double> Dists(Img.GetWidth(), Img.GetHeight(), std::numeric_limits<double>::infinity());

    // For each site, isotropic expansion
    for (int s = 0; s < Sites.size(); ++s)
    {
        std::priority_queue<std::pair<double, glm::ivec2>,
                            std::vector<std::pair<double, glm::ivec2>>,
                            FrontPropGreater> Q;

        VPart(Sites[s].x, Sites[s].y) = s;
        Q.emplace(0.0, Sites[s]);
        while (!Q.empty())
        {
            double w;
            glm::ivec2 p;
            std::tie(w, p) = Q.top();
            Q.pop();

            if (w >= Dists(p.x, p.y))
                continue;
            
            Dists(p.x, p.y) = w;
            VPart(p.x, p.y) = s;

            glm::ivec2 a;
            for (int dy = -1; dy <= 1; ++dy)
            {
                a.y = p.y + dy;
                if (a.y < 0 || a.y >= Img.GetHeight())
                    continue;
                for (int dx = -1; dx <= 1; ++dx)
                {
                    if (dy == 0 && dx == 0)
                        continue;
                    a.x = p.x + dx;
                    if (a.x < 0  || a.x >= Img.GetWidth())
                        continue;
                    
                    double NewDist = DistFun(Img, p, a) + w;
                    if (Dists(a.x, a.y) > NewDist)
                        Q.emplace(NewDist, a);
                }
            }
        }
    }

    return VPart;
}

cescg::Grid<int> cescg::VoronoiTexture(int Width, 
                                       int Height, 
                                       float Scale, 
                                       float Randomness)
{
    cescg::Grid<int> VPart(Width, Height, -1);

    #pragma omp parallel for
    for (int j = 0; j < Height; ++j)
    {
        for (int i = 0; i < Width; ++i)
        {
            // Normalize coordinates to shortest dimension of the grid
            glm::vec2 p(i, j);
            p /= std::min(Width, Height);
            // Scale according to input parameter
            p *= Scale;
            
            // Get coordinates of enclosing cell
            glm::ivec2 Cell = glm::floor(p);

            // Search 3x3 neighborhood for closest sample
            double MinDist = std::numeric_limits<double>::infinity();
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    glm::ivec2 CurCell = Cell + glm::ivec2(dx, dy);
                    // Position is the middle of the cell plus a (scaled) random offset
                    glm::vec2 Site(CurCell.x + 0.5f, CurCell.y + 0.5f);
                    glm::vec2 Offset(cescg::RandGen::GetInstance().HashFloat(CurCell),
                                     cescg::RandGen::GetInstance().HashFloat(CurCell));
                    Site += Randomness * (0.5f * Offset - 0.5f);
                    double d = glm::distance(p, Site);
                    if (d < MinDist)
                    {
                        MinDist = d;
                        VPart(i, j) = cescg::RandGen::GetInstance().HashInt(CurCell);
                    }
                }
            }
        }
    }

    return VPart;
}