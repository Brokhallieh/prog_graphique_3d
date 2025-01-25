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

#define PI 3.141592653589793f

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

    EZCOGL::Texture2D::SP texAsteroid;

	EZCOGL::Mesh::SP meshSphere;
	
	float scaleBuff;

	std::vector<EZCOGL::MeshRenderer::UP> asteroids_rend;
	std::vector<EZCOGL::GLMat4> modelAsteroids;
	int nbMeshParts;
	std::vector<EZCOGL::GLVec3> ka; // Ambient
	std::vector<EZCOGL::GLVec3> kd; // Diffus
	std::vector<EZCOGL::GLVec3> ks; // Specular
	std::vector<float> ns; // Shininess
	int nbAsteroids;

public:
	Viewer();
	void init_ogl() override;
	void draw_ogl() override;
	void interface_ogl() override;
	void rendCelestialBody(const EZCOGL::GLMat4 &model, EZCOGL::Texture2D::SP *tex);
	void randomModelMatrices(std::vector<EZCOGL::GLMat4> &matrices, int nb);
};

int main(int, char**)
{
	Viewer v;
	return v.launch3d();
}

Viewer::Viewer() : nbAsteroids(10000), scaleBuff(20.f)
{}

EZCOGL::Texture2D::SP textureCelestialBody(const std::string &filename)
{
	EZCOGL::Texture2D::SP tex = EZCOGL::Texture2D::create({GL_LINEAR, GL_REPEAT});
	tex->load(DATA_PATH + filename, false);
	return tex;
}

void Viewer::randomModelMatrices(std::vector<EZCOGL::GLMat4> &matrices, int nb)
{
	matrices.reserve(nb);
	std::srand(std::time(nullptr));
	for (int i = 0; i < nb; ++i)
	{
		// Translations
		float LO = 300000.f;
		float HI = 600000.f;
		float tX = LO + static_cast<float>(rand()) / (static_cast <float>(RAND_MAX/(HI-LO)));
		LO = -50000;
		HI = 50000;
		float tZ = LO + static_cast<float>(rand()) / (static_cast <float>(RAND_MAX/(HI-LO)));
		// Rotation around the star
		LO = -180.f;
		HI = 180.f;
		float radian = LO + static_cast<float>(rand()) / (static_cast <float>(RAND_MAX/(HI-LO)));
		// Scale
		LO = 0.5f;
		HI = 2.f;
		float sc = (LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(HI-LO)))) * scaleBuff;
		// Rotations on itself
		LO = -90.f;
		HI = 90.f;
		float rX = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(HI-LO)));
		float rY = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(HI-LO)));
		float rZ = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(HI-LO)));
		EZCOGL::GLMat4 mat = EZCOGL::Transfo::rotateZ(radian) *
							 EZCOGL::Transfo::translate(tX, 0.f, tZ) *
							 EZCOGL::Transfo::rotateZ(rZ) *
							 EZCOGL::Transfo::rotateY(rY) *
							 EZCOGL::Transfo::rotateX(rX) *
							 EZCOGL::Transfo::scale(sc);
		matrices.emplace_back(mat);
	}
}

void Viewer::init_ogl()
{
	shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")}}, "projet");

	auto meshSphere = EZCOGL::Mesh::Sphere(128); // Each celestial body takes an argument for the number of triangle rafinement
	sphere_rend = meshSphere->renderer(1, -1, 3, -1, -1);

	auto mesh = EZCOGL::Mesh::load(DATA_PATH + "/projet/rock/rock.obj")->data();
	nbMeshParts = mesh.size();
	for (int i = 0; i < nbMeshParts; ++i) {
		asteroids_rend.push_back(mesh[i]->renderer(1, 2, 3, -1, -1));
		ka.push_back(mesh[i]->material()->Ka / 100.f); // ambient material coefficient
		kd.push_back(mesh[i]->material()->Kd); // diffus material coefficient
		ks.push_back(mesh[i]->material()->Ks); // specular material coefficient
		ns.push_back(mesh[i]->material()->Ns); // shininess of the specular material
	}

	randomModelMatrices(modelAsteroids, nbAsteroids);

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

	texAsteroid = textureCelestialBody("/projet/rock/rock.png");

	// Define the color to use when refreshing screen
    glClearColor(0.1, 0.1, 0.1, 1.0);
}

EZCOGL::GLMat4 modelCelestialBody(float distanceToStar, float scale, float obliquity, float siderealPeriod, float revolutionPeriod)
{
	float rotationTime = 2 * PI * EZCOGL::current_time() / revolutionPeriod;
	float secondsTo360Degrees = EZCOGL::current_time() * 360.f;
	return EZCOGL::Transfo::translate(cos(rotationTime) * distanceToStar, sin(rotationTime) * distanceToStar, 0.f) * EZCOGL::Transfo::scale(scale)
		 * EZCOGL::Transfo::rotateY(obliquity) * EZCOGL::Transfo::rotateZ(secondsTo360Degrees / siderealPeriod);
}

void Viewer::rendCelestialBody(const EZCOGL::GLMat4 &model, EZCOGL::Texture2D::SP *tex)
{
	EZCOGL::set_uniform_value(0, model);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(model));
	(*tex)->bind(0);
	sphere_rend->draw(GL_TRIANGLES);
}

void Viewer::draw_ogl()
{
	// Clear the buffer before to draw the next frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the view and projection matrix
	const EZCOGL::GLMat4& view = this->get_view_matrix() * EZCOGL::Transfo::rotateX(180.f);
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();

    // Construct a model matrix for every celestial body with distanceToStar, scale, obliquity, siderealPeriod, revolution perdiod
    const EZCOGL::GLMat4& modelSun = EZCOGL::Transfo::scale(1392.684f * scaleBuff);
	const EZCOGL::GLMat4& modelMercury = modelCelestialBody(57909.f, 4.8794f * scaleBuff, 0.1f, 58.64f, 88.f);
	const EZCOGL::GLMat4& modelVenus = modelCelestialBody(108160.f, 12.1036f * scaleBuff, 177.f, 243.01f, 224.7f);
	const EZCOGL::GLMat4& modelEarth = modelCelestialBody(149600.f, 12.7563f * scaleBuff, 23.f, 23.93f / 24.f, 356.25f);
	const EZCOGL::GLMat4& modelMars = modelCelestialBody(227990.f, 6.7924f * scaleBuff, 25.f, 24.62f / 24.f, 689.f);
	const EZCOGL::GLMat4& modelJupiter = modelCelestialBody(778360.f, 142.984f * scaleBuff, 3.f, 9.92f / 24.f, 365.25f * 11.87f);
	const EZCOGL::GLMat4& modelSaturn = modelCelestialBody(1433500.f, 120.536f * scaleBuff, 27.f, 10.65f / 24.f, 365.25f * 29.45f);
	const EZCOGL::GLMat4& modelUranus = modelCelestialBody(2872400.f, 51.118f * scaleBuff, 98.f, 17.24f / 24.f, 365.25f * 84.07f);
	const EZCOGL::GLMat4& modelNeptune = modelCelestialBody(4498400.f, 49.528f * scaleBuff, 30.f, 16.11f / 24.f, 365.25f * 164.89f);

	// activate Z-buffer
	glEnable(GL_DEPTH_TEST);
	shaderPrg->bind();
	// Uniforms variables send to the GPU
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);

	rendCelestialBody(modelSun, &texSun);
	rendCelestialBody(modelMercury, &texMercury);
	rendCelestialBody(modelVenus, &texVenus);
	rendCelestialBody(modelEarth, &texEarth);
	rendCelestialBody(modelMars, &texMars);
	rendCelestialBody(modelJupiter, &texJupiter);
	rendCelestialBody(modelSaturn, &texSaturn);
	rendCelestialBody(modelUranus, &texUranus);
	rendCelestialBody(modelNeptune, &texNeptune);

	for (int i = 0; i < nbAsteroids; i++)
	{
		EZCOGL::set_uniform_value(0, modelAsteroids[i]);
		texAsteroid->bind(0);
		for (int j = 0; j < nbMeshParts; j++)
		{
			EZCOGL::set_uniform_value(6, ka[j]);
			EZCOGL::set_uniform_value(7, kd[j]);
			EZCOGL::set_uniform_value(8, ks[j]);
			EZCOGL::set_uniform_value(9, ns[j]);
			asteroids_rend[j]->draw(GL_TRIANGLES);
		}
	}
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::Begin("Projet", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});

	//ImGui::SliderFloat("Scale buff", &scaleBuff, 0.1f, 30.f);

	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders"))
		shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")}}, "projet");

	ImGui::End();
}
