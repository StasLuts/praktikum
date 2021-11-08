#pragma once

namespace geo
{
    struct Coordinates
    {
        double lat;
        double lng;
    };

    double ComputeDistance(const Coordinates& from, const Coordinates& to);

}  // namespace geo