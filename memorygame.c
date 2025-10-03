#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Theme symbols
const char *emojis[] = {"🐶", "🐱", "🐻", "🐼", "🐨", "🐯", "🦁", "🐮"};
const char *letters[] = {"A", "B", "C", "D", "E", "F", "G", "H"};
const char *numbers[] = {"1", "2", "3", "4", "5", "6", "7", "8"};

typedef struct {
    char ***solution;
    int **flipped;
    int rows;
    int cols;
    int score;
    int turns;
    int theme;
} GameState;

// Function prototypes
void initialize_board(GameState *game);
void shuffle(const char **array, int n);
void print_board(GameState *game);
int all_cards_matched(GameState *game);
void save_game(GameState *game);
int load_game(GameState *game);

int main() {
    GameState game;
    game.solution = NULL;
    game.flipped = NULL;
    game.rows = 0;
    game.cols = 0;
    game.score = 0;
    game.turns = 0;
    game.theme = 0;

    srand(time(NULL));

    printf("Memory Match Game\n");
    printf("1. New Game\n2. Load Game\n");
    int choice;
    scanf("%d", &choice);
    
    if(choice == 2 && load_game(&game)) {
        printf("Game loaded successfully!\n");
    } else {
        // Difficulty selection
        printf("Select difficulty:\n1. Easy (2x2)\n2. Medium (4x4)\n3. Hard (6x6)\n");
        scanf("%d", &choice);
        switch(choice) {
            case 1: game.rows = game.cols = 2; break;
            case 2: game.rows = game.cols = 4; break;
            case 3: game.rows = game.cols = 6; break;
            default: printf("Invalid choice!\n"); return 1;
        }

        // Theme selection
        printf("Select theme:\n1. Letters\n2. Numbers\n3. Emojis\n");
        scanf("%d", &game.theme);

        // Initialize game state
        game.solution = malloc(game.rows * sizeof(char **));
        game.flipped = malloc(game.rows * sizeof(int *));
        for(int i = 0; i < game.rows; i++) {
            game.solution[i] = malloc(game.cols * sizeof(char *));
            game.flipped[i] = calloc(game.cols, sizeof(int));
        }
        initialize_board(&game);
    }

    printf("Score: %d\n", game.score);
    printf("Find all pairs!\n\n");

    while(1) {
        print_board(&game);
        int row1, col1, row2, col2;
        char input[10];

        // First card input
        while(1) {
            printf("Enter first card (row col) or 'save': ");
            scanf("%s", input);
            if(strcmp(input, "save") == 0) {
                save_game(&game);
                printf("Game saved. Exiting.\n");
                return 0;
            }
            if(sscanf(input, "%d", &row1) != 1) continue;
            scanf("%d", &col1);
            
            if(row1 < 0 || row1 >= game.rows || col1 < 0 || col1 >= game.cols) {
                printf("Invalid coordinates!\n");
            } else if(game.flipped[row1][col1]) {
                printf("Card already flipped!\n");
            } else break;
        }

        // Second card input
        while(1) {
            printf("Enter second card (row col): ");
            scanf("%d %d", &row2, &col2);
            
            if(row2 < 0 || row2 >= game.rows || col2 < 0 || col2 >= game.cols) {
                printf("Invalid coordinates!\n");
            } else if(game.flipped[row2][col2]) {
                printf("Card already flipped!\n");
            } else if(row1 == row2 && col1 == col2) {
                printf("Can't pick same card!\n");
            } else break;
        }

        // Reveal cards
        game.flipped[row1][col1] = 1;
        game.flipped[row2][col2] = 1;
        print_board(&game);

        // Check match
        if(strcmp(game.solution[row1][col1], game.solution[row2][col2]) == 0) {
            game.score += 10;
            printf("Match found! +10 points\n");
        } else {
            game.score -= 5;
            printf("No match! -5 points\n");
            sleep(2);
            game.flipped[row1][col1] = 0;
            game.flipped[row2][col2] = 0;
        }

        game.turns++;

        // Win condition
        if(all_cards_matched(&game)) {
            print_board(&game);
            printf("Congratulations! You won in %d turns!\n", game.turns);
            printf("Final score: %d\n", game.score);
            break;
        }
    }

    // Cleanup
    for(int i = 0; i < game.rows; i++) {
        free(game.solution[i]);
        free(game.flipped[i]);
    }
    free(game.solution);
    free(game.flipped);

    return 0;
}

void initialize_board(GameState *game) {
    const char **theme;
    int theme_size;
    
    switch(game->theme) {
        case 1: theme = letters; theme_size = 8; break;
        case 2: theme = numbers; theme_size = 8; break;
        case 3: theme = emojis; theme_size = 8; break;
        default: return;
    }

    int total = game->rows * game->cols;
    const char **cards = malloc(total * sizeof(const char *));
    int pairs = total / 2;

    for(int i = 0; i < pairs; i++) {
        const char *sym = theme[i % theme_size];
        cards[2*i] = sym;
        cards[2*i+1] = sym;
    }

    shuffle(cards, total);

    for(int i = 0; i < game->rows; i++) {
        for(int j = 0; j < game->cols; j++) {
            game->solution[i][j] = cards[i * game->cols + j];
        }
    }

    free(cards);
}

void shuffle(const char **array, int n) {
    for(int i = n-1; i > 0; i--) {
        int j = rand() % (i+1);
        const char *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void print_board(GameState *game) {
    printf("\n  ");
    for(int j = 0; j < game->cols; j++) printf(" %d ", j);
    printf("\n");
    
    for(int i = 0; i < game->rows; i++) {
        printf("%d ", i);
        for(int j = 0; j < game->cols; j++) {
            if(game->flipped[i][j]) {
                printf("[%s]", game->solution[i][j]);
            } else {
                printf("[ ]");
            }
        }
        printf("\n");
    }
    printf("Score: %d  Turns: %d\n\n", game->score, game->turns);
}

int all_cards_matched(GameState *game) {
    for(int i = 0; i < game->rows; i++) {
        for(int j = 0; j < game->cols; j++) {
            if(!game->flipped[i][j]) return 0;
        }
    }
    return 1;
}

void save_game(GameState *game) {
    FILE *f = fopen("save.dat", "w");
    fprintf(f, "%d %d %d %d %d %d\n", 
        game->rows, game->cols, game->score, game->turns, game->theme);
    
    for(int i = 0; i < game->rows; i++) {
        for(int j = 0; j < game->cols; j++) {
            const char *sym = game->solution[i][j];
            for(int t = 0; t < 8; t++) {
                if(sym == letters[t]) fprintf(f, "%d ", t);
                else if(sym == numbers[t]) fprintf(f, "%d ", t);
                else if(sym == emojis[t]) fprintf(f, "%d ", t);
            }
        }
        fprintf(f, "\n");
    }
    
    for(int i = 0; i < game->rows; i++) {
        for(int j = 0; j < game->cols; j++) {
            fprintf(f, "%d ", game->flipped[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

int load_game(GameState *game) {
    FILE *f = fopen("save.dat", "r");
    if(!f) return 0;
    
    fscanf(f, "%d %d %d %d %d %d", 
        &game->rows, &game->cols, &game->score, &game->turns, &game->theme);
    
    const char **theme;
    int theme_size;
    switch(game->theme) {
        case 1: theme = letters; theme_size = 8; break;
        case 2: theme = numbers; theme_size = 8; break;
        case 3: theme = emojis; theme_size = 8; break;
        default: return 0;
    }
    
    game->solution = malloc(game->rows * sizeof(char **));
    game->flipped = malloc(game->rows * sizeof(int *));
    for(int i = 0; i < game->rows; i++) {
        game->solution[i] = malloc(game->cols * sizeof(char *));
        game->flipped[i] = malloc(game->cols * sizeof(int));
    }
    
    for(int i = 0; i < game->rows; i++) {
        for(int j = 0; j < game->cols; j++) {
            int idx;
            fscanf(f, "%d", &idx);
            game->solution[i][j] = theme[idx];
        }
    }
    
    for(int i = 0; i < game->rows; i++) {
        for(int j = 0; j < game->cols; j++) {
            fscanf(f, "%d", &game->flipped[i][j]);
        }
    }
    
    fclose(f);
    return 1;
}