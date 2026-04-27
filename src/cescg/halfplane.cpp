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