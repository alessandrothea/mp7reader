#include "TmtReader.h"

#include <boost/foreach.hpp>

using std::cout;
using std::endl;

TmtReader::TmtReader(const std::string& path, uint32_t striphdr, uint32_t stripftr) : reader_( path ), header_(striphdr), footer_(stripftr) {    
    if ( !reader_.valid() ) return;
    load();
}

TmtReader::~TmtReader() {
}

void
TmtReader::load() {
    
    buffers_.reserve(reader_.size());
    RawReader::const_iterator it = reader_.begin();
    for( ; it != reader_.end(); ++it ) {
        const RawData& raw = *it;
        
        // Check channel alignment
        std::set< std::vector<PacketRange > > rangeSet;
        
        for( size_t k(0); k<raw.size(); ++k) {
            std::vector<PacketRange > ranges = findPackets(raw.link(0)); 
            rangeSet.insert(ranges);
        }
        cout << "Number of different patterns: " << rangeSet.size() << endl;
        if ( rangeSet.size() != 1 )
            throw std::runtime_error("Links are not aligned!");
        
        std::vector<PacketRange > pr = *(rangeSet.begin());

        // Create the container
        TmtData data;
        data.name_ = raw.name();

        data.packets_.reserve(pr.size());
        
        // loop over the ranges to build packets
        BOOST_FOREACH(PacketRange p, pr) {
            
            // Check if the header/footer zeroed the packet
            if ( p.second-p.first-header_-footer_<= 0 ) {
                // Turn this into an error message
                cout << "Error: packet length is zero (or less) after header/footer stripping. Skipping." << endl;
                continue;
            }
            
            Packet pkt;
            RawData::const_iterator lIt = raw.begin();
            for( ; lIt != raw.end(); ++lIt ) {
                // Here the 64 bit uint is converted into a 32 bit uint, the data valid bit is stripped in the 64->32 bit conversion.
                pkt.links_[lIt->first] = std::vector<uint32_t>(
                        lIt->second.begin() + p.first + header_,
                        lIt->second.begin() + p.second - footer_
                        );
            }
            pkt.first_ = p.first;
            pkt.last_ = p.second;
                    
            data.packets_.push_back(pkt);
        }
        
        buffers_.push_back(data);   
        
        BOOST_FOREACH( Packet p, data.packets_) {
            cout << "Found packet [" << p.first_ << "," << p.last_ << "] " << p.size() << endl;
        }
        
        
    }
}

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
    
    if ( v && (begin != -1 ) ) {
        end = data.size()-1;
        ranges.push_back(std::make_pair(begin,end) );
    }
    
    return ranges;

}