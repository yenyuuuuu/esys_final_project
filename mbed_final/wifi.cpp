#include "wifi.h"

#define WIFI_IDW0XX1    2

#if (defined(TARGET_DISCO_L475VG_IOT01A) || defined(TARGET_DISCO_F413ZH))
#include "ISM43362Interface.h"
extern ISM43362Interface wifi;

#else // External WiFi modules

#if MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1
#include "SpwfSAInterface.h"
extern SpwfSAInterface wifi;
#endif // MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1

#endif

#define SENT_SPACING_TIME 100
#define IP_address "192.168.10.100"
#define Port_number 3000
#define Sample_rate 2


WIFI::WIFI(NetworkInterface* wifi, events::EventQueue &event_queue, TCPSocket* socket)
        : _wifi(wifi), _event_queue(event_queue), _socket(NULL)
{ 
    
}

WIFI::~WIFI(){
    _socket->close(); 
    _wifi->disconnect();
}

void WIFI::connect(TCPSocket* socket){

    int ret;
    do {
        printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
        ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
        if (ret != 0) {
            printf("\nConnection error\n");
        }
    } while (ret != 0);

    printf("Success\n\n");
    printf("MAC: %s\n", wifi.get_mac_address());
    printf("IP: %s\n", wifi.get_ip_address());
    printf("Netmask: %s\n", wifi.get_netmask());
    printf("Gateway: %s\n", wifi.get_gateway());
    printf("RSSI: %d\n\n", wifi.get_rssi());

    nsapi_error_t response;

    SocketAddress addr;
    _wifi->get_ip_address(&addr);
    printf("IP address: %s\n", addr.get_ip_address() ? addr.get_ip_address() : "None");
    printf("Sending request to %s...\n", IP_address);

    do {
        _socket = new TCPSocket;
        _socket->open(_wifi);
        _wifi->gethostbyname(IP_address, &addr);
        addr.set_port(Port_number);
        response = _socket->connect(addr);

        if (NSAPI_ERROR_OK != response){
            printf("Error connecting: %d\n", response);
            _socket->close();
            delete _socket;
            This_thread::sleep_for(5000);
        }
    } while (NSAPI_ERROR_OK != response);
    socket = _socket;
    printf("WIFI connect() return\n");
}


void WIFI:: send_data(Sensor* sensor)
{
    char sbuffer[500] = "";
    nsapi_error_t response;
    nsapi_size_t size = strlen(sbuffer);


    int len = sprintf(sbuffer,"{\"Acc_x\":%.2f,\"Acc_y\":%.2f,\"Acc_z\":%.2f,\"Gyro_x\":%.2f,\"Gyro_y\":%.2f,\"Gyro_z\":%.2f,\"Item_front\":%d,\"Item_back\":%d,\"Acc\":%d}",sensor->_pAccDataXYZ[0],sensor->_pAccDataXYZ[1],sensor->_pAccDataXYZ[2],sensor->_pGyroDataXYZ[0],sensor->_pGyroDataXYZ[1],sensor->_pGyroDataXYZ[2],sensor->itemFront, sensor->itemBack, sensor->acc);
    printf("len: %d", len);

    response = _socket->send(sbuffer,len);
    if (len != response){
        printf("Error sending: %d\n", response);
    }
}

/*
const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}
*/
