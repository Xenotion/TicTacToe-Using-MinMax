#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum  { CIRCLE, CROSS, BORDER, EMPTY };
enum { HUMANWIN, COMPWIN, DRAW };

const int Directions[4] = { 1, 5, 4, 6 };

const int ConvertTo25[9] = {
    6,7,8,
    11,12,13,
    16,17,18
};

const int InMiddle = 4;
const int Corners[4] = { 0 , 2, 6, 8 };

int ply = 0;
int positions = 0;
int maxPly = 0;

int GetNumForDir(int startSq, const int dir, const int *board, const int us) {
    int found = 0;
    while(board[startSq] != BORDER) {
        if(board[startSq] != us) {
            break;
        }
        found++;
        startSq += dir;
    }
    return found;
}

int FindThreeInARow(const int *board, const int ourindex, const int us) {
    int DirIndex = 0;
    int Dir = 0;
    int threeCount = 1;

    for (DirIndex = 0; DirIndex < 4; DirIndex++) {
        Dir = Directions[DirIndex];
        threeCount += GetNumForDir(ourindex + Dir, Dir, board, us);
        threeCount += GetNumForDir(ourindex + Dir * - 1, Dir * -1, board, us);
        if (threeCount == 3) {
            break;
        }
        threeCount = 1;
    }
    return threeCount;
}

int FindThreeInARowAllBoard(const int *board, const int us) {
    int threeFound = 0;
    int i;
    for (i = 0; i < 9; i++) {
        if (board[ConvertTo25[i]] == us) {
            if(FindThreeInARow(board, ConvertTo25[i], us) == 3) {
                threeFound = 1;
                break;
            }
        }
    }
    return threeFound;
}

int EvalForWin(const int *board, const int us) {
    if (FindThreeInARowAllBoard(board, us) != 0) {
        return 1;
    }
    if (FindThreeInARowAllBoard(board, us ^ 1) != 0) {
        return -1;
    }

    return 0;
}

int MinMax(int *board, int side) {

	// check is a win
	// gen all moves for side
	// loop moves, make move, mimax() on move to get score
	// assess bestscore
	// end moves return bestscore
	
	int MoveList[9];
	int MoveCount = 0;
	int bestScore = -2;
	int score = -2;
	int bestMove = -1;
	int Move;
	int index;
	
	if(ply > maxPly) maxPly = ply;	
	positions++;
	
	if(ply > 0) {
		score = EvalForWin(board, side);
		if(score != 0) {					
			return score;
		}		
	}
	
	// fill Move List
	for(index = 0; index < 9; ++index) {
		if( board[ConvertTo25[index]] == EMPTY) {
			MoveList[MoveCount++] = ConvertTo25[index];
		}
	}
	
	// loop all moves
	for(index = 0; index < MoveCount; ++index) {
		Move = MoveList[index];
		board[Move] = side;	
		
		ply++;
		score = -MinMax(board, side^1);
		if(score > bestScore) {			
			bestScore = score;	
			bestMove = Move;
		}
		board[Move] = EMPTY;
		ply--;
	}
	
	if(MoveCount==0) {
		bestScore = FindThreeInARowAllBoard(board, side);	
	}
	
	if(ply!=0)
		return bestScore;	
	else 
		return bestMove;	
}

void InitialiseBoard(int *board) {
    int i = 0;
    for (i = 0; i < 25; i++) {
        board[i] = BORDER;
    }

    for (i = 0; i < 9; i++) {
        board[ConvertTo25[i]] = EMPTY;
    }
}

void PrintBoard(const int *board) {
    int i = 0;
    char pceChars[] = "OX|-";

    printf("\nBoard:\n");
    for (i = 0; i < 9; i++) {
        if (i != CIRCLE && i % 3 == 0) {
            printf("\n\n");
        }
        printf("%4c", pceChars[board[ConvertTo25[i]]]);
    }
    printf("\n");
}

int HasEmpty(const int *board) {
    int i = 0;
    
    for (i = 0; i < 9; i++) {
        if (board[ConvertTo25[i]] == EMPTY) {
            return 1;
        }    
    }

    return 0;
}

void MakeMove(int *board, const int sq, const int side) {
    board[sq] = side;
}

int GetNextBest(const int *board) {

    int ourMove = ConvertTo25[InMiddle];
    
    if(board[ourMove] == EMPTY) {
        return ourMove;
    }

    int i = 0;
    ourMove = -1;

    for (i = 0; i < 4; i++) {
        ourMove = ConvertTo25[Corners[i]];
        if (board[ourMove] == EMPTY) {
            break;
        }
        ourMove = -1;
    }

    return ourMove;
}


int GetWinningMove(int *board, const int side) {
    int ourMove = -1;
    int winFound = 0;
    int i = 0;

    for (i = 0; i < 9; i++) {
        if (board[ConvertTo25[i]] == EMPTY) {
            ourMove = ConvertTo25[i];
            board[ourMove] = side;

            if (FindThreeInARow(board, ourMove, side) == 3) {
                winFound = 1;
            }

            board[ourMove] = EMPTY;
            if (winFound == 1) {
                return ourMove;
            }
            ourMove = -1;
        }
    }
    return ourMove;
}

int GetComputerMove(int *board, const int side) {

    ply = 0;
    positions = 0;
    maxPly = 0;

    int best = MinMax(board, side);
    printf("\nFinished Searching positions:%d maxDepth:%d bestMove:%d\n", positions, maxPly, best);

    return best;

    /*
    int i = 0;
    int numFree = 0;
    int availableMoves[9];
    int randMove = 0;

    randMove = GetWinningMove(board, side);
    if (randMove != -1) {
        return randMove;
    }

    randMove = GetWinningMove(board, side ^ 1);
    if (randMove != -1) {
        return randMove;
    }

    randMove = GetNextBest(board);
    if (randMove != -1) {
        return randMove;
    }

    randMove = 0;

        2, 4, 8
        availableMoves[0] = 2 numFree++ -> 1;
        availableMoves[numFree] = 4 numFree++ -> 2;
        availableMoves[numFree] = 8 numFree++ -> 3;

        rand() % numfree gives 0 to 2
        rand from 0 to 2, return availableMoves[rand]
    

    for (i = 0; i < 9; i++) {
        if (board[ConvertTo25[i]] == EMPTY) {
            availableMoves[numFree++] = ConvertTo25[i];
            
        }
    }

    randMove = (rand() % numFree);
    return availableMoves[randMove];
    */
}

int GetHumanMove(const int *board) {
    char userInput[4];

    int moveOK = 0;
    int move = -1;

    while (moveOK == 0) {
        printf("\nPlease enter a move from 1 to 9:");
        fgets(userInput, 3, stdin);
        fflush(stdin);

        if (strlen(userInput) != 2) {
            printf("\nInvalid strlen()\n");
            continue;
        }

        if (sscanf(userInput, "%d", &move) != 1) {
            move = -1;
            printf("\nInvalid sscanf()\n");
            continue;
        }

        if (move < 1 || move > 9) {
            move = -1;
            printf("\nInvalid range\n");
            continue;
        }

        move--; // zero indexing

        if (board[ConvertTo25[move]] != EMPTY) {
            move =- 1;
            printf("Square not available\n");
            continue;
        }

        moveOK = 1;
    }

    printf("\nMaking Move...%d\n", (move + 1));
    return ConvertTo25[move];
}

void RunGame() {

    int GameOver = 0;
    char Side = CIRCLE;
    int LastMoveMade = 0;

    int *board;
    board = malloc(sizeof(int) * 25);
     
    InitialiseBoard(board);
    PrintBoard(board);

    while (!GameOver) {
        if (Side == CIRCLE) {
            LastMoveMade = GetHumanMove(board);
            printf("\n|Human Move|\n");
            MakeMove(board, LastMoveMade, Side);
            Side = CROSS;
            PrintBoard(board);
            
        } else {
            LastMoveMade = GetComputerMove(board, Side);
            printf("|Computer Move|\n");
            MakeMove(board, LastMoveMade, Side);
            Side = CIRCLE;
            PrintBoard(board);
        }

        if (FindThreeInARow(board, LastMoveMade, Side ^ 1) == 3) {
            printf("Game Over!\n");
            GameOver = 1;
            if(Side == CIRCLE) {
                printf("Computer Wins!\n");
            } else {
                printf("Human Wins!\n");
            }
            printf("\nFinal Board Status\n");
            PrintBoard(board);
        }

        if (!HasEmpty(board)) {
            printf("\nGame Over!\n");
            GameOver = 1;
            printf("\nIt's a Draw!\n");
        }
    }

    free(board);
}

int main(int argc, char **argv) {
    
    srand(time(NULL));
    RunGame();

    return 0;
}