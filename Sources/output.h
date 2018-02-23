#ifndef SOURCES_OUTPUT_H_
#define SOURCES_OUTPUT_H_

#define BLOCK_TIMEOUT (1000)

typedef struct output
{
	const char clear[5];
	char buffer[100];
} OUTPUT_BUFFER_STRUCT, * OUTPUT_BUFFER_PTR;

#define CODE_clearline ((unsigned char*)"\033[2K")
#define CODE_clearline_home ((unsigned char*)"\033[2K\r")
#define CODE_clearterm ((unsigned char*)"\033[2J")
#define CODE_clearterm_home ((unsigned char*)"\033[2J\r")
#define CODE_nextline ((unsigned char*)"\n\r")

OUTPUT_BUFFER_PTR init_output_ptr() {
	static OUTPUT_BUFFER_STRUCT s = {
			.clear="\033[2K\r",
			.buffer={'\0'}
	};

	return &s;
}

#endif /* SOURCES_OUTPUT_H_ */
