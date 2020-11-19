#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "geometry_type.h"

typedef coord_t xy_t[2];

vertices_t dump_vertices(const int n, const coord_t xy[n][2], double angle) {
    double csita = cos(angle);
    double ssita = sin(angle);

    coord_t* seq_x = calloc(n, sizeof(seq_x[0]));
    coord_t* seq_y = calloc(n, sizeof(seq_y[0]));
    for (int i = 0; i < n; ++i) {
        seq_x[i] = csita * xy[i][0] - ssita * xy[i][1];
        seq_y[i] = ssita * xy[i][0] + csita * xy[i][1];
    }
    vertices_t vertices = vertices_create(n, seq_x, seq_y);
    free(seq_x); free(seq_y);
    return vertices;
}

vertices_t read_vertices_from(const char* filename) {
    char buf[128];
    FILE *fp = fopen(filename, "r");
    size_t count = 0;
    size_t sz = 100;
    xy_t *xy;
    xy = (xy_t*) calloc(sz, sizeof(xy[0]));

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        char *end;
        xy[count][0] = strtod(buf, &end);
        xy[count][1] = strtod(end, NULL);
        ++count;
        if (count >= sz) {
            sz += 100;
            xy = (xy_t*)realloc(xy, sz * sizeof(xy[0]));
        }
    }

    fclose(fp);

    vertices_t res = dump_vertices(count, xy, 0.0);
    free(xy);
    return res;
}

