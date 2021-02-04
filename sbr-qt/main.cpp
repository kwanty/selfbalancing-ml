#include <QCoreApplication>
#include <QSerialPort>
#include <iostream>
#include <SBRCP.h>
#include <QUdpSocket>
#include <QNetworkDatagram>


//#define _MODE_WIFI //WiFi mode using UDP and ESP32 module
//#define _MODE_BLUETOOTH //Bluetooth via Serial or just standard wired Serial (115200 baud)

#define _ROBOT_IP "192.168.4.1"
#define _LOCAL_IP "192.168.4.2"
#define _DEST_PORT 1235
#define _LOCAL_PORT 1234

#define _SERIAL_PORT "ttyUSB0"

void parseRxPacket(SBRCP_data_t *d);

SBRCP protocol(&parseRxPacket);
QUdpSocket sock;
QSerialPort port;

char data[50];
uint8_t dlen = 0;

//handles udp "interrupt"
void receiveDataUDP(void)
{
    while(sock.hasPendingDatagrams())
    {
        QNetworkDatagram d = sock.receiveDatagram();
        protocol.parseRx((uint8_t*)d.data().data(), d.data().size());
    }
}

//handles serial "interrupt"
void receiveDataSerial(void)
{
    static char data[50];
    static uint8_t dataLen = 0;
    dataLen += port.read(&data[dataLen], sizeof(data) - dataLen);
    if((dataLen > 1) && (data[dataLen - 1] == '\r') && (data[dataLen - 2] == '\n')) //wait for \n\r at the end, data may not always come in one piece
    {
        protocol.parseRx((uint8_t*)data, dataLen);
        dataLen = 0;
    }
    if(dataLen == sizeof(data))
        dataLen = 0;
}

//converts 4 bytes (little endian) into a float type variable
float bytesToFloat(uint8_t *data)
{
    uint32_t tmp = 0; //temporary variable for type conversion
    tmp |= *data; //copy data to the buffer
    tmp |= *(data + 1) << 8;
    tmp |= *(data + 2) << 16;
    tmp |= *(data + 3) << 24;
    float ret = 0;
    memcpy(&ret, &tmp, 4);
    return ret;
}

//displays received packet
void parseRxPacket(SBRCP_data_t *d)
{
    if(d->type == DATA_MPU)
    {
        std::cout << std::endl << "MPU data received" << std::endl;
        std::cout << "Accelerometer: X=" << bytesToFloat(&(d->payload[0]))
                << " Y=" << bytesToFloat(&(d->payload[4])) << " Z=" << bytesToFloat(&(d->payload[8])) << std::endl;
        std::cout << "Gyroscope: X=" << bytesToFloat(&(d->payload[12]))
                << " Y=" << bytesToFloat(&(d->payload[16])) << " Z=" << bytesToFloat(&(d->payload[20])) << std::endl;
    }
    else if(d->type == DATA_ERROR)
    {
        std::cout << std::endl << "Error packet received!" << std::endl;
    }
}


//MPU rate in microseconds
void setMPUrate(uint32_t rate)
{
    SBRCP_data_t d;
    d.type = DATA_CMD_RATE;
    d.payload[0] = rate & 0xFF;
    d.payload[1] = (rate & 0xFF00) >> 8;
    d.payload[2] = (rate & 0xFF0000) >> 16;
    d.payload[3] = (rate & 0xFF000000) >> 24;
    d.size = 4;
    uint8_t buf[20];
    uint8_t len = 0;
    protocol.parseTx(&d, buf, &len);
#ifdef _MODE_WIFI
    sock.writeDatagram((char*)buf, len, QHostAddress(_ROBOT_IP), _DEST_PORT);
#else
    port.write((char*)buf, len);
#endif
    std::cout << "Setting MPU rate" << std::endl;
}

//sets motors
//m1, m2 speeds in range -255 to 255. 0 stops the motor
void setMotors(int16_t m1, int16_t m2)
{
    SBRCP_data_t d;
    d.type = DATA_CMD_MOTORS;
    d.payload[0] = m1 & 0xFF;
    d.payload[1] = (m1 & 0xFF00) >> 8;
    d.payload[2] = (m2 & 0xFF);
    d.payload[3] = (m2 & 0xFF00) >> 8;
    d.size = 4;
    uint8_t buf[20];
    uint8_t len = 0;
    protocol.parseTx(&d, buf, &len);

#ifdef _MODE_WIFI
    sock.writeDatagram((char*)buf, len, QHostAddress(_ROBOT_IP), _DEST_PORT);
#else
    port.write((char*)buf, len);
#endif
    std::cout << "Setting motors" << std::endl;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef _MODE_WIFI
    sock.bind(QHostAddress(_LOCAL_IP), _LOCAL_PORT);
    QObject::connect(&sock, &QUdpSocket::readyRead, receiveDataUDP);
#else
    QObject::connect(&port, &QSerialPort::readyRead, receiveDataSerial);
    port.setPortName(_SERIAL_PORT); //serial port name
    port.setBaudRate(115200); //must be set to 115200
    if(!port.open(QIODevice::ReadWrite))
    {
        std::cout << "Connection failed";
        a.exit();
    }
#endif
    setMPUrate(50000); //example: set MPU rate to 1s
    setMotors(-30, 30); //example: stop motors

    return a.exec();
}
