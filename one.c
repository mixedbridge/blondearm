#include <stdio.h>
//test2

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

	}
}

int main(){

	// buffer to hold input chars from hex string
	char *input_buffer = NULL;
	size_t input_buffer_size = 0;

	// buffer to hold output bytes (int values) generated from hex pairs
	uint8_t *output_buffer = NULL;
	size_t output_buffer_size = 0; 

	ssize_t chars_read = -1;

	while ((chars_read = getline(&input_buffer, &input_buffer_size, stdin)) >= 0){
		if (input_buffer[chars_read - 1] == '\n'){
			chars_read -= 1;
			input_buffer[chars_read] = '\0';
		}
		printf("Here: Size:%zd %s\n", chars_read, input_buffer);
	}
	
	free(input_buffer);  // allocated by getline
}

