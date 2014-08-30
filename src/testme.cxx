#include "../include/PacketReader.h"

#include <iostream>
#include <iomanip>

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
        PacketReader r(path,1);
        const TmtData& data = r.get(0);
        TmtData::const_iterator it;
        for( it = data.begin();  it != data.end(); ++it ) {
            cout << "Found packet ->[" << it->first_ << "," << it->last_ << "] " << it->size() << endl;
        }
        
    } catch ( std::logic_error e ) {
        cout << "Exception!" << endl;
        cout << e.what() << endl;
    }


    return 0;    
}