#include "LoRaWan_APP.h"
#include "Arduino.h"

uint8_t devEui[] = { 0x00, 0x00, 0xA0, 0x3B, 0x3C, 0x43, 0xCA, 0x48 };
uint8_t appEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0x12, 0x34, 0xAB, 0xCD };
uint8_t appKey[] = { 0x2A, 0x3F, 0x72, 0xF6, 0x0B, 0x94, 0x5B, 0x69,
                     0x58, 0x96, 0xA0, 0x8B, 0xCB, 0xE1, 0x01, 0xE7 };

uint16_t userChannelsMask[6]={ 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
//uint16_t userChannelsMask[6] = { 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };


bool loraWanAdr = true;
uint8_t nwkSKey[16] = { 0 };
uint8_t appSKey[16] = { 0 };
uint32_t devAddr = 0;


LoRaMacRegion_t loraWanRegion = LORAMAC_REGION_US915;  // o la que tu gateway usa
DeviceClass_t loraWanClass = CLASS_A;                  // clase A para TTN
//DeviceState_t deviceState = DEVICE_STATE_INIT;

uint32_t appTxDutyCycle = 15000;

bool overTheAirActivation = true;

uint8_t appPort = 2;         // Puerto de aplicación
bool isTxConfirmed = false;  // Confirmar mensajes (ACK)

uint8_t confirmedNbTrials = 4;
static void prepareTxFrame(uint8_t port) {
  /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
  *appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
  *if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
  *if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
  *for example, if use REGION_CN470, 
  *the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
  */
  appDataSize = 4;
  appData[0] = 0x00;
  appData[1] = 0x01;
  appData[2] = 0x02;
  appData[3] = 0x03;
}

//if true, next uplink will add MOTE_MAC_DEVICE_TIME_REQ


void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  // Mcu.begin(HELTEC_BOARD, EXTERNAL_OSC);
}

void loop() {
  switch (deviceState) {
    case DEVICE_STATE_INIT:
      {
#if (LORAWAN_DEVEUI_AUTO)
        LoRaWAN.generateDeveuiByChipID();
#endif
        LoRaWAN.init(loraWanClass, loraWanRegion);
        //both set join DR and DR when ADR off
        LoRaWAN.setDefaultDR(3);
        break;
      }
    case DEVICE_STATE_JOIN:
      {
        LoRaWAN.join();
        break;
      }
    case DEVICE_STATE_SEND:
      {
        prepareTxFrame(appPort);
        LoRaWAN.send();
        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
    case DEVICE_STATE_CYCLE:
      {
        // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
    case DEVICE_STATE_SLEEP:
      {
        LoRaWAN.sleep(loraWanClass);
        break;
      }
    default:
      {
        deviceState = DEVICE_STATE_INIT;
        break;
      }
  }
}
