#pragma once
#include <vector>
#include <GLM\glm.hpp>
#include "Types.h"
#include "TextureLoader.h"

struct Vertex3D{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec4 color;
	glm::vec3 normal;
};


class Model{
	public:

		/**
		Load a model and set it's initial color
		@Param name -> name of the model in Resource folder
		@Paramm color -> initial color of the model. White is default.
		*/
		static Model* load_model(const std::string& name, glm::vec4 color = glm::vec4(1.0f,1.0f,1.0f,1.0f));


		GLTexture texture;
		glm::vec4 color;
		std::vector<Vertex3D> vertices;
		

};