#pragma once

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include <openssl/sha.h>

namespace utils {
// Converts 4 bytes in big endian to int.
int BytesToInt(const std::string& bytes);

// Converts an integer to a 4-byte big endian representation.
std::string IntToBytes(int value);

// Calculates SHA1 hash sum.
// The result is a raw 20-byte binary string (not hex-encoded).
std::string CalculateSHA1(const std::string& msg);

// Converts a binary string to its hexadecimal representation.
// Useful for displaying SHA1 hashes.
std::string HexEncode(const std::string& input);
} // namespace utils
