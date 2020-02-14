#include "symbolMap.h"
#include "utility.h"
#include "ANSI_include.h"

symbol::symbol() {
	strtAddress = 0;
	size = 0;
	virtAddress = 0x80000000;
	alignment = 0;
	isUnused = false;
	isClosure = false;
	isEntry = false;
	offset = 0;
}

void symbolMap::readFromFile( const char* filename ) {
	ifstream inFile( filename, std::ios::binary );
	if ( inFile.is_open() ) {
		readSection( inFile, findStr( inFile, ".init section layout" ), init );
		readSection( inFile, findStr( inFile, "extab section layout" ), extab );
		readSection( inFile, findStr( inFile, "extabindex section layout" ), extabindex );
		readSection( inFile, findStr( inFile, ".text section layout" ), text );
		readSection( inFile, findStr( inFile, ".ctors section layout" ), ctors );
		readSection( inFile, findStr( inFile, ".dtors section layout" ), dtors );
		readSection( inFile, findStr( inFile, ".rodata section layout" ), rodata );
		readSection( inFile, findStr( inFile, ".data section layout" ), data );
		readSection( inFile, findStr( inFile, ".bss section layout" ), bss );
		readSection( inFile, findStr( inFile, ".sdata section layout" ), sdata );
		readSection( inFile, findStr( inFile, ".sbss section layout" ), sbss );
		readSection( inFile, findStr( inFile, ".sdata2 section layout" ), sdata2 );
		readSection( inFile, findStr( inFile, ".sbss2 section layout" ), sbss2 );
	}
	else {
		printf( "Could not open \"%s\"\n", filename );
	}
	inFile.close();
}

void symbolMap::writeToFile( const char* filename ) {
	FILE* outFile;
	outFile = fopen( filename, "w" );
	writeSection( outFile, ".init", init );
	writeSection( outFile, "extab", extab );
	writeSection( outFile, "extabindex", extabindex );
	writeSection( outFile, ".text", text );
	writeSection( outFile, ".ctors", ctors );
	writeSection( outFile, ".dtors", dtors );
	writeSection( outFile, ".rodata", rodata );
	writeSection( outFile, ".data", data );
	writeSection( outFile, ".bss", bss );
	writeSection( outFile, ".sdata", sdata );
	writeSection( outFile, ".sbss", sbss );
	writeSection( outFile, ".sdata2", sdata2 );
	writeSection( outFile, ".sbss2", sbss2 );
	fclose( outFile );
}

void readSection( ifstream& inFile, std::ios::streampos filePos, vector<symbol>& section ) {
	inFile.seekg( filePos );						// Seek to location of section in (*.map) file
	inFile.ignore( 512, '\n');						///????? section layout
	inFile.ignore( 512, '\n');						///  Starting        Virtual
	inFile.ignore( 512, '\n');						///  address  Size   address
	inFile.ignore( 512, '\n');						///  -----------------------
	uint32_t symCount = section.size();				// Initialize Symbol Count to current size of Symbol Vector
	uint32_t currClosure;							// Index of current "Symbol Closure" (parent).
	while ( !inFile.eof() ) {						// Loop until EOF
		symbol tempSymbol;								// Temp "symbol" sub-class
		
		inFile.ignore( 2, EOF );						// Skip leading whitespace
		if ( inFile.peek() == '\r' ) break;				// Check if Section has ended (blank line)
		{												///Starting Address
			char temp[8];									// Temp char array
			inFile.read( temp, 8 );							// Read "Starting Address"
			if ( !memcmp( temp, "UNUSED  ", 8 ) ) {			// Check if is unused
				tempSymbol.isUnused = true;						// Turn on flag if it is
			}
			else {											// Otherwise
				tempSymbol.strtAddress = xatoui( temp, 8 );		// Store "Starting Address" value.
			}
		}
		{												///Size
			inFile.ignore( 1, EOF );						// Skip whitespace
			char temp[6];									// Temp char array
			inFile.read( temp, 6 );							// Read "Size"
			tempSymbol.size = xatoui( temp, 6 );			// Store "Size" value
			if ( tempSymbol.size == 0 ) {					// Symbols with reported size of zero are entry symbols
				tempSymbol.isEntry = true;						// Turn on flag when detected
			}
		}
		{												///Virtual Address
			inFile.ignore( 1, EOF );						// Skip whitespace
			if ( !tempSymbol.isUnused ) {					// Make sure this symbol isn't unused
				char temp[8];									// Temp char array
				inFile.read( temp, 8 );							// Read "Virtual Address"
				tempSymbol.virtAddress = xatoui( temp, 8 );		// Store "Virtual Address" value
			}
			else {											// Otherwise, unused symbols have "........" for a reported "Virtual Address"
				inFile.ignore( 8, EOF );						// so we just skip that
			}
		}
		if (!tempSymbol.isUnused&&!tempSymbol.isEntry){	///Alignment (unless it's an Unused or Entry symbol)
			inFile.ignore( 1, EOF );						// Skip whitespace
			char temp[3] = {0};								// Temp char array (initialized with null)
			inFile.read( temp, 2 );							// Read "Alignment"
			tempSymbol.alignment = atoi( temp );			// Store "Alignment" value
			if ( tempSymbol.alignment == 1 ) {				// Closure symbols always have a reported "Alignment" of 1.
				tempSymbol.isClosure = true;					// Turn on flag when detected
			}
		}
		else {											///Rarely, Closure Symbols can appear to be Entry Symbols by having a reported size of 0 bytes.
			filePos = inFile.tellg();						// Backup file position
			inFile.ignore( 1, EOF );						// Skip whitespace
			char temp[2];									// Temp char array
			inFile.read( temp, 2 );							// Read as if an "Alignment" value were there
			if ( !memcmp( temp, " 1", 2 ) ) {				// Check if what was read looks like a Closure Symbol's Alignment value
				tempSymbol.isEntry = false;						// Revert Entry flag
				tempSymbol.isClosure = true;					// Turn on Closure flag
				tempSymbol.alignment = 1;						// Manually set Alignment
			}
			else {
				inFile.seekg( filePos );
			}
		}
		{												///Symbol Name
			inFile.ignore( 1, EOF );						// Skip whitespace
			char temp[512] = {0};							// Temp char array (initialized with null)
			inFile.get( temp, 512, ' ' );					// Read "Symbol Name"
			tempSymbol.symName = temp;						// Store "Symbol Name" string
		}
		if ( tempSymbol.isEntry ) {						///If this is an Entry Symbol, figure out the "Parent" Symbol.
			if ( !tempSymbol.isUnused ) {					/// Unless it's also unused, in which case it doesn't mention.
				inFile.ignore( 1, EOF );						// Skip whitespace
				inFile.ignore( 10, EOF );						// Skip "(Entry of "
				char temp[512] = {0};							// Temp char array (initialized with null)
				inFile.get( temp, 512, ')' );					// Read "Parent"
				tempSymbol.parent = temp;						// Store "Parent" string
				inFile.ignore( 1, EOF );						// Discard ')' character
			}
		}
		{												///Object Name
			inFile.ignore( 1, EOF );						// Skip whitespace
			if ( !tempSymbol.isUnused ) {					// UNUSED Symbols don't have a tab character in-between the SymName and ObjName
				inFile.ignore( 1, EOF );						// While Used Symbols do
			}
			char temp[512] = {0};							// Temp char array (initialized with null)
			inFile.get( temp, 512, ' ' );					// Read "Object Name"
			tempSymbol.objName = temp;						// Store "Object Name" string
		}
		inFile.ignore( 1, EOF );						///Some symbols do not have a Source Name, but there's always a whitespace char for it.
		if ( inFile.peek() != '\r' ){					///Source Name
			char temp[512] = {0};							// Temp char array (initialized with null)
			inFile.get( temp, 512, '\r' );					// Read "Source Name"
			tempSymbol.srcName = temp;						// Store "Source Name" string
		}
		inFile.ignore( 2, EOF );						// Skip newline (\r\n)
		
		
		section.push_back( tempSymbol );				// Add symbol to section vector
		symCount++;										// Increase Symbol Count
		if ( tempSymbol.isClosure ) {					// If the last symbol processed was a new "Symbol Closure"
			currClosure = symCount-1;					// Set new target "Symbol Closure"
		}
		else {											// Otherwise
			section[currClosure].children.push_back( symCount );	// Add the last symbol processed to the list of children of the current "Symbol Closure"
		}
		
//		std::cout << "Symbol " << symCount << " :" << section[symCount-1].symName << " \t" << section[symCount-1].objName << " \t" << section[symCount-1].srcName << std::endl;
//		std::cout << "currClosure: " << section[currClosure].symName << " \t" << section[currClosure].objName << " \t" << section[currClosure].srcName << std::endl;
//		std::cout << std::endl;
	}
}

void writeSection( FILE* outFile, const char* sectionName, vector<symbol>& section ) {
	fprintf( outFile, "%s section layout\n", sectionName );
	fprintf( outFile, "  Starting        Virtual\n" );
	fprintf( outFile, "  address  Size   address\n" );
	fprintf( outFile, "  -----------------------\n" );
	for ( uint32_t i = 0; i < section.size(); i++ ) {
		if ( !section[i].isUnused && !section[i].isEntry ) {	// [ NOT unused | NOT entry ]
			fprintf( outFile, "  %08x %06x %08x %2u %s \t%s %s\n",
				section[i].strtAddress,
				section[i].size,
				section[i].virtAddress,
				section[i].alignment,
				section[i].symName.c_str(),
				section[i].objName.c_str(),
				section[i].srcName.c_str()
			);
		}
		if ( !section[i].isUnused && section[i].isEntry ) {	// [ NOT unused | entry ]
			fprintf( outFile, "  %08x %06x %08x %s (entry of %s) \t%s %s\n",
				section[i].strtAddress,
				section[i].size,
				section[i].virtAddress,
				section[i].symName.c_str(),
				section[i].parent.c_str(),
				section[i].objName.c_str(),
				section[i].srcName.c_str()
			);
		}
		if ( section[i].isUnused ) {	// [ unused | entry or NOT ]
			fprintf( outFile, "  UNUSED   %06x ........ %s %s %s\n",
				section[i].size,
				section[i].symName.c_str(),
				section[i].objName.c_str(),
				section[i].srcName.c_str()
			);
		}
	}
	fprintf( outFile, "\n\n" );
}
