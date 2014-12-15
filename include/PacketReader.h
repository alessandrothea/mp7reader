/* 
 * File:   TmtReader.h
 * Author: ale
 *
 * Created on August 22, 2014, 6:26 PM
 */

#ifndef TMTREADER_H
#define	TMTREADER_H

#include "RawReader.h"

typedef std::pair<uint32_t, uint32_t> PacketRange;

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

    const std::string& name() const { return name_;}
    
    typedef std::vector<Packet>::const_iterator const_iterator;

    const_iterator begin() const { return packets_.begin(); }

    const_iterator end() const { return packets_.end(); }
    
    size_t size() const { return packets_.size(); }
    
private:

    std::string name_;
    std::vector<Packet> packets_;
    
    friend class PacketReader;
};

class PacketReader {
public:
    typedef std::vector<TmtData>::const_iterator const_iterator;
        
    PacketReader( const std::string& path, uint32_t striphdr = 0, uint32_t stripftr = 0);

    virtual ~PacketReader();

    bool valid() const { return reader_.valid(); }
    
    const TmtData& get( size_t i ) { return buffers_.at(i); }

    const_iterator begin() const { return buffers_.begin(); }

    const_iterator end() const { return buffers_.end(); }
    
    size_t size() const { return buffers_.size(); }
    
private:
    
    void load();
    static std::vector<PacketRange> findPackets( std::vector<uint64_t> data );
    
    std::vector< TmtData > buffers_;
    RawReader reader_;
    uint32_t header_;
    uint32_t footer_;
};

#endif	/* TMTREADER_H */
