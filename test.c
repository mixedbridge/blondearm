#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;

void build_decoding_table() {

	decoding_table = malloc(256);

	int i = 0;
	for (i = 0; i < 64; i++)
		decoding_table[(unsigned char) encoding_table[i]] = i;
}

void padb64(char *input_buffer){
	/* Takes an input char buffer
	 * and makes it hold a multiple of 4 characters
	 * missing characters are padded with '='
	 * can result in 3 '=' at the end
	 * which can never be the case if 
	 * input is actually delivered in base64
	 */

	size_t input_size = strlen(input_buffer);
	size_t pad_input_size = input_size;
	size_t input_mod = input_size % 4;

	if (input_mod > 0) pad_input_size = input_size - input_mod + 4;

	int i = 0;
	if (input_size < pad_input_size){
		input_buffer = realloc((void *)input_buffer, pad_input_size);
		for (i = 0; i < (4 - input_mod); i++){
			input_buffer[input_size + i] = '=';
		}
	}
	input_buffer[pad_input_size] = '\0';

	printf("%s\n", input_buffer);
}

void b642int(char *input_buffer, unsigned char **output_buffer, size_t *output_size){
	
	padb64(input_buffer);
	size_t input_size = strlen(input_buffer);
	assert((input_size % 4) == 0);
	
	*output_size = input_size / 4 * 3;
	if (input_buffer[input_size - 1] == '=') (*output_size)--;
	if (input_buffer[input_size - 2] == '=') (*output_size)--;

	*output_buffer = malloc(*output_size);
	build_decoding_table();

	int i = 0, j = 0;
	for (i = 0, j = 0; i < input_size;){
		uint32_t sextet_a = input_buffer[i] == '=' ? 0 & i++ : 
			decoding_table[input_buffer[i++]];
		
		uint32_t sextet_b = input_buffer[i] == '=' ? 0 & i++ : 
			decoding_table[input_buffer[i++]];

		uint32_t sextet_c = input_buffer[i] == '=' ? 0 & i++ : 
			decoding_table[input_buffer[i++]];

		uint32_t sextet_d = input_buffer[i] == '=' ? 0 & i++ : 
			decoding_table[input_buffer[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < *output_size) (*output_buffer)[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_size) (*output_buffer)[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_size) (*output_buffer)[j++] = (triple >> 0 * 8) & 0xFF;
	}
	(*output_buffer)[(*output_size)] = '\0';
}

int main(){

	/*	
	 *	getline allocates memory if pointer set to Null
	 *	and length set to 0
	 */
	char *b64_buffer = NULL;
	size_t b64_length = 0;

	unsigned char *int_buffer = NULL;
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
			b642int(b64_buffer, &int_buffer, &int_size);
			printf("Input Gets %zu Bytes\n", int_size);
			
			printf("Output: ");
		
			int i = 0;
			for (i = 0; i < int_size; i++)
				printf("%x", int_buffer[i]);
			printf("\n");
		}
	}
	
	free(b64_buffer);
	free(int_buffer);
	free(decoding_table);
}
