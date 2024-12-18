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
	// Shader prg 
	EZCOGL::ShaderProgram::UP shaderPrgCompute;
	EZCOGL::ShaderProgram::UP shaderPrgFullScreen;

    // texture
    EZCOGL::Texture2D::SP tex;
	int texWidth;
	int texHeight;

	// Mandelbrot iterations
	int maxNbIterations;

	// Mouse pos & zoom
	EZCOGL::GLVec2 pos;
	EZCOGL::GLVec3 zoom;
	bool mousePress;
	int32_t mouseButton;

public:
	Viewer();
	void init_ogl() override;
	void draw_ogl() override;
	void interface_ogl() override;
    void mouse_press_ogl(int32_t button, double x, double y) override;
	void mouse_move_ogl(double x, double y) override;
	void mouse_release_ogl(int32_t button, double x, double y) override;
};

int main(int, char**)
{
	Viewer v;
	return v.launch3d();
}

Viewer::Viewer() : texWidth(2048), texHeight(2048), maxNbIterations(100), pos(EZCOGL::GLVec2(0.f, 0.f)), zoom(EZCOGL::GLVec3(-0.5f, 0.f, 4.f)), mousePress(false), mouseButton(0)
{}

void Viewer::init_ogl()
{
	// Shader prg 
	shaderPrgCompute = EZCOGL::ShaderProgram::create({{GL_COMPUTE_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP10-2.cs")}}, "Mandelbrot compute shader");
	shaderPrgFullScreen = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP10-FullScreen.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP10-FullScreen.fs")}}, "draw");
	
	// texture
	tex = EZCOGL::Texture2D::create({GL_LINEAR, GL_REPEAT});
	tex->alloc(texWidth, texHeight, GL_RGBA32F, nullptr);

    // Define the color to use when refreshing screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Viewer::draw_ogl()
{
	// Clear the buffer before to draw the next frame
	glClear(GL_COLOR_BUFFER_BIT);

	// update zoom pos and factor
	if (mousePress)
	{
		zoom[0] += 0.1 * (pos[0] - zoom[0]);	// pos X
		zoom[1] += 0.1 * (pos[1] - zoom[1]);	// pos Y
		zoom[2] *= mouseButton == 1 ? 1.01f : 0.99f; // zoom/unzoom factor
		if (zoom[2] > 4.0f) zoom[2] = 4.0f;
		maxNbIterations += mouseButton == 1 ? -1 : 1; // update numbers of iterations according to zoom
		if (maxNbIterations < 100) maxNbIterations = 100;
	}

	// ********************************************************
	// 1st pass : Compute shader program
	// ********************************************************
	// Bind the shader program
	shaderPrgCompute->bind();
	// mMaxNbIterations = 6 + static_cast<int>((std::cos(EZCOGL::current_time()) * 0.5f + 0.5f) * (65.f - 6.f));
	EZCOGL::set_uniform_value(0, maxNbIterations);
	EZCOGL::set_uniform_value(1, zoom);
	// Bind texture as an image with GL_WRITE_ONLY access
	glBindImageTexture(0/*binding ID*/, tex->id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    // Set kernel configuration
	// - each bloc of threads
	const int blocDimX = 32;
	const int blocDimY = 32;
	// - grid of blocs of threads
	const int gridSizeX = texWidth / blocDimX;
	const int gridSizeY = texHeight / blocDimY;
	const int gridSizeZ = 1;
	// compute
	glDispatchCompute(gridSizeX, gridSizeY, gridSizeZ);
	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// ********************************************************
	// 2nd pass : Draw the result on a full-screen quad
	// ********************************************************
    shaderPrgFullScreen->bind();
	EZCOGL::VAO::none()->bind(); // mandatory ...
    tex->bind(0); // bind the texture containing the result of the compute shader from the previous pass
    glDrawArrays(GL_TRIANGLES, 0, 3); // draw a procedural fullscreen quad
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::Begin("Params",nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});

	ImGui::Text("FPS :(%2.2lf)", fps_);
    if (ImGui::Button("Reload shaders"))
    {
		shaderPrgCompute = EZCOGL::ShaderProgram::create({{GL_COMPUTE_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP10-2.cs")}}, "Mandelbrot compute shader");
	    shaderPrgFullScreen = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP10-FullScreen.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP10-FullScreen.fs")}}, "draw");
    }
	if (ImGui::CollapsingHeader("Texture content"))
		ImGui::Image(reinterpret_cast<ImTextureID>(tex->id()), ImVec2(400, 400), ImVec2(0, 1), ImVec2(1, 0));

	ImGui::SliderInt("nb max iterations", &maxNbIterations, 1, 1500/*nb types*/ );
	if (ImGui::Button("Reset"))
	{
		zoom = EZCOGL::GLVec3(-0.5f, 0.f, 4.f);
		maxNbIterations = 100;
	}
	ImGui::End();
}

void Viewer::mouse_press_ogl(int32_t button, double x, double y)
{
	mousePress = true;
	mouseButton = button;

	double xPos = x / this->width();
    double yPos = y / this->height();
	pos[0] = zoom[0] - zoom[2] / 2.f + xPos * zoom[2];
	pos[1] = zoom[1] + zoom[2] / 2.f - yPos * zoom[2];
}

void Viewer::mouse_move_ogl(double x, double y)
{
    if (!mousePress) return;
    // pos[0] = (x / this->width()) * 2.f - 1.f; // coords between -1;1
    // pos[1] = (1.f - y / this->height()) * 2.f - 1.f; // coords between -1;1
}

void Viewer::mouse_release_ogl(int32_t button, double x, double y)
{
    mousePress = false;
}