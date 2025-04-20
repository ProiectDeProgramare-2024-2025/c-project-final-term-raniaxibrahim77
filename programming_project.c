#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define COLOR_RESET   "\x1B[0m"
#define COLOR_RED     "\x1B[31m"
#define COLOR_GREEN   "\x1B[32m"
#define COLOR_YELLOW  "\x1B[33m"
#define COLOR_BLUE    "\x1B[34m"
#define COLOR_LIGHT_PURPLE "\x1B[95m"
#define COLOR_PINK "\x1B[95m"
#define COLOR_GRAY "\x1B[90m"
#define COLOR_PURPLE "\x1B[35m"
#define COLOR_NAVY "\x1B[38;5;17m"
#define BOLD "\x1B[1m"




#define MAX_MATCHES 100
#define MAX_PLAYERS 50
#define MAX_SCORE 21
#define MATCH_FILE "matches.txt"
#define PLAYER_FILE "players.txt"

typedef struct {
    char player1[50];
    char player2[50];
    int score1;
    int score2;
    char date[11];
} Match;

typedef struct {
    char name[50];
    int wins;
    int losses;
    int matchesPlayed;
} Player;

Match matches[MAX_MATCHES];
Player players[MAX_PLAYERS];
int matchCount = 0, playerCount = 0;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int isValidName(const char *name) {
    for (int i = 0; i < strlen(name); i++) {
        if (!isalpha(name[i]) && name[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

int findOrAddPlayer(char *name) {
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(players[i].name, name) == 0)
            return i;
    }
    strcpy(players[playerCount].name, name);
    players[playerCount].wins = 0;
    players[playerCount].losses = 0;
    players[playerCount].matchesPlayed = 0;
    return playerCount++;
}

void loadData() {
    FILE *matchFile = fopen(MATCH_FILE, "r");
    if (matchFile == NULL) {
        printf(COLOR_RED"Note: No existing match data found. Starting fresh.\n" COLOR_RESET);
        return;
    }

    while (matchCount < MAX_MATCHES &&
           fscanf(matchFile, "%49s %d %49s %d %10s",
               matches[matchCount].player1,
               &matches[matchCount].score1,
               matches[matchCount].player2,
               &matches[matchCount].score2,
               matches[matchCount].date) == 5) {

        if (matches[matchCount].score1 > MAX_SCORE ||
            matches[matchCount].score2 > MAX_SCORE) {
            printf(COLOR_RED "Warning: Invalid score in match record %d\n" COLOR_RESET, matchCount);
            continue;
        }

        int p1 = findOrAddPlayer(matches[matchCount].player1);
        int p2 = findOrAddPlayer(matches[matchCount].player2);

        players[p1].matchesPlayed++;
        players[p2].matchesPlayed++;

        if (matches[matchCount].score1 > matches[matchCount].score2) {
            players[p1].wins++;
            players[p2].losses++;
        } else {
            players[p2].wins++;
            players[p1].losses++;
        }

        matchCount++;
    }

    if (matchCount > 0) {
        printf(COLOR_GREEN "Match data loaded successfully.\n" COLOR_RESET);
    }

    fclose(matchFile);
}

void saveData() {
    FILE *matchFile = fopen(MATCH_FILE, "w");
    if (matchFile == NULL) {
        printf(COLOR_RED "Error: Could not save match data!\n" COLOR_RESET);
        return;
    }

    for (int i = 0; i < matchCount; i++) {
        fprintf(matchFile, "%s %d %s %d %s\n",
        matches[i].player1, matches[i].score1,
        matches[i].player2, matches[i].score2,
        matches[i].date);
    }
    fclose(matchFile);

    FILE *playerFile = fopen(PLAYER_FILE, "w");
    if (playerFile == NULL) {
        printf(COLOR_RED "Error: Could not save player data!\n"COLOR_RESET);
        return;
    }

    for (int i = 0; i < playerCount; i++) {
        fprintf(playerFile, "%s %d %d %d\n",
                players[i].name,
                players[i].wins,
                players[i].losses,
                players[i].matchesPlayed);
    }
    fclose(playerFile);

    printf(COLOR_GREEN "Player data saved successfully.\n" COLOR_RESET);
}

int getValidScore(const char *prompt) {
    int score;
    do {
        printf("%s", prompt);
        scanf("%d", &score);
        if (score < 0 || score > MAX_SCORE)
            printf(COLOR_RED "Invalid score. Try again.\n" COLOR_RESET);
    } while (score < 0 || score > MAX_SCORE);
    return score;
}

void addMatch() {
    if (matchCount >= MAX_MATCHES) {
        printf(COLOR_RED"Error: Match storage is full!\n"COLOR_RESET);
        printf("\nPress Enter to return...");
        getchar(); getchar();
        return;
    }

    printf("Match Date (dd/mm/yyyy): ");
    scanf("%10s", matches[matchCount].date);
    while (getchar() != '\n');  // Flush newline ✅

    printf("Player 1 full name (max 49 characters): ");
    fgets(matches[matchCount].player2, sizeof(matches[matchCount].player2), stdin);
    matches[matchCount].player2[strcspn(matches[matchCount].player2, "\n")] = 0;
    while (getchar() != '\n');


    matches[matchCount].score1 = getValidScore("Player 1 score (0-21): ");

    printf("Player 2 full name (max 49 characters): ");
    fgets(matches[matchCount].player2, sizeof(matches[matchCount].player2), stdin);
    matches[matchCount].player2[strcspn(matches[matchCount].player2, "\n")] = 0;  // clean newline

    while (getchar() != '\n');

    matches[matchCount].score2 = getValidScore("Player 2 score (0-21): ");


    if (!isValidName(matches[matchCount].player2)) {
        printf(COLOR_RED "Invalid name. Use letters only.\n" COLOR_RESET);
        return;
    }

    if (strcmp(matches[matchCount].player1, matches[matchCount].player2) == 0) {
        printf(COLOR_RED "Error: Player names must be different.\n" COLOR_RESET);
        return;
    }

    int p1 = findOrAddPlayer(matches[matchCount].player1);
    int p2 = findOrAddPlayer(matches[matchCount].player2);

    players[p1].matchesPlayed++;
    players[p2].matchesPlayed++;

    if (matches[matchCount].score1 > matches[matchCount].score2) {
        players[p1].wins++;
        players[p2].losses++;
    } else {
        players[p2].wins++;
        players[p1].losses++;
    }

    matchCount++;
    printf(COLOR_GREEN "\nMatch added successfully!\n" COLOR_RESET);
    printf("Press Enter to return...");
    getchar();
}


void showRanking() {

    if (playerCount == 0) {
        printf("No players recorded yet.\n");
        printf("\nPress Enter to return...");
        getchar();
        return;
    }

    Player sortedPlayers[MAX_PLAYERS];
    memcpy(sortedPlayers, players, sizeof(Player) * playerCount);

    for (int i = 0; i < playerCount - 1; i++) {
        int maxIndex = i;
        for (int j = i + 1; j < playerCount; j++) {
            if (sortedPlayers[j].wins > sortedPlayers[maxIndex].wins) {
                maxIndex = j;
            }
        }
        if (maxIndex != i) {
            Player temp = sortedPlayers[i];
            sortedPlayers[i] = sortedPlayers[maxIndex];
            sortedPlayers[maxIndex] = temp;
        }
    }

    printf("\nRank | Player          | Wins | Losses | Matches\n");
    printf("-----|-----------------|------|--------|--------\n");

    for (int i = 0; i < playerCount; i++) {
        const char *color = (i == 0) ? COLOR_YELLOW : COLOR_RESET;

        printf("%-4d | %s%-15s%s | " COLOR_PINK "%-4d" COLOR_RESET
               " | " COLOR_GRAY "%-6d" COLOR_RESET " | %-6d\n",
               i + 1,
               color, sortedPlayers[i].name, COLOR_RESET,
               sortedPlayers[i].wins,
               sortedPlayers[i].losses,
               sortedPlayers[i].matchesPlayed);
    }

    printf("\nPress Enter to return...");
    getchar();
}



void showPlayerHistory() {

    if (playerCount == 0) {
        printf(COLOR_RED "No players recorded yet.\n" COLOR_RESET);
        printf("\nPress Enter to return...");
        getchar();
        return;
    }

    char name[50];
    printf("Enter player name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    // Normalize input to lowercase
    for (int i = 0; name[i]; i++) {
        name[i] = tolower(name[i]);
    }

    int playerIndex = -1;
    for (int i = 0; i < playerCount; i++) {
        char storedName[50];
        strcpy(storedName, players[i].name);
        for (int j = 0; storedName[j]; j++) {
            storedName[j] = tolower(storedName[j]);
        }

        if (strcmp(storedName, name) == 0) {
            playerIndex = i;
            break;
        }
    }

    if (playerIndex == -1) {
        printf(COLOR_RED "\nPlayer not found!\n" COLOR_RESET);
        printf("\nPress Enter to return...");
        getchar(); getchar();
        return;
    }

    printf("\nPlayer: %s\n", players[playerIndex].name);
    printf("Wins: %d | Losses: %d | Matches Played: %d\n",
           players[playerIndex].wins,
           players[playerIndex].losses,
           players[playerIndex].matchesPlayed);

    printf("\nMatch History:\n");
    int matchFound = 0;
    for (int i = 0; i < matchCount; i++) {
        if (strcmp(matches[i].player1, players[playerIndex].name) == 0 ||
            strcmp(matches[i].player2, players[playerIndex].name) == 0) {

            int isP1Winner = matches[i].score1 > matches[i].score2;

            printf("- %s %s%d%s vs %s %s%d%s\n",
                   matches[i].player1,
                   isP1Winner ? COLOR_PINK : COLOR_GRAY,
                   matches[i].score1,
                   COLOR_RESET,
                   matches[i].player2,
                   isP1Winner ? COLOR_GRAY : COLOR_PINK,
                   matches[i].score2,
                   COLOR_RESET);

            matchFound = 1;
        }
    }

    if (!matchFound) {
        printf(COLOR_RED "No matches recorded for this player.\n" COLOR_RESET);
    }

    printf("\nPress Enter to return...");
    getchar(); getchar();
}

void exitProgram() {
    clearScreen();
    printf(COLOR_NAVY BOLD "~~~~~~~~ Exiting Program ~~~~~~~~\n" COLOR_RESET);
    saveData();
    printf(COLOR_GREEN "\nThank you for using Table Tennis Manager!\n" COLOR_RESET);
    printf("Goodbye!\n\n");
    exit(0);
}



int main() {
    loadData();
    int option;

    do {
        clearScreen();
        printf("<><><><><><><><><><><><><><><><><><><><><><><><>\n");
        printf("  %sTABLE TENNIS MATCH MANAGER%s\n", COLOR_PURPLE, COLOR_RESET);
        printf("<><><><><><><><><><><><><><><><><><><><><><><><>\n");
        printf(COLOR_BLUE " 1. " COLOR_RESET "Add New Match\n");
        printf(COLOR_BLUE " 2. " COLOR_RESET "View Player Rankings\n");
        printf(COLOR_BLUE " 3. " COLOR_RESET "View Player History\n");
        printf(COLOR_BLUE " 0. " COLOR_RESET "Exit Program\n");
        printf("<><><><><><><><><><><><><><><><><><><><><><><><>\n");

        printf("Enter your choice: ");

        if (scanf("%d", &option) != 1) {
            printf(COLOR_RED "\nInvalid input! Please enter a number.\n"COLOR_RESET);
            while (getchar() != '\n');
            printf("\nPress Enter to continue...");
            getchar();
            continue;
        }
        while (getchar() != '\n');

        switch (option) {
                case 1:
                    clearScreen();
                    printf(COLOR_NAVY BOLD "~~~~~~~~ Add Match ~~~~~~~~\n" COLOR_RESET);
                addMatch();
                break;

            case 2:
                clearScreen();
                printf(COLOR_NAVY BOLD "~~~~~~~~ Player Rankings ~~~~~~~~\n" COLOR_RESET);
                showRanking();
                break;

            case 3:
                clearScreen();
                printf(COLOR_NAVY BOLD "~~~~~~~~ Player History ~~~~~~~~\n" COLOR_RESET);
                showPlayerHistory();
                break;

            case 0: exitProgram(); break;
                default:
                    printf(COLOR_RED"\nInvalid option! Please try again.\n"COLOR_RESET);
                    printf("\nPress Enter to continue...");
                    getchar();
        }
    } while (1);

    return 0;
}


