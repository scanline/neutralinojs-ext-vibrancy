#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/uri.hpp>
#include <fstream>
#include <stdio.h>
#include "lib/json/json.hpp"
#include "uuid.h"

#if _WIN32
#include "vibrancyWindows.h"
#elif __linux__

#elif __APPLE__

#endif // _WIN32

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

using namespace std;
using json = nlohmann::json;

client *websocketclient;

int main()
{
    cout << "Waiting for stdin..." << endl;
    string stdInput = "";
    cin >> stdInput;
    json data = json::parse(stdInput);

    string idExtension = data["nlExtensionId"];
    string port = data["nlPort"];
    string token = data["nlToken"];
    string connectToken = data["nlConnectToken"];

    try
    {
        websocketclient = new client();
        websocketclient->set_open_handler([&](websocketpp::connection_hdl handler)
        {
		  
        });

        websocketclient->set_fail_handler([&](websocketpp::connection_hdl handler)
        {
            exit(0);
        });

        websocketclient->set_message_handler([&](websocketpp::connection_hdl handler, client::message_ptr msg)
        {
            json nativeMessage = json::parse(msg->get_payload());

            if (nativeMessage.contains("event"))
            {
                string event=nativeMessage["event"].get<string>();
                if(event=="extClientConnect")
                {

                }
                else if(event=="eventToExtension")
                {

                }
                else if(event=="clientConnect")
                {
                    json message;
                    message["id"] = getUUID();
                    message["method"] = "app.broadcast";
                    message["accessToken"] = token;
                    message["data"]["event"] = "eventFromExtension";
                    message["data"]["data"] = "Extension connected!";
                    websocketclient->send(handler,message.dump(),websocketpp::frame::opcode::text);
                }
                else if(event=="vibrancy")
                {
                    WindowEffects::vibrancy(nativeMessage["data"]["activate"], nativeMessage["data"]["pid"]);
                }
            }

        });

        websocketclient->set_access_channels(websocketpp::log::alevel::all);
        websocketclient->clear_access_channels(websocketpp::log::alevel::frame_payload);
        websocketclient->set_error_channels(websocketpp::log::elevel::all);
        websocketclient->init_asio();
        websocketpp::lib::error_code ec;

        websocketpp::uri_ptr location = websocketpp::lib::make_shared<websocketpp::uri>(false, "localhost", port, "?extensionId=" + idExtension + "&connectToken=" + connectToken);
        client::connection_ptr con = websocketclient->get_connection(location, ec);

        websocketclient->connect(con);
        websocketclient->run();
    }
    catch (const std::exception & e)
    {
        cout << e.what() << endl;
    }
    catch (websocketpp::lib::error_code e)
    {
        cout << e.message() << endl;
    }
    catch (...)
    {
        cout << "other exception" << endl;
    }
    return 0;
}
