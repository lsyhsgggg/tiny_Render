#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include "geometry.h"

template <> template <> Vec3<int>::Vec3(const Vec3<float>& v) : x(int(v.x + .5)), y(int(v.y + .5)), z(int(v.z + .5)) {
}

template <> template <> Vec3<float>::Vec3(const Vec3<int>& v) : x(v.x), y(v.y), z(v.z) {
}


Matrix::Matrix(int r, int c) : m(std::vector<std::vector<float> >(r, std::vector<float>(c, 0.f))), rows(r), cols(c) { }

int Matrix::nrows() {
    return rows;
}

int Matrix::ncols() {
    return cols;
}
Matrix Matrix::identity(int dimensions) {
    Matrix E(dimensions, dimensions);
    for (int i = 0; i < dimensions; i++) {
        for (int j = 0; j < dimensions; j++) {
            E[i][j] = (i == j ? 1.f : 0.f);
        }
    }
    return E;
}

std::vector<float>& Matrix::operator[]( int i) {
    assert(i >= 0 && i < rows);
    return m[i];
}



Matrix Matrix::transpose() {
    Matrix result(cols, rows);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[j][i]= m[i][j];
    return result;
}

Matrix Matrix::inverse() {
    assert(rows == cols);
    // augmenting the square matrix with the identity matrix of the same dimensions a => [ai]
    Matrix result(rows, cols * 2);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i][j] = m[i][j];
    for (int i = 0; i < rows; i++)
        result[i][i + cols] = 1;
    // first pass
    for (int i = 0; i < rows - 1; i++) {
        // normalize the first row
        for (int j = result.cols - 1; j >= 0; j--)
            result[i][j] /= result[i][i];
        for (int k = i + 1; k < rows; k++) {
            float coeff = result[k][i];
            for (int j = 0; j < result.cols; j++) {
                result[k][j] -= result[i][j] * coeff;
            }
        }
    }
    // normalize the last row
    for (int j = result.cols - 1; j >= rows - 1; j--)
        result[rows - 1][j] /= result[rows - 1][rows - 1];
    // second pass
    for (int i = rows - 1; i > 0; i--) {
        for (int k = i - 1; k >= 0; k--) {
            float coeff = result[k][i];
            for (int j = 0; j < result.cols; j++) {
                result[k][j] -= result[i][j] * coeff;
            }
        }
    }
    // cut the identity matrix back
    Matrix truncate(rows, cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            truncate[i][j] = result[i][j + cols];
    return truncate;
}

std::ostream& operator<<(std::ostream& s, Matrix& m) {
    for (int i = 0; i < m.nrows(); i++) {
        for (int j = 0; j < m.ncols(); j++) {
            s << m[i][j];
            if (j < m.ncols() - 1) s << "\t";
        }
        s << "\n";
    }
    return s;
}

Matrix operator*(const Matrix& a, const Matrix& b)
{
    assert(a.cols == b.rows);
    Matrix result(a.rows, b.cols);
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < b.cols; j++) {
            result.m[i][j] = 0.f;
            for (int k = 0; k < a.cols; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}
Matrix operator*(const Matrix& mat, const Vec3f& v) {
    if (mat.rows == 4) {
        Matrix vg(4, 1);
        vg[0] = { v.x };
        vg[1] = { v.y };
        vg[2] = { v.z };
        vg[3] = { 1.0f };
        return mat * vg;
    }
    else{
        Matrix vg(3, 1);
        vg[0] = { v.x };
        vg[1] = { v.y };
        vg[2] = { v.z };
        return mat * vg;
    }
   
}

Matrix mat3x3( Matrix& mat)
{
    Matrix ans(3, 3);
    if (mat.rows != 4 || mat.cols != 4) return ans.identity(3);
    ans[0] = std::vector<float>(mat[0].begin(), mat[0].end() - 1);
    ans[1] = std::vector<float>(mat[1].begin(), mat[1].end() - 1);
    ans[2] = std::vector<float>(mat[2].begin(), mat[2].end() - 1);
    return ans;
}


