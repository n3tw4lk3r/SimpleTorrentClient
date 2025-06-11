#include "utils.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include <openssl/sha.h>

namespace utils {
int BytesToInt(const std::string& bytes) {
    if (bytes.size() != 4) {
        throw std::invalid_argument("BytesToInt expects exactly 4 bytes");
    }

    return (static_cast<unsigned char>(bytes[0]) << 24) |
           (static_cast<unsigned char>(bytes[1]) << 16) |
           (static_cast<unsigned char>(bytes[2]) << 8)  |
           (static_cast<unsigned char>(bytes[3]));
}

std::string IntToBytes(int value) {
    std::string result(4, '\0');
    result[0] = static_cast<char>((value >> 24) & 0xFF);
    result[1] = static_cast<char>((value >> 16) & 0xFF);
    result[2] = static_cast<char>((value >> 8) & 0xFF);
    result[3] = static_cast<char>(value & 0xFF);
    return result;
}

std::string CalculateSHA1(const std::string& msg) {
    unsigned char hash[SHA_DIGEST_LENGTH]; // SHA_DIGEST_LENGTH == 20

    // can't use static_cast here
    SHA1(reinterpret_cast<const unsigned char*>(msg.data()), msg.size(), hash);
    
    return std::string(hash, hash + SHA_DIGEST_LENGTH);
}

std::string HexEncode(const std::string& input) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    for (unsigned char c : input) {
        ss << std::setw(2) << static_cast<int>(c);
    }

    return ss.str();
}
} // namespace utils
