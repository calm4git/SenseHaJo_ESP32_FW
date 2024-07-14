#include "PubSubClient.h"
#include "mqtt_client.h"

WiFiClient netclient;
PubSubClient client(netclient);

mqtt_client::mqtt_client(){
    /* we setup some parameter */
    this->OnConnectEvent.clear();
    this->OnDisconnectEvent.clear();
    this->OnErrorEvent.clear();
    this->OnMessageEvent.clear();
    //
    std::string host="";
    std::uint16_t port=0;
    bool use_mqtts=false;
    client.setCallback(std::bind(&mqtt_client::callback, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
}
mqtt_client::~mqtt_client(){
    //We should do a clean up here...

}

void mqtt_client::loop( void ){
    client.loop();
    if(false==this->connected){
        if(true==client.connected()){
            this->connected=true;
            /* Call OnConnected callbacks from here */
            for (auto Event : OnConnectEvent){
                if(Event != NULL){
                    Event();
                }
            }
        }   
    } else {
        if(false==client.connected()){
            this->connected=false;
            for (auto Event : OnDisconnectEvent){
                if(Event != NULL){
                    Event();
                }
            }
        }
    }

}

void mqtt_client::setcredentials(std::string user, std::string password){
    this->user = user;
    this->password = password;
}

bool mqtt_client::connect ( std::string const& host , uint16_t port, bool use_mqtts){
    this->host = host;
    this->port = port;
    this->use_mqtts = use_mqtts;

    if(true == client.connected()){
        client.disconnect();
        while(true == client.connected()){
            client.loop();
        }
    }
    Serial.printf("host %s, port: %i", host.c_str(), port);
    client.setServer(host.c_str(),port);
    // currently no mqtts supported
    if(this->user != ""){
        client.connect(this->_id.c_str(), this->user.c_str(), this->password.c_str());
    } else {
        client.connect("test");
    }

    return true;
}

bool mqtt_client::disconnect( void ){
    if(true==client.connected()){
        client.disconnect();
        while(true==client.connected()){
           this->loop();
        }
    }

    return true;
}

bool mqtt_client::publish(mqtt_data_t data){
    bool done = false;
    if(true == client.connected()){
        client.publish(data.topic, data.data, data.datalen);
        done = true;
    }
    return done;
}

void mqtt_client::OnConnected(OnConnectCB cb){
    this->OnConnectEvent.push_back(cb);
}

void mqtt_client::OnNewMessage(std::string const& topic, OnNewMessageCB cb){
    this->Topics.push_back(topic);
    this->OnMessageEvent.push_back(cb);
    if(true == client.connected()){
        client.subscribe(topic.c_str());
    }

}

void mqtt_client::OnDisconnect(OnDisconnectCB cb){
    this->OnConnectEvent.push_back(cb);
}

void mqtt_client::OnError(OnErrorCB cb){
    this->OnErrorEvent.push_back(cb);
}

/* callback from client library */
void mqtt_client::callback(char* topic, byte* payload, unsigned int length) {
    mqtt_data_t message = { .topic=topic, .data=payload,.datalen=length};
    for (auto Event : OnMessageEvent){
        if(Event != NULL){
            Event(message);
        }
    }
}





/*

Server: MQTT, e.g mosquitto

Clients: ESP32, e.g. PubSubclient 

Topics Tree : 

Coffeemaker
 -> SenesEhajo
        -> A0CD3243
            -> Status
            -> Waterleve
            -> Settings

*/