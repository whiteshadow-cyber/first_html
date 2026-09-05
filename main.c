#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_LINE 512
#define MAX_ROUTINE_LINES 50 
#define MAX_INPUT_CHARS 10

// ===== VIRTUAL RESOLUTION =====
#define BASE_WIDTH   400
#define BASE_HEIGHT  225
#define SCALE        4

// ===== PATHS =====
const char *backgroundPath = "E:\\VS project 1st\\backgrounds\\";
const char *songPath       = "E:\\VS project 1st\\song\\song.ogg";
const char *routinePath    = "E:\\VS project 1st\\routine\\routine.txt";
const char *fontPath       = "E:\\VS project 1st\\fonts\\fonts\\fonts.ttf";

// ===== STRUCTURE DEFINITIONS =====

typedef enum {
    SCREEN_INPUT,
    SCREEN_CALENDAR,
    SCREEN_ROUTINE
} GameScreen;

typedef struct {
    Texture2D backgrounds[12];
    Texture2D routineBackground;
    Texture2D inputBackground;
    Music bgMusic;
    Font mainFont;
} Assets;

typedef struct {
    int year;
    int month;
    int selectedDay;
    int semester;
} CalendarState;

typedef struct {
    int inputStep;
    char yearInput[MAX_INPUT_CHARS];
    char monthInput[MAX_INPUT_CHARS];
    char semesterInput[MAX_INPUT_CHARS];
    int yearLetterCount;
    int monthLetterCount;
    int semesterLetterCount;
} InputState;


typedef struct {
    GameScreen currentScreen;
    CalendarState calendar;
    InputState input;
    Assets assets;
} AppState;

// ===== CONSTANTS =====
int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
const char *monthNames[] = {
    "january","february","march","april","may","june",
    "july","august","september","october","november","december"
};

// ===== UTILITY FUNCTIONS =====
int isLeapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int GetFirstWeekday(int y, int m) {
    if (m < 3) { m += 12; y--; }
    int k = y % 100;
    int j = y / 100;
    int h = (1 + 13*(m+1)/5 + k + k/4 + j/4 + 5*j) % 7;
    return (h + 6) % 7;
}

const char* GetDayName(int y, int m, int d) {
    static const char *days[] = {
        "Sunday","Monday","Tuesday","Wednesday",
        "Thursday","Friday","Saturday"
    };
    int first = GetFirstWeekday(y, m);
    return days[(first + d - 1) % 7];
}

// ===== LOAD BACKGROUNDS =====
void LoadBackgrounds(Assets *assets) {
    for (int i = 0; i < 12; i++) {
        char path[512];
        sprintf(path, "%s%s.jpg", backgroundPath, monthNames[i]);
        assets->backgrounds[i] = LoadTexture(path);
    }

    char routinePath[512];
    sprintf(routinePath, "%sroutine.jpg", backgroundPath);
    assets->routineBackground = LoadTexture(routinePath);
    
    char inputPath[512];
    sprintf(inputPath, "%sinput.jpg", backgroundPath);
    assets->inputBackground = LoadTexture(inputPath);
    
    if (assets->routineBackground.id == 0) {
        printf("Warning: Could not load routine.jpg from %s\n", routinePath);
    } else {
        printf("Routine background loaded successfully!\n");
    }
    
    if (assets->inputBackground.id == 0) {
        printf("Warning: Could not load input.jpg from %s\n", inputPath);
    } else {
        printf("Input background loaded successfully!\n");
    }
}

// ===== DRAW INPUT SCREEN =====
void DrawInputScreen(const InputState *input, const Assets *assets) {
    if (assets->inputBackground.id) {
        float s = fmaxf(
            (float)BASE_WIDTH / assets->inputBackground.width,
            (float)BASE_HEIGHT / assets->inputBackground.height
        );
        DrawTextureEx(assets->inputBackground, (Vector2){0,0}, 0, s, WHITE);
    } else {
        ClearBackground((Color){30, 40, 60, 255});
    }
    
    DrawRectangle(0, 0, BASE_WIDTH, BASE_HEIGHT, (Color){0, 0, 0, 120});
    
    // Title
    const char *title = "Retro CALENDAR";
    Vector2 titleSize = MeasureTextEx(assets->mainFont, title, 14, 1);
    DrawTextEx(assets->mainFont, title, 
        (Vector2){(BASE_WIDTH - titleSize.x)/2, 15}, 
        14, 1, YELLOW);
    
    // Subtitle
    const char *subtitle = "Welcome User!";
    Vector2 subSize = MeasureTextEx(assets->mainFont, subtitle, 9, 1);
    DrawTextEx(assets->mainFont, subtitle, 
        (Vector2){(BASE_WIDTH - subSize.x)/2, 35}, 
        9, 1, WHITE);
    
    int startY = 65;
    Color labelColor = LIGHTGRAY;
    Color activeColor = GREEN;
    Color inactiveColor = GRAY;
    
    // Year input - FIXED SIZE
    const char *yearLabel = "enter year:";
    DrawTextEx(assets->mainFont, yearLabel, (Vector2){40, startY}, 6, 1, 
        input->inputStep == 0 ? activeColor : labelColor);
    
    Rectangle yearBox = {40, startY + 13, 120, 20};
    DrawRectangleRec(yearBox, input->inputStep == 0 ? (Color){100, 100, 100, 200} : (Color){60, 60, 60, 200});
    DrawRectangleLinesEx(yearBox, 2, input->inputStep == 0 ? YELLOW : GRAY);
    DrawTextEx(assets->mainFont, input->yearInput, (Vector2){yearBox.x + 5, yearBox.y + 6}, 6, 1, WHITE);
    
    if (input->inputStep == 0 && ((int)(GetTime() * 2) % 2 == 0)) {
        Vector2 textSize = MeasureTextEx(assets->mainFont, input->yearInput, 6, 1);
        DrawRectangle(yearBox.x + 5 + textSize.x, yearBox.y + 6, 2, 10, WHITE);
    }
    
    // Month input - FIXED SIZE
    startY += 43;
    const char *monthLabel = "enter month (1-12):";
    DrawTextEx(assets->mainFont, monthLabel, (Vector2){40, startY}, 6, 1, 
        input->inputStep == 1 ? activeColor : (input->inputStep > 1 ? labelColor : inactiveColor));
    
    Rectangle monthBox = {40, startY + 13, 120, 20};
    DrawRectangleRec(monthBox, input->inputStep == 1 ? (Color){100, 100, 100, 200} : (Color){60, 60, 60, 200});
    DrawRectangleLinesEx(monthBox, 2, input->inputStep == 1 ? YELLOW : GRAY);
    DrawTextEx(assets->mainFont, input->monthInput, (Vector2){monthBox.x + 5, monthBox.y + 6}, 6, 1, WHITE);
    
    if (input->inputStep == 1 && ((int)(GetTime() * 2) % 2 == 0)) {
        Vector2 textSize = MeasureTextEx(assets->mainFont, input->monthInput, 6, 1);
        DrawRectangle(monthBox.x + 5 + textSize.x, monthBox.y + 6, 2, 10, WHITE);
    }
    
    // Semester input - FIXED SIZE
    startY += 43;
    const char *semLabel = "enter semester (0-8):";
    DrawTextEx(assets->mainFont, semLabel, (Vector2){40, startY}, 6, 1, 
        input->inputStep == 2 ? activeColor : (input->inputStep > 2 ? labelColor : inactiveColor));
    
    const char *semHint = "(0 to skip)";
    DrawTextEx(assets->mainFont, semHint, (Vector2){40, startY + 9}, 5, 1, ORANGE);
    
    Rectangle semBox = {40, startY + 19, 120, 20};
    DrawRectangleRec(semBox, input->inputStep == 2 ? (Color){100, 100, 100, 200} : (Color){60, 60, 60, 200});
    DrawRectangleLinesEx(semBox, 2, input->inputStep == 2 ? YELLOW : GRAY);
    DrawTextEx(assets->mainFont, input->semesterInput, (Vector2){semBox.x + 5, semBox.y + 6}, 6, 1, WHITE);
    
    if (input->inputStep == 2 && ((int)(GetTime() * 2) % 2 == 0)) {
        Vector2 textSize = MeasureTextEx(assets->mainFont, input->semesterInput, 6, 1);
        DrawRectangle(semBox.x + 5 + textSize.x, semBox.y + 6, 2, 10, WHITE);
    }
    
    // Bottom instructions
    const char *instruction = "UP/DOWN:Navigate | LEFT/RIGHT:Change | ENTER:Continue";
    Vector2 instSize = MeasureTextEx(assets->mainFont, instruction, 5, 1);
    DrawTextEx(assets->mainFont, instruction, 
        (Vector2){(BASE_WIDTH - instSize.x)/2, BASE_HEIGHT - 18}, 
        5, 1, LIGHTGRAY);
}

// ===== HANDLE INPUT SCREEN =====
void UpdateInputScreen(InputState *input, CalendarState *calendar, GameScreen *currentScreen) {
    if (IsKeyPressed(KEY_DOWN) && input->inputStep < 2) {
        input->inputStep++;
    }
    if (IsKeyPressed(KEY_UP) && input->inputStep > 0) {
        input->inputStep--;
    }
    
    if (input->inputStep == 0) {
        if (IsKeyPressed(KEY_RIGHT) && calendar->year < 9999) {
            calendar->year++;
            sprintf(input->yearInput, "%d", calendar->year);
            input->yearLetterCount = strlen(input->yearInput);
        }
        if (IsKeyPressed(KEY_LEFT) && calendar->year > 1900) {
            calendar->year--;
            sprintf(input->yearInput, "%d", calendar->year);
            input->yearLetterCount = strlen(input->yearInput);
        }
    }
    else if (input->inputStep == 1) {
        if (IsKeyPressed(KEY_RIGHT) && calendar->month < 12) {
            calendar->month++;
            sprintf(input->monthInput, "%d", calendar->month);
            input->monthLetterCount = strlen(input->monthInput);
        }
        if (IsKeyPressed(KEY_LEFT) && calendar->month > 1) {
            calendar->month--;
            sprintf(input->monthInput, "%d", calendar->month);
            input->monthLetterCount = strlen(input->monthInput);
        }
    }
    else if (input->inputStep == 2) {
        if (IsKeyPressed(KEY_RIGHT) && calendar->semester < 8) {
            calendar->semester++;
            sprintf(input->semesterInput, "%d", calendar->semester);
            input->semesterLetterCount = strlen(input->semesterInput);
        }
        if (IsKeyPressed(KEY_LEFT) && calendar->semester > 0) {
            calendar->semester--;
            sprintf(input->semesterInput, "%d", calendar->semester);
            input->semesterLetterCount = strlen(input->semesterInput);
        }
    }
    
    int key = GetCharPressed();
    
    if (input->inputStep == 0) {
        while (key > 0) {
            if ((key >= '0' && key <= '9') && input->yearLetterCount < MAX_INPUT_CHARS - 1) {
                input->yearInput[input->yearLetterCount] = (char)key;
                input->yearInput[input->yearLetterCount + 1] = '\0';
                input->yearLetterCount++;
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && input->yearLetterCount > 0) {
            input->yearLetterCount--;
            input->yearInput[input->yearLetterCount] = '\0';
        }
        
        if (IsKeyPressed(KEY_ENTER) && input->yearLetterCount > 0) {
            calendar->year = atoi(input->yearInput);
            input->inputStep = 1;
        }
    }
    else if (input->inputStep == 1) {
        while (key > 0) {
            if ((key >= '0' && key <= '9') && input->monthLetterCount < MAX_INPUT_CHARS - 1) {
                input->monthInput[input->monthLetterCount] = (char)key;
                input->monthInput[input->monthLetterCount + 1] = '\0';
                input->monthLetterCount++;
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && input->monthLetterCount > 0) {
            input->monthLetterCount--;
            input->monthInput[input->monthLetterCount] = '\0';
        }
        
        if (IsKeyPressed(KEY_ENTER) && input->monthLetterCount > 0) {
            calendar->month = atoi(input->monthInput);
            if (calendar->month >= 1 && calendar->month <= 12) {
                input->inputStep = 2;
            }
        }
    }
    else if (input->inputStep == 2) {
        while (key > 0) {
            if ((key >= '0' && key <= '9') && input->semesterLetterCount < MAX_INPUT_CHARS - 1) {
                input->semesterInput[input->semesterLetterCount] = (char)key;
                input->semesterInput[input->semesterLetterCount + 1] = '\0';
                input->semesterLetterCount++;
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && input->semesterLetterCount > 0) {
            input->semesterLetterCount--;
            input->semesterInput[input->semesterLetterCount] = '\0';
        }
        
        if (IsKeyPressed(KEY_ENTER) && input->semesterLetterCount > 0) {
            calendar->semester = atoi(input->semesterInput);
            if (calendar->semester >= 0 && calendar->semester <= 8) {
                *currentScreen = SCREEN_CALENDAR;
            }
        }
    }
}

// ===== DRAW CALENDAR ===== [LOCKED - DON'T CHANGE]
void DrawCalendar(const CalendarState *calendar, const Assets *assets) {
    if (assets->backgrounds[calendar->month - 1].id) {
        float s = fmaxf(
            (float)BASE_WIDTH / assets->backgrounds[calendar->month - 1].width,
            (float)BASE_HEIGHT / assets->backgrounds[calendar->month - 1].height
        );
        DrawTextureEx(assets->backgrounds[calendar->month - 1], (Vector2){0,0}, 0, s, WHITE);
    } else ClearBackground(RAYWHITE);

    DrawRectangle(0,0,BASE_WIDTH,BASE_HEIGHT,(Color){0,0,0,120});

    int dim = daysInMonth[calendar->month - 1];
    if (calendar->month == 2 && isLeapYear(calendar->year)) dim = 29;

    char title[64];
    sprintf(title, "%s %d", monthNames[calendar->month - 1], calendar->year);
    title[0] = (char)toupper(title[0]);

    Vector2 ts = MeasureTextEx(assets->mainFont, title, 10, 1);
    DrawTextEx(assets->mainFont, title,
        (Vector2){(BASE_WIDTH - ts.x)/2, 10},
        10, 1, YELLOW);

    int cellW = 30;
    int cellH = 24;
    int startX = (BASE_WIDTH - cellW * 7) / 2;
    int startY = 50;

    const char *days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    for (int i = 0; i < 7; i++) {
        Vector2 ds = MeasureTextEx(assets->mainFont, days[i], 6, 1);
        DrawTextEx(assets->mainFont, days[i],
            (Vector2){startX + i*cellW + (cellW-ds.x)/2, startY-12},
            6, 1, YELLOW);
    }

    int first = GetFirstWeekday(calendar->year, calendar->month);

    for (int d = 1; d <= dim; d++) {
        int col = (d - 1 + first) % 7;
        int row = (d - 1 + first) / 7;
        int x = startX + col * cellW;
        int y = startY + row * cellH;

        Color bg = (d == calendar->selectedDay)
            ? (Color){255,215,0,220}
            : (Color){230,230,230,170};

        DrawRectangle(x+1,y+1,cellW-2,cellH-2,bg);
        DrawRectangleLines(x+1,y+1,cellW-2,cellH-2,BLACK);

        char num[4];
        sprintf(num,"%d",d);
        Vector2 ns = MeasureTextEx(assets->mainFont, num, 8, 1);

        DrawTextEx(assets->mainFont, num,
            (Vector2){x+(cellW-ns.x)/2,y+(cellH-ns.y)/2},
            8,1,(d==calendar->selectedDay)?BLACK:DARKGRAY);
    }

    const char *inst = "ARROWS:MOVE  ENTER:ROUTINE  0:BACK  ESC:QUIT";
    Vector2 instSize = MeasureTextEx(assets->mainFont, inst, 5, 1);
    DrawTextEx(assets->mainFont, inst,
        (Vector2){(BASE_WIDTH - instSize.x)/2, BASE_HEIGHT-15},
        5,1,WHITE);
}

// ===== DRAW ROUTINE ===== [FIXED - NOW HANDLES BLANK LINES]
void DrawRoutine(int sem, const char *dayName, int day, const Assets *assets) {
    if (assets->routineBackground.id) {
        float s = fmaxf(
            (float)BASE_WIDTH / assets->routineBackground.width,
            (float)BASE_HEIGHT / assets->routineBackground.height
        );
        DrawTextureEx(assets->routineBackground,(Vector2){0,0},0,s,WHITE);
    } else ClearBackground((Color){20,30,50,255});

    DrawRectangle(0,0,BASE_WIDTH,BASE_HEIGHT,(Color){255,255,255,100});

    // TITLE
    char title[64];
    sprintf(title,"Semester %d - Day %d",sem,day);
    Vector2 titleSize = MeasureTextEx(assets->mainFont, title, 14, 1);
    DrawTextEx(assets->mainFont, title, 
        (Vector2){(BASE_WIDTH - titleSize.x) / 2, 10}, 
        14, 1, (Color){255, 100, 50, 255});
    
    // DAY NAME
    Vector2 daySize = MeasureTextEx(assets->mainFont, dayName, 10, 1);
    DrawTextEx(assets->mainFont, dayName, 
        (Vector2){(BASE_WIDTH - daySize.x) / 2, 28}, 
        10, 1, (Color){50, 100, 200, 255});

    FILE *f = fopen(routinePath,"r");
    if (!f) {
        DrawTextEx(assets->mainFont, "routine.txt not found", (Vector2){30, 100}, 12, 1, RED);
        DrawTextEx(assets->mainFont, "PRESS 0 TO RETURN", (Vector2){10, BASE_HEIGHT-15}, 10, 1, DARKGRAY);
        return;
    }

    char line[MAX_LINE];
    char targetSem[32], targetDay[32];
    sprintf(targetSem,"#Semester%d",sem);
    sprintf(targetDay,"#%s",dayName);

    int reading = 0;
    int y = 48;

    while (fgets(line,sizeof(line),f)) {
        line[strcspn(line,"\r\n")] = 0;

        if (strstr(line, targetSem)) { 
            reading = 1; 
            continue; 
        }
        
        if (reading == 1 && strstr(line, targetDay)) { 
            reading = 2; 
            continue; 
        }
        
        if (reading == 2) {
            char *endPos = strstr(line, "#end");
            if (endPos) {
                *endPos = '\0';
                int len = strlen(line);
                while (len > 0 && (line[len-1] == ' ' || line[len-1] == '\t')) {
                    line[len-1] = '\0';
                    len--;
                }
                if (strlen(line) > 0) {
                    if (strstr(line, "NO CLASSES YA")) {
                        Vector2 textSize = MeasureTextEx(assets->mainFont, line, 16, 1);
                        DrawTextEx(assets->mainFont, line, 
                            (Vector2){(BASE_WIDTH - textSize.x) / 2, BASE_HEIGHT / 2 - 20}, 
                            16, 1, (Color){0, 150, 0, 255});
                    } else {
                        DrawTextEx(assets->mainFont, line, (Vector2){10, y}, 8, 1, BLACK);
                        y += 11;
                    }
                }
                break;
            }
            
            if (line[0] == '#') {
                break;
            }
            
            // FIXED: Draw the line if it has content, but always increment y
            if (strlen(line) > 0) {
                DrawTextEx(assets->mainFont, line, (Vector2){10, y}, 8, 1, BLACK);
            }
            // Always increment y to create spacing (even for blank lines)
            y += 11;
            
            if (y > BASE_HEIGHT - 30) break;
        }
    }
    fclose(f);

    DrawTextEx(assets->mainFont, "PRESS 0 TO RETURN", (Vector2){10, BASE_HEIGHT-18}, 12, 1, (Color){200,100,0,255});
}

// ===== INITIALIZATION FUNCTION =====
void InitializeAppState(AppState *app) {
    app->calendar.year = 2026;
    app->calendar.month = 1;
    app->calendar.selectedDay = 1;
    app->calendar.semester = 0;
    
    app->input.inputStep = 0;
    strcpy(app->input.yearInput, "2026");
    strcpy(app->input.monthInput, "1");
    strcpy(app->input.semesterInput, "0");
    app->input.yearLetterCount = 4;
    app->input.monthLetterCount = 1;
    app->input.semesterLetterCount = 1;
    
    app->currentScreen = SCREEN_INPUT;
}

// ===== MAIN =====
int main(void) {
    InitWindow(BASE_WIDTH*SCALE, BASE_HEIGHT*SCALE, "RETRO Calendar");
    SetTargetFPS(60);

    InitAudioDevice();
    
    AppState app;
    InitializeAppState(&app);
    
    app.assets.bgMusic = LoadMusicStream(songPath);
    if (app.assets.bgMusic.stream.buffer != NULL) {
        SetMusicVolume(app.assets.bgMusic, 0.5f);
        PlayMusicStream(app.assets.bgMusic);
    }

    app.assets.mainFont = LoadFontEx(fontPath, 96, 0, 0);
    if (!app.assets.mainFont.texture.id) 
        app.assets.mainFont = GetFontDefault();

    LoadBackgrounds(&app.assets);

    RenderTexture2D target = LoadRenderTexture(BASE_WIDTH, BASE_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    const char *dayName = NULL;
    int savedDay = 1;

    while (!WindowShouldClose()) {
        if (app.assets.bgMusic.stream.buffer != NULL)
            UpdateMusicStream(app.assets.bgMusic);

        if (app.currentScreen == SCREEN_INPUT) {
            UpdateInputScreen(&app.input, &app.calendar, &app.currentScreen);
        }
        else if (app.currentScreen == SCREEN_CALENDAR) {
            int dim = daysInMonth[app.calendar.month - 1];
            if (app.calendar.month == 2 && isLeapYear(app.calendar.year)) dim = 29;

            if (IsKeyPressed(KEY_RIGHT) && app.calendar.selectedDay < dim) 
                app.calendar.selectedDay++;
            if (IsKeyPressed(KEY_LEFT) && app.calendar.selectedDay > 1) 
                app.calendar.selectedDay--;
            if (IsKeyPressed(KEY_DOWN) && app.calendar.selectedDay + 7 <= dim) 
                app.calendar.selectedDay += 7;
            if (IsKeyPressed(KEY_UP) && app.calendar.selectedDay - 7 > 0) 
                app.calendar.selectedDay -= 7;

            if (IsKeyPressed(KEY_ENTER) && app.calendar.semester) {
                savedDay = app.calendar.selectedDay;
                dayName = GetDayName(app.calendar.year, app.calendar.month, app.calendar.selectedDay);
                app.currentScreen = SCREEN_ROUTINE;
            }
            
            if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_KP_0)) {
                app.currentScreen = SCREEN_INPUT;
                app.calendar.selectedDay = 1;
            }
        }
        else if (app.currentScreen == SCREEN_ROUTINE) {
            if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_KP_0)) {
                app.currentScreen = SCREEN_CALENDAR;
            }
        }

        BeginTextureMode(target);
            if (app.currentScreen == SCREEN_INPUT) {
                DrawInputScreen(&app.input, &app.assets);
            }
            else if (app.currentScreen == SCREEN_CALENDAR) {
                DrawCalendar(&app.calendar, &app.assets);
            }
            else if (app.currentScreen == SCREEN_ROUTINE) {
                DrawRoutine(app.calendar.semester, dayName, savedDay, &app.assets);
            }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        int scale = fmin(GetScreenWidth()/BASE_WIDTH,
                         GetScreenHeight()/BASE_HEIGHT);
        if (scale < 1) scale = 1;

        Rectangle src = {0,0,BASE_WIDTH,-BASE_HEIGHT};
        Rectangle dst = {
            (GetScreenWidth()-BASE_WIDTH*scale)/2,
            (GetScreenHeight()-BASE_HEIGHT*scale)/2,
            BASE_WIDTH*scale,
            BASE_HEIGHT*scale
        };

        DrawTexturePro(target.texture, src, dst, (Vector2){0,0}, 0, WHITE);
        DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadRenderTexture(target);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}