#pragma once

#include <string>
#include <vector>

class BencodeParser {
public:
    BencodeParser() = default;
    ~BencodeParser() = default;

    // Parses a .torrent file and returns the list of parsed tokens.
    std::vector<std::string> parseFile(const std::string& filename);

    // Parses a raw bencoded string and returns the list of parsed tokens.
    std::vector<std::string> parseString(const std::string& str);

    // Returns SHA1 hash of the 'info' section.
    std::string getHash();

    // Returns list of 20-byte piece hashes.
    std::vector<std::string> getPieceHashes();

private:
    // Internal recursive processing functions
    std::string readFixedAmount(std::size_t amount);
    std::string readUntilDelimiter(char delimiter);
    std::string process();
    void processDictionary();
    void processList();

    // Internal state
    std::string toDecode;
    std::string infoHash;
    std::vector<std::string> parsed;
    std::vector<std::string> piecesHashes;
    std::string rawInfoSection;
    std::size_t idx = 0;
    bool hashCalculated = false;

    // Resets internal state when parsing new file/string.
    void reset();
};
