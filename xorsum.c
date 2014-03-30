/*
*	Takes 2 buffers of hexadecimal as command line input
*	Returns XOR sum of input
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

int main(){

	// buffer to hold input chars from hex string
	char *input = NULL;
	size_t input_size = 0;
	
	// buffer to hold output bytes (int values) generated from hex pairs
	uint8_t *output = NULL;
	size_t output_size = 0; 

	uint8_t *buffA = NULL;
	uint8_t *buffB = NULL;

	ssize_t chars_read = -1;
	size_t bytes_written = 0;
	FILE *fp = fopen("tobexored.txt", "r");
	int i = 0;
	int j = 0;

	while ((chars_read = getline(&input, &input_size, fp)) >= 0){
		if (input[chars_read - 1] == '\n'){
			chars_read -= 1;
			input[chars_read] = '\0';
		}

		if (chars_read > 0){
			bytes_written = hex2bin(input, &output, &output_size);
		}

		if (i == 0){
			buffA = malloc(bytes_written);
			memcpy(buffA, output, bytes_written);
			i++;
		} else {
			buffB = malloc(bytes_written);
			memcpy(buffB, output, bytes_written);
		}
	}

	for (i = 0; i < bytes_written; i++){
		printf("%0x ", buffA[i] ^ buffB[i]);
	}
	printf("\n");

	free(input);  // allocated by getline
	free(output);
	free(buffA);
	free(buffB);
}
