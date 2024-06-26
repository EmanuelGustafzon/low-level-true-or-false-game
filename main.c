#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// true or false game

static bool playing;

typedef struct Question Question;
typedef struct ListOfQuestions ListOfQuestions;

struct Question {
    char question[60];
    bool isTrue;
};

struct ListOfQuestions {
    Question* questions;
    int numOfQuestions;
    int capacity;
};

void init_list(ListOfQuestions* list, int capacity);
int create_and_append_question(ListOfQuestions* list, char question[], bool isTrue);
void play_game(ListOfQuestions* list, int* score);
void* counter(void* arg);

int main() {
    pthread_t counterThread;
    int status;

    int* count = malloc(sizeof(int));
    status = pthread_create(&counterThread, NULL, counter, count);
    if(status) {
        printf("could not create counter thread");
    }

    ListOfQuestions list;
    init_list(&list, 10);
    create_and_append_question(&list, "Bananas is always yellow", false); 
    create_and_append_question(&list, "Paris is the capital of France", true); 
    create_and_append_question(&list, "The highest mountian in the world is 8048 meters over the sea", true); 

    int* score = malloc(sizeof(int));
    *score = 0;
    
    playing = true;
    play_game(&list, score);
    playing = false;

    void* finaltimeFinishingQuiz;
    pthread_join(counterThread, &finaltimeFinishingQuiz);
    if(finaltimeFinishingQuiz != NULL) {
        int time = *(int*)finaltimeFinishingQuiz;
        printf("You finished the quiz in: %d seconds\n", time);
        free(finaltimeFinishingQuiz);
    }

    free(list.questions);
    free(count);

  return 0;
}

void* counter(void* arg) {
    int count = *(int*)arg;
    int* finalTime = (int*)malloc(sizeof(int));
    while(playing) {
        sleep(1);
        count++;
    }
    *finalTime = count; 
    pthread_exit((void*)finalTime);
}

void init_list(ListOfQuestions* list, int capacity) {
    list->capacity = capacity;
    list->numOfQuestions = 0;
    list->questions = malloc(list->capacity * sizeof(Question));
}

int create_and_append_question(ListOfQuestions* list, char question[], bool isTrue) {
    if(list->capacity == list->numOfQuestions) {
        printf("the list of question is full");
        return 1;
    }
    Question quest;
    strcpy(quest.question, question);
    quest.isTrue = isTrue;

    list->questions[list->numOfQuestions] = quest;
    list->numOfQuestions++;
    return 0;
}

void play_game(ListOfQuestions* list, int* score) {
    int answer;

    for(int i = 0; i < list->numOfQuestions; i++) {
        printf("your score: %d\n", *score);

        printf("Question: ");
        printf("%s\n", list->questions[i].question);

        while (1) {
            printf("type 0 for false and 1 for true: ");
            int result = scanf("%d", &answer);

            if (result == 1 && (answer == 0 || answer == 1)) {
                while (getchar() != '\n');
                break;
            } else {
                printf("Invalid input. Please enter 0 or 1.\n");
                // Clear the input 
                while (getchar() != '\n');
            }
        }

        if(answer == list->questions[i].isTrue) {
            printf("\033[1;32m you are correct \033[0m\n");
            *score += 1;
        } else {
            printf("\033[1;31m you are wrong \033[0m\n");
        }
    }
    clearConsole();

    printf("Your final score is:  %d\n", *score);
    free(score);
}
