#include "vectors.h"


// Basic arithmetic
/*void v2d_add(v2d *dest, const v2d *a, const v2d *b)
{
    dest->x = fix16_add(a->x, b->x);
    dest->y = fix16_add(a->y, b->y);
}*/

void v2d_sub(v2d *dest, const v2d *a, const v2d *b)
{
    dest->x = fix16_sub(a->x, b->x);
    dest->y = fix16_sub(a->y, b->y);
}

void v2d_mul_s(v2d *dest, const v2d *a, fix16_t b)
{
    dest->x = fix16_mul(a->x, b);
    dest->y = fix16_mul(a->y, b);
}

/*void v2d_div_s(v2d *dest, const v2d *a, fix16_t b)
{
    dest->x = fix16_div(a->x, b);
    dest->y = fix16_div(a->y, b);
}*/

// Norm
fix16_t v2d_norm(const v2d *a)
{
    return fa16_norm(&a->x, &a->y - &a->x, 2);
}

void v2d_normalize(v2d *dest, const v2d *a)
{
    v2d_div_s(dest, a, v2d_norm(a));
}

// Dot product
fix16_t v2d_dot(const v2d *a, const v2d *b)
{
    return fix16_add(fix16_mul(a->x, b->x), fix16_mul(a->y, b->y));
}

// Rotation (positive direction = counter-clockwise, angle in radians)
void v2d_rotate(v2d *dest, const v2d *a, fix16_t angle)
{
    fix16_t c = fix16_cos(angle);
    fix16_t s = fix16_sin(angle);
    
    dest->x = fix16_add(fix16_mul(c, a->x), fix16_mul(-s, a->y));
    dest->y = fix16_add(fix16_mul(s, a->x), fix16_mul(c, a->y));
}
fix16_t fa16_norm(const fix16_t *a, uint_fast8_t a_stride, uint_fast8_t n)
{
    fix16_t sum = 0;
    fix16_t max = 0;
    
    // Calculate inclusive OR of all values to find out the maximum.
    {
        uint_fast8_t i;
        const fix16_t *p = a;
        for (i = 0; i < n; i++, p += a_stride)
        {
            max |= fix16_abs(*p);
        }
    }
    
    // To avoid overflows, the values before squaring can be max 128.0,
    // i.e. v & 0xFF800000 must be 0. Also, to avoid overflow in sum,
    // we need additional log2(n) bits of space.
    int_fast8_t scale = clz(max) - 9 - ilog2(n) / 2;
    
    while (n--)
    {
        fix16_t val = scale_value(*a, scale);
        fix16_t product = fix16_mul(val, val);
        sum = fix16_add(sum, product);
        
        a += a_stride;
    }
    
    if (sum == fix16_overflow)
        return sum;
    
    fix16_t result = fix16_sqrt(sum);
    return scale_value(result, -scale);
}

uint_fast8_t ilog2(uint_fast8_t v)
{
    uint_fast8_t result = 0;
    if (v & 0xF0) { result += 4; v >>= 4; }
    while (v) { result++; v >>= 1; }
    return result;
}

uint8_t clz(uint32_t x)
{
  uint8_t result = 0;
  if (x == 0) return 32;
  while (!(x & 0xF0000000)) { result += 4; x <<= 4; }
  while (!(x & 0x80000000)) { result += 1; x <<= 1; }
  return result;
}


fix16_t scale_value(fix16_t value, int_fast8_t scale)
{
    if (scale > 0)
    {
        fix16_t temp = value << scale;
        if (temp >> scale != value)
            return fix16_overflow;
        else
            return temp;
    }
    else if (scale < 0)
    {
        return value >> -scale;
    }
    else
    {
        return value;
    }
}