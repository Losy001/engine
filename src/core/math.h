#pragma once

#include <core/core.h>

static constexpr float INF = __builtin_inff();
static constexpr float PI = 3.141592653589793238462643383f;
static constexpr float TAU = 6.283185307179586f;

[[clang::overloadable]]
static inline float degrees(float rad) 
{
	return rad * 180.0f / PI;
}

[[clang::overloadable]]
static inline float radians(float deg) 
{
	return deg * PI / 180.0f;
}

[[clang::overloadable]]
static inline double clamp(double d, double min, double max)
{
	const double t = d < min ? min : d;
	return t > max ? max : t;
}

[[clang::overloadable]]
static inline vec3 cross(vec3 a, vec3 b) 
{
	return (vec3){ 
		a.y * b.z - a.z * b.y, 
		a.z * b.x - a.x * b.z, 
		a.x * b.y - a.y * b.x 
	};
}

[[clang::overloadable]]
static inline float dot(vec3 a, vec3 b) 
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

[[clang::overloadable]]
static inline float dot(quat a, quat b) 
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

[[clang::overloadable]]
static inline float length(quat v) 
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

[[clang::overloadable]]
static inline float length(vec3 v) 
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

[[clang::overloadable]]
static inline float length(vec2 v) 
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

[[clang::overloadable]]
static inline vec3 normalize(vec3 v) 
{
	return v / length(v);
}

[[clang::overloadable]]
static inline vec2 normalize(vec2 v) 
{
	return v / length(v);
}

[[clang::overloadable]]
static inline quat normalize(quat v)
{
	return v / length(v);
}

[[clang::overloadable]]
static inline float distance(vec3 a, vec3 b)
{
	return length(b - a);
}

[[clang::overloadable]]
static inline vec3 rotate(vec3 v, quat q) 
{
	return v + 2.0f * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

[[clang::overloadable]]
static inline float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

[[clang::overloadable]]
static inline vec3 lerp(vec3 a, vec3 b, float t)
{
	return a + t * (b - a);
}
[[clang::overloadable]]
static inline quat lerp(quat a, quat b, float t)
{
	return a + t * (b - a);
}

[[clang::overloadable]]
static inline quat slerp(quat p, quat q, float t) 
{
	const float d = dot(p, q);
	const float absd = fabsf(d);

	constexpr float value_eps = 10.0f * FLT_EPSILON;

	if ((1.0f - absd) < value_eps) 
	{
		return normalize(lerp(d < 0 ? -p : p, q, t));
	}

	const float npq = sqrtf(dot(p, p) * dot(q, q));
	const float cos_a = clamp(absd / npq, -1.0f, 1.0f);
	const float a = acosf(cos_a);
	const float a0 = a * (1.0f - t);
	const float a1 = a * t;
	const float sina = sqrtf(1.0f - cos_a * cos_a);

	if (sina < value_eps) 
	{
		return normalize(lerp(p, q, t));
	}

	const float isina = 1.0f / sina;
	const float s0 = sinf(a0) * isina;
	const float s1 = sinf(a1) * isina;

	return normalize(s0 * p + ((d < 0.0f) ? -s1 : s1) * q);
}

// or get_percentage, basically
[[clang::overloadable]]
static inline float inv_lerp(float from, float to, float value)
{
	return (value - from) / (to - from);
}

[[clang::overloadable]]
static inline mat4 identity(void) 
{
	mat4 mat;

	mat[0][0] = 1.0f;
	mat[0][1] = 0.0f;
	mat[0][2] = 0.0f;
	mat[0][3] = 0.0f;
	
	mat[1][0] = 0.0f;
	mat[1][1] = 1.0f;
	mat[1][2] = 0.0f;
	mat[1][3] = 0.0f;

	mat[2][0] = 0.0f;
	mat[2][1] = 0.0f;
	mat[2][2] = 1.0f;
	mat[2][3] = 0.0f;

	mat[3][0] = 0.0f;
	mat[3][1] = 0.0f;
	mat[3][2] = 0.0f;
	mat[3][3] = 1.0f;

	return mat;
}

[[clang::overloadable]]
static inline mat4 rotate_x(float x) 
{
	mat4 mat = identity();

	float c = cosf(x);
	float s = sinf(x);

	mat[1][1] = c;
	mat[2][1] = s;
	mat[1][2] = -s;
	mat[2][2] = c;

	return mat;
}

[[clang::overloadable]]
static inline mat4 rotate_y(float y) 
{
	mat4 mat = identity();

	float c = cosf(y);
	float s = sinf(y);

	mat[0][0] = c;
	mat[2][0] = -s;
	mat[0][2] = s;
	mat[2][2] = c;

	return mat;
}

[[clang::overloadable]]
static inline mat4 rotate_z(float z) 
{
	mat4 mat = identity();

	float c = cosf(z);
	float s = sinf(z);

	mat[0][0] = c;
	mat[1][0] = s;
	mat[0][1] = -s;
	mat[1][1] = c;

	return mat;
}

[[clang::overloadable]]
static inline mat4 rotate(vec3 angle) 
{
	return rotate_x(angle.x) * rotate_y(angle.y) * rotate_z(angle.z);
}

[[clang::overloadable]]
static inline mat4 rotate(quat q)
{
	mat4 mat = identity();

	vec3 r2 = q.xyz + q.xyz;

	vec3 x = q.x * r2;
	vec3 y = q.y * r2;

	float zz = q.z * r2.z;
	vec3 w = q.w * r2;

	mat[0][0] = (1.0f - (y.y + zz));
	mat[1][0] = (x.y + w.z);
	mat[2][0] = (x.z - w.y);
	mat[0][1] = (x.y - w.z);
	mat[1][1] = (1.0f - (x.x + zz));
	mat[2][1] = (y.z + w.x);
	mat[0][2] = (x.z + w.y);
	mat[1][2] = (y.z - w.x);
	mat[2][2] = (1.0f - (x.x + y.y));

	return mat;
}

[[clang::overloadable]]
static inline mat4 scale(vec3 s) 
{
	mat4 mat = identity();

	mat[0][0] = s.x;
	mat[1][1] = s.y;
	mat[2][2] = s.z;

	return mat;
}

[[clang::overloadable]]
static inline mat4 translate(vec3 v) 
{
	mat4 mat = identity();

	mat[0][3] = v.x;
	mat[1][3] = v.y;
	mat[2][3] = v.z;

	return mat;
}

[[clang::overloadable]]
static inline mat4 rts(quat r, vec3 t, vec3 s) 
{
	mat4 mat;

	vec3 r2 = r.xyz + r.xyz;

	vec3 x = r.x * r2;
	vec3 y = r.y * r2;
	float zz = r.z * r2.z;
	vec3 w = r.w * r2;

	mat[0][0] = s.x * (1.0f - (y.y + zz));
	mat[1][0] = s.x * (x.y + w.z);
	mat[2][0] = s.x * (x.z - w.y);
	mat[3][0] = 0.0f;

	mat[0][1] = s.y * (x.y - w.z);
	mat[1][1] = s.y * (1.0f - (x.x + zz));
	mat[2][1] = s.y * (y.z + w.x);
	mat[3][1] = 0.0f;

	mat[0][2] = s.z * (x.z + w.y);
	mat[1][2] = s.z * (y.z - w.x);
	mat[2][2] = s.z * (1.0f - (x.x + y.y));
	mat[3][2] = 0.0f;

	mat[0][3] = t.x;
	mat[1][3] = t.y;
	mat[2][3] = t.z;
	mat[3][3] = 1.0f;

	return mat;
}

[[clang::overloadable]]
static inline mat4 transpose(mat4 mat) 
{
	return __builtin_matrix_transpose(mat);
}

[[clang::overloadable]]
static inline mat4 ortho(float left, float right, float bottom, float top, float z_near, float z_far)
{
	mat4 mat;
	
	float r = 1.0f / (z_far - z_near);
	
	mat[0][0] = 2.0f / (right - left);
	mat[1][0] = 0.0f;
	mat[2][0] = 0.0f;
	mat[3][0] = 0.0f;

	mat[0][1] = 0.0f;
	mat[1][1] = 2.0f / (top - bottom);
	mat[2][1] = 0.0f;
	mat[3][1] = 0.0f;

	mat[0][2] = 0.0f;
	mat[1][2] = 0.0f;
	mat[2][2] = r;
	mat[3][2] = 0.0f;
	
	mat[0][3] = -(right + left) / (right - left);
	mat[1][3] = -(top + bottom) / (top - bottom);
	mat[2][3] = -r * z_near;
	mat[3][3] = 1.0f;

	return mat;
}

[[clang::overloadable]]
static inline mat4 perspective(float fov, float aspect_ratio, float z_near) 
{
	float f = 1.0f / tanf(radians(fov) * 0.5f);
	
	mat4 mat;

	mat[0][0] = f / aspect_ratio;
	mat[1][0] = 0.0f;
	mat[2][0] = 0.0f;
	mat[3][0] = 0.0f;

	mat[0][1] = 0.0f;
	mat[1][1] = f;
	mat[2][1] = 0.0f;
	mat[3][1] = 0.0f;

	mat[0][2] = 0.0f;
	mat[1][2] = 0.0f;
	mat[2][2] = 0.0f;
	mat[3][2] = 1.0f;

	mat[0][3] = 0.0f;
	mat[1][3] = 0.0f;
	mat[2][3] = z_near;
	mat[3][3] = 0.0f;

	return mat;
}

[[clang::overloadable]]
static inline mat4 from_array(const float* array)
{
	mat4 mat;

	mat[0][0] = array[0 * 4 + 0];
	mat[0][1] = array[1 * 4 + 0];
	mat[0][2] = array[2 * 4 + 0];
	mat[0][3] = array[3 * 4 + 0];

	mat[1][0] = array[0 * 4 + 1];
	mat[1][1] = array[1 * 4 + 1];
	mat[1][2] = array[2 * 4 + 1];
	mat[1][3] = array[3 * 4 + 1];

	mat[2][0] = array[0 * 4 + 2];
	mat[2][1] = array[1 * 4 + 2];
	mat[2][2] = array[2 * 4 + 2];
	mat[2][3] = array[3 * 4 + 2];

	mat[3][0] = array[0 * 4 + 3];
	mat[3][1] = array[1 * 4 + 3];
	mat[3][2] = array[2 * 4 + 3];
	mat[3][3] = array[3 * 4 + 3];

	return mat;
}