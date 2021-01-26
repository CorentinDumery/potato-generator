
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/png/readPNG.h>
#include <igl/file_dialog_save.h>
#include <igl/writeOFF.h>
#include <imgui.h>

#include "potato.h"

/* TODO
- potato keeping track of its perturbations
- vertices on the potato correspond to vertices on the initial sphere
- Move along normal instead ?
- use custom shader
- des sliders pour gérer high/low frequences, et une random seed, et 'Generate' qui change juste la seed
*/

int main(int argc, char *argv[])
{
    Potato potato = Potato();

    for (int i = 0; i < 100; i++)
    {
        potato.addOscillation();
    }

    for (int i = 0; i < 3; i++)
    {
        potato.smooth();
    }
    potato.scale(1.0, 1.5, 0.8);
    potato.computeAO();

    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(potato.getV(), potato.getF());
    Eigen::MatrixXd V = potato.getV();
    std::cout << "Vertices matrix size: " << V.rows() << "x" << V.cols() << std::endl;
    Eigen::MatrixXi F = potato.getF();

    potato.setAOFactor(1.3);
    viewer.data().set_colors(potato.getColors());

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
    float ao_factor;

    menu.callback_draw_custom_window = [&]() {
        bool show = true;
        ImGui::SetNextWindowSize(ImVec2(300, 400));
        if (ImGui::Begin("Potato")) {
            ImGui::SetClipboardText("Potato");
            ImGui::SetNextWindowPos(ImVec2(0.f * menu.menu_scaling(), 5),
                                    ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(150, 600), ImGuiCond_FirstUseEver);
            if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
                const char* items[] = { "Potato", "Yellow matcap", "Matcap potato"};
                static int item_current = 0;
                if (ImGui::Combo("Texturing", &item_current, items, IM_ARRAYSIZE(items))){

                    std::vector<std::string> textures = {"potato.png", "yellow_matcap.png", "potato.png"};

                    Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> R, G, B, A;
                    if (!igl::png::readPNG("../images/" + textures[item_current], R, G, B, A))
                    {
                        std::cout << "Error: couldn't read texture" << std::endl;
                    }

                    viewer.data().set_texture(R, G, B);

                    if (item_current == 0){
                        viewer.data().use_matcap = false;
                    }
                    else {
                        viewer.data().use_matcap = true;
                    }
                }
                
                if (item_current == 0){
                    if (ImGui::SliderFloat("Ambient Occlusion", &ao_factor, 0.1f, 10.0f, "%.3f"))
                    {
                        potato.setAOFactor(ao_factor);
                        viewer.data().set_colors(potato.getColors());
                    }
                    if (ImGui::SliderFloat("Shininess", &viewer.data().shininess, 0.1f, 10.0f, "%.3f"))
                    {
                    }
                    if (ImGui::SliderFloat("Lighting", &viewer.core().lighting_factor, 0.0f, 5.0f, "%.3f"))
                    {
                    }

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
                if (ImGui::Button("Perturbation", ImVec2((w - p) / 2.f, 0)))
                {
                    for (int i = 0; i < 10; i++)
                    {
                        potato.addOscillation();
                    }
                    viewer.data().set_mesh(potato.getV(), potato.getF());
                }

                make_checkbox("Show texture", viewer.data().show_texture);
                make_checkbox("Show lines", viewer.data().show_lines);
                make_checkbox("Show faces", viewer.data().show_faces);
                make_checkbox("Rotate", continuous_rotation);

                if (ImGui::Button("Save potato", ImVec2((w - p), 0)))
                {
                    std::string filename = igl::file_dialog_save();
                    igl::writeOFF("filename", potato.getV(), potato.getF());
                }
            }

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
    viewer.data().set_face_based(0u);
    viewer.core().is_animating = true;
    viewer.launch();
}