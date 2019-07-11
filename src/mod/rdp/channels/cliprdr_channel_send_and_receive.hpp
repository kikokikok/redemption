/*
    This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

    This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

    You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     675 Mass Ave, Cambridge, MA 02139, USA.

    Product name: redemption, a FLOSS RDP proxy
    Copyright (C) Wallix 2015
    Author(s): Christophe Grosjean, Raphael Zhou, Clément Moroldo
*/


#pragma once

#include "core/channel_list.hpp"
#include "core/RDP/clipboard.hpp"
#include "mod/rdp/channels/base_channel.hpp"
#include "mod/rdp/channels/sespro_launcher.hpp"
#include "utils/stream.hpp"
#include "system/ssl_sha256.hpp"

#include <unordered_map>
#include <vector>

#define FILE_LIST_FORMAT_NAME "FileGroupDescriptorW"



struct ClipboardSideData
{
    struct file_contents_request_info
    {
        uint32_t lindex;
        uint64_t position;
        uint32_t cbRequested;
        uint32_t clipDataId;
        uint32_t offset;
    };
    using file_contents_request_info_inventory_type = std::unordered_map<uint32_t /*streamId*/, file_contents_request_info>;

    struct file_info_type
    {
        std::string file_name;
        uint64_t size;
        uint64_t sequential_access_offset;
        SslSha256 sha256;
    };
    using file_info_inventory_type = std::vector<file_info_type>;

    using file_stream_data_inventory_type = std::unordered_map<uint32_t /*clipDataId*/, file_info_inventory_type>;

    uint16_t message_type = 0;
    bool use_long_format_names = false;
    uint32_t clipDataId = 0;
    uint32_t file_list_format_id = 0;
    uint32_t dataLen = 0;
    uint32_t streamId = 0;
    StaticOutStream<RDPECLIP::FileDescriptor::size()> file_descriptor_stream;

    file_contents_request_info_inventory_type file_contents_request_info_inventory;
    file_stream_data_inventory_type file_stream_data_inventory;

    std::string provider_name;

    uint32_t last_dwFlags = RDPECLIP::FILECONTENTS_SIZE;

    ClipboardSideData(std::string provider_name)
      : provider_name(std::move(provider_name))
    {}

    void set_file_contents_request_info_inventory(uint32_t lindex, uint64_t position, uint32_t cbRequested, uint32_t clipDataId, uint32_t offset, uint32_t streamID) {
        this->file_contents_request_info_inventory[streamID] =
        {
            lindex,
            position,
            cbRequested,
            clipDataId,
            offset
        };
    }

    void update_file_contents_request_inventory(RDPECLIP::FileDescriptor const& fd) {
        file_info_inventory_type & file_info_inventory =
                this->file_stream_data_inventory[this->clipDataId];
            file_info_inventory.push_back({ fd.fileName(), fd.file_size(), 0, SslSha256() });
    }
};

struct ClipboardData
{
    ClipboardSideData server_data {"server"};
    ClipboardSideData client_data {"client"};

    uint32_t requestedFormatId = 0;
};

struct ClipboardCapabilitiesReceive {

    ClipboardCapabilitiesReceive(ClipboardSideData & clip_data, InStream& chunk, const RDPVerbose verbose) {
        {
            const unsigned int expected = 4;   //     cCapabilitiesSets(2) +
                                                //     pad1(2)
            if (!chunk.in_check_rem(expected)) {
                LOG(LOG_ERR,
                    "ClipboardVirtualChannel::process_%s_clipboard_capabilities_pdu: "
                        "Truncated CLIPRDR_CAPS, need=%u remains=%zu", clip_data.provider_name,
                    expected, chunk.in_remain());
                throw Error(ERR_RDP_DATA_TRUNCATED);
            }
        }

        LOG_IF(bool(verbose & RDPVerbose::cliprdr), LOG_INFO,
            "ClipboardVirtualChannel::process_%s_message: "
                "Clipboard Capabilities PDU", clip_data.provider_name);

        const uint16_t cCapabilitiesSets = chunk.in_uint16_le();
        assert(1 == cCapabilitiesSets);

        chunk.in_skip_bytes(2); // pad1(2)

        for (uint16_t i = 0; i < cCapabilitiesSets; ++i) {
            RDPECLIP::CapabilitySetRecvFactory f(chunk);

            if (f.capabilitySetType() == RDPECLIP::CB_CAPSTYPE_GENERAL) {
                RDPECLIP::GeneralCapabilitySet general_caps;

                general_caps.recv(chunk, f);

                if (bool(verbose & RDPVerbose::cliprdr)) {
                    LOG(LOG_INFO,
                        "ClipboardVirtualChannel::process_%s_clipboard_capabilities_pdu: "
                            "General Capability Set", clip_data.provider_name);
                    general_caps.log(LOG_INFO);
                }

                clip_data.use_long_format_names =
                    bool(general_caps.generalFlags() & RDPECLIP::CB_USE_LONG_FORMAT_NAMES);

            }
        }
    }
};

struct FilecontentsRequestReceive {

    uint32_t dwFlags = 0;
    uint32_t streamID = 0;
    uint32_t lindex = 0;
    uint32_t requested = 0;

    FilecontentsRequestReceive(ClipboardSideData & clip_state, InStream& chunk, const RDPVerbose verbose, uint32_t dataLen) {
        LOG(LOG_INFO, "dataLen=%u FileContentsRequestPDU::minimum_size()=%zu", dataLen, RDPECLIP::FileContentsRequestPDU::minimum_size());
        if (dataLen >= RDPECLIP::FileContentsRequestPDU::minimum_size()) {
            RDPECLIP::FileContentsRequestPDU file_contents_request_pdu;

            file_contents_request_pdu.receive(chunk);
            if (bool(verbose & RDPVerbose::cliprdr)) {
                file_contents_request_pdu.log(LOG_INFO);
            }

            this->dwFlags  = file_contents_request_pdu.dwFlags();
            clip_state.last_dwFlags = this->dwFlags;
            this->streamID = file_contents_request_pdu.streamId();
            this->lindex   = file_contents_request_pdu.lindex();
            this->requested = file_contents_request_pdu.cbRequested();

            if ((RDPECLIP::FILECONTENTS_RANGE == this->dwFlags) && file_contents_request_pdu.clipDataId()) {

                clip_state.set_file_contents_request_info_inventory(
                    file_contents_request_pdu.lindex(),
                    file_contents_request_pdu.position(),
                    file_contents_request_pdu.cbRequested(),
                    file_contents_request_pdu.clipDataId(), 0, this->streamID);
            }
        }
    }
};

struct FilecontentsRequestSendBack {

    FilecontentsRequestSendBack(uint32_t dwFlags, uint32_t streamID, VirtualChannelDataSender* sender)
    {
        if (!sender) {
            return ;
        }

        StaticOutStream<256> out_stream;

        switch (dwFlags)
        {
            case RDPECLIP::FILECONTENTS_RANGE:
            {
                RDPECLIP::FileContentsResponseRange pdu(streamID);
                RDPECLIP::CliprdrHeader header( RDPECLIP::CB_FILECONTENTS_RESPONSE,
                                                RDPECLIP::CB_RESPONSE_FAIL,
                                                pdu.size());
                header.emit(out_stream);
                pdu.emit(out_stream);
            }
            break;

            case RDPECLIP::FILECONTENTS_SIZE:
            {
                RDPECLIP::FileContentsResponseSize pdu(streamID, 0);
                RDPECLIP::CliprdrHeader header( RDPECLIP::CB_FILECONTENTS_RESPONSE,
                                                RDPECLIP::CB_RESPONSE_FAIL,
                                                pdu.size());
                header.emit(out_stream);
                pdu.emit(out_stream);
            }
            break;
        }

        sender->operator()(
            out_stream.get_offset(),
            CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST,
            out_stream.get_data(),
            out_stream.get_offset());
    }
};

struct FormatDataRequestReceive
{
    FormatDataRequestReceive(ClipboardData & clip_data, const RDPVerbose verbose, InStream& chunk) {
        check_throw(chunk, 4, "CLIPRDR_FORMAT_DATA_REQUEST", ERR_RDP_DATA_TRUNCATED);

        clip_data.requestedFormatId = chunk.in_uint32_le();

        LOG_IF(bool(verbose & RDPVerbose::cliprdr), LOG_INFO,
            "requestedFormatId=%s(%u)",
            RDPECLIP::get_FormatId_name(clip_data.requestedFormatId),
            clip_data.requestedFormatId);
    }
};

struct FormatDataRequestSendBack
{
    FormatDataRequestSendBack(VirtualChannelDataSender* sender)
    {
        if (!sender) {
            return ;
        }

        StaticOutStream<256> out_stream;
        RDPECLIP::CliprdrHeader header(RDPECLIP::CB_FORMAT_DATA_RESPONSE, RDPECLIP::CB_RESPONSE_FAIL, 0);
        header.emit(out_stream);

        sender->operator()(
            out_stream.get_offset(),
            CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST,
            out_stream.get_data(),
            out_stream.get_offset());
    }
};

struct FormatDataResponseReceiveFileList
{
    std::vector<RDPECLIP::FileDescriptor> files_descriptors;

    FormatDataResponseReceiveFileList(InStream & chunk, const RDPECLIP::CliprdrHeader & in_header, bool param_dont_log_data_into_syslog, const uint32_t file_list_format_id, const uint32_t flags, OutStream & file_descriptor_stream, const RDPVerbose verbose, char const* direction)
    {
        if (flags & CHANNELS::CHANNEL_FLAG_FIRST) {
            if (!(in_header.msgFlags() & RDPECLIP::CB_RESPONSE_FAIL) && (in_header.dataLen() >= 4 /* cItems(4) */)) {
                const uint32_t cItems = chunk.in_uint32_le();

                if (!param_dont_log_data_into_syslog) {
                    LOG(LOG_INFO,
                        "Sending %sFileGroupDescriptorW(%u) clipboard data to %s. "
                            "cItems=%u",
                        ((flags & CHANNELS::CHANNEL_FLAG_LAST) ? "" : "(chunked) "),
                        file_list_format_id, direction, cItems);
                }
            }
        } else {

            if (file_descriptor_stream.get_offset()) {
                const uint32_t complementary_data_length =
                    RDPECLIP::FileDescriptor::size() -
                        file_descriptor_stream.get_offset();

                assert(chunk.in_remain() >= complementary_data_length);

                file_descriptor_stream.out_copy_bytes(chunk.get_current(),
                    complementary_data_length);

                chunk.in_skip_bytes(complementary_data_length);

                RDPECLIP::FileDescriptor fd;

                InStream in_stream(
                    file_descriptor_stream.get_data(),
                    file_descriptor_stream.get_offset()
                );
                fd.receive(in_stream);
                if (bool(verbose & RDPVerbose::cliprdr)) {
                    fd.log(LOG_INFO);
                }

                this->files_descriptors.push_back(std::move(fd));

                file_descriptor_stream.rewind();
            }
        }

        while (chunk.in_remain() >= RDPECLIP::FileDescriptor::size()) {
            RDPECLIP::FileDescriptor fd;

            fd.receive(chunk);

            if (bool(verbose & RDPVerbose::cliprdr)) {
                fd.log(LOG_INFO);
            }

            this->files_descriptors.push_back(std::move(fd));
        }

        if (chunk.in_remain()) {
            file_descriptor_stream.rewind();

            file_descriptor_stream.out_copy_bytes(
                chunk.get_current(), chunk.in_remain());

            chunk.in_skip_bytes(chunk.in_remain());
        }
    }
};

struct FormatDataResponseReceive
{
    std::string data_to_dump;

    FormatDataResponseReceive(const uint32_t requestedFormatId, InStream & chunk, const uint32_t flags)
    {
        if (flags & CHANNELS::CHANNEL_FLAG_FIRST) {
//                 InStream chunk_serie = chunk.clone();

            constexpr size_t const max_length_of_data_to_dump = 256;

            switch (requestedFormatId) {
    /*
                case RDPECLIP::CF_TEXT:
                {
                    const size_t length_of_data_to_dump = std::min(
                        chunk.in_remain(), max_length_of_data_to_dump);
                    const std::string data_to_dump(
                        ::char_ptr_cast(chunk.get_current()),
                        length_of_data_to_dump);
                    LOG(LOG_INFO, "%s", data_to_dump);
                }
                break;
    */
                case RDPECLIP::CF_UNICODETEXT:
                {
                    assert(!(chunk.in_remain() & 1));

                    const size_t length_of_data_to_dump = std::min(
                        chunk.in_remain(), max_length_of_data_to_dump * 2);

                    constexpr size_t size_of_utf8_string =
                        max_length_of_data_to_dump *
                            maximum_length_of_utf8_character_in_bytes;

                    uint8_t utf8_string[size_of_utf8_string + 1] {};
                    const size_t length_of_utf8_string = ::UTF16toUTF8(
                        chunk.get_current(), length_of_data_to_dump / 2,
                        utf8_string, size_of_utf8_string);
                    data_to_dump.assign(
                        ::char_ptr_cast(utf8_string),
                        ((length_of_utf8_string && !utf8_string[length_of_utf8_string - 1]) ?
                            length_of_utf8_string - 1 :
                            length_of_utf8_string));
                }
                break;

                case RDPECLIP::CF_LOCALE:
                {
                    const uint32_t locale_identifier = chunk.in_uint32_le();
                    data_to_dump = std::to_string(locale_identifier);
                }
                break;
            }
        }
    }
};


struct ServerMonitorReadySendBack {

    ServerMonitorReadySendBack(const RDPVerbose verbose, const bool current_use_long_format_names, BaseVirtualChannel * base_channel) {
        LOG_IF(bool(verbose & RDPVerbose::cliprdr), LOG_INFO,
            "ClipboardVirtualChannel::process_server_monitor_ready_pdu: "
                "Send Clipboard Capabilities PDU.");

        RDPECLIP::GeneralCapabilitySet general_cap_set(
            RDPECLIP::CB_CAPS_VERSION_1,
            RDPECLIP::CB_USE_LONG_FORMAT_NAMES);
        RDPECLIP::ClipboardCapabilitiesPDU clipboard_caps_pdu(1);
        RDPECLIP::CliprdrHeader caps_clipboard_header(RDPECLIP::CB_CLIP_CAPS, RDPECLIP::CB_RESPONSE__NONE_,
            clipboard_caps_pdu.size() + general_cap_set.size());

        StaticOutStream<1024> caps_stream;

        caps_clipboard_header.emit(caps_stream);
        clipboard_caps_pdu.emit(caps_stream);
        general_cap_set.emit(caps_stream);

        base_channel->send_message_to_server(
            caps_stream.get_offset(),
            CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST,
            caps_stream.get_data(),
            caps_stream.get_offset());

        LOG_IF(bool(verbose & RDPVerbose::cliprdr), LOG_INFO,
            "ClipboardVirtualChannel::process_server_monitor_ready_pdu: "
                "Send Format List PDU.");

        RDPECLIP::FormatListPDUEx format_list_pdu;
        format_list_pdu.add_format_name(RDPECLIP::CF_TEXT);

        const bool use_long_format_names = current_use_long_format_names;
        const bool in_ASCII_8 = format_list_pdu.will_be_sent_in_ASCII_8(use_long_format_names);

        RDPECLIP::CliprdrHeader list_clipboard_header(RDPECLIP::CB_FORMAT_LIST,
            RDPECLIP::CB_RESPONSE__NONE_ | (in_ASCII_8 ? RDPECLIP::CB_ASCII_NAMES : 0),
            format_list_pdu.size(use_long_format_names));

        StaticOutStream<256> list_stream;

        list_clipboard_header.emit(list_stream);
        format_list_pdu.emit(list_stream, use_long_format_names);

        base_channel->send_message_to_server(
            list_stream.get_offset(),
            CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST | CHANNELS::CHANNEL_FLAG_SHOW_PROTOCOL,
            list_stream.get_data(),
            list_stream.get_offset());
    }

};

// TODO copy from /browser_client_JS/src/red_channels/clipboard.cpp
namespace Cliprdr
{
    REDEMPTION_DIAGNOSTIC_PUSH
    REDEMPTION_DIAGNOSTIC_CLANG_IGNORE("-Wgnu-string-literal-operator-template")
    // TODO string_literal<cs...>::view()
    template<class C, C... cs>
    constexpr std::array<uint8_t, sizeof...(cs) * 2> const operator "" _utf16()
    {
        std::array<uint8_t, sizeof...(cs) * 2> a{};
        char s[] {cs...};
        auto p = a.data();
        for (char c : s)
        {
            p[0] = c;
            p[1] = 0;
            p += 2;
        }
        return a;
    }
    REDEMPTION_DIAGNOSTIC_POP

    constexpr auto file_list_format_name = "FileGroupDescriptorW"_av;
    constexpr auto file_list_format_name_utf16 = "FileGroupDescriptorW"_utf16;

    enum class Charset : bool
    {
        Ascii,
        Utf16,
    };

    struct FormatListExtractorData
    {
        uint32_t format_id;
        Charset charset;
        cbytes_view av_name;
    };

    enum class IsLongFormat : bool;
    enum class IsAscii : bool;

    // formatId(4) + wszFormatName(variable, min = "\x00\x00" => 2)
    constexpr size_t min_long_format_name_data_length = 6;

    // formatId(4) + formatName(32)
    constexpr size_t short_format_name_length = 32;
    constexpr size_t short_format_name_data_length = short_format_name_length + 4;

    constexpr size_t format_name_data_length[] = {
        short_format_name_data_length,
        min_long_format_name_data_length
    };

    enum class ExtractResult : int8_t
    {
        Ok,
        WaitingData,
        LongFormatNameTooLong,
    };

    inline ExtractResult format_list_extract(
        FormatListExtractorData& data, InStream& stream,
        IsLongFormat is_long_format, IsAscii is_ascii) noexcept
    {
        if (stream.in_remain() < format_name_data_length[int(is_long_format)])
        {
            return ExtractResult::WaitingData;
        }

        data.format_id = stream.in_uint32_le();

        if (bool(is_long_format))
        {
            data.charset = Charset::Utf16;
            auto av = stream.remaining_bytes();
            data.av_name = av.first(UTF16ByteLen(av));

            if (data.av_name.size() == av.size())
            {
                return ExtractResult::LongFormatNameTooLong;
            }

            stream.in_skip_bytes(data.av_name.size() + 2);
        }
        else
        {
            auto av = stream.remaining_bytes();
            if (bool(is_ascii))
            {
                data.charset = Charset::Ascii;
                data.av_name = av.first(strnlen(av.as_charp(), short_format_name_length));
            }
            else
            {
                data.charset = Charset::Utf16;
                data.av_name = av.first(UTF16ByteLen(av.first(short_format_name_length)));
            }

            stream.in_skip_bytes(short_format_name_length);
        }

        return ExtractResult::Ok;
    }
}


struct FormatListReceive
{
    uint32_t file_list_format_id = 0;

    FormatListReceive(
        const bool use_long_format,
        const RDPECLIP::CliprdrHeader & in_header,
        InStream & chunk,
        std::unordered_map<uint32_t, std::string> & format_name_inventory,
        const RDPVerbose verbose)
    {
        LOG_IF(bool(verbose & RDPVerbose::cliprdr), LOG_INFO,
            "%s variant of Format List PDU is used for exchanging updated format names.",
            use_long_format
                ? "Long Format Name"
            : (in_header.msgFlags() & RDPECLIP::CB_ASCII_NAMES)
                ? "Short Format Name (ASCII 8)"
                : "Short Format Name"
        );

        Cliprdr::FormatListExtractorData extracted_data;

        auto buf = chunk.remaining_bytes();
        InStream in_stream(buf.first(std::min<size_t>(in_header.dataLen(), buf.size())));

        Cliprdr::ExtractResult r;

        while ((r = Cliprdr::format_list_extract(
            extracted_data, in_stream,
            Cliprdr::IsLongFormat(use_long_format),
            Cliprdr::IsAscii(in_header.msgFlags() & RDPECLIP::CB_ASCII_NAMES))
        ) == Cliprdr::ExtractResult::Ok)
        {
            const size_t max_length_of_format_name   = 256;
            constexpr size_t size_of_utf8_buffer
                = max_length_of_format_name * maximum_length_of_utf8_character_in_bytes;
            uint8_t utf8_buffer[size_of_utf8_buffer + 1];

            array_view_const_u8 utf8_av_name = (extracted_data.charset == Cliprdr::Charset::Ascii)
                ? extracted_data.av_name
                : ::UTF16toUTF8_buf(extracted_data.av_name, make_array_view(utf8_buffer));

            LOG_IF(bool(verbose & RDPVerbose::cliprdr), LOG_INFO,
                "formatId=%s(%u) wszFormatName=\"%.*s\"",
                RDPECLIP::get_FormatId_name(extracted_data.format_id),
                extracted_data.format_id, int(utf8_av_name.size()), utf8_av_name.data());

            format_name_inventory[extracted_data.format_id]
                .assign(::char_ptr_cast(utf8_av_name.data()), utf8_av_name.size());

            array_view_const_u8 ref_name = const_bytes_view(Cliprdr::file_list_format_name);
            // TODO operator==(array_view, array_view) ?
            if (ref_name.size() == utf8_av_name.size()
             && !memcmp(ref_name.data(), utf8_av_name.data(), utf8_av_name.size())
            ) {
                this->file_list_format_id = extracted_data.format_id;
            }
        }

        chunk.in_skip_bytes(in_stream.get_offset());

        if (r == Cliprdr::ExtractResult::LongFormatNameTooLong) {
            LOG(LOG_ERR, "Truncated CLIPRDR_FORMAT_LIST (Long) FormatName");
            throw Error(ERR_RDP_UNSUPPORTED);
        }

        if (chunk.in_remain()) {
            LOG(LOG_WARNING, "Truncated CLIPRDR_FORMAT_LIST remains=%zu", chunk.in_remain());
        }
    }
};

struct FormatListSendBack
{
    FormatListSendBack(VirtualChannelDataSender* sender)
    {
        if (!sender) {
            return ;
        }

        RDPECLIP::FormatListResponsePDU pdu;

        RDPECLIP::CliprdrHeader header(RDPECLIP::CB_FORMAT_LIST_RESPONSE, RDPECLIP::CB_RESPONSE_OK, pdu.size());

        StaticOutStream<256> out_stream;

        header.emit(out_stream);
        pdu.emit(out_stream);

        sender->operator()(
            out_stream.get_offset(),
            CHANNELS::CHANNEL_FLAG_FIRST | CHANNELS::CHANNEL_FLAG_LAST,
            out_stream.get_data(),
            out_stream.get_offset());
    }
};

struct LockClipDataReceive {

    LockClipDataReceive(ClipboardSideData & clip_receiver_side_data, ClipboardSideData & clip_sender_side_data, InStream & chunk, const RDPVerbose verbose, const RDPECLIP::CliprdrHeader & header) {
        if (header.dataLen() >= 4 /* clipDataId(4) */) {
            {
                const unsigned int expected = 4;
                if (!chunk.in_check_rem(expected)) {
                    LOG(LOG_ERR,
                        "ClipboardVirtualChannel::process_%s_message: "
                            "Truncated CLIPRDR_LOCK_CLIPDATA, "
                            "need=%u remains=%zu", clip_sender_side_data.provider_name,
                        expected, chunk.in_remain());
                    throw Error(ERR_RDP_DATA_TRUNCATED);
                }
            }

           clip_receiver_side_data.clipDataId  = chunk.in_uint32_le();

            LOG_IF(bool(verbose & RDPVerbose::cliprdr), LOG_INFO,
                "ClipboardVirtualChannel::process_%s_message: "
                    "clipDataId=%u", clip_sender_side_data.provider_name, clip_receiver_side_data.clipDataId);

            clip_receiver_side_data.file_stream_data_inventory[clip_receiver_side_data.clipDataId] = ClipboardSideData::file_info_inventory_type();
        }
    }
};

struct UnlockClipDataReceive {

    UnlockClipDataReceive(ClipboardSideData & clip_receiver_side_data, ClipboardSideData & clip_sender_side_data, InStream & chunk, const RDPVerbose verbose, const RDPECLIP::CliprdrHeader & header) {
         if (header.dataLen() >= 4 /* clipDataId(4) */) {
            {
                const unsigned int expected = 4;    // clipDataId(4)
                if (!chunk.in_check_rem(expected)) {
                    LOG(LOG_ERR,
                        "ClipboardVirtualChannel::process_%s_message: "
                            "Truncated CLIPRDR_UNLOCK_CLIPDATA, "
                            "need=%u remains=%zu",clip_sender_side_data.provider_name,
                        expected, chunk.in_remain());
                    throw Error(ERR_RDP_DATA_TRUNCATED);
                }
            }

            uint32_t const clipDataId = chunk.in_uint32_le();

            LOG_IF(bool(verbose & RDPVerbose::cliprdr), LOG_INFO,
                "ClipboardVirtualChannel::process_%s_message: "
                    "clipDataId=%u",clip_sender_side_data.provider_name, clipDataId);

            clip_receiver_side_data.file_stream_data_inventory.erase(clipDataId);
        }
    }
};

struct FileContentsResponseReceive {

    bool must_log_file_info_type = false;
    ClipboardSideData::file_info_type file_info;

    FileContentsResponseReceive(ClipboardSideData & clip_side_data, const RDPECLIP::CliprdrHeader & header, const uint32_t flags, InStream & chunk) {
        if (flags & CHANNELS::CHANNEL_FLAG_FIRST) {

            clip_side_data.dataLen = header.dataLen();

            if (clip_side_data.dataLen >= 4) {
                clip_side_data.streamId = chunk.in_uint32_le();
            }
        }

        if (clip_side_data.file_contents_request_info_inventory.end() !=
            clip_side_data.file_contents_request_info_inventory.find(clip_side_data.streamId))
        {
            ClipboardSideData::file_contents_request_info& file_contents_request =
                clip_side_data.file_contents_request_info_inventory[clip_side_data.streamId];

            {
                ClipboardSideData::file_info_inventory_type& file_info_inventory =
                    clip_side_data.file_stream_data_inventory[
                        file_contents_request.clipDataId];

                this->file_info = file_info_inventory[file_contents_request.lindex];

                uint64_t const file_contents_request_position_current = file_contents_request.position + file_contents_request.offset;

                if (chunk.in_remain()) {
                    if (this->file_info.sequential_access_offset == file_contents_request_position_current) {

                        uint32_t const length_ = std::min({
                                static_cast<uint32_t>(chunk.in_remain()),
                                static_cast<uint32_t>(this->file_info.size - this->file_info.sequential_access_offset),
                                file_contents_request.cbRequested - file_contents_request.offset
                            });

                        this->file_info.sha256.update({ chunk.get_current(), length_ });

                        file_contents_request.offset       += length_;
                        this->file_info.sequential_access_offset += length_;

                        this->must_log_file_info_type = this->file_info.sequential_access_offset == this->file_info.size;
                    }
                }
            }
        }
    }
};
