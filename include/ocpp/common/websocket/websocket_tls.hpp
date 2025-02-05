// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_TLS_HPP
#define OCPP_WEBSOCKET_TLS_HPP

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include <ocpp/common/websocket/websocket_base.hpp>

namespace ocpp {

typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> tls_context;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

///
/// \brief contains a websocket abstraction that can connect to TLS and non-TLS websocket endpoints
///
class WebsocketTLS : public WebsocketBase {
private:
    tls_client wss_client;
    std::shared_ptr<PkiHandler> pki_handler;
    
    /// \brief Extracts the hostname from the provided \p uri
    /// FIXME(kai): this only works with a very limited subset of hostnames and should be extended to work spec conform
    /// \returns the extracted hostname
    std::string get_hostname(std::string uri);

    /// \brief Called when a TLS websocket connection gets initialized, manages the supported TLS versions, cipher lists
    /// and how verification of the server certificate is handled
    tls_context on_tls_init(std::string hostname, websocketpp::connection_hdl hdl, int32_t security_profile);

    /// \brief Connect to a TLS websocket, if the provided \p authorization_header is not empty use it to add a HTTP
    /// Authorization header
    void connect_tls(int32_t security_profile, bool try_once);

    /// \brief Called when a TLS websocket connection is established, calls the connected callback
    void on_open_tls(tls_client* c, websocketpp::connection_hdl hdl, int32_t security_profile);

    /// \brief Called when a message is received over the TLS websocket, calls the message callback
    void on_message_tls(websocketpp::connection_hdl hdl, tls_client::message_ptr msg);

    /// \brief Called when a TLS websocket connection is closed, tries to reconnect
    void on_close_tls(tls_client* c, websocketpp::connection_hdl hdl);

    /// \brief Called when a TLS websocket connection fails to be established, tries to reconnect
    void on_fail_tls(tls_client* c, websocketpp::connection_hdl hdl, bool try_once);

public:
    /// \brief Creates a new Websocket object with the providede \p connection_options
    explicit WebsocketTLS(const WebsocketConnectionOptions& connection_options,
                          std::shared_ptr<PkiHandler> pki_handler);

    /// \brief connect to a TLS websocket
    /// \returns true if the websocket is initialized and a connection attempt is made
    bool connect(int32_t security_profile, bool try_once) override;

    /// \brief Reconnects the websocket using the delay, a reason for this reconnect can be provided with the
    /// \p reason parameter
    void reconnect(std::error_code reason, long delay);

    /// \brief closes the websocket
    void close(websocketpp::close::status::value code, const std::string& reason) override;

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message) override;

    /// \brief send a websocket ping
    void ping() override;
};

} // namespace ocpp
#endif // OCPP_WEBSOCKET_HPP
