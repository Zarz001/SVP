// Importing libraries


#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

using LDouble = long double;
using Vector = vector<LDouble>;
using Matrix = vector<Vector>;



// Function to calculate dot product


LDouble dot(const Vector& v1, const Vector& v2) {
    LDouble sum = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        sum += v1[i] * v2[i];
    }
    return sum;
}


// Function to calculate norm


LDouble norm(const Vector& vec) {
    return sqrt(dot(vec, vec));
}


// Function to modify v1 by removing its projection on v2


void projInPlace(Vector* v1, const Vector& v2) {
    if (v1 == nullptr) {
        // Handle the null pointer case appropriately



        std::cerr << "v1 pointer is null in projInPlace function." << std::endl;
        return;
    }

    LDouble dotProduct = dot(*v1, v2);
    LDouble normV2 = dot(v2, v2);

    if (normV2 < 1e-15L) return;  // Check for near-zero denominator

    for (size_t i = 0; i < v1->size(); ++i) {
        (*v1)[i] -= (dotProduct / normV2) * v2[i];
    }
}


// Function to calculate Gram-Schmidt process


void GramSchmidt(const Matrix& basis, Matrix* orthoBasis) {
    for (size_t i = 0; i < basis.size(); ++i) {
        (*orthoBasis)[i] = basis[i];
        for (size_t j = 0; j < i; ++j) {
            projInPlace(&(*orthoBasis)[i], (*orthoBasis)[j]);
        }
    }
}


// Function to check if the length of the is at least 75% of the previous one


bool lovaszCondition(const Matrix& orthoBasis, size_t k) {
    if (k == 0) return true;
    LDouble currentNorm = dot(orthoBasis[k], orthoBasis[k]);
    LDouble previousNorm = 0.75L * dot(orthoBasis[k-1], orthoBasis[k-1]);
    return currentNorm >= previousNorm;
}


// Function to make the basis shorter and orthogonal
// Setting up for LLL and KZ reductions


void sizeReduction(Matrix* basis, Matrix* orthoBasis, size_t k, size_t l) {
    LDouble denom = dot((*orthoBasis)[l], (*orthoBasis)[l]);
    if (abs(denom) < 1e-15L) {
        std::cerr << "Denominator too close to zero" << std::endl;
        return;
    }
    LDouble mu = round(dot((*basis)[k], (*orthoBasis)[l]) / denom);
    for (size_t i = 0; i < (*basis)[k].size(); ++i) {
        (*basis)[k][i] -= mu * (*basis)[l][i];
    }
    GramSchmidt(*basis, orthoBasis);
}


// Function to implement Lenstra-Lenstra-Lovasz (LLL)


void LLLReduction(Matrix* basis) {
    size_t s = basis->size();
    Matrix orthoBasis(s, Vector((*basis)[0].size()));
    GramSchmidt(*basis, &orthoBasis);
    size_t k = 1;
    while (k < s) {
        for (size_t j = k; j > 0; --j) {
            sizeReduction(basis, &orthoBasis, k, j-1);
        }
        if (lovaszCondition(orthoBasis, k)) {
            ++k;
        } else {
            swap((*basis)[k], (*basis)[k-1]);
            GramSchmidt(*basis, &orthoBasis);
            k = max(k-1, size_t(1));
        }
    }
}


// Implementation of Korkin-Zolotarev (KZ) Reduction


void KZReduction(Matrix* basis) {
    size_t s = basis->size();
    Matrix orthoBasis(s, Vector((*basis)[0].size()));
    GramSchmidt(*basis, &orthoBasis);

    for (size_t i = 0; i < s; ++i) {
        for (size_t j = i + 1; j < s; ++j) {
            sizeReduction(basis, &orthoBasis, j, i);
        }
    }
    sort(basis->begin(), basis->end(), [](const Vector& a, const Vector& b) {
        return norm(a) < norm(b);
    });
    GramSchmidt(*basis, &orthoBasis);
}


// Main function to handle input and calling LLL and KZ reduction


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " [vector1] [vector2] ..." << endl;
        return 1;
    }

    Matrix basis;
    vector<LDouble>::size_type vectorCount = 0;

    // Counting and parsing input vectors


    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg.front() == '[') {
            vectorCount++;
            while (arg.back() != ']' && i + 1 < argc) {
                arg += " " + string(argv[++i]);
            }
            if (arg.back() != ']') {
                cerr << "Invalid vector format." << endl;
                return 1;
            }
        }
    }

    // Parsing vectors


    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        Vector vec;

        while (arg.front() != '[' || arg.back() != ']') {
            if (i + 1 < argc) {
                arg += " " + string(argv[++i]);
            } else {
                cerr << "Invalid vector format." << endl;
                return 1;
            }
        }

        stringstream ss(arg.substr(1, arg.size() - 2));
        LDouble component;

        while (ss >> component) {
            vec.push_back(component);
        }

        if (vec.size() != vectorCount) {
            cerr << "Invalid vector format" << endl;
            return 1;
        }

        basis.push_back(vec);
    }

    LLLReduction(&basis);
    KZReduction(&basis);

    LDouble shortestVectorLength = norm(basis[0]);

    // Writing results to a file


    ofstream resultFile("result.txt");
    if (!resultFile.is_open()) {
        cerr << "Could not open result file for writing." << endl;
        return 1;
    }

    resultFile << fixed << setprecision(15);
    resultFile << shortestVectorLength << endl;

    resultFile.close();
    return 0;
}
