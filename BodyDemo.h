#pragma once

#include <gl/glew.h>
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VAO.h>
#include <VrLib/gl/VIO.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/gl/Shader.h>
#include "Demo.h"

#include <glm/glm.hpp>
#include <set>

namespace vrlib
{
	namespace gl { class ShaderProgram; }
	class Texture;
	class Model;
	namespace gui { namespace components { class Slider;  } }
}

class BodyDemo : public Demo
{
public:
	enum class BodyUniforms
	{
		cutNormal,
		tex,
		hasTexture,
		colorMult,
		modelMatrix,
		cutPos
	};

	class Mesh
	{
	public:
		vrlib::Texture* texture;
		glm::vec3 color;

		std::string filename;
		vrlib::gl::VBO<vrlib::gl::VertexPositionTextureNormal>* vbo;
		vrlib::gl::VIO<unsigned short>* vio;


		class SubMesh
		{
		public:
			std::string name;
			int start;
			int length;
			Mesh* mesh;
		};

		std::vector<SubMesh*> submeshes;

	};

	class Node
	{
	public:
		std::string name;
		int id;
		std::vector<Node*> children;

		std::vector<Mesh::SubMesh*> subMeshes;
		Node()
		{
			id = -1;
		}
		void draw(float alpha, std::set<Node*> &drawn, vrlib::gl::Shader<BodyUniforms>* shader);
		void print(int level = 0);
	};

	Node* rootMale;
	Node* rootFemale;
	Node* root;

	vrlib::gl::Shader<BodyUniforms>* shader;

	vrlib::Model* walls;
	vrlib::Model* stand;
	vrlib::Texture* wallTexture;
	vrlib::Texture* standTexture;

	vrlib::gui::components::Slider* opacitySlider;


	float layer;
	bool cut;


	BodyDemo() : Demo("Body") {}

	Node* readModel(std::string dir, std::string jsonfile);

	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix);
	virtual void update();
	virtual vrlib::gui::components::Panel* getPanel();
	void switchBody();
};