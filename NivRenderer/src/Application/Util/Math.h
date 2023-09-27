#pragma once
#include "Base.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Math
{
    struct matrix_t;

    struct vec_t
    {
    public:
        float x, y, z, w;

        void Lerp(const vec_t& v, float t)
        {
            x += (v.x - x) * t;
            y += (v.y - y) * t;
            z += (v.z - z) * t;
            w += (v.w - w) * t;
        }

        void Set(float v) { x = y = z = w = v; }
        void Set(float _x, float _y, float _z = 0.f, float _w = 0.f)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
        }

        vec_t& operator-=(const vec_t& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }
        vec_t& operator+=(const vec_t& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }
        vec_t& operator*=(const vec_t& v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
            return *this;
        }
        vec_t& operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            w *= v;
            return *this;
        }

        vec_t operator*(float f) const;
        vec_t operator-() const;
        vec_t operator-(const vec_t& v) const;
        vec_t operator+(const vec_t& v) const;
        vec_t operator*(const vec_t& v) const;

        const vec_t& operator+() const { return (*this); }
        float Length() const { return sqrtf(x * x + y * y + z * z); };
        float LengthSq() const { return (x * x + y * y + z * z); };
        vec_t Normalize()
        {
            (*this) *= (1.f / (Length() > FLT_EPSILON ? Length() : FLT_EPSILON));
            return (*this);
        }
        vec_t Normalize(const vec_t& v)
        {
            this->Set(v.x, v.y, v.z, v.w);
            this->Normalize();
            return (*this);
        }
        vec_t Abs() const;

        void Cross(const vec_t& v)
        {
            vec_t res;
            res.x = y * v.z - z * v.y;
            res.y = z * v.x - x * v.z;
            res.z = x * v.y - y * v.x;

            x = res.x;
            y = res.y;
            z = res.z;
            w = 0.f;
        }

        void Cross(const vec_t& v1, const vec_t& v2)
        {
            x = v1.y * v2.z - v1.z * v2.y;
            y = v1.z * v2.x - v1.x * v2.z;
            z = v1.x * v2.y - v1.y * v2.x;
            w = 0.f;
        }

        float Dot(const vec_t& v) const { return (x * v.x) + (y * v.y) + (z * v.z) + (w * v.w); }

        float Dot3(const vec_t& v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

        void Transform(const matrix_t& matrix);
        void Transform(const vec_t& s, const matrix_t& matrix);

        void TransformVector(const matrix_t& matrix);
        void TransformPoint(const matrix_t& matrix);
        void TransformVector(const vec_t& v, const matrix_t& matrix)
        {
            (*this) = v;
            this->TransformVector(matrix);
        }
        void TransformPoint(const vec_t& v, const matrix_t& matrix)
        {
            (*this) = v;
            this->TransformPoint(matrix);
        }

        float& operator[](size_t index) { return ((float*)&x)[index]; }
        const float& operator[](size_t index) const { return ((float*)&x)[index]; }
        bool operator!=(const vec_t& other) const { return memcmp(this, &other, sizeof(vec_t)) != 0; }
    };

    inline vec_t makeVect(float _x, float _y, float _z = 0.f, float _w = 0.f)
    {
        vec_t res;
        res.x = _x;
        res.y = _y;
        res.z = _z;
        res.w = _w;
        return res;
    }

    struct matrix_t
    {
    public:
        union
        {
            float m[4][4];
            float m16[16];
            struct
            {
                vec_t right, up, dir, position;
            } v;
            vec_t component[4];
        };

        operator float*() { return m16; }
        operator const float*() const { return m16; }
        void Translation(float _x, float _y, float _z) { this->Translation(makeVect(_x, _y, _z)); }

        void Translation(const vec_t& vt)
        {
            v.right.Set(1.f, 0.f, 0.f, 0.f);
            v.up.Set(0.f, 1.f, 0.f, 0.f);
            v.dir.Set(0.f, 0.f, 1.f, 0.f);
            v.position.Set(vt.x, vt.y, vt.z, 1.f);
        }

        void Scale(float _x, float _y, float _z)
        {
            v.right.Set(_x, 0.f, 0.f, 0.f);
            v.up.Set(0.f, _y, 0.f, 0.f);
            v.dir.Set(0.f, 0.f, _z, 0.f);
            v.position.Set(0.f, 0.f, 0.f, 1.f);
        }
        void Scale(const vec_t& s) { Scale(s.x, s.y, s.z); }

        float GetDeterminant() const
        {
            return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] -
                m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1];
        }

        float Inverse(const matrix_t& srcMatrix, bool affine = false);
        void SetToIdentity()
        {
            v.right.Set(1.f, 0.f, 0.f, 0.f);
            v.up.Set(0.f, 1.f, 0.f, 0.f);
            v.dir.Set(0.f, 0.f, 1.f, 0.f);
            v.position.Set(0.f, 0.f, 0.f, 1.f);
        }
        void Transpose()
        {
            matrix_t tmpm;
            for (int l = 0; l < 4; l++)
            {
                for (int c = 0; c < 4; c++)
                {
                    tmpm.m[l][c] = m[c][l];
                }
            }
            (*this) = tmpm;
        }

        void RotationAxis(const vec_t& axis, float angle);

        void OrthoNormalize()
        {
            v.right.Normalize();
            v.up.Normalize();
            v.dir.Normalize();
        }
    };


    inline void DecomposeMatrix(const float* matrix, float* scale, float* rotation, float* translation)
    {
        matrix_t mat = *(matrix_t*)matrix;

        scale[0] = mat.v.right.Length();
        scale[1] = mat.v.up.Length();
        scale[2] = mat.v.dir.Length();

        mat.OrthoNormalize();

        rotation[0] = atan2f(mat.m[1][2], mat.m[2][2]);
        rotation[1] = atan2f(-mat.m[0][2], sqrtf(mat.m[1][2] * mat.m[1][2] + mat.m[2][2] * mat.m[2][2]));
        rotation[2] = atan2f(mat.m[0][1], mat.m[0][0]);

        translation[0] = mat.v.position.x;
        translation[1] = mat.v.position.y;
        translation[2] = mat.v.position.z;
    }
}