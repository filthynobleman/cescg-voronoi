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
        return a.y < b.y;
    return false;
}

void PixelsFromLine(int x1, int x2, int y,
                    std::vector<glm::ivec2>& Pixels)
{
    for (int x = x1; x <= x2; ++x)
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
    for (int y = c.y; y > a.y; --y)
    {
        PixelsFromLine(XAC, XBC, y, Pixels);
        XAC -= SlopeAC;
        XBC -= SlopeBC;
    }
}


float Cross2D(const glm::vec2& a, const glm::vec2& b)
{
    return a.x * b.y - a.y * b.x;
}


void PixelsFromTriangle(const glm::ivec2& a,
                        const glm::ivec2& b,
                        const glm::ivec2& c,
                        std::vector<glm::ivec2>& Pixels)
{
    glm::ivec4 BBox;
    BBox.x = std::min(a.x, std::min(b.x, c.x));
    BBox.y = std::min(a.y, std::min(b.y, c.y));
    BBox.z = std::max(a.x, std::max(b.x, c.x));
    BBox.w = std::max(a.y, std::max(b.y, c.y));

    glm::vec2 ab = b - a;
    glm::vec2 ac = c - a;
    float A = Cross2D(ab, ac);
    for (int x = BBox.x; x <= BBox.z; ++x)
    {
        for (int y = BBox.y; y <= BBox.w; ++y)
        {
            glm::vec2 p{ x, y };
            p -= a;
            float s = Cross2D(p, ac) / A;
            float t = Cross2D(p, ab) / A;
            if (s < 0 || t < 0 || s + 1 > 1)
                continue;
            Pixels.emplace_back(x, y);
        }
    }
}

#include <iostream>

std::vector<glm::ivec2> cescg::PixelsFromConvexPolygon(const std::vector<glm::ivec2> &Polygon)
{
    glm::ivec2 Centroid{ 0, 0 };
    glm::ivec4 BBox{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max(),
                     std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };
    for (const auto& v : Polygon)
    {
        Centroid += v;
        BBox.x = std::min(BBox.x, v.x);
        BBox.y = std::min(BBox.y, v.y);
        BBox.z = std::max(BBox.x, v.x);
        BBox.w = std::max(BBox.y, v.y);
    }
    Centroid = glm::round(glm::vec2(Centroid) / (float)Polygon.size());

    std::vector<glm::ivec2> Pixels;
    Pixels.reserve((BBox.z - BBox.x) * (BBox.w - BBox.y));
    std::cout << Polygon.size() << ": ";
    std::cout << (BBox.z - BBox.x) << "-by-" << (BBox.w - BBox.y) << std::endl;
    for (int k0 = 0; k0 < Polygon.size(); ++k0)
    {
        int k1 = (k0 + 1) % Polygon.size();
        // Call PixelsFromTriangle() ensuring vertices are sorted by descending y
        std::vector<glm::ivec2> Triangle{ Centroid, Polygon[k0], Polygon[k1] };
        std::sort(Triangle.begin(), Triangle.end(), ComparePoints);
        PixelsFromTriangle(Triangle[0], Triangle[1], Triangle[2], Pixels);
    }

    std::sort(Pixels.begin(), Pixels.end(), ComparePoints);
    auto PEnd = std::unique(Pixels.begin(), Pixels.end());
    Pixels.erase(PEnd, Pixels.end());

    std::cout << "Pixels computed" << std::endl;

    return Pixels;
}