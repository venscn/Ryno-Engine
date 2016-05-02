#pragma once
#include "ValleyScene.h"
#include "FireworkScene.h"
#include "Terrain.h"

namespace Ryno{

	void ValleyScene::start(){

		camera->position = glm::vec4(-100,50,-100, 1);
		camera->movement_speed = .05f;

		bt = game->texture_manager->load_png("pack/161", GAME);
		bn = game->texture_manager->load_png("pack/161_norm", GAME);

		terrain_mesh = game->mesh_manager->create_empty_mesh(GAME);

		game->mesh_builder->set_mesh(terrain_mesh);
		NewTerrain(game->mesh_builder, 100, 2,2, 10,.02f);
		cube_mesh = game->mesh_manager->load_mesh("cube", 1, GAME);
		camera->skybox = game->texture_manager->load_cube_map("full_moon", GAME);
	
		shader.create("Geometry/geometry",GAME);
		
		go.model = new Model();
		go.model->material.set_shader(&shader);
		go.model->material.set_attribute("in_Color", ColorRGBA(255, 255, 255, 0));
		go.model->material.set_attribute("in_Tiling", glm::vec2(10, 10));
		go.model->material.set_uniform("texture_sampler", bt.id);
		go.model->material.set_uniform("normal_map_sampler", bn.id);
		go.model->mesh = cube_mesh;
		go.model->cast_shadows = false;

		dir_shader.create("LightPass/directional", ENGINE);

		go.transform = new Transform();
		go.transform->set_scale(200,1,200);
		go.transform->set_position(0, 0, 0);
		go.dir_light = new DirectionalLight();
		auto* l = go.dir_light;
		l->model = new Model();
		l->model->material.set_shader(&dir_shader);
		l->set_direction(-45, 0);
		l->diffuse_intensity = 0;
		l->set_diffuse_color(255, 255, 200);
		l->specular_intensity = 0;
		l->set_specular_color(255, 255, 200);
		l->ambient_intensity = 0.05f;
		l->set_ambient_color(255, 255, 200);

		I32 s = 15;//size
		poles.resize(s*s);
		cube_mesh = game->mesh_manager->load_mesh("cube", 1, GAME);

		poles[0].model = new Model();
		auto* m = poles[0].model;
		m->material.set_shader(&shader);
		m->material.set_attribute("in_Color", ColorRGBA(255, 255, 255, 0));
		m->material.set_attribute("in_Tiling", glm::vec2(10, 10));
		m->material.set_uniform("texture_sampler", bt.id);
		m->material.set_uniform("normal_map_sampler", bn.id);
		m->mesh = cube_mesh;
		m->cast_shadows = true;
		poles[0].transform = new Transform();
		poles[0].transform->set_scale(2, 20, 2);
		poles[0].transform->set_position(0, 0,0);
		for (I32 i = 0; i < s;i++)
			for (I32 j = 0; j < s; j++)
			{
				if (i == j && i == 0)continue;
				poles[i*s + j].copy(poles[0]);
				poles[i*s + j].transform->set_position((i-s/2) * 20, 10, (j-s/2) * 20);
			
			}

		white = game->texture_manager->load_png("white_pixel", GAME);
		white_normal = game->texture_manager->load_png("normal_pixel", GAME);
		sphere_mesh = game->mesh_manager->load_mesh("sphere", 1, GAME);

		sphere.copy(go);
		sphere.model->material.set_attribute("in_Color", ColorRGBA::white);
		sphere.model->material.set_uniform("texture_sampler", white.id);
		sphere.model->material.set_uniform("normal_map_sampler", white_normal.id);
		sphere.model->cast_shadows = false;

		sphere.transform->set_scale(12, 12, 12);
		sphere.transform->set_position(0, 40, 0);
		sphere.model->mesh = sphere_mesh;


		point_light_shader.create("LightPass/point", ENGINE);

		sphere.point_light = new PointLight();
		auto* p = sphere.point_light;
		p->model = new Model();
		p->model->material.set_shader(&point_light_shader);
		p->set_diffuse_color(255, 80, 0);
		p->diffuse_intensity = 10;
		p->attenuation = .0001;
		p->specular_intensity = 10;
		p->set_specular_color(255, 80, 0);
	}
		

	void ValleyScene::update(){
	}
	
	void ValleyScene::input(){
		if (!game->shell->active){
			if (game->input_manager->is_key_pressed(SDLK_c, KEYBOARD)){
				game->set_scene("firework");
				return;
			
			}
		}
		float speed = .1f;
		if (game->input_manager->is_key_down(SDLK_RIGHT, KEYBOARD)) {
			sphere.transform->position += game->delta_time * speed* glm::vec3(1, 0, 0);
		}
		if (game->input_manager->is_key_down(SDLK_LEFT, KEYBOARD)) {
			sphere.transform->position += game->delta_time * speed * glm::vec3(-1, 0, 0);
		}
		if (game->input_manager->is_key_down(SDLK_UP, KEYBOARD)) {
			sphere.transform->position += game->delta_time * speed * glm::vec3(0, 0, 1);
		}
		if (game->input_manager->is_key_down(SDLK_DOWN, KEYBOARD)) {
			sphere.transform->position += game->delta_time * speed * glm::vec3(0, 0, -1);
		}
	}



}