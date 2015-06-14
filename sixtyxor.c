/*
*	Takes in as input file of 60 char buffers
*	Finds the one line that is encrypted with single char xor
*   Xors it against that character and finds the message
*/


#include <stdio.h>
#include <stdint.h> // uint8_t
#include <string.h> // strlen 
#include <stdlib.h> // realloc

uint8_t hex2int(char c){

	if ((c >= '0') && (c <= '9')){
		// decimal digit
		return (uint8_t)(c - '0');
	} else if ((c >= 'A') && (c <= 'Z')){
		// upper case character
		return (uint8_t)(c - 'A' + 10);
	} else if ((c >= 'a') && (c <= 'z')){
		// lower case character
		return (uint8_t)(c - 'a' + 10);
	} else {
		fprintf(stderr, "encountered non-hexadecimal ASCII character ('%c')\n", c);
		return 255;
	}
}


uint8_t hexpair2bin(char *input_buffer, size_t bytes_written){

	uint8_t value, a, b;

	a = hex2int(input_buffer[bytes_written]);
	b = hex2int(input_buffer[bytes_written + 1]);

	a <<= 4;
	value = a + b;

	return value; 

}

/*	Takes:
*	input line of chars, no of chars, size of output buffer	
*
*	Output:
*	array of values corresponding to hexadecimal char pairs
*   returns no. of byte values written to output_buffer
*
*/
size_t hex2bin(char *input_buffer,
				uint8_t **output_buffer,
				size_t *output_buffer_size){

	size_t bytes_to_write = strlen(input_buffer)/2;

	if (bytes_to_write > *output_buffer_size){
		*output_buffer_size = bytes_to_write * sizeof(uint8_t);
		// casting the realloc to type seems optional, experiment later
		*output_buffer = (uint8_t *)realloc((void *)*output_buffer,
			*output_buffer_size);
	}

	size_t bytes_written = 0;
	while (bytes_written < bytes_to_write){
		(*output_buffer)[bytes_written] = hexpair2bin(input_buffer, 
			bytes_written*2);
		bytes_written += 1;

	}
	return bytes_written;
}

/*  returns score, where score = how likely a string is
*	an English language string, based on the frequency
*	of certain characters.
*/
int score(uint8_t **buffer, size_t *size, int key){

	char *bytes = malloc(*size);
	int i = 0;
	int score = 0;
	
	for (i = 0; i < *size; i++){
		bytes[i] = (*buffer)[i] ^ key;

		switch(bytes[i]){
			case 'E': case 'e': case 'T': case 't':
			case 'A': case 'a': case 'O': case 'o':
			case 'I': case 'i': case 'N': case 'n':
			case 'S': case 's': case 'H': case 'h':
			case 'R': case 'r': //case 'D': case 'd':
				score += 1;
				break;
		}
	}
	free(bytes);
	return score;
}

/*	iterates through 200 bytes, writing the byte with the 
*	highest score (*maxindex) for main() to read
*	
*/
int findkey(uint8_t **buffer, size_t *size, int *maxindex){

	int	*points = malloc(200);
	int i = 0;
	int max = 0;		//maxscore
	*maxindex = 0;		//index of character with max score

	for (i = 0; i < 200; i++){
		points[i] = score(&(*buffer), &(*size), i);
		if (points[i] > max){
			max = points[i];
			*maxindex = i;
		}
	}
	free(points);
	return max;
}

int main(){

	// buffer to hold input chars from hex string
	char *input = NULL;
	size_t input_size = 0;
	
	// buffer to hold output bytes (int values) generated from hex pairs
	uint8_t *output = NULL;
	size_t output_size = 0; 

	ssize_t chars_read = -1;
	size_t bytes_written = 0;
	FILE *fp = fopen("60chars.txt", "r");
	int key = 0;
	int score = 0;
	int linecount = 0;
	int i = 0;
	int c = 0;

	while ((chars_read = getline(&input, &input_size, fp)) >= 0){
		
		//remove terminating new line character
		if (input[chars_read - 1] == '\n'){
			chars_read -= 1;
			input[chars_read] = '\0';
		}

		//convert hexadecimal string to binary values
		if (chars_read > 0){
			bytes_written = hex2bin(input, &output, &output_size);
		}

		score = findkey(&output, &bytes_written, &key);
		if (score > 15){
			i = 0;
			printf("Line: %d, Score: %d, Key: %d\n", linecount, score, key);
			for (i = 0; i < bytes_written; i++){
				c = output[i] ^ key;
				if (c < 32) c = 32; //limits to printable chars including '\0'
				printf("%c", c);
			}
			printf("\n");
		}
		if (linecount++ > 350) break;
//		linecount++;
	}
	printf("****\nParsed %d lines\n", linecount);

	free(input);  // allocated by getline
	free(output);
}
