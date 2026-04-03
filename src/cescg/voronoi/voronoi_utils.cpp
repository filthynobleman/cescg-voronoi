/**
 * @file        voronoi_utils.cpp
 * 
 * @brief       Utility functions for Voronoi decompositions.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-03
 */
#include <cescg/voronoi.hpp>


bool ComparePoints(const glm::ivec2& a, const glm::ivec2& b)
{
    if (a.y < b.y)
        return true;
    if (a.y == b.y)
        return a.x < b.x;
    return false;
}

void PixelsFromLine(int x1, int x2, int y,
                    std::vector<glm::ivec2>& Pixels)
{
    for (int x = std::min(x1, x2); x <= std::max(x1, x2); ++x)
        Pixels.emplace_back(x, y);
}

void PixelsFromFlatBottomTriangle(const glm::ivec2& a,
                                  const glm::ivec2& b,
                                  const glm::ivec2& c,
                                  std::vector<glm::ivec2>& Pixels)
{
    float SlopeAB = (b.x - a.x) / (float)(b.y - a.y);
    float SlopeAC = (c.x - a.x) / (float)(c.y - a.y);
    float XAB = a.x;
    float XAC = a.x;
    for (int y = a.y; y <= b.y; ++y)
    {
        PixelsFromLine(XAB, XAC, y, Pixels);
        XAB += SlopeAB;
        XAC += SlopeAC;
    }
}

void PixelsFromFlatTopTriangle(const glm::ivec2& a,
                               const glm::ivec2& b,
                               const glm::ivec2& c,
                               std::vector<glm::ivec2>& Pixels)
{
    float SlopeAC = (c.x - a.x) / (float)(c.y - a.y);
    float SlopeBC = (c.x - b.x) / (float)(c.y - b.y);
    float XAC = c.x;
    float XBC = c.x;
    for (int y = c.y; y >= a.y; --y)
    {
        PixelsFromLine(XAC, XBC, y, Pixels);
        XAC -= SlopeAC;
        XBC -= SlopeBC;
    }
}


void PixelsFromTriangle(const glm::ivec2& a,
                        const glm::ivec2& b,
                        const glm::ivec2& c,
                        std::vector<glm::ivec2>& Pixels)
{
    if (a.y == b.y)
    {
        PixelsFromFlatTopTriangle(a, b, c, Pixels);
        return;
    }
    else if (b.y == c.y)
    {
        PixelsFromFlatBottomTriangle(a, b, c, Pixels);
    }
    glm::ivec2 t;
    t.x = a.x + ((float)(b.y - a.y) / (float)(c.y - a.y)) * (c.x - a.x);
    t.y = b.y;
    PixelsFromFlatBottomTriangle(a, b, t, Pixels);
    PixelsFromFlatTopTriangle(b, t, c, Pixels);
}

#include <iostream>

std::vector<glm::ivec2> cescg::PixelsFromConvexPolygon(const std::vector<glm::vec2> &Polygon)
{
    glm::ivec4 BBox{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max(),
                     std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };
    glm::vec2 Centroid(0.0f);
    for (const auto& v : Polygon)
    {
        Centroid += v;
        BBox.x = std::min(BBox.x, (int)std::floor(v.x));
        BBox.y = std::min(BBox.y, (int)std::floor(v.y));
        BBox.z = std::max(BBox.z, (int)std::ceil(v.x));
        BBox.w = std::max(BBox.w, (int)std::ceil(v.y));
    }
    Centroid /= Polygon.size();

    std::vector<glm::ivec2> Pixels;
    Pixels.reserve((BBox.z - BBox.x) * (BBox.w - BBox.y));
    for (int k0 = 1; k0 < Polygon.size() - 1; ++k0)
    {
        int k1 = k0 + 1;
        // Call PixelsFromTriangle() ensuring vertices are sorted by descending y
        std::vector<glm::ivec2> Triangle{ Polygon[0], Polygon[k0], Polygon[k1] };
        std::sort(Triangle.begin(), Triangle.end(), ComparePoints);
        PixelsFromTriangle(Triangle[0], Triangle[1], Triangle[2], Pixels);
    }

    std::sort(Pixels.begin(), Pixels.end(), ComparePoints);
    auto PEnd = std::unique(Pixels.begin(), Pixels.end());
    Pixels.erase(PEnd, Pixels.end());

    return Pixels;
}