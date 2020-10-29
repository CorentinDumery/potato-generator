
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/png/readPNG.h>
#include <igl/per_vertex_normals.h>
#include <igl/embree/ambient_occlusion.h>
#include <imgui.h>

#include "potato.h"

/* TODO
- update AO when necessary, make it part of Potato class
- potato keeping track of its perturbations
- vertices on the potato correspond to vertices on the initial sphere
- Move along normal instead ?
- override Viewer::key_pressed
- use custom shader
- des sliders pour gérer high/low frequences, et une random seed, et 'Generate' qui change juste la seed
*/

bool key_down(igl::opengl::glfw::Viewer &viewer, unsigned char key, int modifier)
{
    if (key == '1')
    {
    }
    else if (key == '2')
    {
    }
    return false;
}

int main(int argc, char *argv[])
{
    Potato potato = Potato();
    potato.setRandomColors();
    for (int i = 0; i < 100; i++)
    {
        potato.addOscillation();
    }
    for (int i = 0; i < 3; i++)
    {
        potato.smooth();
    }
    potato.scale(1.0, 1.5, 0.8);

    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(potato.getV(), potato.getF());
    //viewer.data().set_colors(potato.getColors());
    Eigen::MatrixXd V = potato.getV();
    std::cout << "Vertices matrix size: " << V.rows() << "x" << V.cols() << std::endl;
    Eigen::MatrixXi F = potato.getF();

    double ao_factor = 1.3;

    Eigen::VectorXd AO;
    Eigen::MatrixXd N;
    igl::per_vertex_normals(potato.getV(), potato.getF(), N);
    igl::embree::ambient_occlusion(potato.getV(), potato.getF(), potato.getV(), N, 500, AO);
    Eigen::MatrixXd C = Eigen::MatrixXd(V.rows(), 3);
    for (unsigned i = 0; i < C.rows(); ++i)
    {
        C(i, 0) = 1 - ao_factor * AO(i); //std::min<double>(AO(i)+0.2,1);
        C(i, 1) = 1 - ao_factor * AO(i); //std::min<double>(AO(i)+0.2,1);
        C(i, 2) = 1 - ao_factor * AO(i); //std::min<double>(AO(i)+0.2,1);
    }
    viewer.data().set_colors(C);

    igl::opengl::glfw::imgui::ImGuiMenu menu;
    menu.callback_draw_viewer_window = []() {};
    viewer.plugins.push_back(&menu);

    //helper function for menu
    auto make_checkbox = [&](const char *label, unsigned int &option) {
        return ImGui::Checkbox(
            label,
            [&]() { return viewer.core().is_set(option); },
            [&](bool value) { return viewer.core().set(option, value); });
    };

    float light_f;
    uint continuous_rotation = 0;

    menu.callback_draw_custom_window = [&]() {
        bool show = true;
        ImGui::SetNextWindowSize(ImVec2(300, 400));
        if (ImGui::Begin("Potato"))
        {
            ImGui::SetClipboardText("Potato");
            ImGui::SetNextWindowPos(ImVec2(0.f * menu.menu_scaling(), 5),
                                    ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(150, 600), ImGuiCond_FirstUseEver);
            if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::SliderFloat("Shininess", &viewer.data().shininess, 0.0f, 10.0f, "%.3f"))
                {
                }
                if (ImGui::SliderFloat("Light", &viewer.core().lighting_factor, 0.0f, 5.0f, "%.3f"))
                {
                }
            }

            if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
            {
                float w = ImGui::GetContentRegionAvailWidth();
                float p = ImGui::GetStyle().FramePadding.x;
                if (ImGui::Button("Smooth", ImVec2((w - p) / 2.f, 0)))
                {
                    for (int i = 0; i < 10; i++)
                    {
                        potato.smooth();
                    }
                    viewer.data().set_mesh(potato.getV(), potato.getF());
                }
                ImGui::SameLine(0, p);
                if (ImGui::Button("Add 10", ImVec2((w - p) / 2.f, 0)))
                {
                    for (int i = 0; i < 10; i++)
                    {
                        potato.addOscillation();
                    }
                    viewer.data().set_mesh(potato.getV(), potato.getF());
                }
            }
            make_checkbox("Show lines", viewer.data().show_lines);
            make_checkbox("Show faces", viewer.data().show_faces);
            make_checkbox("Rotate", continuous_rotation);
            ImGui::End();
        }
    };

    viewer.callback_post_draw = [&](igl::opengl::glfw::Viewer &v) {
        float t = 0.1;

        if (continuous_rotation)
        {
            Eigen::Matrix3f rot_mat;
            rot_mat << 0.9998000, 0.0, -0.0199987,
                0, 1.0, 0.0,
                0.0199987, 0.0, 0.9998000;
            Eigen::Quaternionf rot = Eigen::Quaternionf(rot_mat);
            viewer.core().trackball_angle *= rot;
        }
        return true;
    };

    viewer.data().show_texture = true;

    Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> R, G, B, A;
    if (!igl::png::readPNG("../images/potato.png", R, G, B, A))
    {
        std::cout << "Error: couldn't read texture" << std::endl;
    }

    viewer.data().set_texture(R, G, B);
    viewer.data().set_uv(V / 10);
    viewer.data().show_texture = true;
    viewer.data().show_lines = 0u;
    viewer.core().orthographic = false;
    viewer.callback_key_down = &key_down;
    viewer.data().set_face_based(0u);
    viewer.core().is_animating = true;
    viewer.launch();
}