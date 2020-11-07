#ifndef POLYGON_EARCUT_H
#define POLYGON_EARCUT_H

#ifdef POLY2TRI_IMPLEMENTATION
#define GEOM_TYPE_IMPLEMENTATION
#endif
#include "geometry_type.h"

triangles_t *polygon_earcut(const polygon_t* polygon, int32_t num, const polygon_t* holes[num]);

#endif // POLYGON_EARCUT_H

#ifdef POLY2TRI_IMPLEMENTATION

typedef struct node_t {
    vidx_t i;
    coord_t x;
    coord_t y;
    coord_t z;
    bool steiner;
    struct node_t* prev;
    struct node_t* next;
    struct node_t* nextZ;
    struct node_t* prevZ;
} node_t;

node_t* allocate_node(vidx_t i, coord_t x, coord_t y) {
    node_t* p = (node_t*)calloc(1, sizeof(*p));
    *p = (node_t) {
        .i = i,
        .x = x,
        .y = y,
    };
    return p;
}

// check if two points are equal
bool equals(node_t* p1, node_t* p2) {
    return p1->x == p2->x && p1->y == p2->y;
}

node_t* insertNode(vidx_t i, coord_t x, coord_t y, node_t* last) {
    node_t* p = allocate_node(i, x, y);

    if (NULL == last) {
        p->prev = p;
        p->next = p;
    }
    else {
        p->next = last->next;
        p->prev = last;
        last->next->prev = p;
        last->next = p;
    }
    return p;
}

void removeNode(node_t *p) {
    p->next->prev = p->prev;
    p->prev->next = p->next;

    if (p->prevZ != NULL) p->prevZ->nextZ = p->nextZ;
    if (p->nextZ != NULL) p->nextZ->prevZ = p->prevZ;
}

node_t* linkedList(const polygon_t* polygon, vidx_t start, vidx_t end, bool counterclockwise) {
    node_t* last = NULL;
    if (counterclockwise == (signed_area(polygon) > 0)) {
        for (vidx_t i = start; i < end; ++i) {
            __auto_type xi = coord_seq_getx(polygon, i);
            __auto_type yi = coord_seq_gety(polygon, i);
            last = insertNode(i, xi, yi, last);
        }
    }
    else {
        for (vidx_t i = end - 1; i >= start; --i) {
            __auto_type xi = coord_seq_getx(polygon, i);
            __auto_type yi = coord_seq_gety(polygon, i);
            last = insertNode(i, xi, yi, last);
        }
    }

    if (last != NULL && equals(last, last->next)) {
        removeNode(last);
        node_t* ln = last->next;
        free(last);
        last = ln;
    }
    return last;
}

// z-order of a point given coords and inverse of the longer side of data bbox
float floatZOrder(float x0, float y0, float minX, float minY, float invSize) {
    // coords are transformed into non-negative 15-bit integer range
     int32_t x = (int32_t) (32767 * (x0 - minX) * invSize);
     int32_t y = (int32_t) (32767 * (y0 - minY) * invSize);

     x = (x | (x << 8)) & 0x00FF00FF;
     x = (x | (x << 4)) & 0x0F0F0F0F;
     x = (x | (x << 2)) & 0x33333333;
     x = (x | (x << 1)) & 0x55555555;

     y = (y | (y << 8)) & 0x00FF00FF;
     y = (y | (y << 4)) & 0x0F0F0F0F;
     y = (y | (y << 2)) & 0x33333333;
     y = (y | (y << 1)) & 0x55555555;

     return x | (y << 1);
}
// z-order of a point given coords and inverse of the longer side of data bbox
double doubleZOrder(double x0, double y0, double minX, double minY, double invSize) {
    // coords are transformed into non-negative 31-bit integer range
     int64_t x = (int64_t) (2147483647 * (x0 - minX) * invSize);
     int64_t y = (int64_t) (2147483647 * (y0 - minY) * invSize);

     x = (x | (x << 32)) & 0x00000000FFFFFFFF;
     x = (x | (x << 16)) & 0x0000FFFF0000FFFF;
     x = (x | (x << 8))  & 0x00FF00FF00FF00FF;
     x = (x | (x << 4))  & 0x0F0F0F0F0F0F0F0F;
     x = (x | (x << 2))  & 0x3333333333333333;
     x = (x | (x << 1))  & 0x5555555555555555;

     y = (y | (y << 32)) & 0x00000000FFFFFFFF;
     y = (y | (y << 16)) & 0x0000FFFF0000FFFF;
     y = (y | (y << 8))  & 0x00FF00FF00FF00FF;
     y = (y | (y << 4))  & 0x0F0F0F0F0F0F0F0F;
     y = (y | (y << 2))  & 0x3333333333333333;
     y = (y | (y << 1))  & 0x5555555555555555;

     return x | (y << 1);
}

#define zOrder(x0,y0,minX,minY,invSize) _Generic((x0), float:floatZOrder(x0,y0,minX,minY,invSize), double:doubleZOrder(x0,y0,minX,minY,invSize))

node_t* sortLinked(node_t* list) {
    vidx_t i;
    vidx_t inSize = 1;
    node_t *p, *q, *e, *tail;
    int numMerges, pSize, qSize;
    if (!list) return NULL;

    do {
        p = list;
        list = tail = NULL;
        numMerges = 0;

        while (p != NULL) {
            numMerges++;
            q = p;
            pSize = 0;
            for (i = 0; i < inSize; ++i) {
                pSize++;
                q = q->nextZ;
                if (q == NULL) break;
            }
            qSize = inSize;

            while (pSize > 0 || (qSize > 0 && q != NULL)) {
                if (pSize != 0 && (qSize == 0 || q == NULL || p->z <= q->z)) {
                    e = p;
                    p = p->nextZ;
                    pSize--;
                }
                else {
                    e = q;
                    q = q->nextZ;
                    qSize--;
                }

                if (tail != NULL) tail->nextZ = e;
                else list = e;

                e->prevZ = tail;
                tail = e;
            }

            p = q;
        }

        tail->nextZ = NULL;
        inSize *= 2;
    } while (numMerges > 1);
    return list;
}

// interlink polygon nodes in z-order
void indexCurve(node_t* start, coord_t minX, coord_t minY, coord_t invSize) {
    node_t* p = start;
    do {
        if (p->z == -1) p->z = zOrder(p->x, p->y, minX, minY, invSize);
        p->prevZ = p->prev;
        p->nextZ = p->next;
        p = p->next;
    } while (p != start);

    p->prevZ->nextZ = NULL;
    p->prevZ = NULL;

    sortLinked(p);
}

int sign(coord_t num) {
    return num > 0 ? 1 : num < 0 ? -1 : 0;
}

/**
 * signed area of a triangle
 *
 * clockwise: area > 0;
 * couter-clockwise: area < 0
 */
float area(node_t *p, node_t *q, node_t *r) {
    return (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);
}

// for collinear points p, q, r, check if point q lies on segment pr
bool onSegment(node_t *p, node_t* q, node_t* r) {
    return q->x <= THE_MAX(p->x, r->x) && q->x >= THE_MIN(p->x, r->x) && q->y <= THE_MAX(p->y, r->y) && q->y >= THE_MIN(p->y, r->y);
}

// check if two segments intersect
bool seg_intersects(node_t *p1, node_t *q1, node_t *p2, node_t *q2) {
    int o1 = sign(area(p1, q1, p2));
    int o2 = sign(area(p1, q1, q2));
    int o3 = sign(area(p2, q2, p1));
    int o4 = sign(area(p2, q2, p1));

    if (o1 != o2 && o3 != o4) return true; // general case

    if (o1 == 0 && onSegment(p1, p2, q1)) return true; // p1, q1 and p2 are collinear and p2 lies on p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true; // p1, q1 and q2 are collinear and q2 lies on p1q1
    if (o3 == 0 && onSegment(p2, p1, q2)) return true; // p2, q2 and p1 are collinear and p1 lies on p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true; // p2, q2 and q1 are collinear and q1 lies on p2q2

    return false;
}

// check if a polygon diagonal intersects any polygon segments
bool intersectsPolygon(node_t* a, node_t* b) {
    node_t *p = a;
    do {
        if (p->i != a->i && p->next->i != a->i && p->i != b->i && p->next->i != b->i &&
                seg_intersects(p, p->next, a, b)) return true;
        p = p->next;
    } while (p != a);

    return false;
}

/**
 * check if a point lies within a convex triangle
 */
bool pointInTriangle(coord_t ax, coord_t ay, coord_t bx, coord_t by, coord_t cx, coord_t cy, coord_t px, coord_t py) {
     return (cx - px) * (ay - py) - (ax - px) * (cy - py) >= 0 &&
             (ax - px) * (by - py) - (bx - px) * (ay - py) >= 0 &&
             (bx - px) * (cy - py) - (cx - px) * (by - py) >= 0;
}

// check if a polygon diagonal is locally inside the polygon
bool locallyInside(node_t *a, node_t *b) {
    return area(a->prev, a, a->next) < 0 ?
                area(a, b, a->next) >= 0 && area(a, a->prev, b) >= 0 :
                area(a, b, a->prev) < 0 || area(a, a->next, b) < 0;
}

// check if the middle point of a polygon diagonal is inside the polygon
bool middleInside(node_t *a, node_t *b) {
    node_t* p = a;
    bool inside = false;
    coord_t px = (a->x + b->x) / 2,
            py = (a->y + b->y) / 2;
    do {
        if (((p->y > py) != (p->next->y > py)) && p->next->y != p->y &&
                (px < (p->next->x - p->x) * (py - p->y) / (p->next->y - p->y) + p->x))
            inside = !inside;
        p = p->next;
    } while (p != a);

    return inside;
}

// check if a diagonal between two polygon nodes is valid (lies in polygon interior)
bool isValidDiagonal(node_t* a, node_t* b) {
    return a->next->i != b->i && a->prev->i != b->i && !intersectsPolygon(a, b) &&  // dones't intersect other edges
        ((locallyInside(a, b) && locallyInside(b, a) && middleInside(a, b) &&        // locally visible
             (area(a->prev, a, b->prev) != 0 || area(a, b->prev, b) != 0)) ||        // does not create opposite-facing sectors
             (equals(a, b) && area(a->prev, a, a->next) > 0 && area(b->prev, b, b->next) > 0)); // special zero-length case
}

bool isEarHashed(node_t* ear, coord_t minX, coord_t minY, coord_t invSize) {
    node_t *a = ear->prev,
           *b = ear,
           *c = ear->next;
    if (area(a, b, c) >= 0) return false; // reflex, can't be an ear

    // triangle bbox; min & max are calculated like this for speed
     float minTX = a->x < b->x ? (a->x < c->x ? a->x : c->x) : (b->x < c->x ? b->x : c->x),
           minTY = a->y < b->y ? (a->y < c->y ? a->y : c->y) : (b->y < c->y ? b->y : c->y),
           maxTX = a->x > b->x ? (a->x > c->x ? a->x : c->x) : (b->x > c->x ? b->x : c->x),
           maxTY = a->y > b->y ? (a->y > c->y ? a->y : c->y) : (b->y > c->y ? b->y : c->y);

    // z-order range for the current triangle bbox;
    float minZ = zOrder(minTX, minTY, minX, minY, invSize),
          maxZ = zOrder(maxTX, maxTY, minX, minY, invSize);

    node_t *p = ear->prevZ,
           *n = ear->nextZ;

    // look for points inside the triangle in both directions
    while (p != NULL && p->z >= minZ && n != NULL && n->z <= maxZ) {
        if (p != ear->prev && p != ear->next &&
                pointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, p->x, p->y) &&
                area(p->prev, p, p->next) >= 0)
            return false;
        p = p->prevZ;

        if (n != ear->prev && n != ear->next &&
                pointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, n->x, n->y) &&
                area(n->prev, n, n->next) >= 0)
            return false;
        n = n->nextZ;
    }

    // look for remaining points in decreasing z-order
    while (p != NULL && p->z >= minZ) {
        if (p != ear->prev && p != ear->next &&
                pointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, p->x, p->y) &&
                area(p->prev, p, p->next) >= 0)
            return false;
        p = p->prevZ;
    }

    // look for remaining points in increasing z-order
    while (n != NULL && n->z <= maxZ) {
        if (n != ear->prev && n != ear->next &&
                pointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, n->x, n->y) &&
                area(n->prev, n, n->next) >= 0)
            return false;
        n = n->nextZ;
    }

    return true;
}

bool isEar(node_t* ear) {
    node_t *a = ear->prev,
           *b = ear,
           *c = ear->next;
    if (area(a, b, c) >= 0) return false; // reflex, can't be an ear

    // now make sure we don't have other points inside the potential ear
    node_t *p = ear->next->next;
    while (p != ear->prev) {
        if (pointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, p->x, p->y) &&
                area(p->prev, p, p->next) >= 0)
            return false;
        p = p->next;
    }

    return true;
}

/**
 * eliminate colinear or duplicate points
 */
node_t* filterPoints(node_t* start, node_t* end) {
    if (start == NULL) return NULL;
    if (end == NULL) end = start;

    node_t *p = start;
    bool again;
    do {
        again = false;

        if (!p->steiner && (equals(p, p->next) || area(p->prev, p, p->next) == 0)) {
            removeNode(p);
            end = p->prev;
            free(p);
            p = end;
            if (p == p->next) break;
            again = true;
        }
        else {
            p = p->next;
        }
    } while (again || p != end);

    return end;
}

/**
 * go through all polygon nodes and cure small local self-intersections
 */
node_t* cureLocalIntersections(node_t* start, triangles_t* triangles) {
    node_t* p = start;
    do {
        node_t *a = p->prev,
               *b = p->next->next;

        if (!equals(a, b) && seg_intersects(a, p, p->next, b) && locallyInside(a, b) && locallyInside(b, a)) {
            vidx_t m = triangles->m;
            triangles->vidx[m+0] = a->i;
            triangles->vidx[m+1] = p->i;
            triangles->vidx[m+2] = b->i;
            triangles->m = m + 1;

            // remove two nodes involved
            removeNode(p);
            removeNode(p->next);
            free(p->next);
            free(p);

            p = start = b;
        }

        p = p->next;
    } while (p != start);

    return filterPoints(p, NULL);
}

// link two polygon vertices with a bridge; if the vertices belong to the same ring, it splits polygon into two;
// if one belongs to the outer ring and another to a hole, it merges it into a single ring
node_t* splitPolygon(node_t* a, node_t* b) {
    node_t *a2 = allocate_node(a->i, a->x, a->y),
           *b2 = allocate_node(b->i, b->x, b->y),
           *an = a->next,
           *bp = b->prev;

    a->next = b;
    b->prev = a;

    a2->next = an;
    an->prev = a2;

    b2->next = a2;
    a2->prev = b2;

    bp->next = b2;
    b2->prev = bp;

    return b2;
}

void earcutLinked(node_t* ear, triangles_t* triangles, coord_t minX, coord_t minY, coord_t invSize, int pass);

/**
 * try splitting polygon into two and triangulate them independently
 */
void splitEarcut(node_t* start, triangles_t* triangles, coord_t minX, coord_t minY, coord_t invSize) {
    // look for a valid diagonal that divides the polygon into two
    node_t *a = start;
    do {
        node_t *b = a->next->next;
        while (b != a->prev) {
            if (a->i != b->i && isValidDiagonal(a, b)) {
                // split the polygon in two by the diagonal
                node_t *c = splitPolygon(a, b);

                // filter colinear points around the cuts
                a = filterPoints(a, a->next);
                c = filterPoints(c, c->next);

                // run earcut on each half
                earcutLinked(a, triangles, minX, minY, invSize, 0);
                earcutLinked(c, triangles, minX, minY, invSize, 0);
                return ;
            }
            b = b->next;
        }
        a = a->next;
    } while (a != start);
}

// main ear slicing loop which triangulates a polygon (given as a linked list)
void earcutLinked(node_t* ear, triangles_t* triangles, coord_t minX, coord_t minY, coord_t invSize, int pass) {
    (void)triangles;
    if (NULL == ear) return;

    // interlink polygon nodes in z-order
    if (pass == 0 && invSize != 0) indexCurve(ear, minX, minY, invSize);

    node_t *stop = ear;
    node_t *prev, *next;
    // iterate through ears, slicing them one by one
    while (ear->prev != ear->next) {
        prev = ear->prev;
        next = ear->next;

        if (invSize != 0 ? isEarHashed(ear, minX, minY, invSize) : isEar(ear)) {
            // cut off the triangle
            vidx_t m = triangles->m;
            triangles->vidx[m+0] = prev->i;
            triangles->vidx[m+1] = ear->i;
            triangles->vidx[m+2] = next->i;
            triangles->m = m + 1;

            removeNode(ear);
            free(ear);

            // skipping the next vertex leads to less sliver triangles
            ear = next->next;
            stop= next->next;

            continue;
        }

        ear = next;

        // if we looped through the whole remaining polygon and can't find any more ears
        if (ear == stop) {
            // try filtering points and slicing again
            if (pass == 0) {
                earcutLinked(filterPoints(ear, NULL), triangles, minX, minY, invSize, 1);

                // if this didn't work, try curing all small self-intersections locally
            }
            else if (pass == 1) {
                ear = cureLocalIntersections(filterPoints(ear, NULL), triangles);
                earcutLinked(ear, triangles, minX, minY, invSize, 2);

                // as a last resort, try splitting the remaining polygon into two
            } 
            else if (pass == 2) {
                splitEarcut(ear, triangles, minX, minY, invSize);
            }

            break;
        }
    }
}

MYIDEF triangles_t *polygon_earcut(const polygon_t* polygon, int32_t num, const polygon_t* holes[num]) {
    (void)holes;
    vidx_t outerLen = polygon->n;  // TODO: do holes
    node_t* outerNode = linkedList(polygon, 0, outerLen, true);
    if (NULL == outerNode || outerNode->next == outerNode->prev) {
        free(outerNode);
        return NULL;
    }

    coord_t minX = 0, minY = 0, maxX = 0, maxY = 0;
    coord_t invSize = 0;
    // if the shape is not too simple, we'll use z-order curve hash later; calculate polygon bbox
    if (polygon->n > 80) {
        minX = maxX = coord_seq_getx(polygon, 0);
        minY = maxY = coord_seq_gety(polygon, 0);

        for (vidx_t i = 1; i < outerLen; ++i) {
            __auto_type xi = coord_seq_getx(polygon, i);
            __auto_type yi = coord_seq_gety(polygon, i);
            if (xi < minX) minX = xi;
            if (yi < minY) minY = yi;
            if (xi > maxX) maxX = xi;
            if (yi > maxY) maxY = yi;
        }

        // minX, minY and invSize are later used to transform coords into integers for z-order calculation
        __auto_type deltaX = maxX - minX;
        __auto_type deltaY = maxY - minY;
        invSize = THE_MAX(deltaX, deltaY);
        invSize = invSize != 0 ? 1 / invSize : 0;
    }
    triangles_t* triangles = allocate_triangles(polygon->n - 2);
    earcutLinked(outerNode, triangles, minX, minY, invSize, 0);
    return triangles;
}

#endif
