#include "raylib.h" //need to add raylib and compile with "make". I put the compiled one "simulation" in the folder so you dont need to. lol
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef PI
#define PI 3.14159f
#endif

#define BASE_REST_LENGTH 40
#define SPRING_CONSTANT 0.0025
#define SPRING_DAMPING 0.0075
#define NODE_FRICTION 15
#define MASS 1

struct vector {
  float x;
  float y;
};

struct particle {
  struct vector position;
  struct vector velocity;
  struct vector acceleration;
  float size;
};

int main() {
  srand(time(NULL));
  struct vector window_size = {1200, 800};
  struct vector world_acceleration = {0, 0.025};
  struct vector size = {6, 4};
  struct vector start = {100, 100};
  struct particle particles[(int)size.x][(int)size.y];
  for (int i = 0; i < size.x; i++) {
    for (int j = 0; j < size.y; j++) {
      float angle = (float)rand() / (float)RAND_MAX * 2.0f * PI;
      particles[i][j].position.x =
          start.x + i * BASE_REST_LENGTH + cos(angle) * 5;
      particles[i][j].position.y =
          start.y + j * BASE_REST_LENGTH + sin(angle) * 5;
      particles[i][j].velocity.x = 0;
      particles[i][j].velocity.y = 0;
      particles[i][j].acceleration.x = 0;
      particles[i][j].acceleration.y = 0;
      particles[i][j].size = 15;
    }
  }

  InitWindow((int)window_size.x, (int)window_size.y, "Soft Body Simulation");
  SetTargetFPS(300);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (int i = 0; i < size.x; i++) {
      for (int j = 0; j < size.y; j++) {
        particles[i][j].acceleration.x = world_acceleration.x;
        particles[i][j].acceleration.y = world_acceleration.y;

        static struct vector selected = {-1, -1};
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
          selected = (struct vector){-1, -1};
        }

        if (selected.x == -1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          float dx = GetMouseX() - particles[i][j].position.x;
          float dy = GetMouseY() - particles[i][j].position.y;
          if (sqrt(dx * dx + dy * dy) < particles[i][j].size * 2) {
            selected.x = (float)i;
            selected.y = (float)j;
          }
        }

        if ((int)selected.x == i && (int)selected.y == j) {
          particles[i][j].position.x = GetMouseX();
          particles[i][j].position.y = GetMouseY();
          particles[i][j].velocity.x = 0;
          particles[i][j].velocity.y = 0;
        }

        int neighborsSize = 1;
        for (int x = -neighborsSize; x <= neighborsSize; x++) {
          for (int y = -neighborsSize; y <= neighborsSize; y++) {
            int nx = i + x;
            int ny = j + y;
            if (x == 0 && y == 0 || nx < 0 || ny < 0 || nx >= size.x ||
                ny >= size.y)
              continue;
            float distance = sqrt(
                pow(particles[i][j].position.x - particles[nx][ny].position.x,
                    2) +
                pow(particles[i][j].position.y - particles[nx][ny].position.y,
                    2));
            float rest_length =
                BASE_REST_LENGTH * (x == 0 || y == 0 ? 1 : 1.414);
            float relative_velocity_x =
                (particles[nx][ny].velocity.x - particles[i][j].velocity.x);
            float relative_velocity_y =
                (particles[nx][ny].velocity.y - particles[i][j].velocity.y);
            particles[i][j].acceleration.x +=
                (particles[nx][ny].position.x - particles[i][j].position.x) *
                    (distance - rest_length) * SPRING_CONSTANT +
                relative_velocity_x * SPRING_DAMPING;
            particles[i][j].acceleration.y +=
                (particles[nx][ny].position.y - particles[i][j].position.y) *
                    (distance - rest_length) * SPRING_CONSTANT +
                relative_velocity_y * SPRING_DAMPING;

            // if (distance < particles[i][j].size + particles[nx][ny].size &&
            //     distance > 0) {
            //   float overlap = 0.5f * (particles[i][j].size +
            //                           particles[nx][ny].size - distance);
            //   float offsetX =
            //       ((particles[i][j].position.x -
            //       particles[nx][ny].position.x) /
            //        distance) *
            //       overlap;
            //   float offsetY =
            //       ((particles[i][j].position.y -
            //       particles[nx][ny].position.y) /
            //        distance) *
            //       overlap;
            //   particles[i][j].position.x += offsetX;
            //   particles[i][j].position.y += offsetY;
            //   particles[nx][ny].position.x -= offsetX;
            //   particles[nx][ny].position.y -= offsetY;
            // }

            if (nx > i || (nx == i && ny > j)) {
              DrawLineEx((Vector2){particles[i][j].position.x,
                                   particles[i][j].position.y},
                         (Vector2){particles[nx][ny].position.x,
                                   particles[nx][ny].position.y},
                         particles[i][j].size / 2, GRAY);
            }
          }
        }
        DrawCircle((int)particles[i][j].position.x,
                   (int)particles[i][j].position.y, particles[i][j].size,
                   BLACK);
      }
    }
    for (int i = 0; i < size.x; i++) {
      for (int j = 0; j < size.y; j++) {

        particles[i][j].velocity.x += particles[i][j].acceleration.x;
        particles[i][j].velocity.y += particles[i][j].acceleration.y;

        particles[i][j].position.x += particles[i][j].velocity.x;
        particles[i][j].position.y += particles[i][j].velocity.y;

        if (particles[i][j].position.x < 0 + particles[i][j].size / 2) {
          particles[i][j].position.x = 0 + particles[i][j].size / 2;
          particles[i][j].velocity.x *= -1;
          particles[i][j].velocity.y /= NODE_FRICTION;
        }
        if (particles[i][j].position.x >
            window_size.x - particles[i][j].size / 2) {
          particles[i][j].position.x = window_size.x - particles[i][j].size / 2;
          particles[i][j].velocity.x *= -1;
          particles[i][j].velocity.y /= NODE_FRICTION;
        }
        if (particles[i][j].position.y < 0 + particles[i][j].size / 2) {
          particles[i][j].position.y = 0 + particles[i][j].size / 2;
          particles[i][j].velocity.y *= -1;
          particles[i][j].velocity.x /= NODE_FRICTION;
        }
        if (particles[i][j].position.y >
            window_size.y - particles[i][j].size / 2) {
          particles[i][j].position.y = window_size.y - particles[i][j].size / 2;
          particles[i][j].velocity.y *= -1;
          particles[i][j].velocity.x /= NODE_FRICTION;
        }
      }
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
