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

#define macro_str(s) #s
#define macro_xstr(s) macro_str(s)
#define DATA_PATH std::string(macro_xstr(DEF_DATA_PATH))
#define SHADERS_PATH std::string(macro_xstr(DEF_SHADERS_PATH))

// using namespace EZCOGL;

class Viewer: public EZCOGL::GLViewer
{
    // Shader programs
	EZCOGL::ShaderProgram::UP lighting_shader_prog;
	EZCOGL::ShaderProgram::UP depth_map_shader_prog;
    
	// Renderers
	EZCOGL::MeshRenderer::UP ground_rend;
	EZCOGL::MeshRenderer::UP cube_rend;
	EZCOGL::MeshRenderer::UP sphere_rend;

	// FBO - offscreen rendering
    EZCOGL::FBO_DepthTexture::SP fbo_depth;

	// Textures
    EZCOGL::Texture2D::SP texGround;
    EZCOGL::Texture2D::SP normalTexGround;
    EZCOGL::Texture2D::SP texCube;
    EZCOGL::Texture2D::SP normalTexCube;
    EZCOGL::Texture2D::SP texSphere;
    EZCOGL::Texture2D::SP normalTexSphere;

	// Objects params
	EZCOGL::GLVec3 cubePos;
	EZCOGL::GLVec3 spherePos;
	EZCOGL::Mesh::SP meshCube;
	EZCOGL::Mesh::SP meshSphere;

	// Lights
	EZCOGL::GLVVec3 lightPositions;
	EZCOGL::GLVVec3 lightColors;
    float darkness;
	float biasMax;

	// Tone mapping
	float exposure;

	// Lighting options
	bool attenuation;
	bool gammaCorrection;
	bool toneMapping;
	bool normalMap;
	bool shadowMap;

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

Viewer::Viewer() : cubePos(2.f, -3.f, -16.f), spherePos(-3.f, -3.f, -13.f), biasMax(0.05f), darkness(0.7), exposure(5.f), attenuation(true), gammaCorrection(true), toneMapping(true), normalMap(true), shadowMap(true)
{}

void Viewer::init_ogl()
{
	// -------------------------------------------------------------------
    // Init Shader Programs
	// -------------------------------------------------------------------
	lighting_shader_prog = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP8.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP8.fs")}}, "Advanced lighting");
	depth_map_shader_prog = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP8_shadow_map.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP8_shadow_map.fs")}}, "shadow_map");
	// -------------------------------------------------------------------
    // GEOMETRY
	// -------------------------------------------------------------------
	// Create geometry : Ground of the scene (a flat cube)
    auto me = EZCOGL::Mesh::Cube();
	// get the associated renderer with positions(1), normals(2), texCoords(3) and tangents(4) VBOs
	ground_rend = me->renderer(1, 2, 3, 4, -1);

	set_scene_center(me->BB()->center());
	set_scene_radius(me->BB()->radius() + 50.f);
	this->cam_.frame_.translation().z() += 100.f;

	// Create geometry : mesh of a cube
	meshCube = EZCOGL::Mesh::Cube();
	cube_rend = meshCube->renderer(1, 2, 3, 4, -1);
	// Create geometry : mesh of a sphere
	meshSphere = EZCOGL::Mesh::Sphere(64);
	sphere_rend = meshSphere->renderer(1, 2, 3, 4, -1);

	// -------------------------------------------------------------------
	// Textures
	// -------------------------------------------------------------------
	texGround = EZCOGL::Texture2D::create({GL_LINEAR, GL_REPEAT});
	texGround->load(DATA_PATH + "/ground/Terracotta_Tiles_006_basecolor.jpg", false);
    normalTexGround = EZCOGL::Texture2D::create({GL_LINEAR, GL_REPEAT});
	normalTexGround->load(DATA_PATH + "/ground/Terracotta_Tiles_006_normal.jpg");

    texCube = EZCOGL::Texture2D::create({GL_LINEAR, GL_REPEAT});
	texCube->load(DATA_PATH + "/wood_025/Wood_025_basecolor.jpg", false);
    normalTexCube = EZCOGL::Texture2D::create({GL_LINEAR, GL_REPEAT});
	normalTexCube->load(DATA_PATH + "/wood_025/Wood_025_normal.jpg");
	
    texSphere = EZCOGL::Texture2D::create({GL_LINEAR, GL_REPEAT});
	texSphere->load(DATA_PATH + "/gems_003/Crystal_003_COLOR.jpeg", false);
    normalTexSphere = EZCOGL::Texture2D::create({GL_LINEAR, GL_REPEAT});
	normalTexSphere->load(DATA_PATH + "/gems_003/Crystal_003_NORM.jpeg");


	// -------------------------------------------------------------------
	// FBO
	// -------------------------------------------------------------------
	// create a FBO with no texture attached to it
	fbo_depth = EZCOGL::FBO_DepthTexture::create({/*EMPTY : no color buffer*/});
	fbo_depth->resize(8192, 8192);

	// ***************
	// Lights init
    // ***************
	lightPositions = EZCOGL::GLVVec3{{7.f, 15.f, -9.f}/*Back light*/, {-3.f, -4.f, 9.f}, {0.f, -4.f, 3.4f}, {3.f, -4.f, 6.f}};
	lightColors = EZCOGL::GLVVec3{{200.f, 200.f, 200.f}/*Back light*/, {1.f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.f}, {0.0f, 1.f, 0.0f}};

	// Define the color to use when refreshing screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Viewer::draw_ogl()
{
	const EZCOGL::GLMat4& modelGround = EZCOGL::Transfo::translate(0, -1, 0) * EZCOGL::Transfo::scale(8.f, 0.05f, 8.f);
    const EZCOGL::GLMat4& modelCube = EZCOGL::Transfo::translate(-2, 2, -3) * EZCOGL::Transfo::rotateX(45.f) * EZCOGL::Transfo::scale(1.f);
    const EZCOGL::GLMat4& modelSphere = EZCOGL::Transfo::translate(1, sin(EZCOGL::current_time())*3, 1) * EZCOGL::Transfo::scale(1.f);

	// View & proj matrix for light point of view
	EZCOGL::GLVec3 lookDir = this->get_camera().pivot_point_f() - lightPositions[0];
	float radius = this->get_camera().scene_radius();
	EZCOGL::GLMat4 lView = EZCOGL::Transfo::look_dir(lightPositions[0], lookDir, EZCOGL::GLVec3(0.f, 1.0, 0.f));
	EZCOGL::GLMat4 lProj = EZCOGL::Transfo::ortho(radius, radius, std::max(lookDir.norm() - radius, 0.01f), lookDir.norm() + radius).transpose();

	// "Classic" view and projection matrix
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();
	const EZCOGL::GLMat4& view = this->get_view_matrix();

	// -------------------------------------------------------------------
	// 1st : light depth map (for shadow mapping)
	// -------------------------------------------------------------------

	// Push the current framebuffer (the default OpenGL framebuffer) and viewport settings on top of a stack
	EZCOGL::FBO::push();
	// Bind the current framebuffer with your FBO and set the viewport settings with the size of this FBO
	fbo_depth->bind();

	// Clear the GL "depth" framebuffer
	glClear(GL_DEPTH_BUFFER_BIT);

	// Enable the Z Depth test
	glEnable(GL_DEPTH_TEST);

	// Set "current" shader program
	depth_map_shader_prog->bind();

	// Shadow map only for the cube and the sphere

    // 1. CUBE
	EZCOGL::set_uniform_value(1, modelCube);
	EZCOGL::set_uniform_value(2, lView);
	EZCOGL::set_uniform_value(3, lProj);
	cube_rend->draw(GL_TRIANGLES);

    // 2. SPHERE
	EZCOGL::set_uniform_value(1, modelSphere);
	sphere_rend->draw(GL_TRIANGLES);

    // -------------------------------------------------------------------
	// 2nd : shading pass
	// -------------------------------------------------------------------

    // - reset GL state (unbind the framebuffer, and revert to default)
	// Pop the framebuffer on top of the stack to get back the default OpenGL frambuffer and viewport settings
	EZCOGL::FBO::pop();

	// Clear the GL "color" and "depth" framebuffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set "current" shader program
	lighting_shader_prog->bind();

    // 1. GROUND
	EZCOGL::set_uniform_value(0, modelGround);
	EZCOGL::set_uniform_value(1, proj);
	EZCOGL::set_uniform_value(2, view);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(modelGround));
	EZCOGL::set_uniform_value(4, lightPositions);
	EZCOGL::set_uniform_value(8, lightColors); // previous uniform is an array of size 4 so 4 + 4 = 8 for the next id
	EZCOGL::set_uniform_value(12, attenuation); // previous uniform is an array of size 4 so 8 + 4 = 12 for the next id
	EZCOGL::set_uniform_value(13, gammaCorrection);
	EZCOGL::set_uniform_value(14, toneMapping);
	EZCOGL::set_uniform_value(15, exposure);
	EZCOGL::set_uniform_value(16, normalMap);
	EZCOGL::set_uniform_value(17, shadowMap);
	EZCOGL::set_uniform_value(18, darkness);
	EZCOGL::set_uniform_value(19, 1.f); // texture tilling factor
	EZCOGL::set_uniform_value(20, lView);
	EZCOGL::set_uniform_value(21, lProj);
	EZCOGL::set_uniform_value(22, biasMax);
	texGround->bind(0);
	normalTexGround->bind(1);
    fbo_depth->depth_texture()->bind(2);
	ground_rend->draw(GL_TRIANGLES);
	glDisable(GL_CULL_FACE);

    // 2. CUBE
    EZCOGL::set_uniform_value(0, modelCube);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(modelCube));
	EZCOGL::set_uniform_value(19, 1.f); // texture tilling factor
	texCube->bind(0);
	normalTexCube->bind(1);
	cube_rend->draw(GL_TRIANGLES);

    // 3. SPHERE
	EZCOGL::set_uniform_value(0, modelSphere);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(modelSphere));
	EZCOGL::set_uniform_value(19, 10.f); // texture tilling factor
	texSphere->bind(0);
	normalTexSphere->bind(1);
	sphere_rend->draw(GL_TRIANGLES);
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::Begin("Advanced Lighting",nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});
	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders"))
    {
	    lighting_shader_prog = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP8.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP8.fs")}}, "Advanced lighting");
	    depth_map_shader_prog = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP8_shadow_map.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/TP8_shadow_map.fs")}}, "shadow_map");
    }

	ImGui::Checkbox("Attenuation", &attenuation);
	ImGui::Checkbox("Gamma correction", &gammaCorrection);
	// if (attenuation && gammaCorrection)
		ImGui::Checkbox("Tone mapping", &toneMapping);
    if (toneMapping) ImGui::SliderFloat("Exposure", &exposure, 0.f, 5.f);
	ImGui::Checkbox("Normal map", &normalMap);
    if (normalMap)
		if (ImGui::CollapsingHeader("Normal map textures content"))
		{
			ImGui::Image(reinterpret_cast<ImTextureID>(normalTexGround->id()), ImVec2(400, 400), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::SameLine();
			ImGui::Image(reinterpret_cast<ImTextureID>(normalTexCube->id()), ImVec2(400, 400), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::SameLine();
			ImGui::Image(reinterpret_cast<ImTextureID>(normalTexSphere->id()), ImVec2(400, 400), ImVec2(0, 1), ImVec2(1, 0));
		}
	ImGui::Checkbox("Shadow map", &shadowMap);
    if (shadowMap)
    {
		if (ImGui::CollapsingHeader("Depth texture content"))
			ImGui::Image(reinterpret_cast<ImTextureID>(fbo_depth->depth_texture()->id()), ImVec2(400, 400), ImVec2(0, 1), ImVec2(1, 0));
    	ImGui::SliderFloat("Darkness", &darkness, 0.f, 1.f);
		ImGui::SliderFloat("Bias max", &biasMax, 0.01f, 0.5f);
	}

	if (ImGui::CollapsingHeader("Lights"))
	{
		ImGui::SliderFloat3("Back light pos", lightPositions[0].data(), -100.f, 100.f);
		ImGui::SliderFloat3("Back light color", lightColors[0].data(), 0.f, 200.f);
		ImGui::SliderFloat3("Red light pos", lightPositions[1].data(), -10.f, 10.f);
		ImGui::SliderFloat3("Blue light pos", lightPositions[2].data(), -10.f, 10.f);
		ImGui::SliderFloat3("Green light pos", lightPositions[3].data(), -10.f, 10.f);
	}

	if (ImGui::CollapsingHeader("Objects"))
	{
		ImGui::SliderFloat("Cube X", &cubePos[0], -10.f, 10.f);
		ImGui::SliderFloat("Cube Y", &cubePos[1], -10.f, 10.f);
		ImGui::SliderFloat("Cube Z", &cubePos[2], -20.f, 20.f);
		ImGui::SliderFloat("Sphere X", &spherePos[0], -10.f, 10.f);
		ImGui::SliderFloat("Sphere Y", &spherePos[1], -10.f, 10.f);
		ImGui::SliderFloat("Sphere Z", &spherePos[2], -20.f, 20.f);
	}

	ImGui::End();
}
