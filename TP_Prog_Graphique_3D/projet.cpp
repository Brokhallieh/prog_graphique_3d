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
	EZCOGL::ShaderProgram::UP shaderPrg;

	EZCOGL::MeshRenderer::UP sphere_rend;

	// Textures
    EZCOGL::Texture2D::SP texSun;
    EZCOGL::Texture2D::SP texMercury;
    EZCOGL::Texture2D::SP texVenus;
    EZCOGL::Texture2D::SP texEarth;
    EZCOGL::Texture2D::SP texMars;
    EZCOGL::Texture2D::SP texJupiter;
    EZCOGL::Texture2D::SP texSaturn;
    EZCOGL::Texture2D::SP texUranus;
    EZCOGL::Texture2D::SP texNeptune;

	EZCOGL::Mesh::SP meshSphere;

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

EZCOGL::Texture2D::SP textureCelestialBody(const std::string &filename)
{
	EZCOGL::Texture2D::SP tex = EZCOGL::Texture2D::create({GL_LINEAR, GL_REPEAT});
	tex->load(DATA_PATH + filename, false);
	return tex;
}

void Viewer::init_ogl()
{
	shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")}}, "projet");

	auto meshSphere = EZCOGL::Mesh::Sphere(128); // Each celestial body takes an argument for the number of triangle rafinement
	sphere_rend = meshSphere->renderer(1, -1, 2, -1, -1);

	// set scene center and radius for the init of matrix view/proj
	set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
	set_scene_radius(200000.f);

	texSun = textureCelestialBody("/projet/textures/2k_sun.jpg");
	texMercury = textureCelestialBody("/projet/textures/2k_mercury.jpg");
	texVenus = textureCelestialBody("/projet/textures/2k_venus.jpg");
	texEarth = textureCelestialBody("/projet/textures/2k_earth_daymap.jpg");
	texMars = textureCelestialBody("/projet/textures/2k_mars.jpg");
	texJupiter = textureCelestialBody("/projet/textures/2k_jupiter.jpg");
	texSaturn = textureCelestialBody("/projet/textures/2k_saturn.jpg");
	texUranus = textureCelestialBody("/projet/textures/2k_uranus.jpg");
	texNeptune = textureCelestialBody("/projet/textures/2k_neptune.jpg");

	// Define the color to use when refreshing screen
    glClearColor(0.1, 0.1, 0.1, 1.0);
}

void rendCelestialBody(const EZCOGL::GLMat4 &model, EZCOGL::Texture2D::SP *tex, EZCOGL::MeshRenderer::UP *rend)
{
	EZCOGL::set_uniform_value(0, model);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(model));
	(*tex)->bind(0);
	(*rend)->draw(GL_TRIANGLES);
}

void Viewer::draw_ogl()
{
	// Clear the buffer before to draw the next frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the view and projection matrix
	const EZCOGL::GLMat4& view = this->get_view_matrix() * EZCOGL::Transfo::rotateX(180.f);
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();

    // Construct a model matrix for both object
    const EZCOGL::GLMat4& modelSun = EZCOGL::Transfo::scale(1392.684f);
    const EZCOGL::GLMat4& modelMercury = EZCOGL::Transfo::translate(5790.9f, 0.f, 0.f) * EZCOGL::Transfo::scale(48.794f) * EZCOGL::Transfo::rotateY(0.1f);
    const EZCOGL::GLMat4& modelVenus =  EZCOGL::Transfo::translate(10816.0f, 0.f, 0.f) * EZCOGL::Transfo::scale(121.036f) * EZCOGL::Transfo::rotateY(177.f);
    const EZCOGL::GLMat4& modelEarth =  EZCOGL::Transfo::translate(14960.0f, 0.f, 0.f) * EZCOGL::Transfo::scale(127.563f) * EZCOGL::Transfo::rotateY(23.f);
    const EZCOGL::GLMat4& modelMars =  EZCOGL::Transfo::translate(22799.0f, 0.f, 0.f) * EZCOGL::Transfo::scale(67.924f) * EZCOGL::Transfo::rotateY(25.f);
    const EZCOGL::GLMat4& modelJupiter =  EZCOGL::Transfo::translate(77836.0f, 0.f, 0.f) * EZCOGL::Transfo::scale(1429.84f) * EZCOGL::Transfo::rotateY(3.f);
    const EZCOGL::GLMat4& modelSaturn =  EZCOGL::Transfo::translate(143350.0f, 0.f, 0.f) * EZCOGL::Transfo::scale(1205.36f) * EZCOGL::Transfo::rotateY(27.f);
    const EZCOGL::GLMat4& modelUranus =  EZCOGL::Transfo::translate(287240.0f, 0.f, 0.f) * EZCOGL::Transfo::scale(511.18f) * EZCOGL::Transfo::rotateY(98.f);
    const EZCOGL::GLMat4& modelNeptune =  EZCOGL::Transfo::translate(449840.0f, 0.f, 0.f) * EZCOGL::Transfo::scale(495.28f) * EZCOGL::Transfo::rotateY(30.f);
	// activate Z-buffer
	glEnable(GL_DEPTH_TEST);
	shaderPrg->bind();
	// Uniforms variables send to the GPU
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);

	rendCelestialBody(modelSun, &texSun, &sphere_rend);
	rendCelestialBody(modelMercury, &texMercury, &sphere_rend);
	rendCelestialBody(modelVenus, &texVenus, &sphere_rend);
	rendCelestialBody(modelEarth, &texEarth, &sphere_rend);
	rendCelestialBody(modelMars, &texMars, &sphere_rend);
	rendCelestialBody(modelJupiter, &texJupiter, &sphere_rend);
	rendCelestialBody(modelSaturn, &texSaturn, &sphere_rend);
	rendCelestialBody(modelUranus, &texUranus, &sphere_rend);
	rendCelestialBody(modelNeptune, &texNeptune, &sphere_rend);
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::Begin("Projet", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});

	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders"))
		shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")}}, "projet");

	ImGui::End();
}
