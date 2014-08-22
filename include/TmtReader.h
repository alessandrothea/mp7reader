/* 
 * File:   TmtReader.h
 * Author: ale
 *
 * Created on August 22, 2014, 6:26 PM
 */

#ifndef TMTREADER_H
#define	TMTREADER_H

#include "RawReader.h"

class Packet {
public:
    typedef std::map< uint32_t, std::vector<uint32_t> > LinkMap;
    
    size_t size() const { return last_-first_+1; }
    uint32_t first_;
    uint32_t last_;
    LinkMap links_; 
};

class TmtData {
public:
    
    size_t size() const { return packets_.size(); }

    const std::string& name() const { return name_;}

private:

    std::string name_;
    std::vector<Packet> packets_;
    
    friend class TmtReader;
};

class TmtReader {
public:
    TmtReader( const std::string& path );
    virtual ~TmtReader();
    bool valid() const { return reader_.valid(); }
    
private:
    
    void load();
    
    std::vector< TmtData > buffers_;
    RawReader reader_;
};

typedef std::pair<uint32_t, uint32_t> PacketRange;
std::vector<PacketRange> findPackets( std::vector<uint64_t> data );

#endif	/* TMTREADER_H */

