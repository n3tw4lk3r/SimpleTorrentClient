#include "BencodeParser.hpp"

#include <fstream>
#include <iostream>

#include "utils.hpp"

std::string BencodeParser::readFixedAmount(std::size_t amount) {
    std::string res = toDecode.substr(idx, amount);
    idx += amount;
    return res;
}

std::string BencodeParser::readUntilDelimiter(char delimiter) {
    std::string res;
    while (idx < toDecode.size() && toDecode[idx] != delimiter) {
        res += toDecode[idx++];
    }
    if (idx < toDecode.size()) {
        ++idx; // skip delimiter
    }
    return res;
}

std::string BencodeParser::process() {
    if (idx >= toDecode.size()) {
        return {};
    }

    char ch = toDecode[idx];
    if (std::isdigit(ch)) {
        int str_len = std::stoi(readUntilDelimiter(':'));
        std::string tmp = readFixedAmount(str_len);
        parsed.push_back(tmp);
        return tmp;
    }

    if (ch == 'i') {
        ++idx;
        parsed.push_back(readUntilDelimiter('e'));
        return {};
    }

    if (ch == 'd') {
        processDictionary();
        return {};
    }

    if (ch == 'l') {
        processList();
        return {};
    }

    return {};
}

void BencodeParser::processDictionary() {
    ++idx;
    std::string key;
    bool insideInfo = false;
    std::size_t infoStart = 0;

    while (idx < toDecode.size() && toDecode[idx] != 'e') {
        if (key.empty()) {
            key = process();
            if (key == "info") {
                infoStart = idx;
                insideInfo = true;
            }
        } else {
            process();
            if (insideInfo) {
                std::size_t infoEnd = idx;
                rawInfoSection = toDecode.substr(infoStart,
                                                 infoEnd - infoStart);
                insideInfo = false;
            }
            key.clear();
        }
    }

    if (idx < toDecode.size()) {
        ++idx;
    }
}

void BencodeParser::processList() {
    ++idx;
    while (idx < toDecode.size() && toDecode[idx] != 'e') {
        process();
    }
    if (idx < toDecode.size()) {
        ++idx;
    }
}

std::vector<std::string> BencodeParser::parseFile(const std::string& filename) {
    reset();
    std::ifstream input(filename, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    toDecode.assign((std::istreambuf_iterator<char>(input)),
                     std::istreambuf_iterator<char>());

    process();
    return parsed;
}

std::vector<std::string> BencodeParser::parseString(std::string str) {
    reset();
    toDecode = str;
    process();
    return parsed;
}

std::string BencodeParser::getHash() {
    if (!hashCalculated && !rawInfoSection.empty()) {
        infoHash = utils::CalculateSHA1(rawInfoSection);
        hashCalculated = true;
    }
    return infoHash;
}

std::vector<std::string> BencodeParser::getPieceHashes() {
    if (!piecesHashes.empty()) {
        return piecesHashes;
    }

    std::size_t piecesStart = std::string::npos;
    std::size_t piecesEnd = std::string::npos;

    for (std::size_t i = 0; i < toDecode.size(); ++i) {
        if (toDecode.substr(i, 8) == "6:pieces") {
            std::size_t tempIdx = i + 8;
            std::string lenStr;
            while (tempIdx < toDecode.size() &&
                   std::isdigit(toDecode[tempIdx])) {
                lenStr += toDecode[tempIdx++];
            }
            if (tempIdx < toDecode.size() && toDecode[tempIdx] == ':') {
                ++tempIdx;
                int len = std::stoi(lenStr);
                piecesStart = tempIdx;
                piecesEnd = piecesStart + len;
                break;
            }
        }
    }

    if (piecesStart == std::string::npos || piecesEnd == std::string::npos ||
        piecesEnd > toDecode.size()) {
        throw std::runtime_error("Could not extract piece hashes \
                                  from bencoded string.");
    }

    for (std::size_t i = piecesStart; i + 20 <= piecesEnd; i += 20) {
        piecesHashes.emplace_back(toDecode.substr(i, 20));
    }

    return piecesHashes;
}

void BencodeParser::reset() {
    toDecode.clear();
    parsed.clear();
    piecesHashes.clear();
    idx = 0;
    hashCalculated = false;
    rawInfoSection.clear();
}