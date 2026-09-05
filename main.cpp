#include "raylib.h"


class Player {
  public:
    float x, y;
    float speed;
    float size;

  
    void draw() {
        
        DrawRectangle(x, y, size, size, BLUE); 
    }
};
int main() {
    InitWindow(800, 600, "My Awesome Arch Linux Game");
    SetTargetFPS(60);  
  
    Player my_guy;
    my_guy.x = 400;
    my_guy.y = 300;
    my_guy.speed = 5.0f;
    my_guy.size = 50.0f;

   
    while (!WindowShouldClose()) {
        
       
        if (IsKeyDown(KEY_RIGHT)) my_guy.x += my_guy.speed;
        if (IsKeyDown(KEY_LEFT))  my_guy.x -= my_guy.speed;
        if (IsKeyDown(KEY_DOWN))  my_guy.y += my_guy.speed;
        if (IsKeyDown(KEY_UP))    my_guy.y -= my_guy.speed;

 
        BeginDrawing();
        
        ClearBackground(RAYWHITE); 
   
        my_guy.draw(); 
        
        DrawText("Use Arrow Keys to move!", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}