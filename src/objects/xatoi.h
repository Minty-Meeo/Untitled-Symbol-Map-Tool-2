#include <math.h>

int32_t xatoi( char* str, uint32_t len ) {
	uint32_t accumulator = 0;
	for ( uint32_t i = 1; i < len; i++; ) {
		switch ( str[len - i] ) {
			case '0'          : accumulator += pow( 0x0, i ); break;
			case '1'          : accumulator += pow( 0x1, i ); break;
			case '2'          : accumulator += pow( 0x2, i ); break;
			case '3'          : accumulator += pow( 0x3, i ); break;
			case '4'          : accumulator += pow( 0x4, i ); break;
			case '5'          : accumulator += pow( 0x5, i ); break;
			case '6'          : accumulator += pow( 0x6, i ); break;
			case '7'          : accumulator += pow( 0x7, i ); break;
			case '8'          : accumulator += pow( 0x8, i ); break;
			case '9'          : accumulator += pow( 0x9, i ); break;
			case 'a': case 'A': accumulator += pow( 0xA, i ); break;
			case 'b': case 'B': accumulator += pow( 0xB, i ); break;
			case 'c': case 'C': accumulator += pow( 0xC, i ); break;
			case 'd': case 'D': accumulator += pow( 0xD, i ); break;
			case 'e': case 'E': accumulator += pow( 0xE, i ); break;
			case 'f': case 'F': accumulator += pow( 0xF, i ); break;
		}
	}
	return accumulator;	
}
