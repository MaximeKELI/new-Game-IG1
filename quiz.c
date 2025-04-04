#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_PLAYERS 4
#define QUESTIONS_PER_PLAYER 3  
#define MAX_QUESTIONS 50
#define MAX_OPTIONS 4
#define MAX_NAME_LENGTH 50
#define MAX_QUESTION_LENGTH 256
#define MAX_OPTION_LENGTH 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
    int currentQuestionIndex;
    int questionIndices[QUESTIONS_PER_PLAYER];
} Player;

typedef struct {
    char question[MAX_QUESTION_LENGTH];
    char options[MAX_OPTIONS][MAX_OPTION_LENGTH];
    int correctAnswer;
    bool used;
} Question;

Question questions[MAX_QUESTIONS];
Player players[MAX_PLAYERS];
int numPlayers = 0;
int currentPlayer = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
SDL_Color textColor = { 255, 255, 255, 255 };
SDL_Color highlightColor = { 255, 215, 0, 255 };

void initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Quiz Informatique", 
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Erreur création fenêtre: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(window, -1, 
                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Erreur création renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    const char* fontPaths[] = {
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        NULL
    };

    int i = 0;
    while (fontPaths[i] && !font) {
        font = TTF_OpenFont(fontPaths[i], 24);
        if (!font) {
            fprintf(stderr, "Essai police %s échoué: %s\n", fontPaths[i], TTF_GetError());
        }
        i++;
    }

    if (!font) {
        fprintf(stderr, "\nERREUR: Aucune police trouvée!\n");
        fprintf(stderr, "Installez des polices avec:\n");
        fprintf(stderr, "sudo apt install fonts-freefont-ttf ttf-dejavu-core fonts-liberation\n");
        exit(EXIT_FAILURE);
    }
}

void closeSDL() {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void renderText(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) {
        fprintf(stderr, "Erreur création surface texte: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        fprintf(stderr, "Erreur création texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void initializeQuestions() {
    srand(time(NULL));
    
    // Question 1
    strncpy(questions[0].question, "Quel langage a inspiré C++?", MAX_QUESTION_LENGTH - 1);
    strncpy(questions[0].options[0], "C", MAX_OPTION_LENGTH - 1);
    strncpy(questions[0].options[1], "Java", MAX_OPTION_LENGTH - 1);
    strncpy(questions[0].options[2], "Python", MAX_OPTION_LENGTH - 1);
    strncpy(questions[0].options[3], "Assembly", MAX_OPTION_LENGTH - 1);
    questions[0].correctAnswer = 0;
    questions[0].used = false;

    // Question 2
    strncpy(questions[1].question, "Quelle commande Linux liste les fichiers?", MAX_QUESTION_LENGTH - 1);
    strncpy(questions[1].options[0], "dir", MAX_OPTION_LENGTH - 1);
    strncpy(questions[1].options[1], "ls", MAX_OPTION_LENGTH - 1);
    strncpy(questions[1].options[2], "list", MAX_OPTION_LENGTH - 1);
    strncpy(questions[1].options[3], "show", MAX_OPTION_LENGTH - 1);
    questions[1].correctAnswer = 1;
    questions[1].used = false;

    // Question 3
    strncpy(questions[2].question, "Quel est le gestionnaire de paquets de Ubuntu?", MAX_QUESTION_LENGTH - 1);
    strncpy(questions[2].options[0], "yum", MAX_OPTION_LENGTH - 1);
    strncpy(questions[2].options[1], "pacman", MAX_OPTION_LENGTH - 1);
    strncpy(questions[2].options[2], "apt", MAX_OPTION_LENGTH - 1);
    strncpy(questions[2].options[3], "dnf", MAX_OPTION_LENGTH - 1);
    questions[2].correctAnswer = 2;
    questions[2].used = false;

    
    for (int i = 3; i < MAX_QUESTIONS; i++) {
        questions[i].used = true;
    }
}

void assignQuestionsToPlayers() {
    
    for (int i = 0; i < 3; i++) {
        questions[i].used = false;
    }

    for (int p = 0; p < numPlayers; p++) {
        
        for (int i = 0; i < 3; i++) {
            players[p].questionIndices[i] = i;
        }
        
        players[p].currentQuestionIndex = 0;
        players[p].score = 0;
    }
}

void getNumberOfPlayers() {
    SDL_Event e;
    bool done = false;

    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(EXIT_SUCCESS);
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_4) {
                    numPlayers = e.key.keysym.sym - SDLK_0;
                    done = true;
                }
                else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    exit(EXIT_SUCCESS);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderText("QUIZ INFORMATIQUE", SCREEN_WIDTH/2 - 150, 50, highlightColor);
        renderText("Combien de joueurs? (1-4)", SCREEN_WIDTH/2 - 150, 150, textColor);
        renderText("Appuyez sur 1, 2, 3 ou 4", SCREEN_WIDTH/2 - 150, 200, textColor);

        SDL_RenderPresent(renderer);
    }
}

void getPlayerNames() {
    SDL_Event e;
    char name[MAX_NAME_LENGTH] = "";
    int nameLength = 0;
    int currentPlayerInput = 0;

    while (currentPlayerInput < numPlayers) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(EXIT_SUCCESS);
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN && nameLength > 0) {
                    strncpy(players[currentPlayerInput].name, name, MAX_NAME_LENGTH - 1);
                    players[currentPlayerInput].name[MAX_NAME_LENGTH - 1] = '\0';
                    currentPlayerInput++;
                    name[0] = '\0';
                    nameLength = 0;
                }
                else if (e.key.keysym.sym == SDLK_BACKSPACE && nameLength > 0) {
                    name[--nameLength] = '\0';
                }
                else if (nameLength < MAX_NAME_LENGTH - 1 && isprint(e.key.keysym.sym)) {
                    name[nameLength++] = e.key.keysym.sym;
                    name[nameLength] = '\0';
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        char prompt[100];
        snprintf(prompt, sizeof(prompt), "Entrez le nom du joueur %d:", currentPlayerInput + 1);
        renderText(prompt, SCREEN_WIDTH/2 - 150, 150, textColor);
        
        char namePrompt[MAX_NAME_LENGTH + 50] = "Nom: ";
        strncat(namePrompt, name, sizeof(namePrompt) - strlen(namePrompt) - 1);
        renderText(namePrompt, SCREEN_WIDTH/2 - 150, 200, textColor);
        renderText("Appuyez sur Entrée pour valider", SCREEN_WIDTH/2 - 150, 250, textColor);

        SDL_RenderPresent(renderer);
    }
}

void showQuestion() {
    Player* current = &players[currentPlayer];
    if (current->currentQuestionIndex >= 3) {  
        currentPlayer = (currentPlayer + 1) % numPlayers;
        return;
    }

    int qIndex = current->questionIndices[current->currentQuestionIndex];
    Question* q = &questions[qIndex];
    
    SDL_Event e;
    bool answered = false;
    int selectedOption = -1;

    while (!answered) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(EXIT_SUCCESS);
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_4) {
                    selectedOption = e.key.keysym.sym - SDLK_1;
                    answered = true;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        
        char playerInfo[100];
        snprintf(playerInfo, sizeof(playerInfo), "Joueur: %s - Score: %d", 
                current->name, current->score);
        renderText(playerInfo, 50, 30, textColor);

        
        renderText(q->question, 50, 100, highlightColor);

        
        for (int i = 0; i < MAX_OPTIONS; i++) {
            char optionText[150];
            snprintf(optionText, sizeof(optionText), "%d. %s", i+1, q->options[i]);
            renderText(optionText, 100, 180 + i * 40, textColor);
        }

        renderText("Choisissez une option (1-4):", 50, 350, textColor);

        SDL_RenderPresent(renderer);
    }

    
    if (selectedOption == q->correctAnswer) {
        current->score += 10;
        
        SDL_SetRenderDrawColor(renderer, 0, 50, 0, 255);
        SDL_RenderClear(renderer);
        renderText("Bonne réponse!", SCREEN_WIDTH/2 - 100, 200, highlightColor);
        SDL_RenderPresent(renderer);
        SDL_Delay(1500);
    } else {
        SDL_SetRenderDrawColor(renderer, 50, 0, 0, 255);
        SDL_RenderClear(renderer);
        renderText("Mauvaise réponse!", SCREEN_WIDTH/2 - 100, 200, highlightColor);
        
        char correctAnswer[256];
        snprintf(correctAnswer, sizeof(correctAnswer), "La bonne réponse était: %s", 
                q->options[q->correctAnswer]);
        renderText(correctAnswer, SCREEN_WIDTH/2 - 200, 250, textColor);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(2000);
    }

    current->currentQuestionIndex++;
    currentPlayer = (currentPlayer + 1) % numPlayers;
}

bool allPlayersFinished() {
    for (int i = 0; i < numPlayers; i++) {
        if (players[i].currentQuestionIndex < 3) {  
            return false;
        }
    }
    return true;
}

void showWinners() {
    
    int maxScore = 0;
    for (int i = 0; i < numPlayers; i++) {
        if (players[i].score > maxScore) {
            maxScore = players[i].score;
        }
    }

    
    int winnerCount = 0;
    for (int i = 0; i < numPlayers; i++) {
        if (players[i].score == maxScore) {
            winnerCount++;
        }
    }

    SDL_Event e;
    bool done = false;

    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) {
                done = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
        SDL_RenderClear(renderer);

        if (winnerCount == 1) {
            renderText("VAINQUEUR:", SCREEN_WIDTH/2 - 100, 50, highlightColor);
        } else {
            renderText("VAINQUEURS (ex-aequo):", SCREEN_WIDTH/2 - 150, 50, highlightColor);
        }

 
        int yPos = 150;
        for (int i = 0; i < numPlayers; i++) {
            char scoreText[100];
            snprintf(scoreText, sizeof(scoreText), "%s: %d points", 
                    players[i].name, players[i].score);
            
            if (players[i].score == maxScore) {
                renderText(scoreText, SCREEN_WIDTH/2 - 150, yPos, highlightColor);
            } else {
                renderText(scoreText, SCREEN_WIDTH/2 - 150, yPos, textColor);
            }
            yPos += 40;
        }

        renderText("Appuyez sur une touche pour quitter", 
                 SCREEN_WIDTH/2 - 200, 400, textColor);

        SDL_RenderPresent(renderer);
    }
}

int main() {
    initializeSDL();
    initializeQuestions();
    
    getNumberOfPlayers();
    getPlayerNames();
    assignQuestionsToPlayers();

    while (!allPlayersFinished()) {
        showQuestion();
    }

    showWinners();
    closeSDL();

    return EXIT_SUCCESS;
}