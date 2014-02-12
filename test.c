#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void b642int(char *input_buffer, char *output_buffer, size_t *output_size){
	
	size_t input_size = strlen(input_buffer);
	size_t input_mod = input_size % 4;
	printf("Old Input Size: %zu\n", input_size);

	size_t pad_input_size = input_size;
	if (input_mod > 0) pad_input_size = input_size - input_mod + 4;

	printf("New Input Size: %zu\n", pad_input_size);

	int i = 0;
	if (input_size < pad_input_size){
		input_buffer = realloc((void *)input_buffer, pad_input_size);
		printf("Strlen: %zu\n", strlen(input_buffer));
		for (i = 0; i < (4 - input_mod); i++){
			input_buffer[input_size + i] = '=';
		}
	}
	printf("Strlen: %zu\n", strlen(input_buffer));
	*output_size = 3 * pad_input_size / 4;
	output_buffer = malloc(*output_size);
}

int main(){

	/*	
	 *	getline allocates memory if pointer set to Null
	 *	and length set to 0
	 */
	char *b64_buffer = NULL;
	size_t b64_length = 0;

	char *int_buffer = NULL;
	size_t int_size = 0;

	ssize_t chars_read = -1;

	while ((chars_read = getline(&b64_buffer, &b64_length, stdin)) >= 0){

		if (b64_buffer[chars_read - 1] == '\n'){
			b64_buffer[chars_read - 1] = '\0';
			chars_read -= 1;
		}
		
		if (chars_read > 0){
			/* send stuff to function
			 * that processes input buffer
			 * and returns a buffer of ints
			 */
			b642int(b64_buffer, int_buffer, &int_size);
			printf("Input Gets %zu Bytes\n", int_size);
		}

	}
	
	free(b64_buffer);
	free(int_buffer);
}
