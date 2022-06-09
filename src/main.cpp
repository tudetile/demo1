/*
demo1
Demo code for 3x3 RGB LED matrix driven by IS32FL3237 LED driver and Adafruit-Trinket-5V AVR board with ATtiny85 microcontroller.
When using MS VS Code IDE, please make sure you add "upload_flags = -e" in the platformio.ini file for proper erasing before upload. Otherwise you might get a verification mismatch error and damage the bootloader.
*/

#include <arduino.h>
#include <TinyWireM.h>
#include <avr/pgmspace.h>
#define Addr_GND 0x68
// The slave address is 7 bit format, so that Addr_GND corresponds to 0x34 when beginning transmission

byte PWM_64[64] =
{
0x00,0x04,0x08,0x0c,0x10,0x14,0x18,0x1c,
0x20,0x24,0x28,0x2c,0x30,0x34,0x38,0x3c,
0x40,0x44,0x48,0x4c,0x50,0x54,0x58,0x5c,
0x60,0x64,0x68,0x6c,0x70,0x74,0x78,0x7c,
0x80,0x84,0x88,0x8c,0x90,0x94,0x98,0x9c,
0xa0,0xa4,0xa8,0xac,0xb0,0xb4,0xb8,0xbc,
0xc0,0xc4,0xc8,0xcc,0xd0,0xd4,0xd8,0xdc,
0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc
};

byte Red_LED[9] =
{
0x01,0x07,0x0d,0x13,0x19,0x1f,0x25,0x2b,0x31
};

byte Green_LED[9] =
{
0x03,0x09,0x0f,0x15,0x1b,0x21,0x27,0x2d,0x33
};

byte Blue_LED[9] =
{
0x05,0x0b,0x11,0x17,0x1d,0x23,0x29,0x2f,0x35
};

enum Enum_LED_Mode {White_Breath, Sequential_RGB_Rise, Toggle_White_Cross_And_Corner, Toggle_Color_Cross_And_Corner, Walking_LED, Colorful_Wave, Jumpin_LED, Stick_Rotation};
Enum_LED_Mode LED_Mode = Enum_LED_Mode::White_Breath;

// Communication

void IS_I2C_WriteByte(uint8_t Dev_Add,uint8_t Reg_Add,uint8_t Reg_Dat){
  TinyWireM.beginTransmission(Dev_Add/2); // begin transmission to device
  TinyWireM.send(Reg_Add); // sends register address
  TinyWireM.send(Reg_Dat); // sends regitser data
  TinyWireM.endTransmission(); // stop transmission
}

// Initialize IS32FL3237 LED driver

void Init_IS32FL3237(void){
  uint8_t i=0;
  for(i=0x4A; i<=0x6D; i++){
    IS_I2C_WriteByte(Addr_GND,i,0xFF); // write to all SL scaling registers 
  }
  IS_I2C_WriteByte(Addr_GND,0x6e,0xb0); // write to GCC global current control register
  IS_I2C_WriteByte(Addr_GND,0x49,0x00); // update all PWM & scaling registers
  IS_I2C_WriteByte(Addr_GND,0x00,0x01); // update control register to normal operation, 8-bit PWM, 16MHz clock
}

// General PWM update functions

void Update_All_PWM(byte Duty_Cycle){ // white LED
  uint8_t i=0;
  for(i=0x01; i<=0x47; i=i+2){ // write only PWM_L registers
    IS_I2C_WriteByte(Addr_GND,i, Duty_Cycle); // write Duty_Cycle to PWM_L registers
  }
  IS_I2C_WriteByte(Addr_GND,0x49,0x00); // update all PWM & scaling registers
}

void Update_PWM(byte LED_PWM_Addr,byte Duty_Cycle){ // addressing single LED
  IS_I2C_WriteByte(Addr_GND,LED_PWM_Addr, Duty_Cycle); // write Duty_Cycle to PWM_L register
  IS_I2C_WriteByte(Addr_GND,0x49,0x00); // update all PWM & scaling registers
}

void Update_All_Red_PWM(byte Duty_Cycle){
  uint8_t i=0;
  for(i=0x01; i<=0x47; i=i+6){ // write only PWM_L registers
    IS_I2C_WriteByte(Addr_GND,i, Duty_Cycle); // write Duty_Cycle to PWM_L registers
  }
  IS_I2C_WriteByte(Addr_GND,0x49,0x00); // update all PWM & scaling registers
}

void Update_All_Green_PWM(byte Duty_Cycle){
  uint8_t i=0;
  for(i=0x03; i<=0x47; i=i+6){ // write only PWM_L registers
    IS_I2C_WriteByte(Addr_GND,i, Duty_Cycle); // write Duty_Cycle to PWM_L registers
  }
  IS_I2C_WriteByte(Addr_GND,0x49,0x00); // update all PWM & scaling registers
}

void Update_All_Blue_PWM(byte Duty_Cycle){
  uint8_t i=0;
  for(i=0x05; i<=0x47; i=i+6){ // write only PWM_L registers
    IS_I2C_WriteByte(Addr_GND,i, Duty_Cycle); // write Duty_Cycle to PWM_L registers
  }
  IS_I2C_WriteByte(Addr_GND,0x49,0x00); // update all PWM & scaling registers
}

// Begin of figure functions

void White_Breath_Function(void){
  uint8_t i=0, Max_PWM_Value=63;
  Update_All_PWM(PWM_64[0]);
  for(i=0; i<=Max_PWM_Value; i=i+2){
    Update_All_PWM(PWM_64[i]);
  }
  for(i=0; i<=Max_PWM_Value; i=i+2){
    Update_All_PWM(PWM_64[Max_PWM_Value-i]);
  }
  Update_All_PWM(PWM_64[0]);
}

void Sequential_RGB_Rise_Function(void){
  uint8_t i=0, Max_PWM_Value=63;
  Update_All_PWM(PWM_64[0]);
  for(i=0; i<=Max_PWM_Value; i++){
    Update_All_Red_PWM(PWM_64[i]);
  }
  Update_All_PWM(PWM_64[0]);
  for(i=0; i<=Max_PWM_Value; i++){
    Update_All_Green_PWM(PWM_64[i]);
  }
  Update_All_PWM(PWM_64[0]);
  for(i=0; i<=Max_PWM_Value; i++){
    Update_All_Blue_PWM(PWM_64[i]);
  }
  Update_All_PWM(PWM_64[0]);
}

void Toggle_White_Cross_And_Corner_Function(void){
  uint8_t i=0, PWM_Value=63, Delay_Value=100, Cross_LED[5]={1, 3, 4, 5, 7}, Corner_LED[4]={0, 2, 6, 8};
  Update_All_PWM(PWM_64[0]);
  for(i=0; i<5; i++){
    Update_PWM(Red_LED[Cross_LED[i]], PWM_64[PWM_Value]);
    Update_PWM(Green_LED[Cross_LED[i]], PWM_64[PWM_Value]);
    Update_PWM(Blue_LED[Cross_LED[i]], PWM_64[PWM_Value]);   
  }
  delay(Delay_Value);
  Update_All_PWM(PWM_64[0]);
  for(i=0; i<4; i++){
    Update_PWM(Red_LED[Corner_LED[i]], PWM_64[PWM_Value]);
    Update_PWM(Green_LED[Corner_LED[i]], PWM_64[PWM_Value]);
    Update_PWM(Blue_LED[Corner_LED[i]], PWM_64[PWM_Value]);   
  }
  delay(Delay_Value);
  Update_All_PWM(PWM_64[0]);
}

void Toggle_Color_Cross_And_Corner_Function(void){
  uint8_t i=0, j=0, PWM_Value=63, Delay_Value=100, Cross_LED[5]={1, 3, 4, 5, 7}, Corner_LED[4]={0, 2, 6, 8};
  bool r=0, g=0, b=0;
  Update_All_PWM(PWM_64[0]);
  for(j=0; j<3; j++){
    if(j==0){r=1; g=0; b=1;}
    if(j==1){r=1; g=1; b=0;}
    if(j==2){r=0; g=1; b=1;}
    for(i=0; i<5; i++){
      Update_PWM(Red_LED[Cross_LED[i]], PWM_64[r*PWM_Value]);
      Update_PWM(Green_LED[Cross_LED[i]], PWM_64[g*PWM_Value]);
      Update_PWM(Blue_LED[Cross_LED[i]], PWM_64[b*PWM_Value]);   
    }
    delay(Delay_Value);
    Update_All_PWM(PWM_64[0]);
    for(i=0; i<4; i++){
      Update_PWM(Red_LED[Corner_LED[i]], PWM_64[r*PWM_Value]);
      Update_PWM(Green_LED[Corner_LED[i]], PWM_64[g*PWM_Value]);
      Update_PWM(Blue_LED[Corner_LED[i]], PWM_64[b*PWM_Value]);  
    }
    delay(Delay_Value);
    Update_All_PWM(PWM_64[0]);
  }
}

void Walking_LED_Function(void){
  uint8_t i=0, Delay_Value=50;
  Update_All_PWM(PWM_64[0]);
  for(i=0; i<9; i++){
    Update_PWM(Red_LED[i], PWM_64[63]); // 63
    Update_PWM(Green_LED[i], PWM_64[22]); // 22
    Update_PWM(Blue_LED[i], PWM_64[0]); // 0, walking orange dot
    delay(Delay_Value);
    Update_All_PWM(PWM_64[0]);
  }
}

void Colorful_Wave_Function(void){
  uint8_t i=0, Min_PWM_Value=5, Max_PWM_Value=63;
  Update_All_Red_PWM(PWM_64[Max_PWM_Value]);
  Update_All_Green_PWM(PWM_64[Min_PWM_Value]);
  Update_All_Blue_PWM(PWM_64[Max_PWM_Value]);
  
  for(i=Min_PWM_Value; i<=(Max_PWM_Value-Min_PWM_Value); i++){
    Update_All_Red_PWM(PWM_64[Max_PWM_Value-i]);
    Update_All_Green_PWM(PWM_64[i]);
  }
  for(i=Min_PWM_Value; i<=(Max_PWM_Value-Min_PWM_Value); i++){
    Update_All_Blue_PWM(PWM_64[Max_PWM_Value-i]);
    Update_All_Red_PWM(PWM_64[i]);
  }
  Update_All_PWM(PWM_64[0]);
}

void Jumpin_LED_Function(void){
  uint8_t i=0, j=0, PWM_Value=63, Delay_Value=50, Random_LED;
  Update_All_PWM(PWM_64[0]);
  for(j=0; j<2; j++){
    for(i=0; i<9; i++){
      Random_LED = (uint8_t)random(0,9);
      Update_PWM(Red_LED[Random_LED], PWM_64[PWM_Value]);
      Random_LED = (uint8_t)random(0,9);
      Update_PWM(Green_LED[Random_LED], PWM_64[PWM_Value]);
      Random_LED = (uint8_t)random(0,9);
      Update_PWM(Blue_LED[Random_LED], PWM_64[PWM_Value]);
      delay(Delay_Value);
      Update_All_PWM(PWM_64[0]);
    }
  }
}

void Stick_Rotation_Function(void){
  uint8_t i=0, j=0, k=0, PWM_Value=63, Delay_Value=20, Stick_LED[12]={1, 4, 7, 0, 4, 8, 3, 4, 5, 6, 4, 2};
  bool r=0, g=0, b=0;
  Update_All_PWM(PWM_64[0]);
  for(j=0; j<3; j++){
    if(j==0){r=1; g=0; b=1;}
    if(j==1){r=1; g=1; b=0;}
    if(j==2){r=0; g=1; b=1;}
    for(i=0; i<2; i++){
      for(k=0; k<12; k++){
        Update_PWM(Red_LED[Stick_LED[k]], PWM_64[r*PWM_Value]);
        Update_PWM(Green_LED[Stick_LED[k]], PWM_64[g*PWM_Value]);
        Update_PWM(Blue_LED[Stick_LED[k]], PWM_64[b*PWM_Value]);
        if(k==2 || k==5 || k==8 || k==11){
          delay(Delay_Value);
          Update_All_PWM(PWM_64[0]);
        }
      } 
    }
  }
}

// Begin of main functions of microcontroller

void setup() {
  // put your setup code here, to run once:
  TinyWireM.begin(); // setup I2C communication on Adafruit-Trinket-5V with ATtiny85 microcontroller
  pinMode(1, OUTPUT); // define SDB output on Adafruit-Trinket-5V with ATtiny85 microcontroller
  digitalWrite(1, HIGH); // SDB_HIGH, enable IS32FL3237
  delay(100); // wait 100ms
  Init_IS32FL3237(); // initialize IS32FL3237
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (LED_Mode)
  {
  case Enum_LED_Mode::White_Breath:
    White_Breath_Function();
    LED_Mode = Enum_LED_Mode::Sequential_RGB_Rise;
    break;

  case Enum_LED_Mode::Sequential_RGB_Rise:
    Sequential_RGB_Rise_Function();
    LED_Mode = Enum_LED_Mode::Toggle_White_Cross_And_Corner;
    break;

  case Enum_LED_Mode::Toggle_White_Cross_And_Corner:
    Toggle_White_Cross_And_Corner_Function();
    LED_Mode = Enum_LED_Mode::Toggle_Color_Cross_And_Corner;
    break;

  case Enum_LED_Mode::Toggle_Color_Cross_And_Corner:
    Toggle_Color_Cross_And_Corner_Function();
    LED_Mode = Enum_LED_Mode::Walking_LED;
    break;
  
  case Enum_LED_Mode::Walking_LED:
    Walking_LED_Function();
    LED_Mode = Enum_LED_Mode::Colorful_Wave;
    break;

  case Enum_LED_Mode::Colorful_Wave:
    Colorful_Wave_Function();
    LED_Mode = Enum_LED_Mode::Jumpin_LED;
    break;
  
  case Enum_LED_Mode::Jumpin_LED:
    Jumpin_LED_Function();
    LED_Mode = Enum_LED_Mode::Stick_Rotation;
    break;

  case Enum_LED_Mode::Stick_Rotation:
    Stick_Rotation_Function();
    LED_Mode = Enum_LED_Mode::White_Breath;
    break;
  
  default:
    delay(1000); // by default do nothing
    break;
  }
}