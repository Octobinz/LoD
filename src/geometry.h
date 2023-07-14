#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct vector2
{
	float x;
	float y;
};

struct vector3
{
	float x;
	float y;
	float z;
};

struct vector4
{
	float x;
	float y;
	float z;
	float w;
};

struct triangle
{
	uint8_t indices[3];
};

struct model
{
	uint16_t indices[3];
};

struct matrix_3x3
{
	float m[3][3];
};

struct matrix_4x4
{
	float m[4][4];
};

typedef vector4 point_t;
typedef unsigned int IUINT32;

typedef struct { float r, g, b; } color_t;
typedef struct { float u, v; } texcoord_t;
typedef struct { point_t pos; texcoord_t tc; color_t color; float rhw; } vertex_t;

typedef struct { vertex_t v, v1, v2; } edge_t;
typedef struct { float top, bottom; edge_t left, right; } trapezoid_t;
typedef struct { vertex_t v, step; int x, y, w; } scanline_t;


int CMID(int x, int min, int max);
float interp(float x1, float x2, float t);

/*
	Vector math	
*/
float vector_length(const vector4& v);
float vector_length(const vector3& v);
float vector_length(const vector2& v);

void vertex_mul(vector2& A, float w);
void vector_add(vector2 *z, const vector2 *x, const vector2 *y);
void vector_add(vector4 *z, const vector4 *x, const vector4 *y);
void vector_sub(vector4 *z, const vector4 *x, const vector4 *y);
float vector_dotproduct(const vector4 *x, const vector4 *y);
void vector_crossproduct(vector4 *z, const vector4 *x, const vector4 *y);
void vector_interp(vector4 *z, const vector4 *x1, const vector4 *x2, float t);
void vector_normalize(vector4& v);
void vector_normalize(vector2& v);
void vertex_interp(vertex_t *y, const vertex_t *x1, const vertex_t *x2, float t);
void vertex_division(vertex_t *y, const vertex_t *x1, const vertex_t *x2, float w);
void vertex_add(vertex_t *y, const vertex_t *x);

/*
	Matrix math	
*/
void matrix_add(matrix_4x4 *c, const matrix_4x4 *a, const matrix_4x4 *b);
void matrix_sub(matrix_4x4 *c, const matrix_4x4 *a, const matrix_4x4 *b);
void matrix_mul(matrix_4x4 *c, const matrix_4x4 *a, const matrix_4x4 *b);
void matrix_scale(matrix_4x4 *c, const matrix_4x4 *a, float f);
void matrix_apply(vector4 *y, const vector4 *x, const matrix_4x4 *m);
void matrix_set_identity(matrix_4x4 *m);
void matrix_set_zero(matrix_4x4 *m);
void matrix_set_translate(matrix_4x4 *m, float x, float y, float z);
void matrix_set_scale(matrix_4x4 *m, float x, float y, float z);
void matrix_set_rotate(matrix_4x4 *m, float x, float y, float z, float theta);
void matrix_set_lookat(matrix_4x4 *m, const vector4 *eye, const vector4 *at, const vector4 *up);
void matrix_set_perspective(matrix_4x4 *m, float fovy, float aspect, float zn, float zf);

void trapezoid_edge_interp(trapezoid_t *trap, float y);
void trapezoid_init_scan_line(const trapezoid_t *trap, scanline_t *scanline, int y);

template <typename T> __forceinline  int sgn(T val) 
{
	return (T(0) < val) - (val < T(0));
}