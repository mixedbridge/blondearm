/*
*	Takes a string of hexadecimal
*   Xors it against a single character and finds the message
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

int score(uint8_t **buffer, int key){

	char bytes[34];	//given buffer resolves to 24 bytes
	int i = 0;
	int score = 0;
	
	for (i = 0; i < 34; i++){
		bytes[i] = (*buffer)[i] ^ key;

		switch(bytes[i]){
			case 'E': 
			case 'e':
				score += 1;
				break;
			case 'T': 
			case 't':
				score += 1;
				break;
			case 'A': 
			case 'a':
				score += 1;
				break;
			case 'O':
		   	case 'o':
				score += 1;
				break;
			case 'I':
		   	case 'i':
				score += 1;
				break;
			case 'N': 
			case 'n':
				score += 1;
				break;
		}
	}
	return score;
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
	FILE *fp = fopen("key.txt", "r");

	while ((chars_read = getline(&input, &input_size, fp)) >= 0){
		if (input[chars_read - 1] == '\n'){
			chars_read -= 1;
			input[chars_read] = '\0';
		}

		if (chars_read > 0){
			bytes_written = hex2bin(input, &output, &output_size);
		}
	}

	int	*points = malloc(200);
	int i = 0;
	int max = 0;
	int maxindex = 0;

	for (i = 0; i < 127; i++){
		points[i] = score(&output, i);
		if (points[i] > max){
			max = points[i];
			maxindex = i;
		}
	}
	printf("Score: %d, Key: %c\n", max, maxindex);


	for (i = 0; i < bytes_written; i++){
		printf("%c", output[i] ^ maxindex);
	}
	printf("\n");

	free(input);  // allocated by getline
	free(output);
	free(points);
}
