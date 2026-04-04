/**
 * @file        voronoi_part.cpp
 * 
 * @brief       Implementation of cescg::VoronoiPartitioning()
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-31
 */
#include <cescg/voronoi.hpp>

#include <cescg/delaunay.hpp>


cescg::Grid<int> cescg::VoronoiPartitioning(const cescg::Image &Img, 
                                            const std::vector<glm::ivec2> &Samples)
{
    constexpr int SHWidth = 64;
    cescg::Grid<int> G(Img.GetWidth(), Img.GetHeight());

    // Spatial hashing is required for performance
    int SHSize = (Img.GetWidth() + SHWidth - 1) / SHWidth;
    int SHHeight = (Img.GetHeight() + SHSize - 1) / SHSize;
    cescg::Grid<std::vector<int>> SH(SHWidth, SHHeight);
    for (int s = 0; s < Samples.size(); ++s)
    {
        glm::ivec2 SHCell = Samples[s] / glm::ivec2(SHSize);
        SH(SHCell.x, SHCell.y).push_back(s);
    }
    
    #pragma omp parallel for
    for (int j = 0; j < G.GetHeight(); ++j)
    {
        for (int i = 0; i < G.GetWidth(); ++i)
        {
            glm::ivec2 ij(i, j);
            glm::ivec2 SHCell = ij / glm::ivec2(SHSize);
            double MinD = std::numeric_limits<double>::infinity();
            int Closest = -1;
            int Range = 1;
            while (Closest == -1 && Range < std::max(SHWidth, SHHeight))
            {
                for (int dj = -Range; dj <= Range; ++dj)
                {
                    int SHj = SHCell.y + dj;
                    if (SHj < 0 || SHj >= SH.GetHeight())
                        continue;
                    for (int di = -Range; di <= Range; ++di)
                    {
                        int SHi = SHCell.x + di;
                        if (SHi < 0 || SHi >= SH.GetWidth())
                            continue;

                        for (int k : SH(SHi, SHj))
                        {
                            glm::vec2 Diff(Samples[k] - ij);
                            double D = glm::length(Diff);
                            if (D < MinD)
                            {
                                MinD = D;
                                Closest = k;
                            }
                        }
                    }
                }
                Range++;
            }
            G(i, j) = Closest;
        }
    }

    return G;
}

bool AngleCompare(const glm::vec2& a, const glm::vec2& b)
{
    return std::atan2(a.y, a.x) < std::atan2(b.y, b.x);
}

float Dot2D(const glm::vec2& a, const glm::vec2& b) {
    return glm::dot(a, b);
}

float Cross2D(const glm::vec2& a, const glm::vec2& b) {
    return a.x * b.y - a.y * b.x;
}

struct HalfPlane {
    glm::vec2 n;
    float c;

    float Eval(const glm::vec2& p) const {
        return Dot2D(n, p) + c;
    }
};

HalfPlane PerpBisector(const glm::vec2& pi, const glm::vec2& pj) {
    glm::vec2 mid = 0.5f * (pi + pj);
    glm::vec2 n = pj - pi;  // points from i to j

    // Normalize to improve numerical stability
    float len = glm::length(n);
    if (len > 0.0f) n /= len;

    float c = -Dot2D(n, mid);

    // Keep the half-plane containing pi
    if (Dot2D(n, pi) + c > 0.0f) {
        n = -n;
        c = -c;
    }

    return {n, c};
}

glm::vec2 SegmentLineIntersection(
    const glm::vec2& A,
    const glm::vec2& B,
    float dA,
    float dB)
{
    float denom = dB - dA;

    // Assume already checked it's not degenerate
    float t = -dA / denom;

    // Clamp for safety
    t = glm::clamp(t, 0.0f, 1.0f);

    return A + t * (B - A);
}

std::vector<glm::vec2> ClipPolygon(
    const std::vector<glm::vec2>& poly,
    const HalfPlane& hp)
{
    const float eps = 1e-6f;

    std::vector<glm::vec2> out;

    if (poly.empty()) return out;

    for (size_t i = 0; i < poly.size(); ++i) {
        glm::vec2 A = poly[i];
        glm::vec2 B = poly[(i + 1) % poly.size()];

        float dA = hp.Eval(A);
        float dB = hp.Eval(B);

        if (std::abs(dA) < eps) dA = 0.0f;
        if (std::abs(dB) < eps) dB = 0.0f;

        bool insideA = dA <= 0.0f;
        bool insideB = dB <= 0.0f;

        if (insideA && insideB) {
            // keep B
            out.push_back(B);
        }
        else if (insideA && !insideB) {
            // exiting → add intersection
            glm::vec2 I = SegmentLineIntersection(A, B, dA, dB);
            out.push_back(I);
        }
        else if (!insideA && insideB) {
            // entering → add intersection + B
            glm::vec2 I = SegmentLineIntersection(A, B, dA, dB);
            out.push_back(I);
            out.push_back(B);
        }
        // else both outside → nothing
    }

    return out;
}

std::vector<std::vector<glm::vec2>> VoronoiCompute(
    const std::vector<glm::vec2>& points,
    const std::vector<glm::vec2>& domain)
{
    int n = points.size();
    std::vector<std::vector<glm::vec2>> cells(n);

    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        std::vector<glm::vec2> cell = domain;

        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            HalfPlane hp = PerpBisector(points[i], points[j]);

            cell = ClipPolygon(cell, hp);

            if (cell.empty()) break;
        }

        cells[i] = cell;
    }

    return cells;
}

std::vector<std::vector<glm::vec2>> cescg::VoronoiPartitioning(const std::vector<glm::ivec2> &Samples, 
                                                               const glm::ivec2& BottomLeft,
                                                               const glm::ivec2& TopRight)
{
    std::vector<glm::vec2> Domain;
    Domain.emplace_back(BottomLeft.x, BottomLeft.y);
    Domain.emplace_back(BottomLeft.x, TopRight.y);
    Domain.emplace_back(TopRight.x, TopRight.y);
    Domain.emplace_back(TopRight.x, BottomLeft.y);

    std::vector<glm::vec2> FSamples;
    FSamples.resize(Samples.size());
    for (int i = 0; i < Samples.size(); ++i)
        FSamples[i] = Samples[i];

    std::vector<std::vector<glm::vec2>> Regions = VoronoiCompute(FSamples, Domain);
    for (auto& r : Regions)
    {
        glm::vec2 Centroid(0.0f);
        for (const auto& v : r)
            Centroid += v;
        Centroid /= r.size();
        std::sort(r.begin(), r.end(),
                  [Centroid](const glm::vec2& a, const glm::vec2& b) { return AngleCompare(a - Centroid, b - Centroid); });
    }
    return Regions;
}