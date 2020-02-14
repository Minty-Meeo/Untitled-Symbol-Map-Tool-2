#include "ANSI_include.h"

class symbol {
	public:
		symbol();
		uint32_t strtAddress;
		uint32_t size;
		uint32_t virtAddress;
		uint32_t alignment;
		string mangledName;
		
		string symName;
		string objName;
		string srcName;
		
		bool isUnused;
		bool isClosure;
			vector<uint32_t> children;
		bool isEntry;
			string parent;
			uint32_t offset;
};

class symbolMap {
	public:
		void readFromFile( const char* filename );
		void writeToFile( const char* filename );
		vector<symbol> init;
		vector<symbol> extab;
		vector<symbol> extabindex;
		vector<symbol> text;
		vector<symbol> ctors;
		vector<symbol> dtors;
		vector<symbol> rodata;
		vector<symbol> data;
		vector<symbol> bss;
		vector<symbol> sdata;
		vector<symbol> sbss;
		vector<symbol> sdata2;
		vector<symbol> sbss2;
};

void writeSection( FILE*, const char*, vector<symbol>& );
void readSection( ifstream&, std::ios::streampos, vector<symbol>& );
