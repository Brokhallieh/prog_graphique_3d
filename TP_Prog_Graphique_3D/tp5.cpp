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
#include "fbo.h"
#include <Eigen/Dense>


#define macro_str(s) #s
#define macro_xstr(s) macro_str(s)
#define DATA_PATH std::string(macro_xstr(DEF_DATA_PATH))
#define SHADERS_PATH std::string(macro_xstr(DEF_SHADERS_PATH))

// using namespace EZCOGL;

class Viewer: public EZCOGL::GLViewer
{
	EZCOGL::ShaderProgram::UP shaderPrg;
	EZCOGL::ShaderProgram::UP cubeShaderPrg;
	EZCOGL::FBO::SP fbo;
	EZCOGL::MeshRenderer::UP cube_rend; 

public:
	Viewer();
	
	void init_ogl() override;
	void draw_ogl() override;
	void interface_ogl() override;
	void resize_ogl(int32_t w, int32_t h) override;
};

void Viewer::resize_ogl(int32_t w, int32_t h)
{
    if (fbo)
    {
        fbo->resize(w, h);  // Redimensionner la texture FBO
    }

    glViewport(0, 0, w, h);  // Mettre à jour le viewport
}

int main(int, char**)
{
	Viewer v;
	return v.launch3d();
}

Viewer::Viewer()
{}

void Viewer::init_ogl()
{
	shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP5.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP5.fs")}}, "FBO");
	cubeShaderPrg= EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP5Cube.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP5Cube.fs")}}, "texture");

	// ***********************************
	// Geometry
	// ***********************************

	auto tex_FBO = EZCOGL::Texture2D::create({GL_NEAREST, GL_REPEAT});
	tex_FBO->init(GL_RGBA8);
	fbo = EZCOGL::FBO::create({tex_FBO});

	// CUBE
	auto meshCube = EZCOGL::Mesh::Cube();
    cube_rend = meshCube->renderer(1, -1, 2, -1, -1);

	// set scene center and radius for the init of matrix view/proj
	set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
	set_scene_radius(5.f);

	// Define the color to use when refreshing screen
    glClearColor(0.1, 0.1, 0.1, 1.0);

}

void Viewer::draw_ogl()
{

	EZCOGL::FBO::push();
	fbo->bind();
	// Clear the buffer before to draw the next frame
	glClear(GL_COLOR_BUFFER_BIT);
	
    // Get the view and projection matrix
	const EZCOGL::GLMat4& view = this->get_view_matrix();
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();
    // Construct a model matrix
    const EZCOGL::GLMat4& model = EZCOGL::Transfo::rotateX(-90.0) * EZCOGL::Transfo::scale(1.5f);

	glEnable(GL_DEPTH_TEST);
	// ***********************************
	// Rendering
	// ***********************************
	shaderPrg->bind();
	glDrawArrays(GL_TRIANGLES,0,3);
	
	EZCOGL::FBO::pop();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cubeShaderPrg->bind();

	EZCOGL::set_uniform_value(0, model);
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	fbo->texture(0)->bind(0);
	cube_rend->draw(GL_TRIANGLES);
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 2.0f;
	ImGui::Begin("Tp4", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});
	//ImGui::SliderFloat("reflection", &coeffReflection, 0.f, 1.f);
	//ImGui::SliderFloat("X", &lightPos[0], -200.f, 200.f);
	//ImGui::SliderFloat("Y", &lightPos[1], -200.f, 200.f);
	//ImGui::SliderFloat("Z", &lightPos[2], -200.f, 200.f);
	//ImGui::SliderFloat("Int", &intensity, 0.f, 100.f);
	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders"))
		shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP5.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP5.fs")}}, "car Lighting");
		cubeShaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP5Cube.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP5Cube.fs")}}, "car Lighting");

	
	ImGui::End();
}
