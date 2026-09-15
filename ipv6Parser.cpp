#include "ipv6Parser.h"

#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <format>

IPv6Parser::IPv6Parser()
{
}

std::string IPv6Parser::string_check(std::string &str)
{
    if (str.size() < 4)
    {
        str.insert(0, 4 - str.size(), '0');
    }
    return str;
}

std::optional<std::array<std::string, 8>> IPv6Parser::parseShorthand(std::string shorthand)
{
    std::vector<std::string> stringVector;
    const std::string validHexChars = "0123456789abcdefABCDEF";
    std::string current_string = "";
    std::string previous_string = "";
    int colon_count = 0;
    bool isThereDoubleColon = false;
    itIsAnIPv4 = false;

    shorthand = IPv6Parser::checkForIPv4(shorthand);

    int lg = shorthand.size();
    // GUARDRAIL: Single leading colon check
    if (!shorthand.empty() && shorthand[0] == ':')
    {
        // If it starts with ':', the second character MUST also be ':' (i.e. '::')
        if (shorthand.size() == 1 || shorthand[1] != ':')
        {
            fprintf(stderr, "\nInvalid String: Single leading colon is forbidden!\n");
            fflush(stderr);
            return std::nullopt; // Reject single leading colon
        }
    }

    // GUARDRAIL: Single trailing colon check
    if (!shorthand.empty() && shorthand.back() == ':')
    {
        // If it ends with ':', the second-to-last character MUST also be ':' (i.e. '::')
        if (shorthand.size() < 2 || shorthand[shorthand.size() - 2] != ':')
        {
            fprintf(stderr, "\nInvalid String: Single trailing colon is forbidden!\n");
            fflush(stderr);
            return std::nullopt; // Reject single trailing colon
        }
    }

    for (int i = 0; i < lg; i++)
    {
        char ch = std::tolower(shorthand[i]);

        if (ch == ':')
        {
            colon_count++;

            if (colon_count > 2 || (colon_count == 2 && isThereDoubleColon == true))
            {
                fprintf(stderr, "\nInvalid String!");
                fflush(stderr);
                return std::nullopt; // Reject wrong number of ':'
            }
            if (colon_count == 2)
            {
                isThereDoubleColon = true;
            }
            int c_str_size = current_string.size();

            if (c_str_size > 4)
            {
                fprintf(stderr, "\nInvalid String!");
                fflush(stderr);

                return std::nullopt; // Reject wrong size
            }
            if (c_str_size > 0)
            {
                string_check(current_string);
            }
            stringVector.push_back(current_string);
            previous_string = current_string;
            current_string = "";
        }
        else if (std::isxdigit(static_cast<unsigned char>(ch)))
        {
            colon_count = 0;
            current_string += ch;
        }
        else
        {
            fprintf(stderr, "Invalid String!");
            fflush(stderr);

            return std::nullopt; // Reject invalid character
        }
    }
    int c_str_size = current_string.size();
    if (c_str_size > 4)
    {
        fprintf(stderr, "Invalid String!");
        fflush(stderr);

        return std::nullopt; // Reject wrong size
    }

    if (c_str_size > 0)
    {
        string_check(current_string);
    }

    if (current_string != "" || previous_string != "")
        stringVector.push_back(current_string);

    fprintf(stderr, "\nLength: %d\n", lg);
    fprintf(stderr, "\nVector Length: %d\n", static_cast<int>(stringVector.size()));

    int vector_length = stringVector.size();

    if (vector_length > 1)
    {
        if (stringVector[0] == "" && stringVector[1] == "")
        {
            stringVector.erase(stringVector.begin());
            vector_length -= 1;
        }
    }

    // Look for the "" marker
    auto empty_it = std::find(stringVector.begin(), stringVector.end(), "");

    if (empty_it != stringVector.end())
    {
        // missing_blocks = 8 - (2 - 1) = 7
        int missing_blocks = 8 - (static_cast<int>(stringVector.size()) - 1);

        if (missing_blocks < 1)
        {
            fprintf(stderr, "\nInvalid String: Too many blocks specified alongside '::'\n");
            return std::nullopt;
        }

        // Replaces "" at index 1 with 7 copies of "0000"
        auto pos = stringVector.erase(empty_it);
        stringVector.insert(pos, missing_blocks, "0000");
    }

    if (stringVector.size() != 8)
    {
        fprintf(stderr, "\nInvalid String: Incorrect number of blocks after expansion\n");
        return std::nullopt;
    }

    for (int w = 0; w < 8; w++)
    {
        fprintf(stderr, "Element [%d]: %s\n", w, stringVector[w].c_str());
    }
    fflush(stderr);

    std::array<std::string, 8> resultArray;
    std::move(stringVector.begin(), stringVector.end(), resultArray.begin());

    return resultArray;
}

std::optional<int> IPv6Parser::checkEnteredCidr(std::string cidrString)
{
    int num = 0;
    try
    {
        num = std::stoi(cidrString);
    }
    catch (const std::invalid_argument &e)
    {
        fprintf(stderr, "Invalid CIDR argument");
        fflush(stderr);
        return std::nullopt;
    }
    catch (const std::out_of_range &e)
    {
        fprintf(stderr, "Integer out of range for data type\n");
        fflush(stderr);
        return std::nullopt;
    }

    if (num < 0 || num > 128)
    {
        fprintf(stderr, "Integer out of range");
        fflush(stderr);

        return std::nullopt;
    }
    return num;
}

std::array<uint16_t, 8> IPv6Parser::createIPv6Mask(int cidr)
{
    std::array<uint16_t, 8> mask = {0};
    int full_blocks = cidr / 16;
    for (int i = 0; i < full_blocks; i++)
    {
        mask[i] = 0xFFFFFFFF;
    }

    if (full_blocks < 8)
    {
        int remainder = cidr % 16;
        if (remainder > 0)
        {
            mask[full_blocks] = static_cast<uint16_t>(0xFFFF << 16 - remainder);
        }
    }
    return mask;
}

std::array<uint16_t, 8> IPv6Parser::applyMask(const std::array<uint16_t, 8> &ip, const std::array<uint16_t, 8> &mask)
{
    std::array<uint16_t, 8> result = {0};
    for (int i = 0; i < 8; i++)
    {
        result[i] = ip[i] & mask[i];
    }
    return result;
}

std::array<uint16_t, 8> IPv6Parser::calculateLastAddress(const std::array<uint16_t, 8> &netId, const std::array<uint16_t, 8> &mask)
{

    std::array<uint16_t, 8> result;
    for (int i = 0; i < 8; ++i)
    {
        result[i] = netId[i] | (~mask[i]);
    }
    return result;
}

std::array<int, 2> IPv6Parser::quartetTo2Int(uint16_t quartet)
{
    int secondHalf = quartet & 0x00FF;
    int firstHalf = (quartet & 0xFF00) >> 8;
    std::array<int, 2> result;
    result[0] = firstHalf;
    result[1] = secondHalf;

    return result;
}

std::string IPv6Parser::checkForIPv4(std::string &shorthand)
{
    std::vector<std::string> IPv4Values = {};
    std::string currentString = "";
    int lg = shorthand.size();
    for (int i = 0; i < lg; ++i)
    {
        char ch = shorthand[i];
        if (ch == '.')
        {
            if (i == 0 || i == lg-1)
            {
                fprintf(stderr, "\nNot an IPv4 string");
                fflush(stderr);
                return shorthand;
            }
            IPv4Values.push_back(currentString);
            currentString = "";
        }
        else if (std::isdigit(static_cast<unsigned char>(ch)))
        {
            currentString += ch;
        }
        else
        {
            fprintf(stderr, "\nNot an IPv4 string");
            fflush(stderr);
            return shorthand;
        }
    }
    IPv4Values.push_back(currentString);
    if (IPv4Values.size() != 4) {
          fprintf(stderr, "\nToo many integer values");
            fflush(stderr);
            return shorthand;

        
    }

    std::string ipv6String = "::ffff:";

    for (int j = 0; j < 4; j += 2) {
        int val1 = std::stoi(IPv4Values[j]);
        int val2 = std::stoi(IPv4Values[j+1]);

        // Range check to ensure values fit in a 0-255 byte
        if (val1 > 255 || val2 > 255) {
            fprintf(stderr, "\nOctet out of range");
            return shorthand;
        }

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << std::hex << val1
           << std::setfill('0') << std::setw(2) << std::hex << val2;

        ipv6String += ss.str();
        if (j == 0) {
            ipv6String += ":";
        }
    }

    IPv6Parser::itIsAnIPv4 = true;
  
    return ipv6String;
}