#include "Reader.h"

#include <iostream>
#include <iomanip>

#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
using std::cout;
using std::endl;
using std::hex;
using std::dec;


class Packet {
public:
    typedef std::map< uint32_t, std::vector<uint32_t> > LinkMap;
    
    LinkMap links_; 
};

typedef std::pair<uint32_t, uint32_t> PacketRange;

std::vector<PacketRange> findPackets( std::vector<uint64_t> data ) {
    std::vector<PacketRange> ranges;
    bool v = false;
    int32_t begin(-1), end(-1);
    for ( size_t i(0); i < data.size(); ++i) {
        uint64_t x = data[i];
        if (not v) {
            if ((x >> 32) & 1) {
                v = true;
                begin = i;
            }
            continue;
        } else {
            if ( not ((x >> 32 ) & 1 ) ) {
                v = false;
                end = i-1;
                ranges.push_back(std::make_pair(begin,end) );
            }
            continue;
        }
    }
    
    if ( not v && (begin != -1 ) ) {
        end = data.size()-1;
        ranges.push_back(std::make_pair(begin,end) );
    }
    
    return ranges;

}

         
int main( int argc, char** argv ) {
    
    // proper command line management to be added
    if ( argc != 2 ) {
        
        cout << "Command line: " << argv[0] << " <input file>"<<  endl;
        return -1;
    }
    
    std::string path = argv[1];
    
    try {
        Reader r(path);    
        
        uint64_t x;
        x = r.get(0).link(0).at(0);
        cout << "(0,0) : "  << ((x >> 32) & 1) << "v" << hex << (x & ((1L<<32) -1 ))<< dec << endl;
        x = r.buffers_[0].link(0).at(37);
        cout << "(0,37) : "  << ((x >> 32) & 1) << "v" << hex << (x & ((1L<<32) -1 ))<< dec << endl;
        
        const BufferData& data = r.get(0);
        std::set< std::vector<PacketRange > > rangeSet;
        
        for( size_t k(0); k<data.size(); ++k) {
            std::vector<PacketRange > ranges = findPackets(r.get(0).link(0)); 
            rangeSet.insert(ranges);
        }
        cout << "number of different patterns: " << rangeSet.size() << endl;
        
        std::vector<PacketRange > pr = *(rangeSet.begin());
        
        BOOST_FOREACH(PacketRange p, pr) {
            Packet pkt;
            BufferData::const_iterator lIt = data.begin();
            for( ; lIt != data.end(); ++lIt ) {
                cout << lIt->first << endl;
                // Here the 64 bit uint is converted into a 32 bit uint, stripping the data valid bit.
                std::vector<uint32_t> tmp(lIt->second.begin() + p.first, lIt->second.begin()+p.second);
                
//                return 0;
            }
        }
        
    } catch ( std::logic_error e ) {
        cout << "Exception!" << endl;
        cout << e.what() << endl;
    }

    return 0;    
}