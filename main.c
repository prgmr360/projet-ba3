/*
===============================================================================
File:                   better_grades.c
Course:                 PRG2
Year:                   2024
Author:                 Clément Dieperink
Creation Date:          15.01.2024
Modification Author:    On a rien vu
Modification Date:      10.04.2024
Version:                1.0
Description:            This file implements a more advanced grades management
                        tool which should be able to handle several course
===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SUBJECT_NAME_LENGTH 10
#define MIN_GRADE_VALUE 1
#define MAX_GRADE_VALUE 6

// Structure pour stocker les notes d'une branche
typedef struct {
    double *grades;
    char *name;
    int count;
    int maxGrades;
} Subject;

enum MenuName {
    ADD_GRADE = 1,
    DELETE_GRADE,
    MODIFY_GRADE,
    SWAP_GRADE,
    AVERAGE,
    MODIFY_SUBJECT,
    ADD_SUBJECT,
    QUIT
};

enum ErrorCode {
    NO_ERROR,
    INVALID_INPUT,
    MEMORY_ALLOCATION_FAILED,
    SUBJECT_NOT_FOUND,
    INVALID_GRADE,
    POSITION_NOT_VALID,
    NOT_ENOUGH_GRADES,
    MAX_GRADE,
    INVALID_SUBJECT_NAME
};

void printMenu(Subject *subjects, int numberSubjects);

//OPTION FUNCTIONS
enum ErrorCode addGrade(Subject *subjects, int numberSubjects);
enum ErrorCode deleteGrade(Subject *subjects, int numberSubjects);
enum ErrorCode modifyGrade(Subject *subjects, int numberSubjects);
enum ErrorCode swapGrade(Subject *subjects, int numberSubjects);
void computeAverage(Subject *subjects, int numberSubjects);
enum ErrorCode setSubjectName(Subject *subjects, char **allSubjectsName, int numberSubjects);
enum ErrorCode addSubject(Subject **subjects, int *numberSubjects, double **allGrades, char ***allSubjectNames);

//ERROR MANAGEMENT AND VALIDATIONS
void printError(enum ErrorCode errorCode);
bool validSubjectName(char *name, Subject *subjects, int numberSubjects);
bool isGradeValid(const double);
bool isValidPosition(const int, const int);
int getPosition();
bool getInputName(char *subjectName);
int findSubjectIndex(Subject *subjects, const int numberSubjects, char *subjectName);
double setNewGrade(double *grade);

int main(int argc, char *argv[]) {

    printf("Hello world, this is me !\n");
    printf("I'm the one you're looking for !\n");
    printf("Hello world, this is me !\n");
    printf("I'm the one you're looking for !\n");
    printf("Hello world, this is me !\n");
    printf("I'm the one you're looking for !\n");
    printf("Hello world, this is me !\n");
    printf("I'm the one you're looking for !\n");

    return EXIT_SUCCESS;
    if (argc != 3) {
        printf("Usage: %s <N_SUBJECT> <MAX_GRADES>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int numberSubjects = atoi(argv[1]);
    int maxGrades = atoi(argv[2]);

    if (numberSubjects <= 0 || maxGrades <= 0) {
        printf("Invalid arguments.\n");
        return EXIT_FAILURE;
    }

    // Subjects tab to contain our structs
    Subject *subjects = (Subject *)malloc(numberSubjects * sizeof(Subject));
    if (subjects == NULL) {
        printError(MEMORY_ALLOCATION_FAILED);
        return EXIT_FAILURE;
    }

    // Unidimensional tab to store grades
    double *allGrades = (double *)malloc(maxGrades * numberSubjects * sizeof(double));
    if (allGrades == NULL) {
        free(subjects);
        printError(MEMORY_ALLOCATION_FAILED);
        return EXIT_FAILURE;
    }

    // Pointer on char pointers for all our subject names
    char **allSubject = (char **)malloc(numberSubjects * sizeof(char *));
    if (allSubject == NULL) {
        free(subjects);
        free(allGrades);
        printError(MEMORY_ALLOCATION_FAILED);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < numberSubjects; ++i) {
        subjects[i].grades = allGrades + (i * maxGrades);
        subjects[i].count = 0;
        subjects[i].maxGrades = maxGrades;
    }

    //Adding name of our subjects with the minimum memory allocation needed to store them
    for (int i = 0; i < numberSubjects; ++i) {
        printf("Enter the name for subject %d: ", i + 1);
        char subjectName[MAX_SUBJECT_NAME_LENGTH + 1];
        while (1) {
            bool validEntry = getInputName(subjectName);
            if (!validEntry) {
                printError(INVALID_INPUT);
            } else {
                break;
            }
        }

        allSubject[i] = (char *)malloc(sizeof(char) * (strlen(subjectName) + 1));
        if (allSubject[i] == NULL) {
            for (int j = 0; j < i; ++j) {
                free(allSubject[j]);
            }
            free(subjects);
            free(allGrades);
            free(allSubject);
            printError(MEMORY_ALLOCATION_FAILED);
            return EXIT_FAILURE;
        }

        strcpy(allSubject[i], subjectName);
        subjects[i].name = allSubject[i];

    }

    //MAIN LOOP
    int choice = 0;
    enum ErrorCode ErrorCheck = NO_ERROR;

    do {
        printf("\n");
        printMenu(subjects, numberSubjects);
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        while (getchar() != '\n');

        //OPTIONS
        switch (choice) {
            case ADD_GRADE:
                ErrorCheck = addGrade(subjects, numberSubjects);
                if (ErrorCheck != NO_ERROR) {
                    printError(ErrorCheck);
                }
                break;

            case DELETE_GRADE:
                ErrorCheck = deleteGrade(subjects, numberSubjects);
                if (ErrorCheck != NO_ERROR) {
                    printError(ErrorCheck);
                }
                break;

            case MODIFY_GRADE:
                ErrorCheck = modifyGrade(subjects, numberSubjects);
                if (ErrorCheck != NO_ERROR) {
                    printError(ErrorCheck);
                }
                break;

            case SWAP_GRADE:
                ErrorCheck = swapGrade(subjects, numberSubjects);
                if (ErrorCheck != NO_ERROR) {
                    printError(ErrorCheck);
                }
                break;

            case AVERAGE:
                computeAverage(subjects, numberSubjects);
                break;

            case MODIFY_SUBJECT:
                ErrorCheck = setSubjectName(subjects, allSubject, numberSubjects);
                if (ErrorCheck != NO_ERROR) {
                    printError(ErrorCheck);
                    if (ErrorCheck == MEMORY_ALLOCATION_FAILED) {
                        choice = 8;
                    }
                }
                break;

            case ADD_SUBJECT:
                ErrorCheck = addSubject(&subjects, &numberSubjects, &allGrades, &allSubject);
                if (ErrorCheck != NO_ERROR) {
                    printError(ErrorCheck);
                    if (ErrorCheck == MEMORY_ALLOCATION_FAILED) {
                        choice = 8;
                    }
                }
                break;
            case QUIT:
                choice = 8;
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    while (choice != 8);

    //FREE MEMORY
    free(allGrades);
    for (int i = 0; i < numberSubjects; ++i) {
        free(allSubject[i]);
    }
    free(allSubject);
    free(subjects);

    return EXIT_SUCCESS;
}

void printMenu(Subject *subjects, int numberSubjects) {
    printf("1: Add a grade\n");
    printf("2: Delete a grade\n");
    printf("3: Modify a grade\n");
    printf("4: Swap a grade\n");
    printf("5: Compute average\n");
    printf("6: Modify subject name\n");
    printf("7: Add a subject\n");
    printf("8: Quit\n\n");

    // Find the maximum length of subject names
    int maxSubjectNameLength = 0;
    for (int i = 0; i < numberSubjects; ++i) {
        int currentLength = strlen(subjects[i].name);
        if (currentLength > maxSubjectNameLength) {
            maxSubjectNameLength = currentLength;
        }
    }

    // Print subject names with aligned ":" based on the maximum length
    for (int i = 0; i < numberSubjects; ++i) {
        printf("%-*s : ", maxSubjectNameLength, subjects[i].name);
        if (subjects[i].count == 0) {
            printf("No grades\n");
        } else {
            printf("%.1f", subjects[i].grades[0]);
            for (int j = 1; j < subjects[i].count; ++j) {
                printf(", %.1f", subjects[i].grades[j]);
            }
            printf("\n");
        }
    }
}

//FUNCTIONS
enum ErrorCode addGrade(Subject *subjects, int numberSubjects) {
    char subjectName[MAX_SUBJECT_NAME_LENGTH + 1];
    printf("Enter the subject name: ");

    bool validEntry = getInputName(subjectName);

    if (!validEntry) {
        return INVALID_INPUT;
    }
    int subjectIndex = findSubjectIndex(subjects, numberSubjects, subjectName);

    if (subjectIndex == -1) {
        return SUBJECT_NOT_FOUND;
    }

    if (subjects[subjectIndex].count == subjects[subjectIndex].maxGrades) {
        return MAX_GRADE;
    }

    double newGrade;
    do {
        setNewGrade(&newGrade);
    }
    while (!isGradeValid(newGrade));

    subjects[subjectIndex].grades[subjects[subjectIndex].count++] = newGrade;
    return NO_ERROR;
}

enum ErrorCode deleteGrade(Subject *subjects, int numberSubjects) {
    char subjectName[MAX_SUBJECT_NAME_LENGTH + 1];
    printf("Enter the subject name: ");
    bool validEntry = getInputName(subjectName);

    if (!validEntry) {
        return INVALID_INPUT;
    }

    int subjectIndex = findSubjectIndex(subjects, numberSubjects, subjectName);

    if (subjectIndex == -1) {
        return SUBJECT_NOT_FOUND;
    }

    if (subjects[subjectIndex].count == 0) {
        return NOT_ENOUGH_GRADES;
    }

    int position;
    do {
        position = getPosition();
    }
    while (!isValidPosition(position, subjects[subjectIndex].count));

    for (int i = position - 1; i < subjects[subjectIndex].count - 1; ++i) {
        subjects[subjectIndex].grades[i] = subjects[subjectIndex].grades[i + 1];
    }

    subjects[subjectIndex].count--;

    return NO_ERROR;
}


enum ErrorCode modifyGrade(Subject *subjects, int numberSubjects) {
    char subjectName[MAX_SUBJECT_NAME_LENGTH + 1];
    printf("Enter the subject name: ");
    bool validEntry = getInputName(subjectName);

    if (!validEntry) {
        return INVALID_INPUT;
    }

    int subjectIndex = findSubjectIndex(subjects, numberSubjects, subjectName);

    if (subjectIndex == -1) {
        return SUBJECT_NOT_FOUND;
    }

    if (subjects[subjectIndex].count == 0) {
        return NOT_ENOUGH_GRADES;
    }

    int position;
    do {
        position = getPosition();
    }
    while (!isValidPosition(position, subjects[subjectIndex].count));

    double newGrade;
    do {
        setNewGrade(&newGrade);
    }
    while (!isGradeValid(newGrade));

    subjects[subjectIndex].grades[position - 1] = newGrade;

    return NO_ERROR;
}

enum ErrorCode swapGrade(Subject *subjects, int numberSubjects) {
    char subjectName[MAX_SUBJECT_NAME_LENGTH + 1];
    printf("Enter the subject name: ");
    bool validEntry = getInputName(subjectName);

    if (!validEntry) {
        return INVALID_INPUT;
    }

    int subjectIndex = findSubjectIndex(subjects, numberSubjects, subjectName);

    if (subjectIndex == -1) {
        return SUBJECT_NOT_FOUND;
    }

    if (subjects[subjectIndex].count < 2) {
        return NOT_ENOUGH_GRADES;
    }

    int positions[2];
    do {
        printf("Enter the two grades positions to swap: ");
        if (scanf("%d %d", &positions[0], &positions[1]) != 2) {
            while (getchar() != '\n');
            positions[0] = -1;
            positions[1] = -1;
        }
    }
    while (!isValidPosition(positions[0], subjects[subjectIndex].count) || !isValidPosition(positions[1], subjects[subjectIndex].count));

    double temp = subjects[subjectIndex].grades[positions[0] - 1];
    subjects[subjectIndex].grades[positions[0] - 1] = subjects[subjectIndex].grades[positions[1] - 1];
    subjects[subjectIndex].grades[positions[1] - 1] = temp;

    return NO_ERROR;
}

void computeAverage(Subject *subjects, int numberSubjects) {
    for (int i = 0; i < numberSubjects; ++i) {
        if (subjects[i].count == 0) {
            printf("%s\t: No grades\n", subjects[i].name);
        } else {
            double sum = 0.0;
            for (int j = 0; j < subjects[i].count; ++j) {
                sum += subjects[i].grades[j];
            }
            printf("%s\t: %.1f\n", subjects[i].name, sum / subjects[i].count);
        }
    }
}

enum ErrorCode setSubjectName(Subject *subjects, char **allSubjectsName, int numberSubjects) {
    char subjectName[MAX_SUBJECT_NAME_LENGTH + 1];
    printf("Enter the current subject name: ");
    bool validEntry = getInputName(subjectName);

    if (!validEntry) {
        return INVALID_INPUT;
    }
    validEntry = false;
    int subjectIndex = findSubjectIndex(subjects, numberSubjects, subjectName);

    if (subjectIndex == -1) {
        return SUBJECT_NOT_FOUND;
    }

    char newName[MAX_SUBJECT_NAME_LENGTH + 1];
    printf("Enter the new name for the subject: ");
    validEntry = getInputName(newName);

    if (!validEntry) {
        return INVALID_INPUT;
    }

    if (!validSubjectName(newName, subjects, numberSubjects)) {
        return INVALID_SUBJECT_NAME;
    }

    allSubjectsName[subjectIndex] = (char *)realloc(allSubjectsName[subjectIndex], sizeof(char) * (strlen(newName) + 1));
    if (allSubjectsName[subjectIndex] == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }
    strcpy(allSubjectsName[subjectIndex], newName);
    subjects[subjectIndex].name = allSubjectsName[subjectIndex];

    return NO_ERROR;
}

enum ErrorCode addSubject(Subject **subjects, int *numberSubjects, double **allGrades, char ***allSubjectsName) {
    printf("Enter the name for the new subject: ");
    char newName[MAX_SUBJECT_NAME_LENGTH + 1];

    bool validEntry = getInputName(newName);

    if (!validEntry) {
        return INVALID_INPUT;
    }

    if (!validEntry || !validSubjectName(newName, *subjects, *numberSubjects)) {
        return INVALID_SUBJECT_NAME;
    }

    // Reallocation for our new subject in our structs' tab
    (*numberSubjects)++;
    *subjects = (Subject *)realloc(*subjects, (*numberSubjects) * sizeof(Subject));
    if (*subjects == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }

    // Reallocation of our grades' tab and updating each subjects grades pointers
    *allGrades = (double *)realloc(*allGrades, (*numberSubjects * (*subjects)[0].maxGrades * sizeof(double)));
    if (*allGrades == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }

    (*subjects)[*numberSubjects - 1].grades = (*allGrades) + ((*numberSubjects - 1) * ((*subjects)[0].maxGrades));
    (*subjects)[*numberSubjects - 1].count = 0;
    (*subjects)[*numberSubjects - 1].maxGrades = (*subjects)[0].maxGrades;

    for (int i = 0; i < *numberSubjects; ++i) {
        (*subjects)[i].grades = &((*allGrades)[i * ((*subjects)[0].maxGrades)]);
    }

    // Reallocation for our names' tab
    *allSubjectsName = realloc(*allSubjectsName, *numberSubjects * sizeof(char *));
    if (*allSubjectsName == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }

    (*allSubjectsName)[*numberSubjects - 1] = (char *)malloc(sizeof(char) * (strlen(newName) + 1));
    if ((*allSubjectsName)[*numberSubjects - 1] == NULL) {
        return MEMORY_ALLOCATION_FAILED;
    }

    strncpy((*allSubjectsName)[*numberSubjects - 1], newName, strlen(newName));
    (*allSubjectsName)[*numberSubjects - 1][strlen(newName)] = '\0';
    (*subjects)[*numberSubjects - 1].name = (*allSubjectsName)[*numberSubjects - 1];

    return NO_ERROR;
}

//ERROR MANAGEMENT
void printError(enum ErrorCode errorCode) {
    switch (errorCode) {
        case INVALID_INPUT:
            printf("Invalid input.\n");
            break;
        case MEMORY_ALLOCATION_FAILED:
            printf("Memory allocation failed.\n");
            break;
        case SUBJECT_NOT_FOUND:
            printf("Subject not found.\n");
            break;
        case INVALID_GRADE:
            printf("Grade is  not valid.\n");
            break;
        case POSITION_NOT_VALID:
            printf("Position is not valid.\n");
            break;
        case NOT_ENOUGH_GRADES:
            printf("Array is empty\n");
            break;
        case MAX_GRADE:
            printf("Maximum number of grades reached for this subject.\n");
            break;
        case INVALID_SUBJECT_NAME:
            printf("Invalid subject name. Subject name must contain only letters (upper or lower case) and digits, and be at most %d characters long.\n", MAX_SUBJECT_NAME_LENGTH);
            break;
        default:
            printf("An unknown error occurred.\n");
            break;
    }
}

bool isGradeValid(const double grade) {
    if (grade < MIN_GRADE_VALUE || grade > MAX_GRADE_VALUE) {
        printError(INVALID_GRADE);
        return false;
    } else {
        return true;
    }
}

int getPosition() {

    int positionToGet = 0;
    printf("Enter the grade position: ");
    if (scanf("%d", &positionToGet) != 1) {
        while (getchar() != '\n');
        positionToGet = -1;
    }

    return positionToGet;
}

bool isValidPosition(const int position, const int count) {

    if (position <= 0 || position > count) {
        printError(POSITION_NOT_VALID);
        return false;
    } else {
        return true;
    }
}

bool getInputName(char *subjectName) {
    scanf("%10s", subjectName);
    if (getchar() != '\n') {
        while (getchar() != '\n');
        return false;
    }

    int i = 0;
    while (subjectName[i] != '\0') {
        if (!isalnum(subjectName[i])) {
            return false;
        }
        ++i;
    }

    return true;
}


bool validSubjectName(char *name, Subject *subjects, int numberSubjects) {

    for (int i = 0; i < numberSubjects; ++i) {
        if (strcmp(subjects[i].name, name) == 0) {
            return false;
        }
    }

    return true;
}

int findSubjectIndex(Subject *subjects, const int numberSubjects, char *subjectName) {
    int indexToFind = -1;
    for (int i = 0; i < numberSubjects; i++) {
        if (strcmp(subjects[i].name, subjectName) == 0) {
            indexToFind = i;
            break;
        }
    }
    return indexToFind;
}

double setNewGrade(double *grade) {
    printf("Enter the new grade value: ");
    if (scanf("%lf", grade) != 1) {
        while (getchar() != '\n');
        *grade = -1.0;
    }
    return *grade;
}