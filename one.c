/*
*	Takes hexadecimal as command line input
*	Returns input encoded in Base 64
*/


#include <stdio.h>
#include <stdint.h> // uint8_t
#include <string.h> // strlen 
#include <stdlib.h> // realloc

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

/* used to pad output with '=' if input is not multiple of 3
*  indexed by input_size % 3
*  each index corresponds to no. of '=' needed to pad
*/
static int mod_table[] = {0, 2, 1};

/* Takes binary buffer
*  returns buffer encoded in b64
*/
void *int2b64(const uint8_t *data, 
			  char **out,
			  size_t input_size, 
			  size_t *output_size){

	/* 4 output chars for each 3 byte block
	 *  size_t is unsigned, no floating point 
	 *  when dividing by 3 the remainder is thrown away
	 */
	*output_size = 4 * ((input_size + 2) / 3);

	*out = (char *)malloc(*output_size);
	if (out == NULL) return NULL;

	int i = 0, j = 0;

	while (i < input_size){
		size_t octet_a = i < input_size ? data[i++] : 0;
		size_t octet_b = i < input_size ? data[i++] : 0;
		size_t octet_c = i < input_size ? data[i++] : 0;

		// make one 24 bit block
		size_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		/* process octet 6 bits at a time
		 *  push intended 6 bit block to the right
		 *  AND with 00111111 to keep only lowest 6 bits
		 *  store char that corresponds to value
		 */
		(*out)[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		(*out)[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		(*out)[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		(*out)[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	// pad output if input not multiple of 3 bytes
	for (i = 0; i < mod_table[input_size % 3]; i++)
		(*out)[*output_size - 1 - i] = '=';
}

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
	char *input_buffer = NULL;
	size_t input_buffer_size = 0;

	// buffer to hold output bytes (int values) generated from hex pairs
	uint8_t *output_buffer = NULL;
	size_t output_buffer_size = 0; 

	// buffer to hold base64 characters generated
	char *base_buffer = NULL;
	size_t base_buffer_size = 0;

	ssize_t chars_read = -1;
	size_t bytes_written = 0;

	while ((chars_read = getline(&input_buffer, &input_buffer_size, stdin)) >= 0){
		if (input_buffer[chars_read - 1] == '\n'){
			chars_read -= 1;
			input_buffer[chars_read] = '\0';
		}

		if (chars_read > 0){
			bytes_written = hex2bin(input_buffer, 
									&output_buffer, 
									&output_buffer_size);

			int2b64(output_buffer, 
					&base_buffer, 
					bytes_written, 
					&base_buffer_size);

			printf("Base64: %s\n", base_buffer);
		}
	}
	
	free(input_buffer);  // allocated by getline
	free(output_buffer);
	free(base_buffer);
}

