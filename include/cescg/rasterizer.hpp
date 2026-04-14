/**
 * @file        rasterizer.hpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#pragma once


#include <cescg/utils.hpp>
#include <cescg/polygon.hpp>


namespace cescg
{
    
class Rasterizer
{
private:
    int m_Width;
    int m_Height;

    void PixelsFromLine(int x1, int x2, int y,
                        std::vector<glm::ivec2>& Pixels) const;
    void PixelsFromFlatBottomTriangle(const glm::ivec2& a,
                                      const glm::ivec2& b,
                                      const glm::ivec2& c,
                                      std::vector<glm::ivec2>& Pixels) const;
    void PixelsFromFlatTopTriangle(const glm::ivec2& a,
                                   const glm::ivec2& b,
                                   const glm::ivec2& c,
                                   std::vector<glm::ivec2>& Pixels) const;
    void PixelsFromTriangle(const glm::ivec2& a,
                            const glm::ivec2& b,
                            const glm::ivec2& c,
                            std::vector<glm::ivec2>& Pixels) const;

public:
    Rasterizer(int Width, int Height);
    Rasterizer(const glm::ivec2& Size);
    Rasterizer(const cescg::Rasterizer& R);
    Rasterizer(cescg::Rasterizer&& R);
    cescg::Rasterizer& operator=(const cescg::Rasterizer& R);
    cescg::Rasterizer& operator=(cescg::Rasterizer&& R);
    ~Rasterizer();

    int GetCanvasWidth() const;
    int GetCanvasHeight() const;

    void PixelsFromConvexPolygon(const std::vector<glm::vec2> &Poly,
                                 std::vector<glm::ivec2>& Pixels) const;
    void PixelsFromConvexPolygon(const cescg::Polygon &Poly,
                                 std::vector<glm::ivec2>& Pixels) const;

    void PixelsFromCircle(const glm::vec2& Center,
                          float Radius,
                          std::vector<glm::ivec2>& Pixels) const;

    void PixelsFromSegment(const glm::vec2& a,
                           const glm::vec2& b,
                           float Width,
                           std::vector<glm::ivec2>& Pixels) const;

    glm::ivec2 PixelAtCoordinates(const glm::vec2& p) const;
};

} // namespace cescg
