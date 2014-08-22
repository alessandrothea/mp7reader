#include "Reader.h"

#include <iostream>

using std::cout;
using std::endl;

int main( int argc, char** argv ) {
    
    // proper command line management to be added
    if ( argc != 2 ) {
        
        cout << "Command line: " << argv[0] << " <input file>"<<  endl;
        return -1;
    }
    
    std::string path = argv[1];
    
    try {
        Reader r(path);    
        
    } catch ( std::logic_error e ) {
        cout << "Exception!" << endl;
        cout << e.what() << endl;
    }

    return 0;    
}