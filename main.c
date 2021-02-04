#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 25
// ^-- Unfortunately necessary for arrays
// Height is 25 to allow for J and L to spawn correctly

const int SQUARE_WIDTH = 18;
const int CURRENT_PIECE_NUMBER = 9; /* Placeholder for current piece */

const SDL_Color BACKGROUND = {86, 129, 163, 255};
const SDL_Color WHITE = {255, 255, 255};
const SDL_Color GREY = {211, 211, 211};

// Pieces
// 0 = I, 1 = J, 2 = L, 3 = O, 4 = S, 5 = T, 6 = Z

enum PieceMap { Piece_I, Piece_J, Piece_L, Piece_O, Piece_S, Piece_T, Piece_Z };
enum Direction { Direction_Down, Direction_Left, Direction_Right };

const SDL_Color pieceColors[8] = {
    {255, 255, 255},  // 0 = no square so skip
    {0, 255, 255},    // Cyan
    {0, 0, 255},      // Blue
    {255, 165, 0},    // Orange
    {255, 255, 0},    // Yellow
    {0, 255, 0},      // Green
    {128, 0, 128},    // Purple
    {255, 0, 0}       // Red
};

typedef struct {
    int x;
    int y;
} Coord;

// clang-format off
const int pieceRotations[7][4][4][4] = {
    // I
    {
        {
            {0, 0, 1, 0,},
            {0, 0, 1, 0,},
            {0, 0, 1, 0,},
            {0, 0, 1, 0,},
        },
        {
            {0, 0, 0, 0,},
            {1, 1, 1, 1,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 0, 1, 0,},
            {0, 0, 1, 0,},
            {0, 0, 1, 0,},
            {0, 0, 1, 0,},
        },
        {
            {0, 0, 0, 0,},
            {1, 1, 1, 1,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
    },
    // J
    {
        {
            {0, 0, 0, 0,},
            {1, 1, 1, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 1, 0, 0,},
            {0, 1, 0, 0,},
            {1, 1, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {1, 0, 0, 0,},
            {1, 1, 1, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 1, 1, 0,},
            {0, 1, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 0, 0,},
        },
    },
    // L
    {
        {
            {0, 0, 0, 0,},
            {1, 1, 1, 0,},
            {1, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {1, 1, 0, 0,},
            {0, 1, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 0, 1, 0,},
            {1, 1, 1, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 1, 0, 0,},
            {0, 1, 0, 0,},
            {0, 1, 1, 0,},
            {0, 0, 0, 0,},
        },
    },
    // O
    {
        {
            {1, 1, 0, 0,},
            {1, 1, 0, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {1, 1, 0, 0,},
            {1, 1, 0, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {1, 1, 0, 0,},
            {1, 1, 0, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {1, 1, 0, 0,},
            {1, 1, 0, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
    },
    // S
    {
        {
            {0, 1, 1, 0,},
            {1, 1, 0, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 1, 0, 0,},
            {0, 1, 1, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 0, 0, 0,},
            {0, 1, 1, 0,},
            {1, 1, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {1, 0, 0, 0,},
            {1, 1, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 0, 0,},
        },
    },
    // T
    {
        {
            {0, 1, 0, 0,},
            {1, 1, 1, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 1, 0, 0,},
            {0, 1, 1, 0,},
            {0, 1, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 0, 0, 0,},
            {1, 1, 1, 0,},
            {0, 1, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 1, 0, 0,},
            {1, 1, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 0, 0,},
        },
    },
    // Z
    {
        {
            {1, 1, 0, 0,},
            {0, 1, 1, 0,},
            {0, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 0, 1, 0,},
            {0, 1, 1, 0,},
            {0, 1, 0, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 0, 0, 0,},
            {1, 1, 0, 0,},
            {0, 1, 1, 0,},
            {0, 0, 0, 0,},
        },
        {
            {0, 1, 0, 0,},
            {1, 1, 0, 0,},
            {1, 0, 0, 0,},
            {0, 0, 0, 0,},
        },
    }
};
// clang-format on

const int pieceOffsets[7][2] = {
    {0, 0}, {-1, 0}, {-1, 0}, {0, 1}, {0, 0}, {0, 0}, {0, 0},
};

// Utility
bool initializeSDL();
int max(int a, int b) { return a > b ? a : b; }

// For debugging purposes
void printPlayfield(int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]) {
    for (int i = 0; i < PLAYFIELD_HEIGHT; ++i) {
        printf("[ ");
        for (int j = 0; j < PLAYFIELD_WIDTH; ++j) {
            printf("%d ", playfield[i][j]);
        }
        printf("]\n");
    }
}

// Rendering stuff
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                SDL_Rect *rect, SDL_Color color, int x, int y);
void renderPlayfieldBackground(SDL_Renderer *renderer,
                               SDL_Rect playfieldDimensions,
                               SDL_Rect playfieldBorder);
void renderPlayfieldGridlines(SDL_Renderer *renderer,
                              SDL_Rect playfieldDimensions);
void renderPlayfield(SDL_Renderer *renderer,
                     int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                     SDL_Rect playfieldDimensions, int currentColor);

// Actual game functions
int getMaxRightBound(SDL_Rect pieceBounds);
int getMaxBottomBound(SDL_Rect pieceBounds);
int getColorNumber(int pieceIndex, int squareValue);
bool spawnRandomPiece(int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                      SDL_Rect *pieceBounds, int *currentColor,
                      int *pieceIndex);
bool canMoveInDirection(SDL_Rect pieceBounds,
                        int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                        enum Direction direction);
bool canRotatePiece(SDL_Rect *pieceBounds,
                    int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                    int pieceIndex, int *currentRotation);
void dropPieceOneRow(SDL_Rect *pieceBounds,
                     int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]);
void movePieceLeft(SDL_Rect *pieceBounds,
                   int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]);
void movePieceRight(SDL_Rect *pieceBounds,
                    int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]);
void rotatePiece(SDL_Rect *pieceBounds,
                 int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                 int pieceIndex, int *currentRotation);
void convertPieceToStatic(SDL_Rect pieceBounds,
                          int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                          int currentColor);
int clearEmptyRows(int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]);
void shiftAllRowsDown(int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                      int end);
int scoreForRowsCleared(int rowsCleared);

int main() {
    srand(time(0));

    int score = 0;
    int playfieldGrid[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH] = {0};
    /* for (int j = 1; j < 10; ++j) { */
    /*     playfieldGrid[24][j] = 5; */
    /* } */
    /* playfieldGrid[15][4] = 3; */

    if (!initializeSDL()) {
        printf("Initialization failed!\n");
    }

    SDL_Window *window = SDL_CreateWindow("Yeetris", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                          WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window) {
        printf("Couldn't initialize window!\n");
        return 0;
    }

    if (!renderer) {
        printf("Couldn't initialize renderer!\n");
        return 0;
    }

    // So that background color doesn't change from black to blue
    SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g, BACKGROUND.b,
                           BACKGROUND.a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    TTF_Font *arial = TTF_OpenFont("arial.ttf", 25);

    // Setup title
    // SDL_Surface* title = TTF_RenderText_Solid(arial, "Yeetris", WHITE);
    // SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer,
    // title);

    // int titleWidth;
    // int titleHeight;
    // SDL_QueryTexture(titleTexture, NULL, NULL, &titleWidth, &titleHeight);

    // // Centered at top
    // SDL_Rect titleRect = { WINDOW_WIDTH / 2 - titleWidth / 2, 0, titleWidth,
    // titleHeight };

    // Setup playfield
    SDL_Rect playfieldRect = {WINDOW_WIDTH / 2 - 5 * SQUARE_WIDTH, 35,
                              10 * SQUARE_WIDTH, 24 * SQUARE_WIDTH};
    SDL_Rect playfieldBorderRect = playfieldRect;
    playfieldBorderRect.w += 1;
    playfieldBorderRect.h += 1;
    // ^-- fixes a bunch of border related visual issues

    SDL_Event e;
    bool quit = false;

    int counter = 0;
    SDL_Rect textLocation;
    const int SCORE_LENGTH = 20;
    char scoreText[20];

    // Timestep stuff from https://gafferongames.com/post/fix_your_timestep/
    double currentTime = SDL_GetTicks();
    double accumulator = 0;
    int FPS = 60;

    int numberOfFramesToFall = 90;
    int framesSinceLastFall = 0;
    SDL_Rect pieceBounds;
    int pieceIndex = 0;
    int rotationIndex = 0;
    int currentColor;
    spawnRandomPiece(playfieldGrid, &pieceBounds, &currentColor, &pieceIndex);
    printPlayfield(playfieldGrid);

    while (!quit) {
        double newTime = SDL_GetTicks();
        double frameTime = newTime - currentTime;
        currentTime = newTime;

        accumulator += frameTime;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_DOWN:
                        printf("Down is held down!\n");
                        numberOfFramesToFall = 2;
                        break;
                    case SDLK_LEFT:
                        printf("Left is held down\n");
                        if (canMoveInDirection(pieceBounds, playfieldGrid,
                                               Direction_Left)) {
                            movePieceLeft(&pieceBounds, playfieldGrid);
                        }
                        break;
                    case SDLK_RIGHT:
                        printf("Right is held down\n");
                        if (canMoveInDirection(pieceBounds, playfieldGrid,
                                               Direction_Right)) {
                            movePieceRight(&pieceBounds, playfieldGrid);
                        }
                        break;
                    case SDLK_UP:
                        printf("UP is held down\n");
                        if (canRotatePiece(&pieceBounds, playfieldGrid,
                                           pieceIndex, &rotationIndex)) {
                            rotatePiece(&pieceBounds, playfieldGrid, pieceIndex,
                                        &rotationIndex);
                        }
                        break;
                }
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_DOWN:
                        printf("Down was released!\n");
                        numberOfFramesToFall = 30;
                        break;
                }
            }
        }

        while (accumulator > (1000. / FPS)) {
            ++framesSinceLastFall;
            if (framesSinceLastFall > numberOfFramesToFall) {
                /* printf("Current color = %d\n", currentColor); */
                if (canMoveInDirection(pieceBounds, playfieldGrid,
                                       Direction_Down)) {
                    dropPieceOneRow(&pieceBounds, playfieldGrid);
                } else {
                    convertPieceToStatic(pieceBounds, playfieldGrid,
                                         currentColor);
                    int rowsCleared = clearEmptyRows(playfieldGrid);
                    printf("Were able to clear rows\n");
                    score += scoreForRowsCleared(rowsCleared);
                    bool success = spawnRandomPiece(playfieldGrid, &pieceBounds,
                                                    &currentColor, &pieceIndex);
                    rotationIndex = 0;
                    if (!success) {
                        printf("Game over!\n");
                        quit = true;
                    }
                }
                /* printf("New piece bounds: { x = %d, y = %d }\n",
                 * pieceBounds.x, */
                /*        pieceBounds.y); */
                /* printf("Drop one\n"); */
                /* printPlayfield(playfieldGrid); */
                framesSinceLastFall = 0;
            }
            // Update state stuff
            ++counter;
            accumulator -= 1000. / FPS;
        }

        if (quit) {
            break;
        }

        /* Rendering stuff */
        SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g,
                               BACKGROUND.b, BACKGROUND.a);
        SDL_RenderClear(renderer);

        // Draw title
        // SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        renderText(renderer, arial, "Yeetris", &textLocation, WHITE, 5, 0);
        snprintf(scoreText, SCORE_LENGTH, "Score: %d", score);
        renderText(renderer, arial, scoreText, &textLocation, WHITE, 5,
                   textLocation.y + textLocation.h);

        // Draw playfield background/grid
        renderPlayfieldBackground(renderer, playfieldRect, playfieldBorderRect);

        // Render all the tiles in it the playfield
        renderPlayfield(renderer, playfieldGrid, playfieldRect, currentColor);

        SDL_RenderPresent(renderer);
    }

    printf("Counter reached: %d\n", counter);
    printf("time reached: %f\n", currentTime);
    printf("Overall FPS: %f\n", (counter / (currentTime / 1000.)));

    //     SDL_DestroyTexture(titleTexture);
    //     SDL_FreeSurface(title);

    TTF_CloseFont(arial);
    arial = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    TTF_Quit();
    SDL_Quit();
}

bool initializeSDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Couldn't initialize SDL!\n");
        return false;
    }

    if (TTF_Init() < 0) {
        printf("Couldn't initialize SDL_TTF!\n");
        return false;
    }

    return true;
}

// Inefficient solution slightly modified from
// https://stackoverflow.com/questions/22852226/c-sdl2-how-to-regularly-update-a-renderered-text-ttf
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                SDL_Rect *rect, SDL_Color color, int x, int y) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    rect->x = x;
    rect->y = y;
    rect->w = surface->w;
    rect->h = surface->h;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_DestroyTexture(texture);
}

void renderPlayfieldBackground(SDL_Renderer *renderer,
                               SDL_Rect playfieldDimensions,
                               SDL_Rect playfieldBorder) {
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderFillRect(renderer, &playfieldDimensions);

    SDL_SetRenderDrawColor(renderer, GREY.r, GREY.g, GREY.b, GREY.a);

    // Draw grid
    renderPlayfieldGridlines(renderer, playfieldDimensions);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &playfieldBorder);
}

void renderPlayfieldGridlines(SDL_Renderer *renderer,
                              SDL_Rect playfieldDimensions) {
    // Draw vertical lines
    for (int i = playfieldDimensions.x;
         i < playfieldDimensions.x + playfieldDimensions.w; i += SQUARE_WIDTH) {
        SDL_RenderDrawLine(renderer, i, playfieldDimensions.y, i,
                           playfieldDimensions.y + playfieldDimensions.h);
    }

    // Draw horizontal lines
    for (int i = playfieldDimensions.y;
         i < playfieldDimensions.y + playfieldDimensions.h; i += SQUARE_WIDTH) {
        SDL_RenderDrawLine(renderer, playfieldDimensions.x, i,
                           playfieldDimensions.x + playfieldDimensions.w, i);
    }
}

void renderPlayfield(SDL_Renderer *renderer,
                     int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                     SDL_Rect playfieldDimensions, int currentColor) {
    for (int i = 0; i < PLAYFIELD_HEIGHT - 1;
         ++i) {  // Ignore first row when rendering
        for (int j = 0; j < PLAYFIELD_WIDTH; ++j) {
            /* Reset color to white */
            SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b,
                                   WHITE.a);

            /* Figure out where this square will actually be */
            int currentSquare = playfield[i + 1][j];
            /* if (currentSquare == 0) { */
            /*     continue; */
            /* } */

            SDL_Color correspondingColor = currentSquare != CURRENT_PIECE_NUMBER
                                               ? pieceColors[currentSquare]
                                               : pieceColors[currentColor];
            SDL_Rect convertedLocation;
            convertedLocation.x = playfieldDimensions.x + j * SQUARE_WIDTH + 1;
            convertedLocation.y = playfieldDimensions.y + i * SQUARE_WIDTH + 1;
            convertedLocation.w = convertedLocation.h = SQUARE_WIDTH - 1;
            /* All the +1 and -1's are to make the square fit nicely within the
             */
            /* grid */

            /* Actually draw it */
            SDL_SetRenderDrawColor(renderer, correspondingColor.r,
                                   correspondingColor.g, correspondingColor.b,
                                   correspondingColor.a);
            SDL_RenderFillRect(renderer, &convertedLocation);
        }
    }
}

int getMaxRightBound(SDL_Rect pieceBounds) {
    return max(pieceBounds.x + pieceBounds.w, PLAYFIELD_WIDTH);
}

int getMaxBottomBound(SDL_Rect pieceBounds) {
    return max(pieceBounds.y + pieceBounds.h, PLAYFIELD_HEIGHT);
}

int getColorNumber(int pieceIndex, int squareValue) {
    return squareValue == 0 ? 0 : pieceIndex + 1;
}

bool spawnRandomPiece(int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                      SDL_Rect *pieceBounds, int *currentColor,
                      int *pieceIndex) {
    *pieceIndex = rand() % 7;

    int rotations[4][4][4];
    memcpy(&rotations, &pieceRotations[*pieceIndex],
           sizeof(pieceRotations[*pieceIndex]));

    int rowOffset = pieceOffsets[*pieceIndex][0];
    int columnOffset = pieceOffsets[*pieceIndex][1];

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int currentSquare = rotations[0][i][j];

            if (playfield[i + rowOffset + 1][j + columnOffset + 3] != 0 &&
                currentSquare != 0) {
                return false;
            }
            /* playfield[i + rowOffset + 1][j + columnOffset + 3] = */
            /*     getColorNumber(pieceIndex, rotations[0][i][j]); */
            if (currentSquare == 0) {
                continue;
            }
            playfield[i + rowOffset + 1][j + columnOffset + 3] =
                CURRENT_PIECE_NUMBER;
            /* printf("Assigning playfield[%d][%d] to %d\n", i + rowOffset + 1,
             */
            /* j + columnOffset + 3, rotations[0][i][j]); */
        }
    }

    *currentColor = *pieceIndex + 1;

    pieceBounds->x = columnOffset + 3;
    pieceBounds->y = rowOffset + 1;
    pieceBounds->w = pieceBounds->h = 4;
    return true;
}

bool canMoveInDirection(SDL_Rect pieceBounds,
                        int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                        enum Direction direction) {
    if (direction == Direction_Down) {
        for (int i = pieceBounds.y; i < getMaxBottomBound(pieceBounds); ++i) {
            for (int j = pieceBounds.x; j < getMaxRightBound(pieceBounds);
                 ++j) {
                int currentSquare = playfield[i][j];
                if (currentSquare != CURRENT_PIECE_NUMBER) {
                    continue;
                }

                if (i + 1 == PLAYFIELD_HEIGHT) { /* Reached bottom */
                    return false;
                }

                if (playfield[i + 1][j] != CURRENT_PIECE_NUMBER &&
                    playfield[i + 1][j] != 0) { /* Collided with something */
                    printf("Collided with something below\n");
                    return false;
                }
            }
        }
        return true;
    } else if (direction == Direction_Left) {
        for (int i = pieceBounds.y; i < getMaxBottomBound(pieceBounds); ++i) {
            for (int j = pieceBounds.x; j < getMaxRightBound(pieceBounds);
                 ++j) {
                int currentSquare = playfield[i][j];
                if (currentSquare != CURRENT_PIECE_NUMBER) {
                    continue;
                }

                if (j - 1 < 0) { /* Reached left wall */
                    return false;
                }

                if (playfield[i][j - 1] != CURRENT_PIECE_NUMBER &&
                    playfield[i][j - 1] != 0) { /* Collided with something */
                    printf("Collided with something to left\n");
                    return false;
                }
            }
        }

        return true;
    } else if (direction == Direction_Right) {
        for (int i = pieceBounds.y; i < getMaxBottomBound(pieceBounds); ++i) {
            for (int j = pieceBounds.x; j < getMaxRightBound(pieceBounds);
                 ++j) {
                int currentSquare = playfield[i][j];
                if (currentSquare != CURRENT_PIECE_NUMBER) {
                    continue;
                }

                if (j + 1 == PLAYFIELD_WIDTH) { /* Reached right wall */
                    return false;
                }

                if (playfield[i][j + 1] != CURRENT_PIECE_NUMBER &&
                    playfield[i][j + 1] != 0) { /* Collided with something */
                    printf("Collided with something to right\n");
                    return false;
                }
            }
        }

        return true;
    }
    return false;
}

bool canRotatePiece(SDL_Rect *pieceBounds,
                    int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                    int pieceIndex, int *currentRotation) {
    int rotations[4][4][4];
    memcpy(&rotations, &pieceRotations[pieceIndex],
           sizeof(pieceRotations[pieceIndex]));

    /* for (int i = pieceBounds->y; i < pieceBounds->y + pieceBounds->h; ++i) {
     */
    /*     for (int j = pieceBounds->x; j < pieceBounds->x + pieceBounds->w;
     * ++j) { */
    /*         int currentSquare = playfield[i][j]; */
    /*         int correspondingRotationSquare = */
    /*             rotations[*currentRotation + 1][i - pieceBounds->y] */
    /*                      [j - pieceBounds->x]; */
    /*         if (correspondingRotationSquare == 1 && */
    /*             currentSquare != CURRENT_PIECE_NUMBER && currentSquare != 0)
     * { */
    /*             // Rotation would cause collision */
    /*             return false; */
    /*         } */
    /*     } */
    /* } */
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int rotationSquare = rotations[(*currentRotation + 1) % 4][i][j];
            if (rotationSquare == 0) {
                continue;
            }
            int playfieldRow = i + pieceBounds->y;
            int playfieldColumn = j + pieceBounds->x;
            if (playfieldRow >= PLAYFIELD_HEIGHT ||
                playfieldColumn >= PLAYFIELD_WIDTH || playfieldColumn < 0) {
                return false;
            }
        }
    }

    return true;
}
void dropPieceOneRow(SDL_Rect *pieceBounds,
                     int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]) {
    for (int i = getMaxBottomBound(*pieceBounds); i >= pieceBounds->y; --i) {
        for (int j = pieceBounds->x; j < getMaxRightBound(*pieceBounds); ++j) {
            if (j >= PLAYFIELD_WIDTH || i >= PLAYFIELD_HEIGHT) {
                continue;
            }
            if (playfield[i][j] != CURRENT_PIECE_NUMBER) {
                continue;
            }
            playfield[i + 1][j] = playfield[i][j];
            playfield[i][j] = 0;
        }
    }
    pieceBounds->y++;
}

void movePieceLeft(SDL_Rect *pieceBounds,
                   int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]) {
    for (int i = pieceBounds->y; i < getMaxBottomBound(*pieceBounds); ++i) {
        for (int j = pieceBounds->x; j < getMaxRightBound(*pieceBounds); ++j) {
            if (j >= PLAYFIELD_WIDTH || i >= PLAYFIELD_HEIGHT) {
                continue;
            }
            if (playfield[i][j] != CURRENT_PIECE_NUMBER) {
                continue;
            }
            playfield[i][j - 1] = playfield[i][j];
            playfield[i][j] = 0;
        }
    }
    pieceBounds->x--;
}

void movePieceRight(SDL_Rect *pieceBounds,
                    int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]) {
    for (int i = pieceBounds->y; i < getMaxBottomBound(*pieceBounds); ++i) {
        for (int j = getMaxRightBound(*pieceBounds); j >= pieceBounds->x; --j) {
            if (j >= PLAYFIELD_WIDTH || i >= PLAYFIELD_HEIGHT) {
                continue;
            }
            if (playfield[i][j] != CURRENT_PIECE_NUMBER) {
                continue;
            }
            playfield[i][j + 1] = playfield[i][j];
            playfield[i][j] = 0;
        }
    }
    pieceBounds->x++;
}

void rotatePiece(SDL_Rect *pieceBounds,
                 int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                 int pieceIndex, int *currentRotation) {
    int rotations[4][4][4];
    memcpy(&rotations, &pieceRotations[pieceIndex],
           sizeof(pieceRotations[pieceIndex]));

    *currentRotation = (*currentRotation + 1) % 4;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int rotationSquare = rotations[*currentRotation][i][j];
            if (rotationSquare == 0) {
                continue;
            }
            int playfieldRow = i + pieceBounds->y;
            int playfieldColumn = j + pieceBounds->x;
            playfield[playfieldRow][playfieldColumn] =
                10;  // Temporary value to erase all the old squares
        }
    }

    for (int i = pieceBounds->y; i < getMaxBottomBound(*pieceBounds); ++i) {
        for (int j = pieceBounds->x; j < getMaxRightBound(*pieceBounds); ++j) {
            if (playfield[i][j] == 9) {
                playfield[i][j] = 0;
            }
            if (playfield[i][j] == 10) {
                playfield[i][j] = 9;
            }
        }
    }
}

void convertPieceToStatic(SDL_Rect pieceBounds,
                          int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                          int currentColor) {
    for (int i = pieceBounds.y; i < getMaxBottomBound(pieceBounds); ++i) {
        for (int j = pieceBounds.x; j < getMaxRightBound(pieceBounds); ++j) {
            if (playfield[i][j] == CURRENT_PIECE_NUMBER) {
                playfield[i][j] = currentColor;
            }
        }
    }
}

int clearEmptyRows(int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]) {
    /* bool needToShiftRowsDown = false; */
    /* bool emptyRows[PLAYFIELD_HEIGHT] = {0}; */
    int nextRowIndex = 0;
    int rowsCleared = 0;
    int emptyRows[4]; /* Can only clear up to 4 rows */

    for (int i = 0; i < 4; ++i) {
        emptyRows[i] = -1;
    }

    for (int i = 1; i < PLAYFIELD_HEIGHT; ++i) {
        bool hasZero = false;
        for (int j = 0; j < PLAYFIELD_WIDTH; ++j) {
            if (playfield[i][j] == 0) {
                hasZero = true;
                break;
            }
        }
        if (!hasZero) {
            printf("Need to clear!\n");

            /* If the row has no zeros, clear it */
            for (int j = 0; j < PLAYFIELD_WIDTH; ++j) {
                playfield[i][j] = 0;
            }

            ++rowsCleared;
            emptyRows[nextRowIndex++] = i;
        }
    }

    if (rowsCleared == 0) {
        /* printf("Got here\n"); */
        return 0;
    }

    /* int i = PLAYFIELD_HEIGHT - 1; */
    /* #<{(| int nextEmptyRow = -1; |)}># */
    /* while (i >= 2) { */
    /*     if (emptyRows[i]) { */
    /*         printf("Row %d was empty!\n", i); */
    /*         #<{(| nextEmptyRow = i - 1; |)}># */
    /*         #<{(| while (!emptyRows[nextEmptyRow] || i == 1) { |)}># */
    /*         #<{(|     --nextEmptyRow; |)}># */
    /*         #<{(| } |)}># */
    /*         for (int j = 0; j < PLAYFIELD_WIDTH; ++j) { */
    /*             playfield[i][j] = playfield[i - 1][j]; */
    /*         } */
    /*         #<{(| Now the previous row needs to be filled down |)}># */
    /*         emptyRows[i - 1] = true; */
    /*     } */
    /*     --i; */
    /* } */

    /* for (int i = PLAYFIELD_HEIGHT - 1; i >= 1; --i) { */
    /*     if ( */
    /* } */
    int offset = 0;
    for (int i = nextRowIndex - 1; i >= 0; --i) {
        printf("Moving down to row %d\n", emptyRows[i] + offset);
        shiftAllRowsDown(playfield, emptyRows[i] + offset);
        ++offset;
    }

    return rowsCleared;
}

void shiftAllRowsDown(int playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH],
                      int end) {
    printf("Got here to shift\n");
    for (int i = end; i >= 1; --i) {
        for (int j = 0; j < PLAYFIELD_WIDTH; ++j) {
            playfield[i][j] = playfield[i - 1][j];
        }
    }
}

int scoreForRowsCleared(int rowsCleared) {
    switch (rowsCleared) {
        case 1:
            return 100;
        case 2:
            return 300;
        case 3:
            return 500;
        case 4:
            return 800;
        default:
            return 0;
    }
}
