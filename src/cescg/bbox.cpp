/**
 * @file        bbox.cpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#include <cescg/bbox.hpp>


cescg::BoundingBox::BoundingBox(const glm::vec2 &BottomLeft, 
                                const glm::vec2 &TopRight)
    : m_BL(BottomLeft), m_TR(TopRight) { }

cescg::BoundingBox::BoundingBox(const cescg::BoundingBox &BB)
{
    m_BL = BB.m_BL;
    m_TR = BB.m_TR;
}

cescg::BoundingBox::BoundingBox(cescg::BoundingBox &&BB)
{
    m_BL = std::move(BB.m_BL);
    m_TR = std::move(BB.m_TR);
}

cescg::BoundingBox& cescg::BoundingBox::operator=(const cescg::BoundingBox &BB)
{
    m_BL = BB.m_BL;
    m_TR = BB.m_TR;
    return *this;
}

cescg::BoundingBox& cescg::BoundingBox::operator=(cescg::BoundingBox &&BB)
{
    m_BL = std::move(BB.m_BL);
    m_TR = std::move(BB.m_TR);
    return *this;
}

cescg::BoundingBox::~BoundingBox() { }

const glm::vec2 &cescg::BoundingBox::GetBottomLeft() const { return m_BL; }
glm::vec2 cescg::BoundingBox::GetBottomRight() const { return glm::vec2(GetRight(), GetBottom()); }
const glm::vec2 &cescg::BoundingBox::GetTopRight() const { return m_TR; }
glm::vec2 cescg::BoundingBox::GetTopLeft() const { return glm::vec2(GetLeft(), GetTop()); }

void cescg::BoundingBox::SetBottomLeft(const glm::vec2 &P)
{
    SetBottom(P.y);
    SetLeft(P.x);
}

void cescg::BoundingBox::SetBottomRight(const glm::vec2 &P)
{
    SetBottom(P.y);
    SetRight(P.x);
}

void cescg::BoundingBox::SetTopRight(const glm::vec2 &P)
{
    SetTop(P.y);
    SetRight(P.x);
}

void cescg::BoundingBox::SetTopLeft(const glm::vec2 &P)
{
    SetTop(P.y);
    SetLeft(P.x);
}

glm::vec2 cescg::BoundingBox::GetSize() const
{
    return m_TR - m_BL;
}

glm::vec2 cescg::BoundingBox::GetCenter() const
{
    return 0.5f * (m_TR + m_BL);
}

float cescg::BoundingBox::GetBottom() const { return m_BL.y; }
float cescg::BoundingBox::GetTop() const { return m_TR.y; }
float cescg::BoundingBox::GetLeft() const { return m_BL.x; }
float cescg::BoundingBox::GetRight() const { return m_TR.x; }

void cescg::BoundingBox::SetBottom(float V)
{
    if (V >= GetTop())
        throw std::runtime_error("Empty bounding box");
    m_BL.y = V;
}

void cescg::BoundingBox::SetTop(float V)
{
    if (V <= GetBottom())
        throw std::runtime_error("Empty bounding box");
    m_TR.y = V;
}

void cescg::BoundingBox::SetLeft(float V)
{
    if (V >= GetRight())
        throw std::runtime_error("Empty bounding box");
    m_BL.x = V;
}

void cescg::BoundingBox::SetRight(float V)
{
    if (V <= GetLeft())
        throw std::runtime_error("Empty bounding box");
    m_TR.x = V;
}