#include "Reader.h"
//#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string> 

// Namespace resolution
using std::cout;
using std::endl;

const std::vector<uint64_t>& 
Snapshot::link(uint32_t i) {
    LinkMap::iterator it = links_.find(i);
    if ( it == links_.end() )
        throw std::runtime_error("Link id not found");
    
    return  it->second;
}
boost::regex Reader::reBoard_("^Board (.+)");
boost::regex Reader::reLink_("^Link : (.*)");
boost::regex Reader::reQuadChan_("^Quad/Chan : (.*)");
boost::regex Reader::reFrame_("^Frame (\\d{4}) : (.*)");
boost::regex Reader::reValid_("([01])v([0-9a-fA-F]{8})");

Reader::Reader(const std::string& path) : valid_(false), path_(path), file_(path)
//        reBoard_("^Board (.+)"), reLink_("^Link : (.*)"),
//        reQuadChan_("^Quad/Chan : (.*)"), reFrame_("^Frame (\\d{4}) : (.*)"),
//        reValid_("[01]v\\x{8}") 
{
    if (!file_.is_open()) {
        cout << "File " << path << " not found" << endl;
        valid_ = false;
        return;
    }

    load();
}

Reader::~Reader() {
}

// search for Board
// then search for Link
// then search for Frames

// Rules:
// No links or frames before Board
// Only Links after Board
// Only frames after Link until board

// search board
// search links
// read rows

void
Reader::load() {
    using namespace boost;


    // Data, to be stored in a BufferSnapshot object
    while (file_.good()) {
        std::string id = searchBoard();
        cout << "Id: " << id << endl;
        std::vector<uint32_t> links = searchLinks();

        cout << "Links (" << links.size() << ") : ";

        BOOST_FOREACH(uint32_t l, links) {
            cout << l << ",";
        }
        cout << endl;

        std::vector< std::vector<uint64_t> > data = readRows();
        cout << "Data loaded (" << data.size() << ")" << endl;

        // Id, Link # and Data Loaded

        Snapshot s;
        s.name_ = id;

        std::vector< std::vector<uint64_t> > chans( links.size(), std::vector<uint64_t>(data.size()) );

        // Transpose
        for ( size_t i(0); i<links.size(); ++i) {
            for ( size_t j(0); j<data.size(); ++j) {
                chans[i][j] = data[j][i];
            }
        }
        
        // pack
        for ( size_t i(0); i<links.size(); ++i) {
            s.links_.insert( std::make_pair(links[i],chans[i]) );
        }
        
        snaps_.insert(std::make_pair(s.name(),s));
    }

}

std::string
Reader::searchBoard() {
    std::string line;
    std::string id;
    boost::smatch what;

    while (getline(file_, line)) {

        // Trim and skip empties and comments
        boost::trim(line);
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        if (boost::regex_match(line, what, reBoard_)) {
            // Create a new buffer snapshot
            id = what[1];
            return id;
        } else {
            cout << "Here '" << line << "'" << endl;
            throw std::logic_error("Unexpected line found!");
        }
    }
    throw std::logic_error("No board found");
}

std::vector<uint32_t>
Reader::searchLinks() {
    std::string line;
    boost::smatch what;

    while (getline(file_, line)) {

        boost::trim(line);
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        if (boost::regex_match(line, what, reQuadChan_)) {
            // Not used
            continue;
        }

        if (boost::regex_match(line, what, reLink_)) {

            std::vector<std::string> tokens;
            std::string tmp = what[1].str();
            // Trim the line
            boost::trim(tmp);
            // Split line into tokens
            boost::split(tokens, tmp, boost::is_any_of(" \t"), boost::token_compress_on);
            // Convert it into uint32 s
            std::vector<uint32_t> links;
            std::transform(tokens.begin(), tokens.end(), std::back_inserter(links), boost::lexical_cast<uint32_t, const std::string&>);
            return links;
        } else {
            throw std::logic_error("Unexpected line found!");
        }
    }
    throw std::logic_error("No list of links found");
}

uint64_t Reader::validStrToUint64(const std::string& token) {

    boost::smatch what;
    if (!boost::regex_match(token, what, reValid_)) {
        throw std::logic_error("Token '" + token + "' doesn't match the valid format");
    }

    uint64_t value = (uint64_t) (what[1] == "0") << 32;
    value += std::stoul(what[2].str(), 0x0, 16);
    return value;
}

std::vector< std::vector<uint64_t> >
Reader::readRows() {
    std::string line;
    boost::smatch what;
    std::vector< std::vector<uint64_t> > data;
    int place = file_.tellg();
    while (getline(file_, line)) {

        if (boost::regex_match(line, what, reBoard_)) {
            // Upos, next board found. Go back by one line
            file_.seekg(place);
            return data;
        }

        if (boost::regex_match(line, what, reFrame_)) {
            // check frame number
            uint32_t n = boost::lexical_cast<uint32_t>(what[1].str());

            if (n != data.size()) {
                std::stringstream ss;
                ss << "Frame misalignment! (expected " << data.size() << " found " << n;
                throw std::logic_error(ss.str());
            }
            std::vector<std::string> tokens;
            std::string tmp = what[2].str();
            boost::trim(tmp);
            boost::split(tokens, tmp, boost::is_any_of(" \t"), boost::token_compress_on);

            std::vector<uint64_t> row;
            std::transform(tokens.begin(), tokens.end(), std::back_inserter(row), validStrToUint64);

            data.push_back(row);
        }

        place = file_.tellg();
    }

    return data;

}

std::vector<std::string>
Reader::readrow(int nLinks) {

    // input buffers
    std::string line;
    std::getline(file_, line);
    if (!file_.good()) {
        cout << "End of input file! " << path_ << std::endl;
        return std::vector<std::string>(0);
    }

    // split line into tokens
    std::vector<std::string> data;
    boost::split(data, line, boost::is_any_of("\t "), boost::token_compress_on);

    // check we have read the right number of link words
    if ((int) data.size() - 3 != nLinks) {
        cout << "Read " << data.size() << " links, expected " << nLinks << " " << data.at(0) << " " << data.at(data.size() - 1) << std::endl;
        return std::vector<std::string>(0);
    }

    // remove "Frame" and ":"
    std::vector<std::string>::iterator itr = data.begin();
    data.erase(itr);
    itr++;
    data.erase(itr);

    return data;

}