
#pragma once

#include <pgen2/core/packet.h>
#include <pgen2/core/header.h>
#include <pgen2/core/protocol/ip.h>
#include <pgen2/core/ipaddress.h>





namespace pgen {



class udp_header : public header {
    public:
        static const size_t min_length = sizeof(uint16_t)*4;
        static const size_t max_length = min_length;

        uint16_t src;
        uint16_t dst;
        uint16_t len;
        uint16_t check;

    public:
        void clear() override;
        void summary(bool moreinfo=false) const override;
        void write(void* buffer, size_t buffer_len) const override;
        void read(const void* buffer, size_t buffer_len) override;
        size_t length() const override;

    public:
        uint16_t calc_checksum(const ipv4_header& ip, 
                const void* data, size_t datalen) const;
};




class udp : public packet {
    private:
        void init_headers() override;

    public:
        struct port {
            static const uint16_t dns      = 53;
            static const uint16_t dhcps    = 67;
            static const uint16_t dhcpc    = 68;
            static const uint16_t ar_drone = 5556;
            static const uint16_t zundoko  = 9988;
        };

        pgen::udp_header UDP;
        pgen::ipv4_header IP;
        pgen::ethernet_header ETH;

        udp();
        udp(const void* buffer, size_t bufferlen);
        udp(const pgen::udp& rhs);

        void clear() override;
};



} /* namespace pgen */
