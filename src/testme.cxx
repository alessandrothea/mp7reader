#include "TmtReader.h"

#include <iostream>
#include <iomanip>

#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
using std::cout;
using std::endl;
using std::hex;
using std::dec;


         
int main( int argc, char** argv ) {
    
    // proper command line management to be added
    if ( argc != 2 ) {
        
        cout << "Command line: " << argv[0] << " <input file>"<<  endl;
        return -1;
    }
    
    std::string path = argv[1];
    
    try {
        TmtReader r(path);
        
    } catch ( std::logic_error e ) {
        cout << "Exception!" << endl;
        cout << e.what() << endl;
    }


    return 0;    
}