
#include <Eigen/Core>

class Potato
{
public:
    Potato();
    void addOscillation();
    void smooth();
    void setRandomColors();
    void scale(double scalex, double scaley, double scalez);
    Eigen::MatrixXd getV() const { return V_; }
    Eigen::MatrixXi getF() const { return F_; }
    Eigen::MatrixXd getColors() const { return colors_; }

private:
    Eigen::MatrixXd V_;
    Eigen::MatrixXi F_;
    Eigen::MatrixXd colors_;
};