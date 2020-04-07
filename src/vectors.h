#ifndef _VECTORS_H_
#define _VECTORS_H_

#include <fixmath/fix16.h>


typedef struct {
    fix16_t x;
    fix16_t y;
} v2d;

// Basic arithmetic

// Get mag
inline static fix16_t v2d_get_mag(const v2d *a)
{
  return fix16_sqrt(fix16_add(fix16_mul(a->x,a->x),fix16_mul(a->y,a->y)));
}

//void v2d_add(v2d *dest, const v2d *a, const v2d *b);

inline void v2d_add(v2d *dest, const v2d *a, const v2d *b)
{
    dest->x = fix16_add(a->x, b->x);
    dest->y = fix16_add(a->y, b->y);
}

void v2d_sub(v2d *dest, const v2d *a, const v2d *b);
void v2d_mul_s(v2d *dest, const v2d *a, fix16_t b);
//void v2d_div_s(v2d *dest, const v2d *a, fix16_t b);

inline void v2d_div_s(v2d *dest, const v2d *a, fix16_t b)
{
    dest->x = fix16_div(a->x, b);
    dest->y = fix16_div(a->y, b);
}

// Norm
fix16_t v2d_norm(const v2d *a);
void v2d_normalize(v2d *dest, const v2d *a);

// Dot product
fix16_t v2d_dot(const v2d *a, const v2d *b);

// Rotation (positive direction = counter-clockwise, angle in radians)
void v2d_rotate(v2d *dest, const v2d *a, fix16_t angle);

// Calculates the norm of a vector of size n.
fix16_t fa16_norm(const fix16_t *a, uint_fast8_t a_stride, uint_fast8_t n);
uint_fast8_t ilog2(uint_fast8_t);
uint8_t clz(uint32_t);
fix16_t scale_value(fix16_t , int_fast8_t s);

/* 3D vector operations */

typedef struct {
	fix16_t x;
	fix16_t y;
	fix16_t z;
} v3d;

// Basic arithmetic
void v3d_add(v3d *dest, const v3d *a, const v3d *b);
void v3d_sub(v3d *dest, const v3d *a, const v3d *b);
void v3d_mul_s(v3d *dest, const v3d *a, fix16_t b);
void v3d_div_s(v3d *dest, const v3d *a, fix16_t b);

// Norm
fix16_t v3d_norm(const v3d *a);
void v3d_normalize(v3d *dest, const v3d *a);

// Dot product
fix16_t v3d_dot(const v3d *a, const v3d *b);

// Cross product
void v3d_cross(v3d *dest, const v3d *a, const v3d *b);

#endif // _VECTORS_H_