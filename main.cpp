#include <time.h>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>

#include <GL/gl3w.h>
#include <SDL.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

// Our stuff.
#include "env.h"
#include "pixels.h"

class Pattern {
 public:
  static const int kPixelCount = 37;
  static const int kRingCount = 4;
  static const int kPixelCountPerRing[kRingCount];

  // Updates pixels for a new frame.
  virtual void Update(const Env& env) = 0;

  // Given LED count [0..kPixelCount-1), return which ring it is in, and the
  // number in the ring.
  static void GetPos(int num, int* ring, int* in_ring) {
    for (*ring = 0; *ring < kRingCount; (*ring)++) {
      if (num < kPixelCountPerRing[*ring]) {
        *in_ring = num;
        return;
      }
      num -= kPixelCountPerRing[*ring];
    }
    assert(false);
  }

  const thirty_seven::RGB& pixel(int i) const { return pixels_[i]; }

 protected:
  thirty_seven::RGB pixels_[kPixelCount];
};

const int Pattern::kPixelCountPerRing[kRingCount] = {1, 6, 12, 18};

class AllRed : public Pattern {
 public:
  void Update(const Env& env) final {
    for (int i = 0; i < kPixelCount; ++i) {
      // int x = (micros() / 1000) % 256;
      int x = (i * 10 + env.ms() / 8) % 256;
      pixels_[i].SetHSV(x, 255, 255);
    }
  };
};

void DrawPattern(const Pattern& pattern) {
  ImGui::SetNextWindowSize(ImVec2(600, 600));
  ImGui::Begin("demo");

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  // Find middle point.
  ImVec2 middle = ImGui::GetWindowPos();
  middle.x += ImGui::GetWindowWidth() / 2;
  middle.y += ImGui::GetWindowHeight() / 2;

  // corner:
  // draw_list->AddCircleFilled(middle, 50, IM_COL32(0, 255, 0, 255), 20);

  const int kBetweenRings = 80;
  const int kRadius = 30;

  for (int i = 0; i < Pattern::kPixelCount; ++i) {
    int ring, in_ring;
    pattern.GetPos(i, &ring, &in_ring);
    const thirty_seven::RGB& pixel = pattern.pixel(i);
    int ring_diameter = kBetweenRings * ring;
    float angle_rad = static_cast<float>(in_ring) /
                      Pattern::kPixelCountPerRing[ring] * 2 * M_PI;
    int x = middle.x + sinf(angle_rad) * ring_diameter;
    int y = middle.y - cosf(angle_rad) * ring_diameter;
    draw_list->AddCircleFilled(ImVec2(x, y), kRadius,
                               IM_COL32(pixel.r, pixel.g, pixel.b, 255), 20);
  }
  ImGui::End();
}

// Main code
int main(int, char**) {
  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
      0) {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 Core + GLSL 150
  const char* glsl_version = "#version 150";
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_FLAGS,
      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);  // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window* window =
      SDL_CreateWindow("thirty seven", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1);  // Enable vsync

  if (gl3wInit() != 0) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return 1;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Our state
  // bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  AllRed pattern;
  bool done = false;
  while (!done) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application. Generally you may always pass all inputs
    // to dear imgui, and hide them from your application based on those two
    // flags.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) {
        done = true;
      }
      if (event.type == SDL_WINDOWEVENT &&
          event.window.event == SDL_WINDOWEVENT_CLOSE &&
          event.window.windowID == SDL_GetWindowID(window)) {
        done = true;
      }
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    // Uncomment to show demo window.
    // ImGui::ShowDemoWindow(nullptr);

    Env env(clock_gettime_nsec_np(CLOCK_REALTIME) / 1000);
    pattern.Update(env);
    DrawPattern(pattern);

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
