#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#define BOARD_LENGTH 20

int score = 0;

// Function to set terminal to non-blocking mode
void setNonBlockingInput() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);  // Get current terminal attributes
    term.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    // Make read non-blocking
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

// Function to restore terminal to original mode
void resetTerminalMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

//Function to print the board to the screen
void renderBoard(char board[BOARD_LENGTH][BOARD_LENGTH], int head[2], int apple[2], int** body){
	printf("\033[H\033[J\n\n"); // ANSI escape sequence to clear screen	
	for (int i = 0;i<BOARD_LENGTH; i++){
		for (int j = 0; j<BOARD_LENGTH ; j++){
			if (j == head[0] && i == head[1]){
				printf("@  ");
			}
			else{
				if (j == apple[0] && i == apple[1]){
					printf("O  ");
				}
				else{
					int isBody = 0;
					for(int b = 0; b< score; b++){
						if (body[b][0] == j && body[b][1] == i){
							isBody = 1;
							break;
						}
					}
					if (isBody){
						printf("X  ");
					}
					else{
						printf("%c  ",board[i][j]);
					}
				}
			}
			
		}
		printf("\n");
	}
	printf("\n\n");
}

int main(){
	//Creating and initializing board	
	char board[BOARD_LENGTH][BOARD_LENGTH];
	for (int i = 0;i<BOARD_LENGTH; i++){
		for (int j = 0; j<BOARD_LENGTH ; j++){
			board[i][j] = '.';
		}
	}

	//Set the initial head location
	int head[2];
	int apple[2];
	srand(time(NULL));
	head[0] = BOARD_LENGTH/2;
    head[1] = BOARD_LENGTH/2;
	apple[0] = rand()%BOARD_LENGTH;
	apple[1] = rand()%BOARD_LENGTH;	
	char input; //keep track of user input
	char state = 'r'; //keep track of current direction
	setNonBlockingInput();
	int** body = malloc(score * sizeof(int*));
	int someVariable = 0;
	while(1){
		//Handle user input
		input = getchar();
		if (input != EOF){
			switch (state){
				case('r'):
					if (input == 'w'){
						state = 'u';
					}
					if (input == 's'){
						state = 'd';
					}
					break;
				
				case('u'):
					if (input == 'd'){
						state = 'r';
					}
					if (input == 'a'){
						state = 'l';
					}
					break;
				
				case('d'):
					if (input == 'd'){
						state = 'r';
					}
					if (input == 'a'){
						state = 'l';
					}
					break;
					
				case('l'):
					if (input == 'w'){
						state = 'u';
					}
					if (input == 's'){
						state = 'd';
					}
					break;
			}
		}
		if (input == 'q'){
			break;
		}
		//Update locations and render
		int oldHead[2];
		oldHead[0] = head[0];
		oldHead[1] = head[1];
		switch (state){

			case('r'):
				if (head[0] < BOARD_LENGTH-1){
					head[0]++;
				}
				else{
					goto endGame;
				}
				break;
			case('u'):
				if (head[1] > 0){
					head[1]--;
				}
				else{
					goto endGame;
				}
				break;

			case('d'):
				if (head[1] < BOARD_LENGTH-1){
					head[1]++;
				}
				else{
					goto endGame;
				}
				break;
			case('l'):
				if (head[0] > 0){
					head[0]--;
				}
				else{
					goto endGame;
				}
				break;
		}
		for (int i = 0; i<score; i++){
			if (head[0] == body[i][0] && head[1] == body[i][1]){
				goto endGame;
			}
		}	
		if (apple[0] == head[0] && apple[1] == head[1]){
			//printf("Got apple!\n");
			apple[0] = rand()%20;
			apple[1] = rand()%20;
			//for (int i = 0; i<score; i++){
			//	if (body[i][0]
			//}
			score++;
			body = realloc(body, score*sizeof(int*));
			body[score-1] = malloc(2*sizeof(int));
			body[score-1][0] = head[0];
			body[score-1][1] = head[1];
		}
		if (score == 1){
			body[0][0] = oldHead[0];
			body[0][1] = oldHead[1];
		}

		if (score>1){
			for (int i = 0; i< score-1; i++){
				body[i][0] = body[i+1][0];
				body[i][1] = body[i+1][1];
			}
			body[score-1][0] = oldHead[0];
			body[score-1][1] = oldHead[1];
		}	
		renderBoard(board, head, apple, body);	
		//printf("Head coords: %d , %d \n", head[0], head[1]);
		printf("Score: %d\n",score);
		//someVariable++;
		usleep(70000);	
	}

	endGame:	
		printf("Game Over!\n\n");
		resetTerminalMode();
		for (int i = 0; i< score; i++){
			free(body[i]);
		}
		free(body);
		return 0;
	resetTerminalMode();
	return 0;
}

