/*******************************************************************************
* EasyCppOGL:   Copyright (C) 2019,                                            *
* Sylvain Thery, IGG Group, ICube, University of Strasbourg, France            *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: thery@unistra.fr                                        *
*******************************************************************************/


#include <iostream>
#include "shader_program.h"
#include "gl_viewer.h"
#include "mesh.h"

#define macro_str(s) #s
#define macro_xstr(s) macro_str(s)
#define DATA_PATH std::string(macro_xstr(DEF_DATA_PATH))
#define SHADERS_PATH std::string(macro_xstr(DEF_SHADERS_PATH))

// using namespace EZCOGL;

class Viewer: public EZCOGL::GLViewer
{
    // Shader programs
	EZCOGL::ShaderProgram::UP tone_mapping_shader_prog;
    
	// Renderers
	EZCOGL::MeshRenderer::UP cornelBox_rend;
	EZCOGL::MeshRenderer::UP cube_rend;
	EZCOGL::MeshRenderer::UP sphere_rend;

	// Objects params
	EZCOGL::GLVec3 cubePos;
	EZCOGL::GLVec3 spherePos;

	// Lights
	EZCOGL::GLVVec3 lightPositions;
	EZCOGL::GLVVec3 lightColors;

	// Tone mapping
	float exposure;

	// Lighting options
	bool attenuation;
	bool gammaCorrection;
	bool toneMapping;

public:
	Viewer();
	void init_ogl() override;
	void draw_ogl() override;
	void interface_ogl() override;
};

int main(int, char**)
{
	Viewer v;
	return v.launch3d();
}

Viewer::Viewer() : cubePos(1.f, 1.f, 5.5f), spherePos(-1.f, -1.f, 2.5f), exposure(1.f), attenuation(false), gammaCorrection(false), toneMapping(false)
{}

void Viewer::init_ogl()
{
	// -------------------------------------------------------------------
    // Init Shader Programs
	// -------------------------------------------------------------------
	tone_mapping_shader_prog = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP7.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP7.fs")}}, "HDR");
	// -------------------------------------------------------------------
    // GEOMETRY
	// -------------------------------------------------------------------
	// Create geometry : Cornel box containing the scene
    auto me = EZCOGL::Mesh::CornelBox();
	// get the associated renderer with positions(1), and normals(2) VBO
	cornelBox_rend = me->renderer(1, 2, -1, -1, -1);

	set_scene_center(me->BB()->center());
	set_scene_radius(me->BB()->radius() + 50.f);
	this->cam_.frame_.translation().z() += 106.f;

	// Create geometry : mesh of a cube
	me = EZCOGL::Mesh::Cube();
	cube_rend = me->renderer(1, 2, -1, -1, -1);
	// Create geometry : mesh of a sphere
	me = EZCOGL::Mesh::Sphere(64);
	sphere_rend = me->renderer(1, 2, -1, -1, -1);

	// ***************
	// Lights init
    // ***************
	lightPositions = EZCOGL::GLVVec3{{0.f,  0.f, -18.f}/*Back light*/, {-1.4f, -1.9f, 9.f}, {0.f, -1.8f, 4.f}, {0.8f, -1.7f, 6.f}};
	lightColors = EZCOGL::GLVVec3{{50.f, 50.f, 50.f}/*Back light*/, {1.f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.f}, {0.0f, 1.f, 0.0f}};

	// Define the color to use when refreshing screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Viewer::draw_ogl()
{
	// Clear the GL "color" and "depth" framebuffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable the Z Depth test
	glEnable(GL_DEPTH_TEST);

	// Get the view and projection matrix
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();
	const EZCOGL::GLMat4& view = this->get_view_matrix();

	// Set "current" shader program
	tone_mapping_shader_prog->bind();

	// 1. BOX
    const EZCOGL::GLMat4& modelBox = EZCOGL::Transfo::scale(2.5f, 2.5f, 20.f);
	EZCOGL::set_uniform_value(0, modelBox);
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(modelBox));
	EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(0.2f, 0.2f, 0.2f));	// object albedo
	EZCOGL::set_uniform_value(5, lightPositions);
	EZCOGL::set_uniform_value(9, lightColors); // previous uniform is an array of size 4 so 5 + 4 = 9 for the next id
	EZCOGL::set_uniform_value(13, attenuation); // previous uniform is an array of size 4 so 9 + 4 = 13 for the next id
	EZCOGL::set_uniform_value(14, gammaCorrection);
	EZCOGL::set_uniform_value(15, toneMapping);
	EZCOGL::set_uniform_value(16, exposure);
	cornelBox_rend->draw(GL_TRIANGLES);

    // 2. CUBE
    const EZCOGL::GLMat4& modelCube = EZCOGL::Transfo::translate(cubePos) * EZCOGL::Transfo::scale(0.5f);
	EZCOGL::set_uniform_value(0, modelCube);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(modelCube));
	EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(0.9f, 0.9f, 0.9f));	// object albedo
	cube_rend->draw(GL_TRIANGLES);

    // 3. SPHERE
    const EZCOGL::GLMat4& modelSphere = EZCOGL::Transfo::translate(spherePos) * EZCOGL::Transfo::scale(1.5f) * EZCOGL::Transfo::rotateX(90.f);
	EZCOGL::set_uniform_value(0, modelSphere);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(modelSphere));
	EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(0.4f, 0.4f, 0.4f));	// object albedo
	sphere_rend->draw(GL_TRIANGLES);
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::Begin("Advanced Lighting",nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});
	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders"))
		tone_mapping_shader_prog = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP7.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP7.fs")}}, "HDR");

	ImGui::SliderFloat3("Back light pos", lightPositions[0].data(), -100.f, 100.f);
	ImGui::SliderFloat3("Back light color", lightColors[0].data(), 0.f, 200.f);
	ImGui::Checkbox("Attenuation", &attenuation);
	ImGui::Checkbox("Gamma correction", &gammaCorrection);
	// if (attenuation && gammaCorrection)
		ImGui::Checkbox("Tone mapping", &toneMapping);
	if (toneMapping) ImGui::SliderFloat("Exposure", &exposure, 0.f, 5.f);

	ImGui::Text("Obejcts position");
	ImGui::SliderFloat("Cube X", &cubePos[0], -1.f, 1.f);
	ImGui::SliderFloat("Cube Y", &cubePos[1], -1.f, 1.f);
	ImGui::SliderFloat("Cube Z", &cubePos[2], -20.f, 20.f);
	ImGui::SliderFloat("Sphere X", &spherePos[0], -1.f, 1.f);
	ImGui::SliderFloat("Sphere Y", &spherePos[1], -1.f, 1.f);
	ImGui::SliderFloat("Sphere Z", &spherePos[2], -20.f, 20.f);


	ImGui::End();
}
