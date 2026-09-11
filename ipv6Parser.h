#ifndef IPV6PARSER_H
#define IPV6PARSER_H

#include <cstdint>
#include <array>
#include <string>
#include <optional>
#include <sstream>
#include <iomanip>

class IPv6Parser
{

public:
    using IPv6Blocks = std::array<uint16_t, 8>;

    static std::optional<std::array<std::string, 8>> parseShorthand(std::string shorthand);
    static std::optional<int>checkEnteredCidr(std::string cidrString);


    static std::string string_check(std::string &str);

    static std::array<uint16_t, 8> createIPv6Mask(int cidr);
    static std::array<uint16_t, 8> applyMask(const std::array<uint16_t,8>& ip, const std::array<uint16_t,8>& mask);

    static std::array<uint16_t, 8> calculateLastAddress(const std::array<uint16_t, 8>& netId, const std::array<uint16_t, 8>& mask);
    

   

    struct IPv6Address
    {
        std::array<std::string, 8> rawStrings{};
        std::array<uint16_t, 8> blocks{};

        // Default constructor
        IPv6Address() = default;

        // Construct directly from the parser's std::array<std::string, 8>
        explicit IPv6Address(const std::array<std::string, 8> &strArray)
            : rawStrings(strArray)
        {
            for (size_t i = 0; i < 8; ++i)
            {
                // Converts hex string (e.g. "0db8" or "1") to uint16_t value
                blocks[i] = static_cast<uint16_t>(std::stoul(rawStrings[i], nullptr, 16));
            }
        }

        // Helper: Returns full canonical representation ("2001:0db8:0000:0000:0000:0000:0000:0001")
        std::string toFullString() const
        {
            std::ostringstream oss;
            for (size_t i = 0; i < 8; ++i)
            {
                  oss << std::hex << std::setw(4) << std::setfill('0') << blocks[i];
                if (i < 7)
                    oss << " : ";
            }
            return oss.str();
        }
        std::array<uint16_t, 8> getBlocks() const
        {
            return (this->blocks);

        }
    };

    IPv6Parser();
};

#endif
