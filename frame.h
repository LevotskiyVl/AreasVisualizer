#ifndef _FRAME_H_
#define _FRAME_H_

/*
 *  This module defines a frame object and an all necessery
 *  utilities to work with. A frame is generally used to point
 *  to a certain area on image.
 */


#include "gip_config.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

/*
 *  Point at image.
 *      x - stands for column
 *      y - stands for row
 */
struct gip_point {
    size_t x;
    size_t y;
};


/*
 *  Rectangle geometry:
 *      h - height
 *      w - width
 */
struct gip_rect {
    size_t h;
    size_t w;
};

struct frames_filter_settings {
    size_t   min_height;
    size_t   min_width;
    size_t   max_height;
    size_t   max_width;
    size_t   num_of_symbls;
    size_t   img_height;
    size_t   img_width;
    //float    geom;
    uint8_t* img;
    bool     filters_on;
};
// Method for accessing rectangle's square
#define gip_rect_square(rect) ((rect)->h * (rect)->w)


/*
 *  Area at image:
 *      uplc - upper left corner
 *      rect - geometry
*/
struct gip_area {
    struct gip_point uplc; // upper left corner
    struct gip_rect  rect;
};

/*
 *  Area methods. Each method takes a pointer to area object.
 */

// Method to create an are with upper left corner at (0,0) and given geometry.
#define gip_area_wrap(area, height, width)\
    {\
        (area)->uplc.x = 0;\
        (area)->uplc.y = 0;\
        (area)->rect.h = height;\
        (area)->rect.w = width;\
    }


// Methods for accessing general geometry properties.
#define gip_area_height(area) ((area)->rect.h)
#define gip_area_width(area)  ((area)->rect.w)
#define gip_area_square(area) gip_rect_square(&((area)->rect))


// Methods for accessing area's coordinates:

// area's top line
#define gip_area_top(area)    ((area)->uplc.y)

// area's bottom line
#define gip_area_bottom(area) (gip_area_top(area) + gip_area_height(area) - 1)

// area's left column
#define gip_area_left(area)   ((area)->uplc.x)

// area's right column
#define gip_area_right(area)  (gip_area_left(area) + gip_area_width(area) - 1)

/*
 *  Frame object.
 *      size - number of non background area's pixels
 *      area - frame's area.
 *
 *  Frame is an area on image with knowledge about
 *  it's contents (size field).
 */
struct gip_frame {
    size_t size;          // number of non-background pixels
    size_t num_of_symbls;
    uint8_t symbls[15];
    uint8_t weights[15];
    struct gip_area area; // frame's area
};

/*
 *  Array of frames.
 *      size  - number of frames in array
 *      begin - beginning of an raw frame array
 */
struct gip_frame_array {
    size_t size;
    struct gip_frame* begin;
};

/*
 *  Transitions is an auxiliary object which holds
 *  information about frame's transitions.
 *      horizontal - number of horizontal transitions
 *      vertical   - number of vertical transitions
 */
struct gip_transitions {
    size_t horizontal;
    size_t vertical;
};

/*
 *  Type of function to filter a frame. It is a
 *  boolean predicate which takes an additional
 *  pointer to external data.
 */
typedef bool (*gip_frame_filter)(struct gip_frame const*, void const*);

/*
 *  Parameters of frame search.
 *      cuda_filter_height - height of cuda's filter
 *      cuda_filter_width  - width  of cuda's filter
 *      graythresh_delta   - delta value for evaluating binarization threshold
 *      graythresh_mult    - multiplier for graythresh_delta (for leveling binarization)
 *      max_hole_size      - maximum size of hole which will be removed from white background
 */
struct gip_frame_search_params {
    size_t cuda_filter_height;
    size_t cuda_filter_width;
    double graythresh_delta;
    double graythresh_mult;
    size_t max_hole_size;
    int search_method;
};

#ifdef __cplusplus
extern "C" {
#endif

    // Checks whether first area contains a second one
    DLLEXPORT
        bool gip_area_contains
            (struct gip_area const* this_area
            ,struct gip_area const* that_area);

    // Creates a frame array of certain size
    DLLEXPORT
        struct gip_frame_array* gip_create_frame_array(size_t const size);

    // Deletes a frame array
    DLLEXPORT
        void gip_delete_frame_array(struct gip_frame_array* frames);

    // Draws an area on image.
    /*
     *  area  - where to draw a frame (not a gip_frame)
     *  color - what color to use (from 0 to 255)
     *  width - frame's width
     */
    DLLEXPORT
        bool gip_draw_area
            (uint8_t* img
            ,size_t const img_h
            ,size_t const img_w
            ,struct gip_area const* area
            ,uint8_t const color
            ,size_t const width);

    // Counts area's transitions.
    /*
     *  Result will be written to out parameter.
     *  So it shouldn't be null pointer.
     *  Count_transaction - for binarize image
     *  count_transaction2 - for greyskale image
     */
    DLLEXPORT
        void gip_count_transitions
            (uint8_t const* img
            ,size_t const img_h
            ,size_t const img_w
            ,struct gip_area const* area
            ,struct gip_transitions* out);
    DLLEXPORT
        void gip_count_transitions2
            (uint8_t * img
            ,size_t const img_h
            ,size_t const img_w
            ,struct gip_area const* area
            ,struct gip_transitions* out);

    // Collects all frames from given black-white image.
    // (does not perform any image processing operations).
    DLLEXPORT
        struct gip_frame_array* gip_collect_frames
            (uint8_t* img
            ,size_t const img_h
            ,size_t const img_w
            ,int method);

    // Filters frame array using given filter function.
    DLLEXPORT
        struct gip_frame_array* gip_filter_frames
            (bool (*pred)(struct gip_frame const*, void const*)
            ,void const* pred_data
            ,struct gip_frame_array const* frames);

    // Returns frame array collected from given image (any grayscale)
    // applying image processing operations.
    DLLEXPORT
        struct gip_frame_array* gip_search_frames
            (uint8_t * img
            , size_t const img_h
            , size_t const img_w
            , struct gip_frame_search_params const* search_data);

#ifdef __cplusplus
}
#endif


#endif//_FRAME_H_
