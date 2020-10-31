
#include <Eigen/Core>

class Potato
{
public:
    Potato();
    void addOscillation();
    void smooth();
    void setRandomColors();
    void scale(double scalex, double scaley, double scalez);
    void setAOFactor(double ao_factor);
    void computeAO();
    Eigen::MatrixXd getV() const { return V_; }
    Eigen::MatrixXi getF() const { return F_; }
    Eigen::MatrixXd getColors() const;

private:
    Eigen::MatrixXd V_;
    Eigen::MatrixXi F_;
    Eigen::MatrixXd N_;
    Eigen::MatrixXd base_color_;
    Eigen::MatrixXd ambient_occlusion_;
    double ao_factor_ = 1.3;
    void computeNormals();
};