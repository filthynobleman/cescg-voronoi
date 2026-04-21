/**
 * @file        rasterizer.cpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-04-11
 */
#include <cescg/rasterizer.hpp>


cescg::Rasterizer::Rasterizer(int Width, int Height)
{
    m_Width = Width;
    m_Height = Height;
}

cescg::Rasterizer::Rasterizer(const glm::ivec2 &Size)
{
    m_Width = Size.x;
    m_Height = Size.y;
}

cescg::Rasterizer::Rasterizer(const cescg::Rasterizer &R)
{
    m_Width = R.m_Width;
    m_Height = R.m_Height;
}

cescg::Rasterizer::Rasterizer(cescg::Rasterizer &&R)
{
    m_Width = R.m_Width;
    m_Height = R.m_Height;
}

cescg::Rasterizer& cescg::Rasterizer::operator=(const cescg::Rasterizer &R)
{
    m_Width = R.m_Width;
    m_Height = R.m_Height;
    return *this;
}

cescg::Rasterizer& cescg::Rasterizer::operator=(cescg::Rasterizer &&R)
{
    m_Width = R.m_Width;
    m_Height = R.m_Height;
    return *this;
}

cescg::Rasterizer::~Rasterizer() { }

void cescg::Rasterizer::PixelsFromLine(int x1, int x2, int y, 
                                       std::vector<glm::ivec2> &Pixels) const
{
    int xmin = glm::clamp(std::min(x1, x2), 0, m_Width - 1);
    int xmax = glm::clamp(std::max(x1, x2), 0, m_Width - 1);
    for (int x = xmin; x <= xmax; ++x)
        Pixels.emplace_back(x, y);
}

void cescg::Rasterizer::PixelsFromFlatBottomTriangle(const glm::ivec2 &a, 
                                                     const glm::ivec2 &b, 
                                                     const glm::ivec2 &c, 
                                                     std::vector<glm::ivec2> &Pixels) const
{
    float SlopeBC = (b.x - c.x) / (float)(b.y - c.y);
    float SlopeAC = (a.x - c.x) / (float)(a.y - c.y);
    float XBC = c.x;
    float XAC = c.x;
    int ymin = glm::clamp(a.y, 0, m_Height - 1);
    int ymax = glm::clamp(c.y, 0, m_Height - 1);
    for (int y = ymax; y >= ymin; --y)
    {
        PixelsFromLine(XBC, XAC, y, Pixels);
        XBC -= SlopeBC;
        XAC -= SlopeAC;
    }
}

void cescg::Rasterizer::PixelsFromFlatTopTriangle(const glm::ivec2 &a, 
                                                  const glm::ivec2 &b, 
                                                  const glm::ivec2 &c, 
                                                  std::vector<glm::ivec2> &Pixels) const
{
    float SlopeAC = (a.x - c.x) / (float)(a.y - c.y);
    float SlopeAB = (a.x - b.x) / (float)(a.y - b.y);
    float XAC = a.x;
    float XAB = a.x;
    int ymin = glm::clamp(a.y, 0, m_Height - 1);
    int ymax = glm::clamp(c.y, 0, m_Height - 1);
    for (int y = ymin; y <= ymax; ++y)
    {
        PixelsFromLine(XAC, XAB, y, Pixels);
        XAC += SlopeAC;
        XAB += SlopeAB;
    }
}

void cescg::Rasterizer::PixelsFromTriangle(const glm::ivec2 &a, 
                                           const glm::ivec2 &b,
                                           const glm::ivec2 &c, 
                                           std::vector<glm::ivec2> &Pixels) const
{
    if (a.y == b.y)
    {
        PixelsFromFlatBottomTriangle(a, b, c, Pixels);
        return;
    }
    else if (b.y == c.y)
    {
        PixelsFromFlatTopTriangle(a, b, c, Pixels);
    }
    glm::ivec2 t;
    t.x = a.x + ((float)(b.y - a.y) / (float)(c.y - a.y)) * (c.x - a.x);
    t.y = b.y;
    PixelsFromFlatTopTriangle(a, b, t, Pixels);
    PixelsFromFlatBottomTriangle(b, t, c, Pixels);
}


bool ComparePoints(const glm::ivec2& a, const glm::ivec2& b)
{
    if (a.y < b.y)
        return true;
    if (a.y == b.y)
        return a.x < b.x;
    return false;
}




void cescg::Rasterizer::PixelsFromConvexPolygon(const std::vector<glm::vec2> &Poly, 
                                                std::vector<glm::ivec2> &Pixels) const
{
    glm::ivec4 BBox{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max(),
                     std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };
    glm::vec2 Centroid(0.0f);
    for (const auto& v : Poly)
    {
        Centroid += v;
        BBox.x = std::min(BBox.x, (int)std::floor(v.x));
        BBox.y = std::min(BBox.y, (int)std::floor(v.y));
        BBox.z = std::max(BBox.z, (int)std::ceil(v.x));
        BBox.w = std::max(BBox.w, (int)std::ceil(v.y));
    }
    Centroid /= Poly.size();

    Pixels.clear();
    Pixels.reserve((BBox.z - BBox.x) * (BBox.w - BBox.y));
    for (int k0 = 1; k0 < Poly.size() - 1; ++k0)
    {
        int k1 = k0 + 1;
        // Call PixelsFromTriangle() ensuring vertices are sorted by descending y
        std::vector<glm::ivec2> Triangle{ Poly[0], Poly[k0], Poly[k1] };
        std::sort(Triangle.begin(), Triangle.end(), ComparePoints);
        PixelsFromTriangle(Triangle[0], Triangle[1], Triangle[2], Pixels);
    }

    std::sort(Pixels.begin(), Pixels.end(), ComparePoints);
    auto PEnd = std::unique(Pixels.begin(), Pixels.end());
    Pixels.erase(PEnd, Pixels.end());
}

void cescg::Rasterizer::PixelsFromConvexPolygon(const cescg::Polygon &Poly, 
                                                std::vector<glm::ivec2> &Pixels) const
{
    PixelsFromConvexPolygon(Poly.GetVertices(), Pixels);
}

void cescg::Rasterizer::PixelsFromCircle(const glm::vec2 &Center, 
                                         float Radius, 
                                         std::vector<glm::ivec2> &Pixels) const
{
    double r_sqr = (Radius + 0.5) * (Radius + 0.5);
    Pixels.clear();
    Pixels.reserve(4 * r_sqr);
    for (int dj = - Radius; dj <= Radius; ++dj)
    {
        int jj = Center.y + dj;
        if (jj < 0 || jj >= m_Height)
            continue;
        for (int di = -Radius; di <= Radius; ++di)
        {
            int ii = Center.x + di;
            if (ii < 0 || ii >= m_Width)
                continue;
            
            double d_sqr = di * di + dj * dj;
            if (d_sqr > r_sqr)
                continue;

            Pixels.emplace_back(ii, jj);
        }
    }
}

void cescg::Rasterizer::PixelsFromSegment(const glm::vec2 &a, 
                                          const glm::vec2 &b, 
                                          float Width, 
                                          std::vector<glm::ivec2> &Pixels) const
{
    glm::ivec2 Diff = b - a;
    float XDiff = Diff.x;
    float YDiff = Diff.y;
    Pixels.clear();
    Pixels.reserve((size_t)glm::round(glm::length(glm::vec2(Diff)) * Width));
    if (std::abs(XDiff) > std::abs(YDiff))
    {
        float Slope = YDiff / XDiff;
        for (int x = std::min(a.x, b.x); x <= std::max(a.x, b.x); ++x)
        {
            int y = a.y + ((x - a.x) * Slope);
            for (int yy = y - Width / 2; yy <= y + Width / 2; ++yy)
                Pixels.emplace_back(x, yy);
        }
    }
    else
    {
        float Slope = XDiff / YDiff;
        for (int y = std::min(a.y, b.y); y <= std::max(a.y, b.y); ++y)
        {
            int x = a.x + ((y - a.y) * Slope);
            for (int xx = x - Width / 2; xx <= x + Width / 2; ++xx)
                Pixels.emplace_back(xx, y);
        }
    }
}

glm::ivec2 cescg::Rasterizer::PixelAtCoordinates(const glm::vec2 &p) const
{
    glm::ivec2 Result(glm::round(p.x), glm::round(p.y));
    Result.x = glm::clamp(Result.x, 0, m_Width - 1);
    Result.y = glm::clamp(Result.y, 0, m_Height - 1);
    return Result;
}

int cescg::Rasterizer::GetCanvasWidth() const { return m_Width; }
int cescg::Rasterizer::GetCanvasHeight() const { return m_Height; }