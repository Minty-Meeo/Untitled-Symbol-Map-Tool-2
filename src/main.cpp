#include "symbolMap.h"
#include "utility.h"

#include <iostream>
int main( uint32_t argc, char* argv[] ) {
	symbolMap mainMap;
	
	mainMap.readFromFile( argv[1] );
	mainMap.writeToFile( "test.txt" );
	
}
