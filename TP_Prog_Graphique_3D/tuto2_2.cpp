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
	EZCOGL::ShaderProgram::UP shaderPrgCube;
	EZCOGL::ShaderProgram::UP shaderPrgSphere;
	EZCOGL::MeshRenderer::UP cube_rend;
	EZCOGL::MeshRenderer::UP sphere_rend;

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

Viewer::Viewer()
{}

void Viewer::init_ogl()
{
	shaderPrgCube = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TUTO2_2.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TUTO2_2Cube.fs")}}, "Cube");
	shaderPrgSphere = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TUTO2_2.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TUTO2_2Sphere.fs")}}, "Sphere");

	// ***********************************
	// Geometry
	// ***********************************
    // CUBE
	auto meshCube = EZCOGL::Mesh::Cube();
	cube_rend = meshCube->renderer(1, -1, -1, -1, -1); // initiate a cube mesh renderer with a VAO containing 5 VBOs to
	// store several attributes per vertex. Here we only use the first VBO for 3D positions attribute with location
	// index = 1, the 4 others are init with the value -1 we don't need them (for now).
	// SPHERE
	auto meshSphere = EZCOGL::Mesh::Sphere(128); // The sphere take an argument for the number of triangle rafinement
	sphere_rend = meshSphere->renderer(1, -1, -1, -1, -1);
    

	// set scene center and radius for the init of matrix view/proj
	set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
	set_scene_radius(5.f);

	// Define the color to use when refreshing screen
    glClearColor(0.1, 0.1, 0.1, 1.0);
}

void Viewer::draw_ogl()
{
	// Clear the buffer before to draw the next frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the view and projection matrix
	const EZCOGL::GLMat4& view = this->get_view_matrix();
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();

    // Construct a model matrix for both object
    const EZCOGL::GLMat4& modelCube = EZCOGL::Transfo::rotateX(-60.f) * EZCOGL::Transfo::scale(1.f);
	const EZCOGL::GLMat4& modelSphere = EZCOGL::Transfo::translate(0.f, 0.f, -5.f);
	// You must bind your shader program but that's all !
	shaderPrgCube->bind();
	// Uniforms variables send to the GPU
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	// activate Z-buffer
	glEnable(GL_DEPTH_TEST);
	// 1st Cube
	EZCOGL::set_uniform_value(0, modelCube);
	cube_rend->draw(GL_TRIANGLES); // the draw function of the mesh renderer handle the corresponding VAO binding
	// 2nd Sphere
	shaderPrgSphere->bind();
	// Uniforms variables send to the GPU
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	EZCOGL::set_uniform_value(0, modelSphere);
	sphere_rend->draw(GL_TRIANGLES); // again, the draw function of the mesh renderer handle the corresponding VAO binding

	// ***********************************
	// Rendering
	// ***********************************

    
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::Begin("Tuto 2", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});

	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders"))
		shaderPrgCube = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TUTO2_2.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TUTO2_2Cube.fs")}}, "Cube");
		shaderPrgSphere = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TUTO2_2.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TUTO2_2Sphere.fs")}}, "Sphere");

	ImGui::End();
}
