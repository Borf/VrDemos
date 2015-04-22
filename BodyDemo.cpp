#include "BodyDemo.h"
#include <VrLib/json.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <VrLib/VrLib.h>
#include <vrlib/Log.h>
#include <VrLib/gui/components/Panel.h>
#include <VrLib/gui/components/CheckBox.h>
#include <VrLib/gui/components/Slider.h>
#include <VrLib/gui/layoutmanagers/TableLayout.h>

#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <VrLib/Texture.h>
#include <VrLib/model.h>


#define getBit(c, bit)	(((c>>bit)&0x1) != 0)

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo( size_needed, 0 );
	WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo( size_needed, 0 );
	MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

void decompressAttribsInner( std::wstring &str, int inputStart, int inputEnd, std::vector<float> &output, int outputStart, int stride, int decodeOffset, float decodeScale ) 
{
	int  prev = 0;
	for ( int j = inputStart; j < inputEnd; j ++ ) {
		wchar_t code = str[ j ];
		prev += ( code >> 1 ) ^ ( -( code & 1 ) );
		output[ outputStart ] = decodeScale * ( prev + decodeOffset );
		outputStart += stride;
	}
}

void decompressIndices( std::wstring &str, int inputStart, int numIndices, std::vector<unsigned short> &output, int outputStart ) 
{
	int highest = 0;
	for ( int i = 0; i < numIndices; i ++ ) {
		wchar_t code = str[inputStart++];
		output[ outputStart++] = highest - code;
		if (code == 0 ) 
			highest++;
	}
}



BodyDemo::Node* BodyDemo::readModel(std::string dir, std::string jsonfile)
{
	vrlib::json::Value newValue = vrlib::json::readJson(std::ifstream(jsonfile.c_str()));
	//Json::Value::Members members = newValue["urls"].getMemberNames();

	std::multimap<std::string, Mesh::SubMesh*> subMeshMap;

	for (vrlib::json::Value::Iterator it = newValue["urls"].begin(); it != newValue["urls"].end(); it++)
	{
		std::string fileName = it.key();
		vrlib::json::Value &val = it.value();

		FILE* pFile = NULL;
		fopen_s(&pFile, (dir + fileName).c_str(), "rb");
		fseek(pFile, 0, SEEK_END);
		int len = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		char* buf = new char[len];
		fread(buf, 1, len, pFile);
		fclose(pFile);

		std::wstring data = utf8_decode(std::string(buf, len));


		for(size_t iii = 0; iii < val.size(); iii++)
		{
			vrlib::json::Value config = val[iii];
			vrlib::json::Value decodeParams = newValue["decodeParams"];
			vrlib::json::Value decodeOffsets = decodeParams["decodeOffsets"];

			int attribStart = config["attribRange"][0u].asInt();
			int numVerts = config["attribRange"][1u].asInt();

			int indexStart = config["indexRange"][0u].asInt();
			int numIndices = 3*config["indexRange"][1u].asInt();

			int stride = decodeParams["decodeScales"].size();

			std::vector<float> attribsOut;
			attribsOut.resize(stride * numVerts);
			int inputOffset = attribStart;
			for(int j = 0; j < stride; j++)
			{
				int end = inputOffset + numVerts;
				float decodeScale = decodeParams["decodeScales"][j].asFloat();
				decompressAttribsInner( data, inputOffset, end, attribsOut, j, stride, decodeOffsets[j].asInt(), decodeScale );
				inputOffset = end;
			}

			std::vector<unsigned short> indicesOut(numIndices);
			decompressIndices( data, inputOffset, numIndices, indicesOut, 0 );


			for(size_t ii = 0; ii < attribsOut.size(); ii+=8)
				attribsOut[ii+4] = 1 - attribsOut[ii+4];

			Mesh* mesh = new Mesh();

			mesh->vbo = new vrlib::gl::VBO<vrlib::gl::VertexPositionTextureNormal>();
			mesh->vbo->setData(attribsOut.size() / 8, (vrlib::gl::VertexPositionTextureNormal*)&attribsOut[0], GL_STATIC_DRAW);

			mesh->vio = new vrlib::gl::VIO<unsigned short>();
			mesh->vio->setData(indicesOut.size(), &indicesOut[0], GL_STATIC_DRAW);


			mesh->color = glm::vec3(1,1,1);
			mesh->texture = NULL;

			if(newValue["materials"][config["material"].asString()].isMember("map_kd"))
				mesh->texture = new vrlib::Texture(dir + newValue["materials"][config["material"].asString()]["map_kd"].asString());
			if(newValue["materials"][config["material"].asString()].isMember("Kd"))
				mesh->color = glm::vec3(newValue["materials"][config["material"].asString()]["Kd"][0u].asInt()/255.0f,
				newValue["materials"][config["material"].asString()]["Kd"][1u].asInt()/255.0f,
				newValue["materials"][config["material"].asString()]["Kd"][2u].asInt()/255.0f);

			int start = 0;
			for(size_t ii = 0; ii < config["names"].size(); ii++)
			{
				Mesh::SubMesh* subMesh = new Mesh::SubMesh();
				subMesh->mesh = mesh;
				subMesh->name = config["names"][ii].asString();

				subMesh->start = start;
				subMesh->length = config["lengths"][ii].asInt();
				mesh->submeshes.push_back(subMesh);
				start += subMesh->length;
				subMeshMap.insert(std::pair<std::string, Mesh::SubMesh*>(subMesh->name, subMesh));
			}

			assert(mesh->vio->getLength() == start);
		}
	}


	vrlib::json::Value metadata = vrlib::json::readJson(std::ifstream(dir + "entity_metadata.json"));

	std::map<int, Node*> nodes;
	for(size_t i = 0; i < metadata["leafs"].size(); i++)
	{
		Node* node = new Node();
		node->id = metadata["leafs"][i][0u].asInt();
		node->name = metadata["leafs"][i][1u].asString();

		std::pair<std::multimap<std::string, Mesh::SubMesh*>::iterator, std::multimap<std::string, Mesh::SubMesh*>::iterator> ppp = subMeshMap.equal_range(node->name);
		for (std::multimap<std::string, Mesh::SubMesh*>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
			node->subMeshes.push_back(it2->second);
		if(ppp.first == ppp.second)
			vrlib::logger << "Unable to find " << node->name << vrlib::Log::newline;
		nodes[node->id] = node;
	}
	for(size_t i = 0; i < metadata["nodes"].size(); i++)
	{
		Node* node = new Node();
		node->id = metadata["nodes"][i][0u].asInt();
		node->name = metadata["nodes"][i][1u].asString();
		std::pair<std::multimap<std::string, Mesh::SubMesh*>::iterator, std::multimap<std::string, Mesh::SubMesh*>::iterator> ppp = subMeshMap.equal_range(node->name);
		for (std::multimap<std::string, Mesh::SubMesh*>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
			node->subMeshes.push_back(it2->second);
		if(ppp.first != ppp.second)
			vrlib::logger << "Able to find " << node->name << vrlib::Log::newline;
		nodes[node->id] = node;
	}

	for(size_t i = 0; i < metadata["dag"].size(); i++)
	{
		int id = metadata["dag"][i][0u].asInt();
		for(size_t ii = 0; ii < metadata["dag"][i][1u].size(); ii++)
			nodes[id]->children.push_back(nodes[metadata["dag"][i][1u][ii].asInt()]);
	}

	Node* root = new Node();
	root->name = "root";
	for(size_t i = 0; i < metadata["layers"].size(); i++)
		root->children.push_back(nodes[metadata["layers"][i].asInt()]);
	return root;
}



void BodyDemo::init()
{
	vrlib::logger << "Reading male body" << vrlib::Log::newline;
	rootMale = readModel("data/JohanDemo/zygote/adult_male/", "data/JohanDemo/zygote/adult_male/adult_male.json");
	vrlib::logger << "Reading female body" << vrlib::Log::newline;
	rootFemale = readModel("data/JohanDemo/zygote/adult_female/", "data/JohanDemo/zygote/adult_female/adult_female.json");
	walls = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("cavewall.shape");
	wallTexture = new vrlib::Texture("data/CubeMaps/Marble/total.jpg");
	stand = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("cube.shape");// , new ModelLoadOptions(1.0f));

	shader = new vrlib::gl::Shader<BodyUniforms>("data/JohanDemo/zygote/shader.vert", "data/JohanDemo/zygote/shader.frag");
	shader->link();
	shader->registerUniform(BodyUniforms::cutNormal, "cutNormal");
	shader->registerUniform(BodyUniforms::cutDist, "cutDist");
	shader->registerUniform(BodyUniforms::tex, "tex");
	shader->registerUniform(BodyUniforms::hasTexture, "hasTexture");
	shader->registerUniform(BodyUniforms::colorMult, "colorMult");
	shader->registerUniform(BodyUniforms::modelMatrix, "modelMatrix");
	shader->registerUniform(BodyUniforms::cutPos, "cutPos");
	shader->use();
	shader->setUniform(BodyUniforms::tex, 0);

}

void BodyDemo::start()
{
	layer = 0;
	root = rootFemale;

	shader->use();
	shader->setUniform(BodyUniforms::cutDist, -100.0f);
	shader->setUniform(BodyUniforms::cutNormal, glm::vec3(0,1,0));
	glUseProgram(0);
	cut = false;
}


void BodyDemo::Node::print(int level)
{
	for(int i = 0; i < level; i++)
		printf("  ");
	printf("%s\n", name.c_str());
	for(size_t i = 0; i < children.size(); i++)
		children[i]->print(level+1);
}


void BodyDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	{
		float modelview[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		Sleep(0);
	}

	glDisable(GL_CULL_FACE);
	//glTranslatef(0, 1.5f, 0);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	if(wallTexture != NULL)
		glBindTexture(GL_TEXTURE_2D, wallTexture->texid);
	walls->draw(NULL);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
//	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(0,-1.1f,-0.5f);
	glColor3f(0.1f, 0.1f, 0.1f);
	glScalef(0.5, 1, 2);
	stand->draw(NULL);
	glPopMatrix();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);


	glm::vec3 wandPos = glm::vec3(wandMat * glm::vec4(0,0,0,1));
	glm::vec3 forward = wandPos - glm::vec3(wandMat * glm::vec4(0,0,1,1));

	glm::vec3 point = wandPos;
	//now calculate distance point  & plane at 0,0,0 with normal 'forward'
	float dist = glm::dot(forward, point);

	glm::vec3 center = forward*dist;
	glm::vec3 rotAxis = glm::cross(glm::vec3(0,1,0), forward);
	float rotAngle = glm::acos(glm::dot(glm::vec3(0,1,0), forward));
	glm::mat4 rotMatrix = glm::rotate(glm::mat4(), rotAngle, rotAxis);

	//printf("Pos:    %f\t%f\t%f\n", wandPos.x, wandPos.y, wandPos.z);
	//printf("normal: %f\t%f\t%f\tDist: %f\n", forward.x, forward.y, forward.z, dist);


	glColor3f(1,1,1);
//	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);



	shader->use();
	shader->setUniform(BodyUniforms::cutPos, wandPos);
	if (cut)
	{
		shader->setUniform(BodyUniforms::cutDist, dist);
		shader->setUniform(BodyUniforms::cutNormal, forward);
	}



	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.5f, 0.3f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f, 0.01f, 0.01f));
	shader->setUniform(BodyUniforms::modelMatrix, modelMatrix);
	{
		float modelview[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		Sleep(0);
	}

	glPushMatrix();
	glTranslatef(0.0f, -0.5f, 0.3f);
	glRotatef(-90, 1, 0, 0);
	glScalef(0.01f, 0.01f, 0.01f);

	{
		float modelview[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		Sleep(0);
	}



	std::set<Node*> drawn;
	for(int i = root->children.size()-1; i >= 0; i--)
	{
		float alpha = i - layer+1;
		if(alpha < 0)
			continue;
		if(alpha > 1)
			alpha = 1;

		root->children[i]->draw(alpha, drawn, shader);
	}
	glPopMatrix();
	glUseProgram(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	if(cut)
	{
		glPushMatrix();

		glDisable(GL_LIGHTING);
		glColor4f(1,1,1,0.5f);
		glBegin(GL_TRIANGLE_FAN);
		for(float d = 0; d < 2 * M_PI; d += (float)(M_PI/10.0f))
		{
			glm::vec3 bla0 = wandPos + glm::vec3(rotMatrix * glm::vec4(cos(d),0,sin(d),1));
			glVertex3f(bla0.x, bla0.y, bla0.z);
		}
		glEnd();
		glColor4f(1,1,1,1);
		glBegin(GL_LINE_LOOP);
		for(float d = 0; d < 2 * M_PI; d += (float)(M_PI/10.0f))
		{
			glm::vec3 bla0 = wandPos + glm::vec3(rotMatrix * glm::vec4(cos(d),0,sin(d),1));
			glVertex3f(bla0.x, bla0.y, bla0.z);
		}
		glEnd();
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

}


void BodyDemo::Node::draw(float alpha, std::set<Node*> &drawn, vrlib::gl::Shader<BodyUniforms>* shader)
{
	if(drawn.find(this) != drawn.end())
		return;
	drawn.insert(this);

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

	for(size_t i = 0; i < subMeshes.size(); i++)
	{
		Mesh::SubMesh* subMesh = subMeshes[i];

		if(subMesh->mesh->texture)
		{
			shader->setUniform(BodyUniforms::hasTexture, 1);
			shader->setUniform(BodyUniforms::colorMult, glm::vec4(1,1,1, alpha));
			glColor4f(1,1,1, alpha);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, subMesh->mesh->texture->texid);
		}
		else
		{
			shader->setUniform(BodyUniforms::hasTexture, 0);
			shader->setUniform(BodyUniforms::colorMult, glm::vec4(subMesh->mesh->color, alpha));
			glColor4f(subMesh->mesh->color.r, subMesh->mesh->color.g, subMesh->mesh->color.b, alpha);
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_COLOR_MATERIAL);
		}

		subMesh->mesh->vio->bind();
		subMesh->mesh->vbo->bind();
		subMesh->mesh->vbo->setPointer();
		glDrawElements(GL_TRIANGLES, subMesh->length, GL_UNSIGNED_SHORT, BUFFER_OFFSET(subMesh->start * sizeof(short)));
	}
	for(size_t i = 0; i < children.size(); i++)
		children[i]->draw(alpha, drawn, shader);
}


void BodyDemo::update()
{
	if(rightButton == vrlib::TOGGLE_ON)
	{
		cut = !cut;
		if(!cut)
		{
			glm::vec3 wandPos = glm::vec3(wandMat * glm::vec4(0,0,0,1));
			glm::vec3 forward = wandPos - glm::vec3(wandMat * glm::vec4(0,0,1,1));

			glm::vec3 point = wandPos;
			//now calculate distance point  & plane at 0,0,0 with normal 'forward'
			float dist = glm::dot(forward, point);

			glm::vec3 center = forward*dist;
			glm::vec3 rotAxis = glm::cross(glm::vec3(0,1,0), forward);
			float rotAngle = glm::acos(glm::dot(glm::vec3(0,1,0), forward));
			glm::mat4 rotMatrix = glm::rotate(glm::mat4(), rotAngle, rotAxis);

			shader->use();
			shader->setUniform(BodyUniforms::cutDist, dist);
			shader->setUniform(BodyUniforms::cutNormal, forward);
		}
	}
}



class BodyPanel : public vrlib::gui::components::Panel
{
public:
	BodyPanel(vrlib::gui::layoutmanagers::LayoutManager* manager) : vrlib::gui::components::Panel(manager){};
	virtual float minWidth() 	{	return 0.55f; }
	virtual float minHeight()	{	return 0.6f; }
};

class BodySlider : public vrlib::gui::components::Slider
{
	BodyDemo* demo;
public:
	BodySlider(BodyDemo* demo) : vrlib::gui::components::Slider(0, 100000, 0) { this->demo = demo; }
	void drag(glm::vec3 intersect)
	{
		vrlib::gui::components::Slider::drag(intersect);
		demo->layer = -0.5f + (value/max) * 8;
	}
};

vrlib::gui::components::Panel* BodyDemo::getPanel()
{
	vrlib::gui::components::Panel* p = new BodyPanel(new vrlib::gui::layoutmanagers::TableLayout(1));
	p->add(new BodySlider(this));
	p->add(new vrlib::gui::components::CheckBox(true, [this]() { this->switchBody(); }));
	return p;
}

void BodyDemo::switchBody()
{
	root = (root == rootFemale) ? rootMale : rootFemale;
}
