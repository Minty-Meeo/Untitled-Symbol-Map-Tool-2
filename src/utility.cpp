#include "ansi_include.h"

// Convert C-String of hexadecimal characters of a specified length into a uint32
uint32_t xatoui( char* str, uint32_t len ) {
	uint32_t accumulator = 0;
	for ( uint32_t i = 0; i < len; i++ ) {
		switch ( str[( len - i ) - 1] ) {
			case '0'          : accumulator += 0x0 * pow( 16, i ); break;
			case '1'          : accumulator += 0x1 * pow( 16, i ); break;
			case '2'          : accumulator += 0x2 * pow( 16, i ); break;
			case '3'          : accumulator += 0x3 * pow( 16, i ); break;
			case '4'          : accumulator += 0x4 * pow( 16, i ); break;
			case '5'          : accumulator += 0x5 * pow( 16, i ); break;
			case '6'          : accumulator += 0x6 * pow( 16, i ); break;
			case '7'          : accumulator += 0x7 * pow( 16, i ); break;
			case '8'          : accumulator += 0x8 * pow( 16, i ); break;
			case '9'          : accumulator += 0x9 * pow( 16, i ); break;
			case 'a': case 'A': accumulator += 0xa * pow( 16, i ); break;
			case 'b': case 'B': accumulator += 0xb * pow( 16, i ); break;
			case 'c': case 'C': accumulator += 0xc * pow( 16, i ); break;
			case 'd': case 'D': accumulator += 0xd * pow( 16, i ); break;
			case 'e': case 'E': accumulator += 0xe * pow( 16, i ); break;
			case 'f': case 'F': accumulator += 0xf * pow( 16, i ); break;
		}
	}
	return accumulator;	
}
// Find string in file
std::ios::pos_type findStr( ifstream& inFile, const char* curStr ) {
	inFile.seekg( std::ios::beg );									// Return to start of file before search
	
	std::ios::pos_type filePos;										// FilePos for backup during search and eventual returning purposes
	size_t cStrLen = strlen( curStr );										// Pre-calculated strlen result for multiple uses
	
	while( true ) {
		if ( inFile.peek() == curStr[0] ) {							// If first char of "CURRENT STRING" is found in file
			filePos = inFile.tellg();								// Backup file position
			char tempStr[cStrLen];									// Allocate "TEMP STRING" size of "CURRENT STRING"
			inFile.read( tempStr, cStrLen );						// Read from inFile into "TEMP STRING"
			if ( !memcmp( curStr, tempStr, cStrLen ) )				// Compare "CURRENT STRING" to "TEMP STRING"
				break;												// If it's a match, escape while loop with backup file position intact
			inFile.seekg( filePos );								// Otherwise, return to normal execution
		}
		inFile.get();												// Advance one character in file
		if ( inFile.eof() ) {										// Fail if EOF is reached
			filePos = EOF;
			break;
		}
	}
	
	return filePos;
}

//		BYTE SWAPPING FUNCTIONS / DEFINES

#define _BSWAP_16(v) (((v & 0xff00) >> 8) | \
                      ((v & 0x00ff) << 8))

#define _BSWAP_32(v) (((v & 0xff000000) >> 24) | \
                      ((v & 0x00ff0000) >>  8) | \
                      ((v & 0x0000ff00) <<  8) | \
                      ((v & 0x000000ff) << 24) )

unsigned int swap32(unsigned int value) {
    return _BSWAP_32( value );
}
unsigned short swap16(unsigned short value) {
    return _BSWAP_16( value );
}


//	FILE READING FUNCTIONS
char* read_CStr( ifstream& inFile, unsigned int length ) {
	char temp[length+1] ;
	inFile.read( temp, length );
	return temp;
}

uint32_t read_u32( ifstream& inFile ) {
	uint32_t value;
	
	inFile.read( reinterpret_cast<char*>(&value), 4 );
	return swap32( value );
}
uint16_t read_u16( ifstream& inFile ) {
	uint16_t value;
	
	inFile.read( reinterpret_cast<char*>(&value), 2 );
	return swap16( value );
}
uint8_t read_u8( ifstream& inFile ) {
	uint8_t value;
	
	inFile.read( reinterpret_cast<char*>(&value), 1 );
	return value;
}
float read_float(ifstream& inFile ) {
	unsigned int value;
	
	inFile.read( reinterpret_cast<char*>(&value), 4 );
	value = swap32( value );
	return reinterpret_cast<float&>(value);
}

void write_u32( ofstream& outFile, uint32_t value ) {
	value = swap32( value );
	outFile.write( reinterpret_cast<char*>(&value), 4 );
}
void write_u16( ofstream& outFile, uint16_t value ) {
	value = swap16( value );
	outFile.write( reinterpret_cast<char*>(&value), 2 );
}
void write_u8( ofstream& outFile, uint8_t value ) {
	outFile.write( reinterpret_cast<char*>(&value), 1 );
}

void write_float(ofstream& outFile, float value ) {
	unsigned int value2 = reinterpret_cast<unsigned int&>(value);
	value2 = swap32( value2 );
	outFile.write( reinterpret_cast<char*>(&value2), 4 );
}


void skip( ifstream& inFile, std::streampos offset ) {
	inFile.seekg( offset, std::ios_base::cur );
}
void skip( ofstream& outFile, std::streampos offset ) {
	outFile.seekp( offset, std::ios_base::cur );
}
void align( ifstream& inFile, std::streampos value ) {
	std::streampos offset = value - ( inFile.tellg() % value );
	
	if ( offset == value ) return;
	
	inFile.seekg( offset, std::ios_base::cur );
}
void align( ofstream& outFile, std::streampos value ) {
	std::streampos offset = value - ( outFile.tellp() % value );
	
	if ( offset == value ) return;
	
	outFile.seekp( offset, std::ios_base::cur );
}

string getUserInput( string prompt ) {
	std::cout << prompt;
	string temp;
	std::cin >> temp;
	return temp;
}

void ignoreAll( char ignoredChar, ifstream& inFile ) {
	while (true) {
		if (inFile.peek() == ignoredChar)
			inFile.ignore(1);
		else
			break;
	}
}
