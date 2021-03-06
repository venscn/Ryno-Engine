#include "Game.h"
#include "Scene.h"

namespace RynoEngine{
	void Game::init_external_systems(){

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			std::cout<<"Failed to initialize SDL: " + std::string(SDL_GetError())<<std::endl;
		}

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		if ((window = SDL_CreateWindow("Ryno Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL)) == NULL){
			std::cout<<"Failed to create SDL window: "+ std::string(SDL_GetError())<<std::endl;
		}

		//MOUSE INITIALIZATIONS
		SDL_ShowCursor(GL_FALSE);

		//JOYSTICK INITIALIZATIONS
		SDL_Joystick *joy;
		if (SDL_NumJoysticks() > 0){
			joy = SDL_JoystickOpen(0);
			SDL_JoystickEventState(SDL_ENABLE);
		}


		SDL_GLContext gl_context;
		if ((gl_context = SDL_GL_CreateContext(window)) = nullptr){
			std::cout<<"Failed to create GL_context: " + std::string(SDL_GetError())<<std::endl;
		}

		glewExperimental = GLU_TRUE;
		GLenum error;
		if ((error = glewInit()) != GLEW_OK){
			std::cout << "Failed to initialize glew." << std::endl;
		}


		SDL_GL_SetSwapInterval(0); //disable vsync

	

		game_state = GameState::Running;
	}

	void Game::init_internal_systems(){

		deferred_renderer = new DeferredRenderer();
		deferred_renderer->init();
		shell = Shell::get_instance();
		shell->init();
		log = Log::get_instance();
		log->init();
	
		particle_manager = ParticleManager::get_instance();
		particle_manager->init();
		audio_manager = AudioManager::get_instance();
		audio_manager->init();
		time_manager = TimeManager::get_instance();
		time_manager->init(60);
	    texture_manager = TextureManager::get_instance();
		input_manager = InputManager::get_instance();
		input_manager->init(window);
		mesh_manager = MeshManager::get_instance();
		
		simple_drawer = SimpleDrawer::get_instance();
		mesh_builder = MeshBuilder::get_instance();
		
		
				
	}

	Game* Game::get_instance()
	{
		static Game game;
		return &game;
	}

	void Game::init()
	{
		init_external_systems();
		init_internal_systems();
		set_scene(0);
	}

	void Game::run(){

		game_state = GameState::Running;
		scene->start();
		
		//Main Loop
		while (game_state != GameState::Exit){
			if (game_state == GameState::ChangeScene) {
				Batch3DGeometry::sorting = true;
				Batch3DShadow::sorting = true;
				scene->start();
				game_state = GameState::Running;
			}
			time_manager->begin_frame();
			handle_input();
			if (game_state == GameState::ChangeScene)
				continue;
			scene->camera_update();
			if (game_state != GameState::Paused) update();
			draw();
			time_manager->end_frame(&time, &delta_time);
			if (game_state != GameState::Paused) time_manager->print_fps();
		}
		end();
		exit_game();


	}
	void Game::set_scene(const std::string& scene_name)
	{
		set_scene(SceneManager::get_scene_nr(scene_name));
	}
	void Game::set_scene(I32 scene_nr)
	{
		if (!SceneManager::scene_exists(scene_nr))
			return;
		if (scene)
			destroy_scene(scene);
		scene = SceneManager::new_scene(scene_nr);
		deferred_renderer->set_camera(scene->camera);
		game_state = GameState::ChangeScene;
		shell->request_pause = false;
	
	}
	void Game::next_scene()
	{
		if (scene)
			destroy_scene(scene);
		scene = SceneManager::next_scene();
		deferred_renderer->set_camera(scene->camera);
		game_state = GameState::ChangeScene;
		shell->request_pause = false;

	}
	void Game::reset_scene()
	{
		if (scene)
			destroy_scene(scene);
		scene = SceneManager::reset_scene();
		deferred_renderer->set_camera(scene->camera);
		game_state = GameState::ChangeScene;
		shell->request_pause = false;
	}

	void Game::destroy_scene(Scene* s){
		particle_manager->reset();
		mesh_builder->reset();
		mesh_manager->reset();
		texture_manager->reset();
		audio_manager->reset();
		CPUProfiler::reset();
		GPUProfiler::reset();
		delete scene;
		scene = nullptr;
	}

	

	void Game::draw(){

		deferred_renderer->init_frame();
		deferred_renderer->geometry_pass();
		deferred_renderer->spot_light_pass();
		deferred_renderer->point_light_pass();
		deferred_renderer->directional_light_pass();
		deferred_renderer->skybox_pass();
		deferred_renderer->GUI_pass();
		deferred_renderer->final_pass();

		SDL_GL_SwapWindow(window);

	}

	void Game::end()
	{
		SDL_JoystickClose(game_controller);
		deferred_renderer->destroy();
	}

	void Game::exit_game(){
	
		delete log;
		delete shell;
		SDL_Quit();
		exit(0);
	}	

	void Game::handle_input(){


		//Reads input from user
		input_manager->update();
		//Exits if requested
		if (input_manager->get_input() == Input::EXIT_REQUEST){
			game_state = GameState::Exit;
			return;
		}

		//Process console inputs
		shell->process_input();

		 if (shell->request_exit)
			 game_state = GameState::Exit;
		 else if (shell->request_pause)
			 game_state = GameState::Paused;
		 else if (game_state != GameState::ChangeScene)
			game_state = GameState::Running;
		
		

		//Process user inputs
		 if (game_state != GameState::Paused) {
			 scene->input();
			 scene->input_scripts();
		 }
	}

		
	void Game::update()
	{
		particle_manager->update(delta_time);
		scene->update();
		scene->update_scripts();
	}

}