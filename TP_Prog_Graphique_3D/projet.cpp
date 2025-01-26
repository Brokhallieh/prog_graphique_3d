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
#include "texturecubemap.h"

#define macro_str(s) #s
#define macro_xstr(s) macro_str(s)
#define DATA_PATH std::string(macro_xstr(DEF_DATA_PATH))
#define SHADERS_PATH std::string(macro_xstr(DEF_SHADERS_PATH))

#define PI 3.141592653589793f

// using namespace EZCOGL;

class Viewer: public EZCOGL::GLViewer
{
	EZCOGL::ShaderProgram::UP shaderPrg;
    EZCOGL::ShaderProgram::UP shaderPrgCube;
    EZCOGL::ShaderProgram::UP shaderPrgSun;
    EZCOGL::ShaderProgram::UP shaderPrgEarth;

	EZCOGL::MeshRenderer::UP sphere_rend;
	EZCOGL::MeshRenderer::UP cube_rend;

	// Textures
    EZCOGL::Texture2D::SP texSun;
    EZCOGL::Texture2D::SP texMercury;
    EZCOGL::Texture2D::SP texVenus;
    EZCOGL::Texture2D::SP texEarth;
    EZCOGL::Texture2D::SP texCloudsEarth;
    EZCOGL::Texture2D::SP texEarthNight;
    EZCOGL::Texture2D::SP texMars;
    EZCOGL::Texture2D::SP texJupiter;
    EZCOGL::Texture2D::SP texSaturn;
    EZCOGL::Texture2D::SP texUranus;
    EZCOGL::Texture2D::SP texNeptune;

    EZCOGL::Texture2D::SP texAsteroid;

	EZCOGL::TextureCubeMap::SP tex_envMap;

	float scaleBuffAll;
	float scaleBuffTelluric;
	float scaleBuffAsteroids;
	float scaleBuffGiants;

	float timeBuff;

	float secondsTo360Degrees;

	float previous_time;

	std::vector<EZCOGL::MeshRenderer::UP> asteroids_rend;
	std::vector<EZCOGL::GLMat4> modelAsteroids;
	int nbMeshParts;
	int nbAsteroids;

	std::vector<EZCOGL::GLVec3> kaAsteroids; // Ambient
	std::vector<EZCOGL::GLVec3> kdAsteroids; // Diffus
	std::vector<EZCOGL::GLVec3> ksAsteroids; // Specular
	std::vector<float> nsAsteroids; // Shininess

	EZCOGL::GLVec3 ka; // Ambient
	EZCOGL::GLVec3 ks; // Specular
	float ns; // Shininess

	EZCOGL::GLVec3 lightPos;
	float intensity;

public:
	Viewer();
	void init_ogl() override;
	void draw_ogl() override;
	void interface_ogl() override;
	EZCOGL::Texture2D::SP textureCelestialBody(const std::string &filename);
	void randomModelMatrices(std::vector<EZCOGL::GLMat4> &matrices, int nb);
	EZCOGL::GLMat4 modelCelestialBody(float distanceToStar, float scale, float obliquity, float siderealPeriod, float revolutionPeriod);
	void rendCelestialBody(const EZCOGL::GLMat4 &model, EZCOGL::Texture2D::SP *tex, const EZCOGL::GLMat4 &view);
};

int main(int, char**)
{
	Viewer v;
	return v.launch3d();

}

Viewer::Viewer() : scaleBuffAll(20.f), scaleBuffTelluric(15.f), scaleBuffAsteroids(30.f), scaleBuffGiants(4.f), timeBuff(1.f), previous_time(EZCOGL::current_time()),
				   secondsTo360Degrees(0.f), nbAsteroids(1000), ka(0.02f, 0.02f, 0.02f), ns(15.f), lightPos(0.f, 0.f, 0.f), intensity(15.f)
{}

EZCOGL::Texture2D::SP Viewer::textureCelestialBody(const std::string &filename)
{
	EZCOGL::Texture2D::SP tex = EZCOGL::Texture2D::create();
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
		float LO = 314555.527f;
		float HI = 493672.971f;
		float tX = LO + static_cast<float>(rand()) / (static_cast <float>(RAND_MAX/(HI-LO)));
		LO = -74798.935f;
		HI = 74798.935f;
		float tZ = LO + static_cast<float>(rand()) / (static_cast <float>(RAND_MAX/(HI-LO)));
		// Rotation around the star
		LO = -180.f;
		HI = 180.f;
		float degrees = LO + static_cast<float>(rand()) / (static_cast <float>(RAND_MAX/(HI-LO)));
		// Scale
		LO = 0.01f;
		HI = 1.f;
		float sc = (LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(HI-LO)))) * scaleBuffAll * scaleBuffAsteroids;
		// Rotations on itself
		LO = -180.f;
		HI = 180.f;
		float rX = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(HI-LO)));
		float rY = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(HI-LO)));
		float rZ = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(HI-LO)));
		EZCOGL::GLMat4 mat = EZCOGL::Transfo::rotateZ(degrees) *
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
	shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")}}, "Main");
	shaderPrgCube = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetSkybox.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetSkybox.fs")}}, "Skybox");
	shaderPrgSun = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetSoleil.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetSoleil.fs")}}, "Soleil");
	shaderPrgEarth = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetTerre.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetTerre.fs")}}, "Terre");

	auto meshSphere = EZCOGL::Mesh::Sphere(128); // Each celestial body takes an argument for the number of triangle rafinement
	sphere_rend = meshSphere->renderer(1, 2, 3, -1, -1);

	auto meshAsteroid = EZCOGL::Mesh::load(DATA_PATH + "/projet/rock/rock.obj")->data();
	nbMeshParts = meshAsteroid.size();
	for (int i = 0; i < nbMeshParts; ++i) {
		asteroids_rend.push_back(meshAsteroid[i]->renderer(1, 2, 3, -1, -1));
		kaAsteroids.push_back(ka); // ambient material coefficient
		kdAsteroids.push_back(meshAsteroid[i]->material()->Kd); // diffus material coefficient
		ksAsteroids.push_back(meshAsteroid[i]->material()->Ks); // specular material coefficient
		nsAsteroids.push_back(meshAsteroid[i]->material()->Ns); // shininess of the specular material
	}

	randomModelMatrices(modelAsteroids, nbAsteroids);

	auto meshCube = EZCOGL::Mesh::CubePosOnly();
    cube_rend = meshCube->renderer(1, -1, -1, -1, -1);

	// set scene center and radius for the init of matrix view/proj
	set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
	set_scene_radius(200000.f);

	texSun = textureCelestialBody("/projet/textures/2k_sun.jpg");
	texMercury = textureCelestialBody("/projet/textures/2k_mercury.jpg");
	texVenus = textureCelestialBody("/projet/textures/2k_venus.jpg");
	texEarth = textureCelestialBody("/projet/textures/2k_earth_daymap.jpg");
	texCloudsEarth = textureCelestialBody("/projet/textures/2k_earth_clouds.jpg");
	texEarthNight = textureCelestialBody("/projet/textures/2k_earth_nightmap.jpg");
	texMars = textureCelestialBody("/projet/textures/2k_mars.jpg");
	texJupiter = textureCelestialBody("/projet/textures/2k_jupiter.jpg");
	texSaturn = textureCelestialBody("/projet/textures/2k_saturn.jpg");
	texUranus = textureCelestialBody("/projet/textures/2k_uranus.jpg");
	texNeptune = textureCelestialBody("/projet/textures/2k_neptune.jpg");

	texAsteroid = textureCelestialBody("/projet/rock/rock.png");

	tex_envMap = EZCOGL::TextureCubeMap::create();
	tex_envMap->load({DATA_PATH + "/projet/textures/skybox/skybox_1k.png", DATA_PATH +  "/projet/textures/skybox/skybox_1k.png", DATA_PATH + "/projet/textures/skybox/skybox_1k.png",
					  DATA_PATH + "/projet/textures/skybox/skybox_1k.png", DATA_PATH + "/projet/textures/skybox/skybox_1k.png", DATA_PATH + "/projet/textures/skybox/skybox_milky_way_1k.png"});

	// Define the color to use when refreshing screen
    glClearColor(0.1, 0.1, 0.1, 1.0);
}

EZCOGL::GLMat4 Viewer::modelCelestialBody(float distanceToStar, float scale, float obliquity, float siderealPeriod, float revolutionPeriod)
{
	float current_time = EZCOGL::current_time();
	secondsTo360Degrees += (current_time - previous_time) * 360.f * timeBuff;
	previous_time = current_time;
	return EZCOGL::Transfo::rotateZ(-secondsTo360Degrees / revolutionPeriod) * EZCOGL::Transfo::translate(distanceToStar, 0.f, 0.f) * EZCOGL::Transfo::scale(scale * scaleBuffAll)
		 * EZCOGL::Transfo::rotateY(obliquity) * EZCOGL::Transfo::rotateZ(-secondsTo360Degrees / siderealPeriod);
}

void Viewer::rendCelestialBody(const EZCOGL::GLMat4 &model, EZCOGL::Texture2D::SP *tex, const EZCOGL::GLMat4 &view)
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
	EZCOGL::GLMat4 view2 = view;
    view2.block<3,1>(0, 3).setZero();
    view2.block<3,1>(0, 0).normalize(); 
    view2.block<3,1>(0, 1).normalize(); 
    view2.block<3,1>(0, 2).normalize();
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();

    const EZCOGL::GLMat4& modelSun = EZCOGL::Transfo::scale(1392.684f * scaleBuffAll);

    // Construct a model matrix for every celestial body with distanceToStar, scale, obliquity, siderealPeriod, revolution perdiod
	const EZCOGL::GLMat4& modelMercury = modelCelestialBody(57909.f, 4.8794f * scaleBuffTelluric, 0.1f, 58.64f, 88.f);
	const EZCOGL::GLMat4& modelVenus = modelCelestialBody(108160.f, 12.1036f * scaleBuffTelluric, 177.f, 243.01f, 224.7f);
	const EZCOGL::GLMat4& modelEarth = modelCelestialBody(149600.f, 12.7563f * scaleBuffTelluric, 23.f, 23.93f / 24.f, 365.25f);
	const EZCOGL::GLMat4& modelMars = modelCelestialBody(227990.f, 6.7924f * scaleBuffTelluric, 25.f, 24.62f / 24.f, 689.f);
	const EZCOGL::GLMat4& modelJupiter = modelCelestialBody(778360.f, 142.984f * scaleBuffGiants, 3.f, 9.92f / 24.f, 365.25f * 11.87f);
	const EZCOGL::GLMat4& modelSaturn = modelCelestialBody(1433500.f, 120.536f * scaleBuffGiants, 27.f, 10.65f / 24.f, 365.25f * 29.45f);
	const EZCOGL::GLMat4& modelUranus = modelCelestialBody(2872400.f, 51.118f * scaleBuffGiants, 98.f, 17.24f / 24.f, 365.25f * 84.07f);
	const EZCOGL::GLMat4& modelNeptune = modelCelestialBody(4498400.f, 49.528f * scaleBuffGiants, 30.f, 16.11f / 24.f, 365.25f * 164.89f);

	for (int i = 0; i < nbAsteroids; ++i)
	{
		float currentTimeToDegree = (EZCOGL::current_time() -  previous_time) * timeBuff;
		modelAsteroids[i] *= EZCOGL::Transfo::rotateX(currentTimeToDegree * 360.f);
		modelAsteroids[i] = EZCOGL::Transfo::rotateZ(-currentTimeToDegree) * modelAsteroids[i];
	}

    glDisable(GL_DEPTH_TEST);
    shaderPrgCube->bind();
	EZCOGL::set_uniform_value(0, view2);
	EZCOGL::set_uniform_value(1, proj);
    tex_envMap->bind(0);
    cube_rend->draw(GL_TRIANGLES);

	glEnable(GL_DEPTH_TEST);
	shaderPrgSun->bind();
	EZCOGL::set_uniform_value(0, modelSun);
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	texSun->bind(0);
	sphere_rend->draw(GL_TRIANGLES);

	shaderPrg->bind();
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(intensity, intensity, intensity));
	EZCOGL::set_uniform_value(5, lightPos);
	EZCOGL::set_uniform_value(6, ka);
	ks = EZCOGL::GLVec3(0.65f, 0.65f, 0.65f);
	EZCOGL::set_uniform_value(8, ks);
	EZCOGL::set_uniform_value(9, ns);
	bool isPlanet = true;
	EZCOGL::set_uniform_value(10, isPlanet);

	rendCelestialBody(modelMercury, &texMercury, view);
	rendCelestialBody(modelVenus, &texVenus, view);

	shaderPrgEarth->bind();
	EZCOGL::set_uniform_value(0, modelEarth);
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(modelEarth));
	EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(intensity, intensity, intensity));
	EZCOGL::set_uniform_value(5, lightPos);
	EZCOGL::set_uniform_value(6, ka);
	EZCOGL::set_uniform_value(7, ks);
	EZCOGL::set_uniform_value(8, ns);

	texEarth->bind(0);
	texCloudsEarth->bind(1);
	texEarthNight->bind(2);
	sphere_rend->draw(GL_TRIANGLES);

	shaderPrg->bind();
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(intensity, intensity, intensity));
	EZCOGL::set_uniform_value(5, lightPos);
	EZCOGL::set_uniform_value(6, ka);
	EZCOGL::set_uniform_value(8, ks);
	EZCOGL::set_uniform_value(9, ns);
	EZCOGL::set_uniform_value(10, isPlanet);
	rendCelestialBody(modelMars, &texMars, view);
	ks = EZCOGL::GLVec3(0.2f, 0.2f, 0.2f);
	EZCOGL::set_uniform_value(8, ks);
	rendCelestialBody(modelJupiter, &texJupiter, view);
	rendCelestialBody(modelSaturn, &texSaturn, view);
	rendCelestialBody(modelUranus, &texUranus, view);
	rendCelestialBody(modelNeptune, &texNeptune, view);

	isPlanet = false;
	EZCOGL::set_uniform_value(10, isPlanet);
	for (int i = 0; i < nbAsteroids; i++)
	{
		EZCOGL::set_uniform_value(0, modelAsteroids[i]);
		EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(view * modelAsteroids[i]));
		EZCOGL::set_uniform_value(5, EZCOGL::Transfo::sub33(view * modelAsteroids[i]) * lightPos);
		texAsteroid->bind(0);
		for (int j = 0; j < nbMeshParts; j++)
		{
			EZCOGL::set_uniform_value(6, kaAsteroids[j]);
			EZCOGL::set_uniform_value(7, kdAsteroids[j]);
			EZCOGL::set_uniform_value(8, ksAsteroids[j]);
			EZCOGL::set_uniform_value(9, nsAsteroids[j]);
			asteroids_rend[j]->draw(GL_TRIANGLES);
		}
	}
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::Begin("Projet", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});

	//ImGui::SliderFloat("Scale buff All", &scaleBuffAll, 1.f, 30.f);
	//ImGui::SliderFloat("Scale buff Telluric", &scaleBuffTelluric, 1.f, 50.f);
	//ImGui::SliderFloat("Scale buff Giants", &scaleBuffGiants, 1.f, 30.f);
	ImGui::SliderFloat("Nombre de jours par seconde", &timeBuff, 0.05f, 100.f);

	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Recharger les shaders"))
	{
		shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")}}, "Main");
		shaderPrgCube = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetSkybox.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetSkybox.fs")}}, "Skybox");
		shaderPrgCube = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetSoleil.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetSoleil.fs")}}, "Soleil");
		shaderPrgEarth = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetTerre.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projetTerre.fs")}}, "Terre");
	}

	ImGui::End();
}
