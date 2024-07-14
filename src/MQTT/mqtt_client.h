#ifndef __MQTT_CLIENT_H__
    #define __MQTT_CLIENT_H__


#include <stdio.h>
#include <functional>
#include <vector>
#include <WiFiClient.h>

class mqtt_client {

    public:
        typedef struct {
            const char* topic;
            const uint8_t* data;
            uint32_t datalen;
        } mqtt_data_t;

        using OnConnectCB = std::function<void(void)>;
        using OnDisconnectCB = std::function<void(void)>;
        using OnNewMessageCB = std::function<void(mqtt_data_t)>;
        using OnErrorCB = std::function<void(void)>;
        void OnConnected(OnConnectCB cb);
        void OnNewMessage(std::string const& topic, OnNewMessageCB cb);
        void OnDisconnect(OnDisconnectCB cb);
        void OnError(OnErrorCB cb);
        
        mqtt_client();  
        ~mqtt_client();

        void loop( void );
        void set_clientid(std::string id);
        bool connect ( std::string const& host , uint16_t port, bool use_mqtts=false);
        bool disconnect( void );
        bool publish(mqtt_data_t data);

        void setcredentials(std::string user, std::string password);

    private:

    std::vector<OnConnectCB> OnConnectEvent;        
    std::vector<OnDisconnectCB> OnDisconnectEvent;
    std::vector<OnNewMessageCB> OnMessageEvent;
    std::vector<OnErrorCB> OnErrorEvent;
    std::vector<std::string> Topics;

    void callback(char* topic, byte* payload, unsigned int length);

    std::string host="127.0.0.1";
    std::uint16_t port=1883;
    bool use_mqtts=false;
    std::string _id="";
    std::string user="";
    std::string password="";
    bool connected=false;
};

#endif