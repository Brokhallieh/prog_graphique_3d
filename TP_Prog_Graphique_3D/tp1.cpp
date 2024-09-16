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

#define macro_str(s) #s
#define macro_xstr(s) macro_str(s)
#define DATA_PATH std::string(macro_xstr(DEF_DATA_PATH))
#define SHADERS_PATH std::string(macro_xstr(DEF_SHADERS_PATH))

// using namespace EZCOGL;

class Viewer: public EZCOGL::GLViewer
{
	EZCOGL::ShaderProgram::UP shaderPrg;
	EZCOGL::VAO::UP vao;
	EZCOGL::GLVec3 color;

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
	color = EZCOGL::GLVec3(0.f, 0.f, 0.f);

	shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP1.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP1.fs")}}, "Square");
	
	// ***********************************
	// Geometry
	// ***********************************

	// create a VBO with the position of 4 2D vertices (for point and line loop draw)
	EZCOGL::VBO::SP vbo_p = EZCOGL::VBO::create(EZCOGL::GLVVec2{{-0.5f, -0.5f}/*(x0,y0)*/, {0.5f, -0.5f}/*(x1,y1)*/, {0.5f, 0.5f}/*(x2,y2)*/, {0.5f, 0.5f}/*(x2,y2)*/, {-0.5f, 0.5f}/*(x3,y3)*/, {-0.5f, -0.5f}/*(x0,y0)*/});
	
	// create the VAO and associate the VBO to it
	vao = EZCOGL::VAO::create({{1, vbo_p}}); // 1 is the "location index". We use this ID in the vertex shader to get this VBO with a "in" variable

	// Define the color to use when refreshing screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
}

void Viewer::draw_ogl()
{
	// Clear the buffer before to draw the next frame
	glClear(GL_COLOR_BUFFER_BIT);
	// enable the use of gl_PointSize in the shader to define the size of a point on the screen when using GL_POINTS
	glEnable(GL_PROGRAM_POINT_SIZE);

	// ***********************************
	// Rendering
	// ***********************************
	// Bind the shader program (EZCOGL wrapper)
	shaderPrg->bind();
	// Bind the VAO (EZCOGL wrapper)
	vao->bind();

	EZCOGL::set_uniform_value(0, color);

	float time = EZCOGL::current_time();
	EZCOGL::set_uniform_value(2, time);

	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);*/
	//marche avec (BACK et CCW) où (FRONT et CW) sinon ne dessine que les points et les lignes)

	const EZCOGL::GLMat4& model = EZCOGL::Transfo::rotateZ(45.f);
	EZCOGL::set_uniform_value(4, model);

	// 1st pass : Draw Points
	glDrawArrays(GL_POINTS, 0, 5);
	// 2nd pass : Draw Lines
	glDrawArrays(GL_LINE_LOOP, 0, 5);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_TRIANGLES, 3, 6);
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::Begin("TP1 Introduction",nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});

	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders"))
		shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP1.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP1.fs")}}, "Square");

	ImGui::SliderFloat("Red", &color[0], 0.f, 1.f);
	ImGui::SliderFloat("Green", &color[1], 0.f, 1.f);
	ImGui::SliderFloat("Blue", &color[2], 0.f, 1.f);
	
	ImGui::End();
}
