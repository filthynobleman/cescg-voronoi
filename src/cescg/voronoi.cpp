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
cescg::VoronoiRegion cescg::VoronoiDiagram::GetRegion(int i) const { return m_Regions[i]; }
std::vector<cescg::VoronoiRegion> cescg::VoronoiDiagram::GetRegions() const { return m_Regions; }

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
        for (int j = 0; j < NumSamples(); ++j)
        {
            if (i == j)
                continue;

            cescg::HalfPlane Hij = cescg::PerpendicularBisector(m_Samples[i], m_Samples[j]);
            m_Regions[i].SetPolygon(cescg::CutPolygon(Hij, m_Regions[i].GetPolygon()));

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
            glm::vec2 NewSite = m_Regions[i].GetPolygon().GetCentroid();
            Delta += glm::distance(m_Samples[i], NewSite);
            m_Samples[i] = NewSite;
        }
        Delta /= (float)NumSamples();
        
        Compute();

        if (Delta <= 1e-3f)
            break;
    }
}