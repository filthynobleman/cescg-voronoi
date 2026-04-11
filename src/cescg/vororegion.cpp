/**
 * @file        vororegion.cpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#include <cescg/vororegion.hpp>


cescg::VoronoiRegion::VoronoiRegion(const glm::vec2 &Site, 
                                    const cescg::Polygon &Polygon)
    : m_Site(Site), m_Polygon(Polygon) { }

cescg::VoronoiRegion::VoronoiRegion(const glm::vec2 &Site, 
                                    const std::vector<glm::vec2> &Polygon)
    : m_Site(Site), m_Polygon(Polygon) { }

cescg::VoronoiRegion::VoronoiRegion(const cescg::VoronoiRegion &Reg)
    : m_Polygon(Reg.m_Polygon)
{
    m_Site = Reg.m_Site;
}

cescg::VoronoiRegion::VoronoiRegion(cescg::VoronoiRegion &&Reg)
    : m_Polygon(std::move(Reg.m_Polygon))
{
    m_Site = std::move(Reg.m_Site);
}

cescg::VoronoiRegion &cescg::VoronoiRegion::operator=(const cescg::VoronoiRegion &Reg)
{
    m_Site = Reg.m_Site;
    m_Polygon = Reg.m_Polygon;
    return *this;
}

cescg::VoronoiRegion &cescg::VoronoiRegion::operator=(cescg::VoronoiRegion &&Reg)
{
    m_Site = std::move(Reg.m_Site);
    m_Polygon = std::move(Reg.m_Polygon);
    return *this;
}

cescg::VoronoiRegion::~VoronoiRegion() { }

const glm::vec2 &cescg::VoronoiRegion::GetSite() const { return m_Site; }
void cescg::VoronoiRegion::SetSite(const glm::vec2 &Site) { m_Site = Site; }
const cescg::Polygon &cescg::VoronoiRegion::GetPolygon() const { return m_Polygon; }
void cescg::VoronoiRegion::SetPolygon(const cescg::Polygon &Poly) { m_Polygon = Poly; }
void cescg::VoronoiRegion::SetPolygon(const std::vector<glm::vec2> &Poly) { m_Polygon = cescg::Polygon(Poly); }