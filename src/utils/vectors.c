#include <math.h>
#include <stdbool.h>

#include "utils.h"

float perpendicularDistance(DPPoint p, DPPoint a, DPPoint b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;

    if (dx == 0.0f && dy == 0.0f)
        return hypotf(p.x - a.x, p.y - a.y);

    return fabsf(
        dy * p.x -
        dx * p.y +
        b.x * a.y -
        b.y * a.x
    ) / sqrtf(dx * dx + dy * dy);
}

void douglasPeucker(
    DPPoint *points,
    int start,
    int end,
    float epsilon
) {
    float dmax = 0.0f;
    int index = -1;

    for (int i = start + 1; i < end; i++) {
        float d = perpendicularDistance(
            points[i],
            points[start],
            points[end]
        );

        if (d > dmax) {
            dmax = d;
            index = i;
        }
    }

    if (dmax > epsilon && index != -1) {
        points[index].valid = true;

        douglasPeucker(
            points,
            start,
            index,
            epsilon
        );

        douglasPeucker(
            points,
            index,
            end,
            epsilon
        );
    }
}