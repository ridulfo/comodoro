#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "glass-ping.c"

typedef enum { Halted, Running } Status;

// n breaks intervals, after 3 short breaks, take a long break
int n_intervals = 0;

int interval_work = 15;      // Minutes
int interval_break = 1;      // Minutes
int interval_long_break = 5; // Minutes

double start_time;
Status status = Halted;

void start() {
  start_time = GetTime();
  status = Running;
}

void stop() {
  start_time = 0;
  status = Halted;
}

void FullscreenMode() {
  int break_start_time = GetTime();
  ToggleFullscreen();
  bool should_exit_fullscreen = false;
  while (!WindowShouldClose()) {
    int elapsed = GetTime() - break_start_time;
    if (elapsed >=
        60 * (n_intervals >= 3 ? interval_long_break : interval_break)) {
      ToggleFullscreen();
      if (n_intervals >= 3) {
        n_intervals = 0;
      } else {
        n_intervals++;
      }
      break;
    }
    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    int width = GetRenderWidth(), height = GetRenderHeight();

    char *text =
        TextFormat("Take a %s break!", n_intervals >= 3 ? "long" : "short");

    DrawText(text, width / 2 - MeasureText(text, 50) / 2, height / 2 - 50, 50,
             LIGHTGRAY);
    const char *timestr =
        TextFormat("%02d:%02d", (int)elapsed / 60, (int)elapsed % 60);
    DrawText(timestr, width / 2 - MeasureText(timestr, 20) / 2, height / 2, 20,
             LIGHTGRAY);
    if (GuiButton(
            (Rectangle){width / 2 - MeasureText("Exit Fullscreen", 20) / 2,
                        height / 2 + 50, MeasureText("Exit Fullscreen", 20),
                        30},
            "Exit Fullscreen")) {
      should_exit_fullscreen = true;
    }
    EndDrawing();

    if (should_exit_fullscreen) {
      ToggleFullscreen();
      break;
    }
  }
};

int main() {
  InitWindow(272, 160, "Pomodoro");
  SetTargetFPS(60);

  GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

  InitAudioDevice();
  Wave wav = {
    .channels = 2,
    .sampleRate = 44100,
    .sampleSize = 16,
    .data = glass_ping_wav,
    .frameCount = sizeof(glass_ping_wav) / 4,
  };

  Sound sound = LoadSoundFromWave(wav);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    double elapsed = status == Running ? GetTime() - start_time : 0;
    if (status == Running &&
        elapsed >= 60 * interval_work) { // Add * 60 to convert to seconds
      PlaySound(sound);
      stop();
      FullscreenMode();
      PlaySound(sound);

      start();
    }

    for (int i = 0; i < n_intervals; i++) {
      DrawRectangle(8 + 120 + 16 + i * (8 + 24), 8 + 2, 18, 18, LIGHTGRAY);
    }

    char *timestr =
        TextFormat("%02d:%02d", (int)elapsed / 60, (int)elapsed % 60);
    GuiTextBox((Rectangle){8, 8, 120, 24}, timestr, 128, false);
    if (GuiButton((Rectangle){8, 48, 120, 24}, "Start")) {
      start();
    }
    if (GuiButton((Rectangle){8, 88, 120, 24}, "Stop")) {
      stop();
    }
    GuiButton((Rectangle){8, 128, 120, 24}, "IDK");

    GuiLabel((Rectangle){144, 28, 120, 24}, "Work");
    GuiSpinner((Rectangle){144, 48, 120, 24}, NULL, &interval_work, 1, 60,
               true);

    GuiLabel((Rectangle){144, 68, 120, 24}, "Break");
    GuiSpinner((Rectangle){144, 88, 120, 24}, NULL, &interval_break, 1, 60,
               true);
    GuiLabel((Rectangle){144, 109, 120, 24}, "Long Break");
    GuiSpinner((Rectangle){144, 128, 120, 24}, NULL, &interval_long_break, 1,
               60, true);
    EndDrawing();

    // Bounds checking
    if (interval_work < 1) {
      interval_work = 1;
    }
    if (interval_break < 1) {
      interval_break = 1;
    }
    if (interval_long_break < 1) {
      interval_long_break = 1;
    }
  }

  CloseWindow();
  return 0;
}
