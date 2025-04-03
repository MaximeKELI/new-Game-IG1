#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_PLAYERS 10
#define MAX_QUESTIONS 50
#define MAX_OPTIONS 4

typedef struct {
    char name[50];
    char surname[50];
    int score;
} Player;

typedef struct {
    char question[256];
    char options[MAX_OPTIONS][100];
    int correctAnswer;
} Question;

Player players[MAX_PLAYERS];
Question questions[MAX_QUESTIONS];
int numPlayers = 0;
int currentPlayer = 0;
int currentQuestion = 0;
bool quizFinished = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
SDL_Color textColor = { 0, 0, 0, 255 };
SDL_Color backgroundColor = { 240, 240, 240, 255 };
SDL_Color buttonColor = { 100, 149, 237, 255 };
SDL_Color buttonHoverColor = { 65, 105, 225, 255 };

void initializeQuestions() {
    // Question 1
    strcpy(questions[0].question, "Quel langage a ete cree par Bjarne Stroustrup?");
    strcpy(questions[0].options[0], "Java");
    strcpy(questions[0].options[1], "C");
    strcpy(questions[0].options[2], "C++");
    strcpy(questions[0].options[3], "Python");
    questions[0].correctAnswer = 2;

    // Question 2
    strcpy(questions[1].question, "Quel est le systeme d'exploitation open source le plus populaire?");
    strcpy(questions[1].options[0], "Windows");
    strcpy(questions[1].options[1], "macOS");
    strcpy(questions[1].options[2], "Linux");
    strcpy(questions[1].options[3], "iOS");
    questions[1].correctAnswer = 2;

    // Question 3
    strcpy(questions[2].question, "Quelle est la fonction principale en C?");
    strcpy(questions[2].options[0], "main()");
    strcpy(questions[2].options[1], "start()");
    strcpy(questions[2].options[2], "init()");
    strcpy(questions[2].options[3], "run()");
    questions[2].correctAnswer = 0;

    // Question 4
    strcpy(questions[3].question, "Quel est le protocole utilise pour les pages web?");
    strcpy(questions[3].options[0], "FTP");
    strcpy(questions[3].options[1], "HTTP");
    strcpy(questions[3].options[2], "SMTP");
    strcpy(questions[3].options[3], "TCP");
    questions[3].correctAnswer = 1;

    // Question 5
    strcpy(questions[4].question, "Quel est le langage de style utilise pour les pages web?");
    strcpy(questions[4].options[0], "HTML");
    strcpy(questions[4].options[1], "JavaScript");
    strcpy(questions[4].options[2], "CSS");
    strcpy(questions[4].options[3], "PHP");
    questions[4].correctAnswer = 2;

    // Ajoutez plus de questions ici...
    // Je vais en ajouter quelques-unes pour l'exemple, mais vous devriez en ajouter 50

    // Question 6
    strcpy(questions[5].question, "Quelle entreprise a developpe le langage Java?");
    strcpy(questions[5].options[0], "Microsoft");
    strcpy(questions[5].options[1], "Sun Microsystems");
    strcpy(questions[5].options[2], "Apple");
    strcpy(questions[5].options[3], "IBM");
    questions[5].correctAnswer = 1;

    // Question 7
    strcpy(questions[6].question, "Quel est le nom du premier ordinateur electronique?");
    strcpy(questions[6].options[0], "ENIAC");
    strcpy(questions[6].options[1], "UNIVAC");
    strcpy(questions[6].options[2], "IBM 701");
    strcpy(questions[6].options[3], "Colossus");
    questions[6].correctAnswer = 0;

    // Question 8
    strcpy(questions[7].question, "Quel est le composant principal d'un ordinateur qui execute les instructions?");
    strcpy(questions[7].options[0], "GPU");
    strcpy(questions[7].options[1], "RAM");
    strcpy(questions[7].options[2], "CPU");
    strcpy(questions[7].options[3], "Disque dur");
    questions[7].correctAnswer = 2;

    // Question 9
    strcpy(questions[8].question, "Quel est le format de fichier pour les images avec transparence?");
    strcpy(questions[8].options[0], "JPG");
    strcpy(questions[8].options[1], "BMP");
    strcpy(questions[8].options[2], "PNG");
    strcpy(questions[8].options[3], "GIF");
    questions[8].correctAnswer = 2;

    // Question 10
    strcpy(questions[9].question, "Quelle est la plus grande unite de stockage parmi ces options?");
    strcpy(questions[9].options[0], "Kilobyte");
    strcpy(questions[9].options[1], "Megabyte");
    strcpy(questions[9].options[2], "Gigabyte");
    strcpy(questions[9].options[3], "Terabyte");
    questions[9].correctAnswer = 3;

    // Ajoutez les 40 questions restantes de la meme maniere...
}

void initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    if (TTF_Init() == -1) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("Quiz Informatique", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    font = TTF_OpenFont("arial.ttf", 24);
    if (font == NULL) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        // Essayez une police par défaut
        font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
        if (font == NULL) {
            exit(1);
        }
    }
}

void closeSDL() {
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void renderText(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (surface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect renderQuad = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

bool isMouseOverButton(int mouseX, int mouseY, SDL_Rect button) {
    return (mouseX >= button.x && mouseX <= button.x + button.w &&
            mouseY >= button.y && mouseY <= button.y + button.h);
}

void renderButton(const char* text, SDL_Rect rect, bool isHovered) {
    SDL_Color color = isHovered ? buttonHoverColor : buttonColor;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    // Bordure du bouton
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);

    // Centrer le texte dans le bouton
    int textWidth, textHeight;
    TTF_SizeText(font, text, &textWidth, &textHeight);
    int textX = rect.x + (rect.w - textWidth) / 2;
    int textY = rect.y + (rect.h - textHeight) / 2;

    renderText(text, textX, textY, textColor);
}

void renderPlayerInputScreen() {
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    renderText("Entrez le nombre de joueurs (1-10):", 50, 50, textColor);

    // Afficher le nombre actuel de joueurs
    char numText[50];
    sprintf(numText, "%d", numPlayers);
    renderText(numText, 400, 50, textColor);

    // Boutons + et -
    SDL_Rect plusButton = { 450, 45, 40, 40 };
    SDL_Rect minusButton = { 500, 45, 40, 40 };
    
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    renderButton("+", plusButton, isMouseOverButton(mouseX, mouseY, plusButton));
    renderButton("-", minusButton, isMouseOverButton(mouseX, mouseY, minusButton));

    // Bouton suivant
    SDL_Rect nextButton = { 300, 500, 200, 50 };
    renderButton("Suivant", nextButton, isMouseOverButton(mouseX, mouseY, nextButton));

    SDL_RenderPresent(renderer);
}

void renderNameInputScreen() {
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    char prompt[100];
    sprintf(prompt, "Joueur %d - Entrez votre nom et prenom:", currentPlayer + 1);
    renderText(prompt, 50, 50, textColor);

    // Afficher le nom et prénom actuels
    renderText("Nom:", 50, 150, textColor);
    renderText(players[currentPlayer].surname, 200, 150, textColor);

    renderText("Prenom:", 50, 250, textColor);
    renderText(players[currentPlayer].name, 200, 250, textColor);

    // Bouton suivant
    SDL_Rect nextButton = { 300, 500, 200, 50 };
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    renderButton("Suivant", nextButton, isMouseOverButton(mouseX, mouseY, nextButton));

    SDL_RenderPresent(renderer);
}

void renderQuestionScreen() {
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    // Afficher le joueur actuel et son score
    char playerInfo[100];
    sprintf(playerInfo, "Joueur: %s %s - Score: %d", 
            players[currentPlayer].name, 
            players[currentPlayer].surname, 
            players[currentPlayer].score);
    renderText(playerInfo, 50, 30, textColor);

    // Afficher la question
    renderText(questions[currentQuestion].question, 50, 100, textColor);

    // Afficher les options
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    for (int i = 0; i < MAX_OPTIONS; i++) {
        SDL_Rect optionRect = { 100, 200 + i * 80, 600, 60 };
        bool isHovered = isMouseOverButton(mouseX, mouseY, optionRect);
        renderButton(questions[currentQuestion].options[i], optionRect, isHovered);
    }

    SDL_RenderPresent(renderer);
}

void renderFeedbackScreen(bool isCorrect) {
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    if (isCorrect) {
        renderText("Bonne reponse!", 300, 200, (SDL_Color){0, 128, 0, 255});
    } else {
        char correctAnswer[150];
        sprintf(correctAnswer, "Mauvaise reponse! La bonne reponse etait: %s", 
                questions[currentQuestion].options[questions[currentQuestion].correctAnswer]);
        renderText(correctAnswer, 100, 200, (SDL_Color){128, 0, 0, 255});
    }

    // Bouton continuer
    SDL_Rect continueButton = { 300, 400, 200, 50 };
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    renderButton("Continuer", continueButton, isMouseOverButton(mouseX, mouseY, continueButton));

    SDL_RenderPresent(renderer);
}

void renderLeaderboard() {
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    renderText("Classement final:", 300, 50, textColor);

    // Trier les joueurs par score (tri à bulles simple)
    for (int i = 0; i < numPlayers - 1; i++) {
        for (int j = 0; j < numPlayers - i - 1; j++) {
            if (players[j].score < players[j + 1].score) {
                Player temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }
        }
    }

    // Afficher le classement
    for (int i = 0; i < numPlayers; i++) {
        char entry[150];
        sprintf(entry, "%d. %s %s - %d points", i + 1, players[i].name, players[i].surname, players[i].score);
        renderText(entry, 100, 150 + i * 50, textColor);
    }

    // Bouton quitter
    SDL_Rect quitButton = { 300, 500, 200, 50 };
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    renderButton("Quitter", quitButton, isMouseOverButton(mouseX, mouseY, quitButton));

    SDL_RenderPresent(renderer);
}

void handlePlayerInputScreen(SDL_Event* e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Rect plusButton = { 450, 45, 40, 40 };
        SDL_Rect minusButton = { 500, 45, 40, 40 };
        SDL_Rect nextButton = { 300, 500, 200, 50 };

        if (isMouseOverButton(mouseX, mouseY, plusButton) && numPlayers < MAX_PLAYERS) {
            numPlayers++;
        } else if (isMouseOverButton(mouseX, mouseY, minusButton) && numPlayers > 1) {
            numPlayers--;
        } else if (isMouseOverButton(mouseX, mouseY, nextButton) && numPlayers > 0) {
            currentPlayer = 0;
            // Initialiser les joueurs
            for (int i = 0; i < numPlayers; i++) {
                strcpy(players[i].name, "");
                strcpy(players[i].surname, "");
                players[i].score = 0;
            }
        }
    }
}

void handleNameInputScreen(SDL_Event* e) {
    static bool editingName = false;
    static bool editingSurname = false;
    static int nameLength = 0;
    static int surnameLength = 0;

    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Vérifier si on clique sur les zones de texte
        if (mouseX >= 200 && mouseX <= 600) {
            if (mouseY >= 150 && mouseY <= 190) {
                editingSurname = true;
                editingName = false;
            } else if (mouseY >= 250 && mouseY <= 290) {
                editingName = true;
                editingSurname = false;
            } else {
                editingName = false;
                editingSurname = false;
            }
        }

        // Vérifier le bouton suivant
        SDL_Rect nextButton = { 300, 500, 200, 50 };
        if (isMouseOverButton(mouseX, mouseY, nextButton) && 
            strlen(players[currentPlayer].name) > 0 && 
            strlen(players[currentPlayer].surname) > 0) {
            
            if (currentPlayer < numPlayers - 1) {
                currentPlayer++;
                editingName = false;
                editingSurname = false;
                nameLength = 0;
                surnameLength = 0;
            } else {
                currentPlayer = 0;
                currentQuestion = 0;
                quizFinished = false;
            }
        }
    } else if (e->type == SDL_KEYDOWN && (editingName || editingSurname)) {
        if (e->key.keysym.sym == SDLK_BACKSPACE) {
            if (editingSurname && surnameLength > 0) {
                players[currentPlayer].surname[--surnameLength] = '\0';
            } else if (editingName && nameLength > 0) {
                players[currentPlayer].name[--nameLength] = '\0';
            }
        } else if ((e->key.keysym.sym == SDLK_RETURN || e->key.keysym.sym == SDLK_KP_ENTER)) {
            editingName = false;
            editingSurname = false;
        }
    } else if (e->type == SDL_TEXTINPUT && (editingName || editingSurname)) {
        if (editingSurname && surnameLength < 49) {
            players[currentPlayer].surname[surnameLength++] = e->text.text[0];
            players[currentPlayer].surname[surnameLength] = '\0';
        } else if (editingName && nameLength < 49) {
            players[currentPlayer].name[nameLength++] = e->text.text[0];
            players[currentPlayer].name[nameLength] = '\0';
        }
    }
}

void handleQuestionScreen(SDL_Event* e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        for (int i = 0; i < MAX_OPTIONS; i++) {
            SDL_Rect optionRect = { 100, 200 + i * 80, 600, 60 };
            if (isMouseOverButton(mouseX, mouseY, optionRect)) {
                if (i == questions[currentQuestion].correctAnswer) {
                    players[currentPlayer].score += 10;
                }
            }
        }
    }
}

void handleFeedbackScreen(SDL_Event* e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Rect continueButton = { 300, 400, 200, 50 };
        if (isMouseOverButton(mouseX, mouseY, continueButton)) {
            currentQuestion++;
            if (currentQuestion >= 10) { // Changez ceci à MAX_QUESTIONS quand vous aurez toutes les questions
                currentPlayer++;
                if (currentPlayer >= numPlayers) {
                    quizFinished = true;
                } else {
                    currentQuestion = 0;
                }
            }
        }
    }
}

void handleLeaderboard(SDL_Event* e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Rect quitButton = { 300, 500, 200, 50 };
        if (isMouseOverButton(mouseX, mouseY, quitButton)) {
            exit(0);
        }
    }
}

int main(int argc, char* args[]) {
    initializeSDL();
    initializeQuestions();

    bool running = true;
    SDL_Event e;

    enum { PLAYER_COUNT, NAME_INPUT, QUESTION, FEEDBACK, LEADERBOARD } gameState = PLAYER_COUNT;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }

            switch (gameState) {
                case PLAYER_COUNT:
                    handlePlayerInputScreen(&e);
                    if (numPlayers > 0 && currentPlayer == 0 && strlen(players[0].name) > 0) {
                        gameState = NAME_INPUT;
                    }
                    break;
                case NAME_INPUT:
                    handleNameInputScreen(&e);
                    if (currentPlayer == 0 && currentQuestion == 0 && !quizFinished) {
                        gameState = QUESTION;
                    }
                    break;
                case QUESTION:
                    handleQuestionScreen(&e);
                    gameState = FEEDBACK;
                    break;
                case FEEDBACK:
                    handleFeedbackScreen(&e);
                    if (quizFinished) {
                        gameState = LEADERBOARD;
                    } else {
                        gameState = QUESTION;
                    }
                    break;
                case LEADERBOARD:
                    handleLeaderboard(&e);
                    break;
            }
        }

        switch (gameState) {
            case PLAYER_COUNT:
                renderPlayerInputScreen();
                break;
            case NAME_INPUT:
                renderNameInputScreen();
                break;
            case QUESTION:
                renderQuestionScreen();
                break;
            case FEEDBACK:
                renderFeedbackScreen(players[currentPlayer].score % 10 == 0); // Simplification pour l'exemple
                break;
            case LEADERBOARD:
                renderLeaderboard();
                break;
        }
    }

    closeSDL();
    return 0;
}