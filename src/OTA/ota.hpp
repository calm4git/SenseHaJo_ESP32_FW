#include <string>

class ota {

    private:
        bool started=false;
        String hostname="";
        String password="";
    public:
    ota();
    ~ota();
    void setHostname(String hostname);
    void setPassword(String password);
    void loop( void );

};