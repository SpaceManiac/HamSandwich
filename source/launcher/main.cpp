// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_sdl_opengl3/ folder**
// See imgui_impl_sdl.cpp for details.

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>

struct Asset
{
	std::string filename;
	std::string sha256sum;
	std::string link;
	long file_id;
	std::string description;
	bool required;
	bool enabled;
};

struct Game
{
	std::string id;
	std::string title;
	std::vector<Asset> assets;

	void play_clicked()
	{
		std::string cmdline = "./";
		cmdline.append(id);
		// TODO: use exec or CreateProcess instead
		system(cmdline.c_str());
	}
};

// ----------------------------------------------------------------------------
// Main code
int main(int argc, char** argv)
{
	const char* bin_dir = get_current_dir_name();  // never free()d because we always need it
	printf("bin_dir: %s\n", bin_dir);

	// Setup SDL
	// (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
	// depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Setup window
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("HamSandwich Launcher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 480, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL2_Init();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	std::vector<Game> games;
	games.push_back(Game { .id = "supreme", .title = "Dr. Lunatic Supreme With Cheese" });
	games.back().assets.push_back(Asset {
		.filename = "supreme8_install.exe",
		//.kind = "nsis"
		.sha256sum = "1c105ad826be1e0697b5de8483c71ff943d04bce91fe3547b6f355e9bc1c42d4",
		.link = "https://hamumu.itch.io/dr-lunatic-supreme-with-cheese",
		.file_id = 700882,
		.description = "Base assets: Dr. Lunatic Supreme With Cheese",
		.required = true,
	});
	games.back().assets.push_back(Asset {
		.filename = "all_supreme_worlds.zip",
		//.kind = "nsis"
		.sha256sum = "",
		.link = "https://hamumu.itch.io/dr-lunatic-supreme-with-cheese",
		.file_id = 824077,
		.description = "Add-ons: All Supreme Worlds",
		.required = false,
	});
	games.push_back(Game { .id = "loonyland", .title = "Loonyland: Halloween Hill" });
	games.push_back(Game { .id = "loonyland2", .title = "Loonyland 2: Winter Woods" });
	games.push_back(Game { .id = "mystic", .title = "Kid Mystic" });
	games.push_back(Game { .id = "sleepless", .title = "Sleepless Hollow" });
	Game* current_game = &games[0];

	bool fullscreen = true;

	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		int windowWidth, windowHeight;
		float leftPaneWidth = 300;
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ leftPaneWidth, (float)windowHeight });
		if (ImGui::Begin("Games", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			for (auto& game : games)
			{
				ImGui::PushID(game.id.c_str());
				if (ImGui::Selectable("", current_game == &game, ImGuiSelectableFlags_AllowDoubleClick))
				{
					current_game = &game;
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						game.play_clicked();
				}
				ImGui::SameLine(0);
				//ImGui::Text("Icon");
				//ImGui::SameLine(64);
				ImGui::Text("%s", game.title.c_str());
				ImGui::PopID();
			}
		}
		ImGui::End();

		if (current_game)
		{
			static bool checked = false;
			ImGui::SetNextWindowPos({ leftPaneWidth, 0 });
			ImGui::SetNextWindowSize({ (float)windowWidth - leftPaneWidth, (float)windowHeight });
			ImGui::Begin(current_game->title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing);

			if (ImGui::Button("Play", { 196, 0 }))
			{
				current_game->play_clicked();
			}
			ImGui::SameLine(212);
			ImGui::Checkbox("Fullscreen", &fullscreen);
			ImGui::Separator();

			ImGui::Text("Manage assets:");

			for (auto& asset : current_game->assets)
			{
				ImGui::PushID(asset.filename.c_str());
				if (asset.required)
				{
					bool dummy = true;
					ImGui::BeginDisabled();
					ImGui::Checkbox(asset.description.c_str(), &dummy);
					ImGui::EndDisabled();
				}
				else
				{
					ImGui::Checkbox(asset.description.c_str(), &asset.enabled);
				}

				ImGui::SameLine(ImGui::GetWindowWidth() - 128);
				ImGui::Button("Download", { 128, 0 });
				ImGui::PopID();
			}

			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
