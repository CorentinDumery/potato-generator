
#include "potato.h"

#include <iostream>
#include <Eigen/Core>
#include <vector>
#include <random>
#include <math.h>
#include <igl/remove_duplicate_vertices.h>
#include <igl/per_vertex_normals.h>
#include <igl/embree/ambient_occlusion.h>

Potato::Potato()
{
    int res = 50;
    double radius = 10;

    V_.resize(res * res, 3);
    F_.resize(2 * (res - 1) * res, 3);
    Eigen::RowVector3d center = Eigen::RowVector3d(0, 0, 0);

    // TODO: improve vertices distribution on initial sphere using Fibonacci algorithm
    for (int j = 0; j < res; j++)
    {
        double z = center(2) + radius * cos(M_PI * (double)j / (double(res - 1)));
        for (int k = 0; k < res; k++)
        {
            double x = center(0) + radius * sin(M_PI * (double)j / (double(res - 1))) * cos(2 * M_PI * (double)k / (double(res - 1)));
            double y = center(1) + radius * sin(M_PI * (double)j / (double(res - 1))) * sin(2 * M_PI * (double)k / (double(res - 1)));
            V_.row(j * res + k) << x, y, z;
        }
    }

    for (int j = 0; j < res - 1; j++)
    {
        for (int k = 0; k < res; k++)
        {
            int v1 = j * res + k;
            int v2 = (j + 1) * res + k;
            int v3 = (j + 1) * res + (k + 1) % res;
            int v4 = j * res + (k + 1) % res;
            F_.row(2 * (res * j + k)) << v1, v2, v3;
            F_.row(2 * (res * j + k) + 1) << v4, v1, v3;
        }
    }

    Eigen::MatrixXd new_V;
    Eigen::MatrixXi new_F, SVI, SVJ;
    igl::remove_duplicate_vertices(V_, F_, 1e-7, new_V, SVI, SVJ, new_F);
    V_ = new_V;
    F_ = new_F;
    computeNormals();
    //computeAO(); //TODO find out why this line doesn't work
}

void Potato::setRandomColors()
{
    base_color_.resize(V_.rows(), 3);
    for (int i = 0; i < V_.rows(); i++)
    {
        base_color_.row(i) = Eigen::RowVector3d(1.0, 1.0, 0.0);
        if (std::rand() % 20 == 0)
        {
            base_color_.row(i) = Eigen::RowVector3d(0.2, 0.1, 0.0);
        }
    }
}

void Potato::addOscillation()
{
    Eigen::RowVector3d direction = Eigen::RowVector3d::Random();
    std::uniform_real_distribution<double> unif(0, 1);
    std::default_random_engine re;
    direction /= direction.norm();
    double off = unif(re) * 10;
    double ampl = unif(re) * 2;
    double freq = unif(re) / ampl;
    double r = 0.1;

    for (int i = 0; i < V_.rows(); i++)
    {
        Eigen::RowVector3d v = V_.row(i);
        double proj = v.dot(direction);
        Eigen::RowVector3d new_v = V_.row(i);
        new_v(0) = v(0) + r * std::sin(off + 2 * 3.14 * freq * proj) * ampl * direction(0);
        new_v(1) = v(1) + r * std::sin(off + 2 * 3.14 * freq * proj) * ampl * direction(1);
        new_v(2) = v(2) + r * std::sin(off + 2 * 3.14 * freq * proj) * ampl * direction(2);
        V_.row(i) = new_v;
    }
}

void Potato::smooth()
{
    Eigen::MatrixXd smooth_V = Eigen::MatrixXd::Zero(V_.rows(), V_.cols());
    smooth_V += V_;
    Eigen::RowVectorXi count_neighbors = Eigen::RowVectorXi::Zero(V_.rows());
    count_neighbors = count_neighbors.array() + 1; //considering self as neighbor
    for (int i = 0; i < F_.rows(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int id0 = F_(i, j);
            int id1 = F_(i, (j + 1) % 3);
            int id2 = F_(i, (j + 2) % 3);
            smooth_V.row(id1) += V_.row(id0);
            smooth_V.row(id2) += V_.row(id0);
            count_neighbors(id1) += 1;
            count_neighbors(id2) += 1;
        }
    }
    for (int i = 0; i < smooth_V.rows(); i++)
    {
        smooth_V.row(i) /= count_neighbors(i);
    }
    V_ = smooth_V;
}

void Potato::scale(double scalex, double scaley, double scalez)
{
    V_.col(0) *= scalex;
    V_.col(1) *= scaley;
    V_.col(2) *= scalez;
}

Eigen::MatrixXd Potato::getColors() const
{
    Eigen::MatrixXd C = Eigen::MatrixXd(V_.rows(), 3);
    for (unsigned i = 0; i < C.rows(); ++i)
    {
        C(i, 0) = 1 - ao_factor_ * ambient_occlusion_(i);
        C(i, 1) = 1 - ao_factor_ * ambient_occlusion_(i);
        C(i, 2) = 1 - ao_factor_ * ambient_occlusion_(i);
    }
    return C;
}

void Potato::setAOFactor(double ao_factor){
    ao_factor_ = ao_factor;
}

void Potato::computeNormals(){
    igl::per_vertex_normals(V_, F_, N_);
}

void Potato::computeAO(){
    igl::embree::ambient_occlusion(V_, F_, V_, N_, 500, ambient_occlusion_);
}