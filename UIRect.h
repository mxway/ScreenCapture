#ifndef SCREENCAPTURE_UIRECT_H
#define SCREENCAPTURE_UIRECT_H
#include <stdio.h>
#include <stdlib.h>

typedef struct _UIRect{
    int x;
    int y;
    int width;
    int height;
} UIRect;



// Union of two Rectangle.
UIRect rect_union(const UIRect *rect1, const UIRect *rect2) ;

// Intersection of two Rectangle.
UIRect rect_intersection(const UIRect *rect1, const UIRect *rect2);

// difference set of two Rectangle
void rect_difference(const UIRect *rect1, const UIRect *rect2, UIRect *result, int *count);

#endif //SCREENCAPTURE_UIRECT_H
