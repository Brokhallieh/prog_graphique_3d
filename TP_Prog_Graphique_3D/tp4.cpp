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
#include <Eigen/Dense>

#define macro_str(s) #s
#define macro_xstr(s) macro_str(s)
#define DATA_PATH std::string(macro_xstr(DEF_DATA_PATH))
#define SHADERS_PATH std::string(macro_xstr(DEF_SHADERS_PATH))

// using namespace EZCOGL;

class Viewer: public EZCOGL::GLViewer
{
	EZCOGL::ShaderProgram::UP shaderPrg;
    EZCOGL::ShaderProgram::UP shaderPrgCube;

    EZCOGL::MeshRenderer::UP cube_rend;
	// We use one renderer for each mesh part
	std::vector<EZCOGL::MeshRenderer::UP> car_rend;
	int nbMeshParts;
	EZCOGL::GLVec3 lightPos;
	float intensity;
	float coeffRefl;
	
	// Material
	std::vector<EZCOGL::GLVec3> ka; // Ambient
	std::vector<EZCOGL::GLVec3> kd; // Diffus
	std::vector<EZCOGL::GLVec3> ks; // Specular
	std::vector<float> ns; // Shininess
	std::vector<float> opacity;

    EZCOGL::TextureCubeMap::SP tex_envMap;

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

Viewer::Viewer() : lightPos(200.f, 200.f, -200.f), intensity(15.f), coeffRefl(1.f)
{}

void Viewer::init_ogl()
{
	shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/tp4Car.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/tp4Car.fs")}}, "car Lighting");
	shaderPrgCube = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/tp4Cube.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/tp4Cube.fs")}}, "cube tex");
	

	// ***********************************
	// Geometry
	// ***********************************
    // Load OBJ file mesh

    // CUBE
	auto meshCube = EZCOGL::Mesh::Cube();
    cube_rend = meshCube->renderer(1, -1, 2, -1, -1);

    tex_envMap = EZCOGL::TextureCubeMap::create();
	tex_envMap->load({DATA_PATH + "/skybox/left.jpg", DATA_PATH +  "/skybox/right.jpg", DATA_PATH + "/skybox/top.jpg",
					  DATA_PATH + "/skybox/bottom.jpg", DATA_PATH + "/skybox/front.jpg", DATA_PATH + "/skybox/back.jpg"});


	auto mesh = EZCOGL::Mesh::load(DATA_PATH + "/models/mustang_GT.obj")->data();
	nbMeshParts = mesh.size();
	// set the renderer for all the meshes parts
	for (int i = 0; i < nbMeshParts; ++i)
	{
		car_rend.push_back(mesh[i]->renderer(1, 2, -1, -1, -1));
		ka.push_back(mesh[i]->material()->Ka / 100.f); // ambient material coefficient
		kd.push_back(mesh[i]->material()->Kd); // diffus material coefficient
		ks.push_back(mesh[i]->material()->Ks); // specular material coefficient
		ns.push_back(mesh[i]->material()->Ns); // shininess of the specular material
		opacity.push_back(mesh[i]->material()->opacity);
	}

	// set scene center and radius for the init of matrix view/proj
	set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
	set_scene_radius(80.f);

	// Define the color to use when refreshing screen
    glClearColor(0.1, 0.1, 0.1, 1.0);

}
void Viewer::draw_ogl()
{
	// Clear the buffer before to draw the next frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	


    // Get the view and projection matrix
	const EZCOGL::GLMat4& view = this->get_view_matrix();
    EZCOGL::GLMat4 view2 = view;
    view2.block<3,1>(0, 3).setZero();

    view2.block<3,1>(0, 0).normalize(); 
    view2.block<3,1>(0, 1).normalize(); 
    view2.block<3,1>(0, 2).normalize();
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();
    // Construct a model matrix
    const EZCOGL::GLMat4& modelCube = EZCOGL::Transfo::rotateX(0.0);
    const EZCOGL::GLMat4& modelCar = EZCOGL::Transfo::rotateX(-90.0);
    


	// ***********************************
	// Rendering
	// ***********************************
    tex_envMap->bind(0);
    shaderPrgCube->bind();
	//desable depth test
    glDisable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	// Uniforms variables send to the GPU
	EZCOGL::set_uniform_value(0, modelCube);
	EZCOGL::set_uniform_value(1, view2);
	EZCOGL::set_uniform_value(2, proj);
    EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(view * modelCube));
	// Uniforms variables send to the GPU
	EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(intensity, intensity, intensity));
	EZCOGL::set_uniform_value(5, EZCOGL::Transfo::sub33(view * modelCube) * lightPos);
    cube_rend->draw(GL_TRIANGLES);



	shaderPrg->bind();
    // Enable Depth test
	glEnable(GL_DEPTH_TEST);glDepthMask(GL_TRUE);
	// Uniforms variables send to the GPU
	EZCOGL::set_uniform_value(0, modelCar);
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(view * modelCar));
	// Uniforms variables send to the GPU
	EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(intensity, intensity, intensity));
	EZCOGL::set_uniform_value(5, EZCOGL::Transfo::sub33(view * modelCar) * lightPos);
	EZCOGL::set_uniform_value(10,coeffRefl);
	EZCOGL::set_uniform_value(12, view.inverse()*EZCOGL::GLVec4(0.,0.,0.,1.));
	EZCOGL::set_uniform_value(13, EZCOGL::Transfo::inverse_transpose(modelCar));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < nbMeshParts; ++i)
	{
		if (opacity[i] > 0.999f) {
			EZCOGL::set_uniform_value(6, ka[i]);
			EZCOGL::set_uniform_value(7, kd[i]);
			EZCOGL::set_uniform_value(8, ks[i]);
			EZCOGL::set_uniform_value(9, ns[i]);
			EZCOGL::set_uniform_value(11, opacity[i]);
			car_rend[i]->draw(GL_TRIANGLES);
		}
	}
	for (int i = 0; i < nbMeshParts; ++i)
	{
		if (opacity[i] <= 0.999f) {
			EZCOGL::set_uniform_value(6, ka[i]);
			EZCOGL::set_uniform_value(7, kd[i]);
			EZCOGL::set_uniform_value(8, ks[i]);
			EZCOGL::set_uniform_value(9, ns[i]);
			EZCOGL::set_uniform_value(11, opacity[i]);
			car_rend[i]->draw(GL_TRIANGLES);
		}
	}
	glDisable(GL_BLEND);
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 3.0f;
	ImGui::Begin("Tuto 2", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});
	ImGui::SliderFloat("Reflection", &coeffRefl, 0.f, 1.f);
	ImGui::SliderFloat("X", &lightPos[0], -200.f, 200.f);
	ImGui::SliderFloat("Y", &lightPos[1], -200.f, 200.f);
	ImGui::SliderFloat("Z", &lightPos[2], -200.f, 200.f);
	ImGui::SliderFloat("Int", &intensity, 0.f, 100.f);
	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders"))
		shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/tp4Car.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/tp4Car.fs")}}, "car Lighting");
        shaderPrgCube = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/tp4Cube.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/tp4Cube.fs")}}, "cube tex");
	ImGui::End();
}
