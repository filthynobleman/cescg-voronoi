/**
 * @file        bbox.hpp
 * 
 * @brief       Simple bounding box data structure.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#pragma once

#include <cescg/utils.hpp>


namespace cescg
{

class BoundingBox
{
private:
    glm::vec2 m_BL;
    glm::vec2 m_TR;
    
public:
    BoundingBox(const glm::vec2& BottomLeft = { 0.0f, 0.0f },
                const glm::vec2& TopRight = { 1.0f, 1.0f });
    BoundingBox(const cescg::BoundingBox& BB);
    BoundingBox(cescg::BoundingBox&& BB);
    cescg::BoundingBox& operator=(const cescg::BoundingBox& BB);
    cescg::BoundingBox& operator=(cescg::BoundingBox&& BB);
    ~BoundingBox();

    const glm::vec2& GetBottomLeft() const;
    void SetBottomLeft(const glm::vec2& P);
    glm::vec2 GetBottomRight() const;
    void SetBottomRight(const glm::vec2& P);
    const glm::vec2& GetTopRight() const;
    void SetTopRight(const glm::vec2& P);
    glm::vec2 GetTopLeft() const;
    void SetTopLeft(const glm::vec2& P);

    glm::vec2 GetSize() const;
    glm::vec2 GetCenter() const;

    float GetBottom() const;
    void SetBottom(float V);
    float GetTop() const;
    void SetTop(float V);
    float GetLeft() const;
    void SetLeft(float V);
    float GetRight() const;
    void SetRight(float V);
};

} // namespace cescg
