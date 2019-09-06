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
  Copyright (C) Wallix 2013
  Author(s): Christophe Grosjean, Raphael Zhou, Meng Tan
*/


#pragma once

#include "core/RDP/nla/credssp.hpp"
#include "core/RDP/tpdu_buffer.hpp"
#include "utils/hexdump.hpp"
#include "utils/translation.hpp"
#include "system/ssl_sha256.hpp"
#include "system/ssl_md4.hpp"
#include "utils/difftimeval.hpp"

#include "core/RDP/nla/ntlm/ntlm_message.hpp"

#include "transport/transport.hpp"

#include <vector>


class rdpClientNTLM
{
    static constexpr uint32_t cbMaxSignature = 16;
private:
    int send_seq_num = 0;
    int recv_seq_num = 0;

    TSCredentials ts_credentials;

    ClientNonce SavedClientNonce;

    std::vector<uint8_t> PublicKey;
    std::vector<uint8_t> ClientServerHash;
    std::vector<uint8_t> identity_User;
    std::vector<uint8_t> identity_Domain;
    std::vector<uint8_t> identity_Password;

    TimeObj & timeobj;
    Random & rand;

    // NTLMContextClient
    const bool NTLMv2 = true;
    bool UseMIC = true; // like NTLMv2
    NtlmState sspi_context_state = NTLM_STATE_INITIAL;

    SslRC4 SendRc4Seal {};

    //int LmCompatibilityLevel;
    std::vector<uint8_t> Workstation;

    // bool SendSingleHostData;
    // NTLM_SINGLE_HOST_DATA SingleHostData;
//    NTLMAuthenticateMessage AUTHENTICATE_MESSAGE;

    NtlmVersion version;
    std::vector<uint8_t> SavedNegotiateMessage;
    std::vector<uint8_t> SavedChallengeMessage;
    std::vector<uint8_t> SavedAuthenticateMessage;

    array_md5 SessionBaseKey;
    array_md5 ExportedSessionKey;
    array_md5 EncryptedRandomSessionKey;
    array_md5 sspi_context_ClientSigningKey;
    array_md5 ClientSealingKey;
    array_md5 sspi_context_ServerSigningKey;
    array_md5 ServerSealingKey;
    // uint8_t NtProofStr[16];

    // GSS_Acquire_cred
    // ACQUIRE_CREDENTIALS_HANDLE_FN AcquireCredentialsHandle;
    // Inlined

    // GSS_Init_sec_context
    // INITIALIZE_SECURITY_CONTEXT_FN InitializeSecurityContext;

    // GSS_Wrap
    // ENCRYPT_MESSAGE EncryptMessage;

    // GSS_Unwrap
    // DECRYPT_MESSAGE DecryptMessage;
    bool restricted_admin_mode;

    const bool verbose;

    enum class Res : bool { Err, Ok };

    enum : uint8_t { Start, Loop, Final } client_auth_data_state = Start;

    static void sspi_compute_signature(uint8_t* signature, SslRC4& rc4, uint8_t* digest, uint32_t SeqNo)
    {
        uint8_t checksum[8];
        /* RC4-encrypt first 8 bytes of digest */
        rc4.crypt(8, digest, checksum);

        uint32_t version = 1;
        /* Concatenate version, ciphertext and sequence number to build signature */
        memcpy(signature, &version, 4);
        memcpy(&signature[4], checksum, 8);
        memcpy(&signature[12], &SeqNo, 4);
    }

public:
    rdpClientNTLM(uint8_t * user,
               uint8_t * domain,
               uint8_t * pass,
               uint8_t * hostname,
               const char * target_host,
               array_view_const_u8 public_key,
               const bool restricted_admin_mode,
               Random & rand,
               TimeObj & timeobj,
               const bool verbose = false)
        : PublicKey(public_key.data(), public_key.data()+public_key.size())
        , identity_User(::UTF8toUTF16({user,strlen(reinterpret_cast<char*>(user))}))
        , identity_Domain(::UTF8toUTF16({domain,strlen(reinterpret_cast<char*>(domain))}))
        , identity_Password(::UTF8toUTF16({pass,strlen(reinterpret_cast<char*>(pass))}))
        , timeobj(timeobj)
        , rand(rand)
        , Workstation(::UTF8toUTF16({hostname, strlen(char_ptr_cast(hostname))}))
        , restricted_admin_mode(restricted_admin_mode)
        , verbose(verbose)
    {
    }


    void client_authenticate_start(StaticOutStream<65536> & ts_request_emit)
    {
        LOG(LOG_INFO, "Credssp: NTLM Authentication");

        /* receive server response and place in input buffer */
        LOG_IF(this->verbose, LOG_INFO, "NTLM Send Negotiate");
        auto negoTokens = emitNTLMNegotiateMessage();

        LOG_IF(this->verbose, LOG_INFO, "rdpCredssp - Client Authentication : Sending Authentication Token");
        auto v = emitTSRequest(6, negoTokens, {}, {}, 0, {}, false);
        ts_request_emit.out_copy_bytes(v);

        this->SavedNegotiateMessage = std::move(negoTokens);
        this->sspi_context_state = NTLM_STATE_CHALLENGE;
        this->client_auth_data_state = Loop;
        return;
    }


    credssp::State client_authenticate_next(bytes_view in_data, StaticOutStream<65536> & ts_request_emit)
    {
        switch (this->client_auth_data_state)
        {
            case Loop:
            {
                LOG_IF(this->verbose, LOG_INFO, "Client Authentication : Receiving Authentication Token");
                TSRequest ts_request = recvTSRequest(in_data);
                
                // TODO: add error code management if server returns some error code at that point

                /*
                 * from tspkg.dll: 0x00000132
                 * ISC_REQ_MUTUAL_AUTH
                 * ISC_REQ_CONFIDENTIALITY
                 * ISC_REQ_USE_SESSION_KEY
                 * ISC_REQ_ALLOCATE_MEMORY
                 */
                //unsigned long const fContextReq
                //  = ISC_REQ_MUTUAL_AUTH | ISC_REQ_CONFIDENTIALITY | ISC_REQ_USE_SESSION_KEY;

                LOG_IF(this->verbose, LOG_INFO, "NTLMContextClient Read Challenge");
                NTLMChallengeMessage server_challenge = recvNTLMChallengeMessage(ts_request.negoTokens);
                this->SavedChallengeMessage = server_challenge.raw_bytes;

                this->sspi_context_state = NTLM_STATE_AUTHENTICATE;

                LOG_IF(this->verbose, LOG_INFO, "NTLMContextClient Compute response from challenge");

                // ntlmv2_compute_response_from_challenge generates :
                // - timestamp
                // - client challenge
                // - NtChallengeResponse
                // - LmChallengeResponse
                // all strings are in unicode utf16

                array_md4 md4password = ::Md4(this->identity_Password);
                auto userNameUppercase = ::UTF16_to_upper(this->identity_User);
                array_md5 ResponseKeyNT = ::HmacMd5(md4password,userNameUppercase, this->identity_Domain);

                array_md4 md4password_b = ::Md4(this->identity_Password);
                auto userNameUppercase_b = ::UTF16_to_upper(this->identity_User);
                array_md5 ResponseKeyLM = ::HmacMd5(md4password_b,userNameUppercase_b, this->identity_Domain);

                // NTLMv2_Client_Challenge = { 0x01, 0x01, Zero(6), Time, ClientChallenge, Zero(4), ServerName , Zero(4) }
                // Zero(n) = { 0x00, ... , 0x00 } n times
                // ServerName = AvPairs received in Challenge message

                std::vector<uint8_t> temp;
                for (auto x: {1, 1, 0, 0, 0, 0, 0, 0}){
                    temp.push_back(x);

                }

                LOG_IF(this->verbose, LOG_INFO, "NTLMContextClient TimeStamp");

                // compute ClientTimeStamp
                const timeval tv = this->timeobj.get_time();
                push_back_array(temp, out_uint32_le(tv.tv_usec));
                push_back_array(temp, out_uint32_le(tv.tv_sec));

                LOG_IF(this->verbose, LOG_INFO, "NTLMContextClient Generate Client Challenge nonce(8)");
                // ClientChallenge is used in computation of LMv2 and NTLMv2 responses */
                array_challenge ClientChallenge;
                this->rand.random(ClientChallenge.data(), 8);
                push_back_array(temp, {ClientChallenge.data(), 8});
                push_back_array(temp, out_uint32_le(0));
                push_back_array(temp, server_challenge.TargetInfo.buffer);
                push_back_array(temp, out_uint32_le(0));
                // NtProofStr = HMAC_MD5(NTOWFv2(password, user, userdomain), Concat(ServerChallenge, temp))

                LOG_IF(this->verbose, LOG_INFO, "NTLMContextClient Compute response: NtProofStr");

                array_challenge ServerChallenge = server_challenge.serverChallenge;
                array_md5 NtProofStr = ::HmacMd5(make_array_view(ResponseKeyNT),ServerChallenge,temp);

                // NtChallengeResponse = Concat(NtProofStr, temp)
                auto NtChallengeResponse = std::vector<uint8_t>{} << NtProofStr << temp;

                NTLMAuthenticateMessage AuthenticateMessage;
                AuthenticateMessage.NtChallengeResponse.buffer = NtChallengeResponse;

                LOG_IF(this->verbose, LOG_INFO, "Compute response: NtChallengeResponse Ready");

                // LmChallengeResponse.Response = HMAC_MD5(LMOWFv2(password, user, userdomain),
                //                                         Concat(ServerChallenge, ClientChallenge))
                // LmChallengeResponse.ChallengeFromClient = ClientChallenge
                LOG_IF(this->verbose, LOG_INFO, "NTLMContextClient Compute response: LmChallengeResponse");

                auto response = compute_LMv2_Response(ResponseKeyLM, ServerChallenge, ClientChallenge);
                AuthenticateMessage.LmChallengeResponse.buffer.assign(response.data(), response.data()+response.size());
                push_back_array(AuthenticateMessage.LmChallengeResponse.buffer, ClientChallenge);

                LOG_IF(this->verbose, LOG_INFO, "NTLMContextClient Compute response: SessionBaseKey");
                // SessionBaseKey = HMAC_MD5(NTOWFv2(password, user, userdomain), NtProofStr)
                this->SessionBaseKey = ::HmacMd5(make_array_view(ResponseKeyNT), NtProofStr);

                // EncryptedRandomSessionKey = RC4K(KeyExchangeKey, ExportedSessionKey)
                // ExportedSessionKey = NONCE(16) (random 16bytes number)
                // KeyExchangeKey = SessionBaseKey
                // EncryptedRandomSessionKey = RC4K(SessionBaseKey, NONCE(16))

                // generate NONCE(16) exportedsessionkey
                LOG_IF(this->verbose, LOG_INFO, "NTLMContextClient Encrypt RandomSessionKey");
                LOG_IF(this->verbose, LOG_INFO, "NTLMContextClient Generate Exported Session Key");
                this->rand.random(this->ExportedSessionKey.data(), SslMd5::DIGEST_LENGTH);
                this->EncryptedRandomSessionKey = ::Rc4Key(this->SessionBaseKey, this->ExportedSessionKey);

                auto AuthEncryptedRSK = std::vector<uint8_t>{} << this->EncryptedRandomSessionKey;

               
                AuthenticateMessage.EncryptedRandomSessionKey.buffer = AuthEncryptedRSK;

                // NTLM Signing Key @msdn{cc236711} and Sealing Key @msdn{cc236712}
                this->sspi_context_ClientSigningKey = ::Md5(this->ExportedSessionKey,
                        "session key to client-to-server signing key magic constant\0"_av);
                this->ClientSealingKey = ::Md5(this->ExportedSessionKey,
                        "session key to client-to-server sealing key magic constant\0"_av);
                this->sspi_context_ServerSigningKey = ::Md5(this->ExportedSessionKey,
                        "session key to server-to-client signing key magic constant\0"_av);
                this->ServerSealingKey = ::Md5(this->ExportedSessionKey,
                        "session key to server-to-client sealing key magic constant\0"_av);

                this->SendRc4Seal.set_key(this->ClientSealingKey);

                uint32_t flags = set_negotiate_flags(
                                    this->NTLMv2, 
                                    this->UseMIC, 
                                    this->Workstation.size() != 0, 
                                    server_challenge.negoFlags.flags & NTLMSSP_NEGOTIATE_KEY_EXCH);

                AuthenticateMessage.negoFlags.flags = flags;

                this->version.ProductMajorVersion = WINDOWS_MAJOR_VERSION_6;
                this->version.ProductMinorVersion = WINDOWS_MINOR_VERSION_1;
                this->version.ProductBuild        = 7601;
                this->version.NtlmRevisionCurrent = NTLMSSP_REVISION_W2K3;
                AuthenticateMessage.version = this->version;

                if (!(flags & NTLMSSP_NEGOTIATE_KEY_EXCH)) {
                    // If flag is not set, encryted session key buffer is not send
                    AuthenticateMessage.EncryptedRandomSessionKey.buffer.clear();
                }
                if (flags & NTLMSSP_NEGOTIATE_OEM_WORKSTATION_SUPPLIED) {
                    AuthenticateMessage.Workstation.buffer = this->Workstation;
                }

                //flag |= NTLMSSP_NEGOTIATE_OEM_DOMAIN_SUPPLIED;
                AuthenticateMessage.DomainName.buffer = this->identity_Domain;
                AuthenticateMessage.UserName.buffer = this->identity_User;

                StaticOutStream<65535> out_stream;
                if (this->UseMIC) {
                    emitNTLMAuthenticateMessage(out_stream, 
                        AuthenticateMessage.negoFlags.flags,
                        AuthenticateMessage.LmChallengeResponse.buffer,
                        AuthenticateMessage.NtChallengeResponse.buffer,
                        AuthenticateMessage.DomainName.buffer,
                        AuthenticateMessage.UserName.buffer,
                        AuthenticateMessage.Workstation.buffer,
                        AuthenticateMessage.EncryptedRandomSessionKey.buffer,
                        {AuthenticateMessage.MIC, 16},
                        AuthenticateMessage.has_mic,
                        true);

                    this->SavedAuthenticateMessage.assign(out_stream.get_bytes().data(),out_stream.get_bytes().data()+out_stream.get_offset());

                    array_md5 MessageIntegrityCheck = ::HmacMd5(this->ExportedSessionKey,
                                                            this->SavedNegotiateMessage,
                                                            this->SavedChallengeMessage,
                                                            this->SavedAuthenticateMessage);

                    memcpy(AuthenticateMessage.MIC, MessageIntegrityCheck.data(), MessageIntegrityCheck.size());
                }
                out_stream.rewind();
                emitNTLMAuthenticateMessage(out_stream, 
                    AuthenticateMessage.negoFlags.flags,
                    AuthenticateMessage.LmChallengeResponse.buffer,
                    AuthenticateMessage.NtChallengeResponse.buffer,
                    AuthenticateMessage.DomainName.buffer,
                    AuthenticateMessage.UserName.buffer,
                    AuthenticateMessage.Workstation.buffer,
                    AuthenticateMessage.EncryptedRandomSessionKey.buffer,
                    {AuthenticateMessage.MIC, 16},
                    AuthenticateMessage.has_mic,
                    false);
                
                auto out_stream_bytes = out_stream.get_bytes();

                TSRequest ts_request_anwer;
                std::vector<uint8_t> answer_negoTokens(out_stream_bytes.data(),out_stream_bytes.data()+out_stream_bytes.size());
                if (this->verbose) {
                    logNTLMAuthenticateMessage(AuthenticateMessage);
                }
                this->sspi_context_state = NTLM_STATE_FINAL;

                // have_pub_key_auth = true;

                LOG_IF(this->verbose, LOG_INFO, "rdpClientNTLM::encrypt_public_key_echo");
                uint32_t version = ts_request.use_version;


                array_view_u8 public_key = {this->PublicKey.data(),this->PublicKey.size()};
                if (version >= 5) {
                    LOG(LOG_INFO, "rdpClientNTLM::generate client nonce");
                    this->rand.random(this->SavedClientNonce.clientNonce.data(), CLIENT_NONCE_LENGTH);
                    this->SavedClientNonce.initialized = true;
                    
                    LOG(LOG_INFO, "rdpClientNTLM::generate public key hash (client->server)");
                    SslSha256 sha256;
                    uint8_t hash[SslSha256::DIGEST_LENGTH];
                    sha256.update("CredSSP Client-To-Server Binding Hash\0"_av);
                    sha256.update(this->SavedClientNonce.clientNonce);

                    sha256.update({this->PublicKey.data(),this->PublicKey.size()});
                    sha256.final(hash);
                    this->ClientServerHash.assign(hash, hash+sizeof(hash));
                    public_key = {this->ClientServerHash.data(), this->ClientServerHash.size()};
                }

                unsigned long MessageSeqNo = this->send_seq_num++;
                // pubKeyAuth [signature][data_buffer]
                std::vector<uint8_t> pubKeyAuth(public_key.size() + cbMaxSignature);
                std::array<uint8_t,4> seqno{uint8_t(MessageSeqNo),uint8_t(MessageSeqNo>>8),uint8_t(MessageSeqNo>>16),uint8_t(MessageSeqNo>>24)};
                array_md5 digest = ::HmacMd5(this->sspi_context_ClientSigningKey, seqno, public_key);
                this->SendRc4Seal.crypt(public_key.size(), public_key.data(), pubKeyAuth.data()+cbMaxSignature);
                this->sspi_compute_signature(pubKeyAuth.data(), this->SendRc4Seal, digest.data(), MessageSeqNo);

                /* send authentication token to server */
                if (answer_negoTokens.size() > 0){
                    LOG_IF(this->verbose, LOG_INFO, "Client Authentication : Sending Authentication Token");
                }

                LOG_IF(this->verbose, LOG_INFO, "rdpClientNTLM::send");
                auto v = emitTSRequest(ts_request_anwer.version,
                                       answer_negoTokens,
                                       {}, // authInfo,
                                       pubKeyAuth,
                                       0,
                                       this->SavedClientNonce.clientNonce,
                                       this->SavedClientNonce.initialized);
                ts_request_emit.out_copy_bytes(v);

                this->client_auth_data_state = Final;
                return credssp::State::Cont;
            }
            case Final:
            {
                /* Encrypted Public Key +1 */
                LOG_IF(this->verbose, LOG_INFO, "Client Authentication : Receiving Encrypted PubKey + 1");

                TSRequest ts_request = recvTSRequest(in_data);
                uint32_t error_code = ts_request.error_code;

                if (ts_request.pubKeyAuth.size() < cbMaxSignature) {
                    // Provided Password is probably incorrect
                    LOG(LOG_ERR, "DecryptMessage failure: SEC_E_INVALID_TOKEN");
                    // return SEC_E_INVALID_TOKEN;
                    LOG(LOG_ERR, "Could not verify public key echo!");
                    return credssp::State::Err;
                }

                /* Verify Server Public Key Echo */
                unsigned long MessageSeqNo = this->recv_seq_num++;

                // data_in [signature][data_buffer]

                array_view_const_u8 pubkeyAuth_payload = {ts_request.pubKeyAuth.data()+cbMaxSignature, ts_request.pubKeyAuth.size()-cbMaxSignature};
                array_view_const_u8 pubkeyAuth_signature = {ts_request.pubKeyAuth.data(),cbMaxSignature};

                SslRC4 RecvRc4Seal {};
                RecvRc4Seal.set_key(this->ServerSealingKey);
                // decrypt message using RC4
                auto pubkeyAuth_encrypted_payload = Rc4CryptVector(RecvRc4Seal, pubkeyAuth_payload);

                std::array<uint8_t,4> seqno{uint8_t(MessageSeqNo),uint8_t(MessageSeqNo>>8),uint8_t(MessageSeqNo>>16),uint8_t(MessageSeqNo>>24)};
                array_md5 digest = ::HmacMd5(this->sspi_context_ServerSigningKey, seqno, pubkeyAuth_encrypted_payload);
                /* RC4-encrypt first 8 bytes of digest (digest is 16 bytes long)*/
                auto checksum = Rc4Crypt<8>(RecvRc4Seal, digest);
                /* Concatenate version, ciphertext and sequence number to build signature */
                StaticOutStream<16> expected_signature;
                expected_signature.out_uint32_le(1); // version 1
                expected_signature.out_copy_bytes(checksum);
                expected_signature.out_uint32_le(MessageSeqNo);

                if (!are_buffer_equal(pubkeyAuth_signature, expected_signature.get_bytes())) {
                    LOG(LOG_ERR, "public key echo signature verification failed, something nasty is going on!");
                    LOG(LOG_ERR, "Expected Signature:"); hexdump_c(expected_signature.get_bytes());
                    LOG(LOG_ERR, "Actual Signature:"); hexdump_c(pubkeyAuth_signature);
                    return credssp::State::Err;
                }

                if (ts_request.use_version < 5) {
                    // if we are client and protocol is 2,3,4, then get the public key minus one
                    ::ap_integer_decrement_le(pubkeyAuth_encrypted_payload);
                    if (!are_buffer_equal(this->PublicKey, pubkeyAuth_encrypted_payload)){
                        LOG(LOG_ERR, "Server's public key echo signature verification failed");
                        LOG(LOG_ERR, "Expected Signature:"); hexdump_c(expected_signature.get_bytes());
                        LOG(LOG_ERR, "Actual Signature:"); hexdump_c(pubkeyAuth_signature);
                        return credssp::State::Err;
                    }
                }
                else {
                    auto hash = Sha256("CredSSP Server-To-Client Binding Hash\0"_av,
                           this->SavedClientNonce.clientNonce,
                           this->PublicKey);
                    if (!are_buffer_equal(hash, pubkeyAuth_encrypted_payload)){
                        LOG(LOG_ERR, "Server's public key echo signature verification failed");
                        LOG(LOG_ERR, "Expected Signature:"); hexdump_c(expected_signature.get_bytes());
                        LOG(LOG_ERR, "Actual Signature:"); hexdump_c(pubkeyAuth_signature);
                        return credssp::State::Err;
                    }
                }

                /* Send encrypted credentials */
                LOG_IF(this->verbose, LOG_INFO, "rdpClientNTLM::encrypt_ts_credentials");

                {
                    StaticOutStream<65536> ts_credentials_send;
                    std::vector<uint8_t> result;
                    if (this->ts_credentials.credType == 1){
                        if (this->restricted_admin_mode) {
                            LOG(LOG_INFO, "Restricted Admin Mode");
                            result = emitTSCredentialsPassword({},{},{});
                        }
                        else {
                            result = emitTSCredentialsPassword(this->identity_Domain,this->identity_User,this->identity_Password);
                        }
                    }
                    else {
                        // Card Reader Not Supported Yet
                        bytes_view pin;
                        bytes_view userHint;
                        bytes_view domainHint;
                        uint32_t keySpec = 0;
                        bytes_view cardName;
                        bytes_view readerName;
                        bytes_view containerName;
                        bytes_view cspName;
                        result = emitTSCredentialsSmartCard(pin,userHint,domainHint,keySpec,cardName,readerName,containerName, cspName);
                    }
                    
                    ts_credentials_send.out_copy_bytes(result);
                    array_view_const_u8 data_in = {ts_credentials_send.get_data(), ts_credentials_send.get_offset()};
                    unsigned long MessageSeqNo = this->send_seq_num++;

                    // authInfo [signature][data_buffer]
                    std::vector<uint8_t> authInfo(data_in.size() + cbMaxSignature, 0);
                    std::array<uint8_t,4> seqno{uint8_t(MessageSeqNo),uint8_t(MessageSeqNo>>8),uint8_t(MessageSeqNo>>16),uint8_t(MessageSeqNo>>24)};
                    array_md5 digest = ::HmacMd5(this->sspi_context_ClientSigningKey, seqno, data_in);

                    this->SendRc4Seal.crypt(data_in.size(), data_in.data(), authInfo.data()+cbMaxSignature);
                    this->sspi_compute_signature(authInfo.data(), this->SendRc4Seal, digest.data(), MessageSeqNo);

                    auto v = emitTSRequest(ts_request.version,
                                           {}, // negoTokens
                                           authInfo,
                                           {}, // pubKeyAuth
                                           error_code,
                                           this->SavedClientNonce.clientNonce,
                                           this->SavedClientNonce.initialized);
                    ts_request_emit.out_copy_bytes(v);

                }
                this->client_auth_data_state = Start;
                return credssp::State::Finish;
            }
            default:
                return credssp::State::Err;
        }
        return credssp::State::Err;
    }

    uint32_t set_negotiate_flags(bool ntlmv2, bool use_mic, bool send_workstation_name, bool negotiate_key_exchange)
    {
        /*
         * from tspkg.dll: 0x00000132
         * ISC_REQ_MUTUAL_AUTH
         * ISC_REQ_CONFIDENTIALITY
         * ISC_REQ_USE_SESSION_KEY
         * ISC_REQ_ALLOCATE_MEMORY
         */
        //unsigned long const fContextReq
        //  = ISC_REQ_MUTUAL_AUTH | ISC_REQ_CONFIDENTIALITY | ISC_REQ_USE_SESSION_KEY;

        uint32_t flags = 0;
        if (ntlmv2) {
            flags |= NTLMSSP_NEGOTIATE_56;
        }

        if (use_mic) {
            flags |= NTLMSSP_NEGOTIATE_TARGET_INFO;
        }
        if (send_workstation_name) {
            flags |= NTLMSSP_NEGOTIATE_OEM_WORKSTATION_SUPPLIED;
        }
        flags |= NTLMSSP_NEGOTIATE_SEAL;
        if (negotiate_key_exchange) {
            flags |= NTLMSSP_NEGOTIATE_KEY_EXCH;
        }
        flags |= (NTLMSSP_NEGOTIATE_128
              | NTLMSSP_NEGOTIATE_EXTENDED_SESSION_SECURITY
              | NTLMSSP_NEGOTIATE_ALWAYS_SIGN
              | NTLMSSP_NEGOTIATE_NTLM
              | NTLMSSP_NEGOTIATE_SIGN
              | NTLMSSP_REQUEST_TARGET
              | NTLMSSP_NEGOTIATE_UNICODE
              | NTLMSSP_NEGOTIATE_VERSION);
        return flags;
    }
};

