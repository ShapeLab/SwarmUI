#include "ofxLibwebsockets/Util.h"

namespace ofxLibwebsockets {

	int lws_client_callback(struct libwebsocket_context* context, struct libwebsocket *ws, enum libwebsocket_callback_reasons reason, void *user, void *data, size_t len){
		const struct libwebsocket_protocols* lws_protocol = (ws == NULL ? NULL : libwebsockets_get_protocol(ws));
		int idx = lws_protocol? lws_protocol->protocol_index : 0;

		Connection* conn;

		Reactor* reactor = NULL;
		Protocol* protocol;

		for (int i=0; i<(int)reactors.size(); i++){
			if (reactors[i]->getContext() == context){
				reactor =  reactors[i];
				protocol = reactor->protocol(idx);
				conn = ((Client*) reactor)->getConnection();
				if (conn){
					conn->context = context;
				}
				break;
			} else {
			}
		}

		ofLog( OF_LOG_VERBOSE, "[ofxLibwebsockets] " + getCallbackReason(reason) );

		if (reason == LWS_CALLBACK_CLIENT_ESTABLISHED ){
			libwebsocket_callback_on_writable(context, ws);
		} else if (reason == LWS_CALLBACK_CLOSED){
		}

		switch (reason)
		{
			// cases we may use in the future
			case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY:
			case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
			case LWS_CALLBACK_PROTOCOL_INIT: // this may be useful, says we're OK to allocate protocol data
			case LWS_CALLBACK_WSI_CREATE:

			case LWS_CALLBACK_HTTP_BODY_COMPLETION:
			case LWS_CALLBACK_HTTP_FILE_COMPLETION:
			case LWS_CALLBACK_HTTP_WRITEABLE:

			case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
				return 0;

			// check if we allow this connection (default is always yes)
			case LWS_CALLBACK_FILTER_HTTP_CONNECTION:
			case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
				if (protocol != NULL){
					return reactor->_allow(ws, protocol, (int)(long)user)? 0 : 1;
				} else {
					return 0;
				}

			// need to serve up an HTTP file
			case LWS_CALLBACK_HTTP:
				if ( reactor != NULL){
					return reactor->_http(ws, (char*)data);
				} else {
					return 0;
				}

			// we're not really worried about these at the moment
			case LWS_CALLBACK_CLOSED_HTTP:
			case LWS_CALLBACK_ADD_POLL_FD:
			case LWS_CALLBACK_DEL_POLL_FD:
			case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			case LWS_CALLBACK_LOCK_POLL:
			case LWS_CALLBACK_UNLOCK_POLL:
			case LWS_CALLBACK_GET_THREAD_ID:
			case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
			case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
				return 1;

			// catch-all for most important events:
			// LWS_CALLBACK_CLIENT_CONNECTION_ERROR
			// LWS_CALLBACK_CLIENT_ESTABLISHED
			// LWS_CALLBACK_RECEIVE
			// LWS_CALLBACK_CLIENT_RECEIVE
			// LWS_CALLBACK_CLIENT_RECEIVE_PONG
			// LWS_CALLBACK_CLIENT_WRITEABLE
			default:
				if ( reactor != NULL ){
					//conn = *(Connection**)user;
					if (conn && conn->ws != ws && ws != NULL){
						conn->ws = ws;
						conn->context = context;
					}
					return reactor->_notify(conn, reason, (char*)data, len);
				} else {
					return 0;
				}
		}

		return 1; // FAIL (e.g. unhandled case/break in switch)
	}

	int lws_callback(struct libwebsocket_context* context, struct libwebsocket *ws,
								enum libwebsocket_callback_reasons reason, void *user, void *data, size_t len)
	{
		const struct libwebsocket_protocols* lws_protocol = (ws == NULL ? NULL : libwebsockets_get_protocol(ws));
		int idx = lws_protocol? lws_protocol->protocol_index : 0;

		// valid connection w/o a protocol
		if ( ws != NULL && lws_protocol == NULL ){
			// OK for now, returning 0 above
		}

		//bool bAllowAllProtocls = (ws != NULL ? lws_protocol == NULL : false);

		Connection* conn;
		Connection** conn_ptr = (Connection**)user;
		Server* reactor = NULL;
		Protocol* protocol = NULL;

		for (int i=0; i<(int)reactors.size(); i++){
			if (reactors[i]->getContext() == context){
				reactor = (Server*) reactors[i];
				protocol = reactor->protocol( (idx > 0 ? idx : 0) );
				break;
			} else {
			}
		}

		ofLog( OF_LOG_VERBOSE, "[ofxLibwebsockets] "+ getCallbackReason(reason) );

		if (reason == LWS_CALLBACK_ESTABLISHED){
			// server completed handshake, need to ask for next "writable" callback
			libwebsocket_callback_on_writable(context, ws);

			// now is when you can set the "user" data,
			// which we use to instantiate / refer to the ofxLibwebsockets::Connection
			if ( reactor != NULL ){
				*conn_ptr = new Connection(reactor, protocol);
			}
		} else if (reason == LWS_CALLBACK_CLOSED){
			//if (*conn_ptr != NULL)
			//delete *conn_ptr;
		}

		switch (reason)
		{
			// we may use these in the future!
			case LWS_CALLBACK_WSI_CREATE:
			case LWS_CALLBACK_WSI_DESTROY:

			case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:

			case LWS_CALLBACK_HTTP_BODY_COMPLETION:
			case LWS_CALLBACK_HTTP_FILE_COMPLETION:
			case LWS_CALLBACK_HTTP_WRITEABLE:
			case LWS_CALLBACK_CLOSED_HTTP:

			case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:

			case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
			case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
				return 0;

			case LWS_CALLBACK_FILTER_HTTP_CONNECTION:
				if (protocol != NULL ){
					// return 0 == allow, 1 == block
					return reactor->_allow(ws, protocol, (int)(long)data) ? 0 : 1;
				} else {
					return 0;
				}

			case LWS_CALLBACK_PROTOCOL_INIT:
				return 0;

			case LWS_CALLBACK_HTTP:
				return reactor->_http(ws, (char*)data);

				// we're not really worried about this at the moment
			case LWS_CALLBACK_ADD_POLL_FD:
			case LWS_CALLBACK_DEL_POLL_FD:
			case LWS_CALLBACK_LOCK_POLL:
			case LWS_CALLBACK_UNLOCK_POLL:
			case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			case LWS_CALLBACK_PROTOCOL_DESTROY:
			case LWS_CALLBACK_GET_THREAD_ID:
			case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
				return 0;

			case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			case LWS_CALLBACK_ESTABLISHED:
			case LWS_CALLBACK_CLIENT_ESTABLISHED:
			case LWS_CALLBACK_CLOSED:
			case LWS_CALLBACK_SERVER_WRITEABLE:
			case LWS_CALLBACK_CLIENT_WRITEABLE:
			case LWS_CALLBACK_RECEIVE:              // server receive
			case LWS_CALLBACK_CLIENT_RECEIVE:       // client receive
			case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
				if ( user != NULL ){
					conn = *(Connection**)user;
				}
				if (conn != NULL && (conn->ws != ws || conn->ws == NULL) ){
					conn->context = context;
					conn->ws = ws;
					conn->setupAddress();
				}
				if (reactor){
					return reactor->_notify(conn, reason, (char*)data, len);
				} else {
					return 0;
				}

			default:
				break;
		}

		return 0;
	}

	void dump_handshake_info(struct lws_tokens *lwst)
	{
		int n;
		static const char *token_names[WSI_TOKEN_COUNT] = {
			"GET URI",
			"Host",
			"Connection",
			"key 1",
			"key 2",
			"Protocol",
			"Upgrade",
			"Origin",
			"Draft",
			"Challenge",

			/* new for 04 */
			"Key",
			"Version",
			"Sworigin",

			/* new for 05 */
			"Extensions",

			/* client receives these */
			"Accept",
			"Nonce",
			"Http",
			"MuxURL",
		};

		for (n = 0; n < WSI_TOKEN_COUNT; n++) {
			if (lwst[n].token == NULL)
				continue;

			fprintf(stderr, "    %s = %s\n", token_names[n], lwst[n].token);
		}
	}

	string getCallbackReason( int reason ){
		switch (reason){
			case LWS_CALLBACK_ESTABLISHED : return "LWS_CALLBACK_ESTABLISHED";
			case LWS_CALLBACK_CLIENT_CONNECTION_ERROR : return "LWS_CALLBACK_CLIENT_CONNECTION_ERROR";
			case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH : return "LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH";
			case LWS_CALLBACK_CLIENT_ESTABLISHED : return "LWS_CALLBACK_CLIENT_ESTABLISHED";
			case LWS_CALLBACK_CLOSED : return "LWS_CALLBACK_CLOSED";
			case LWS_CALLBACK_CLOSED_HTTP : return "LWS_CALLBACK_CLOSED_HTTP";
			case LWS_CALLBACK_RECEIVE : return "LWS_CALLBACK_RECEIVE";
			case LWS_CALLBACK_CLIENT_RECEIVE : return "LWS_CALLBACK_CLIENT_RECEIVE";
			case LWS_CALLBACK_CLIENT_RECEIVE_PONG : return "LWS_CALLBACK_CLIENT_RECEIVE_PONG";
			case LWS_CALLBACK_CLIENT_WRITEABLE : return "LWS_CALLBACK_CLIENT_WRITEABLE";
			case LWS_CALLBACK_SERVER_WRITEABLE : return "LWS_CALLBACK_SERVER_WRITEABLE";
			case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED: return "LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED";

			case LWS_CALLBACK_HTTP : return "LWS_CALLBACK_HTTP";
			case LWS_CALLBACK_HTTP_BODY: return "LWS_CALLBACK_HTTP_BODY";
			case LWS_CALLBACK_HTTP_BODY_COMPLETION: return "LWS_CALLBACK_HTTP_BODY_COMPLETION";
			case LWS_CALLBACK_HTTP_FILE_COMPLETION : return "LWS_CALLBACK_HTTP_FILE_COMPLETION";
			case LWS_CALLBACK_HTTP_WRITEABLE : return "LWS_CALLBACK_HTTP_WRITEABLE";
			case LWS_CALLBACK_FILTER_NETWORK_CONNECTION : return "LWS_CALLBACK_FILTER_NETWORK_CONNECTION";
			case LWS_CALLBACK_FILTER_HTTP_CONNECTION: return "LWS_CALLBACK_FILTER_HTTP_CONNECTION";
			case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION : return "LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION";
			case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS : return "LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS";
			case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS : return "LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS";
			case LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION : return "LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION";
			case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER : return "LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER";
			case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY : return "LWS_CALLBACK_CONFIRM_EXTENSION_OKAY";

			case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED : return "LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED";
			case LWS_CALLBACK_PROTOCOL_INIT : return "LWS_CALLBACK_PROTOCOL_INIT";
			case LWS_CALLBACK_PROTOCOL_DESTROY : return "LWS_CALLBACK_PROTOCOL_DESTROY";
			case LWS_CALLBACK_WSI_CREATE: return "LWS_CALLBACK_WSI_CREATE";
			case LWS_CALLBACK_WSI_DESTROY: return "LWS_CALLBACK_WSI_DESTROY";
			case LWS_CALLBACK_GET_THREAD_ID: return "LWS_CALLBACK_GET_THREAD_ID";

			case LWS_CALLBACK_ADD_POLL_FD : return "LWS_CALLBACK_ADD_POLL_FD";
			case LWS_CALLBACK_DEL_POLL_FD : return "LWS_CALLBACK_DEL_POLL_FD";
			case LWS_CALLBACK_CHANGE_MODE_POLL_FD : return "LWS_CALLBACK_CHANGE_MODE_POLL_FD";
			case LWS_CALLBACK_LOCK_POLL: return "LWS_CALLBACK_LOCK_POLL";
			case LWS_CALLBACK_UNLOCK_POLL: return "LWS_CALLBACK_UNLOCK_POLL";

			case LWS_CALLBACK_USER: return "LWS_CALLBACK_USER";

			default:
				std::stringstream r;
				r << "Unknown callback reason id: " << reason;
				return r.str();
		}
	}
}
