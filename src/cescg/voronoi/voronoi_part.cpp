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

std::vector<std::vector<glm::vec2>> cescg::VoronoiPartitioning(const std::vector<glm::ivec2> &Samples, 
                                                               const glm::ivec2& BottomLeft,
                                                               const glm::ivec2& TopRight)
{
    // Init each region to the whole bounding box
    std::vector<std::vector<glm::vec2>> Regions;
    Regions.resize(Samples.size());
    for (auto& r : Regions)
    {
        r.emplace_back(BottomLeft);
        r.emplace_back(BottomLeft.x, TopRight.y);
        r.emplace_back(TopRight);
        r.emplace_back(TopRight.x, BottomLeft.y);
    }

    std::vector<glm::vec2> FSamples;
    FSamples.resize(Samples.size());
    for (int i = 0; i < Samples.size(); ++i)
        FSamples[i] = Samples[i];

    // Execute region cuts in parallel
    #pragma omp parallel for
    for (int i = 0; i < Samples.size(); ++i)
    {
        // For each other sample, find perpendicular bisector and cut
        for (int j = 0; j < Samples.size(); ++j)
        {
            if (i == j)
                continue;

            // Find midpoint
            glm::vec2 Midpoint = 0.5f * (FSamples[i] + FSamples[j]);
            // Find the direction of the perpendicular bisector
            glm::vec2 Direction(-(FSamples[j].y - FSamples[i].y), FSamples[j].x - FSamples[i].x);
            Direction = glm::normalize(Direction);
            // Find the two segments that intersect the perpendicular bisector
            std::vector<std::pair<int, glm::vec2>> Intersections;
            for (int k0 = 0; k0 < Regions[i].size(); ++k0)
            {
                int k1 = (k0 + 1) % Regions[i].size();
                glm::vec2 v = Regions[i][k0];
                glm::vec2 e = Regions[i][k1] - v;
                // Intersection at v + t * e = Midpoint + s * Direction
                float det = -e.x * Direction.y + e.y * Direction.x;
                if (std::abs(det) < 1e-6) // Parallel vectors
                    continue;
                float m11 = -Direction.y / det;
                float m12 = Direction.x / det;
                // float m21 = -e.y / det; // We don't need s
                // float m22 = e.x / det; // We don't need s
                float t = m11 * (Midpoint.x - v.x) + m12 * (Midpoint.y - v.y);
                // t must be in [0, 1]
                if (t < 1e-6 || t > 1 - 1e-6)
                    continue;
                Intersections.emplace_back(k0, v + t * e);
            }
            // If no intersections, skip
            if (Intersections.size() < 2)
                continue;
            // Boundary polygon is oriented clockwise
            if (glm::length(Regions[i][Intersections[0].first] - FSamples[j]) < glm::length(Regions[i][Intersections[0].first] - FSamples[i]))
            {
                // If the first vertex of the first intersecting segment is closer to j,
                // then we take from first to second and connect the intersections.
                std::vector<glm::vec2> NewReg;
                for (int k = (Intersections[0].first + 1) % Regions[i].size(); k != (Intersections[1].first + 1) % Regions[i].size(); k = (k + 1) % Regions[i].size())
                    NewReg.emplace_back(Regions[i][k]);
                NewReg.emplace_back(glm::round(Intersections[1].second));
                NewReg.emplace_back(glm::round(Intersections[0].second));
                Regions[i] = NewReg;
            }
            else
            {
                // Otherwise, we take from second to first and connect the intersections
                std::vector<glm::vec2> NewReg;
                for (int k = (Intersections[1].first + 1) % Regions[i].size(); k != (Intersections[0].first + 1) % Regions[i].size(); k = (k + 1) % Regions[i].size())
                    NewReg.emplace_back(Regions[i][k]);
                NewReg.emplace_back(glm::round(Intersections[0].second));
                NewReg.emplace_back(glm::round(Intersections[1].second));
                Regions[i] = NewReg;
            }
        }

    //     glm::vec2 Centroid(0.0f);
    //     for (const auto& v : Regions[i])
    //         Centroid += v;
    //     Centroid /= Regions[i].size();

    //     std::sort(Regions[i].begin(), Regions[i].end(), [Centroid](const glm::vec2& v1, const glm::vec2& v2) { return AngleCompare(v2 - Centroid, v1 - Centroid); });
    }

    return Regions;
}