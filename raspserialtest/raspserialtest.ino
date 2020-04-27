#include <IRLibSendBase.h>    // First include the send base
//Now include only the protocols you wish to actually use.
//The lowest numbered protocol should be first but remainder 
//can be any order.
#include <IRLib_P01_NEC.h>    
#include <IRLib_P02_Sony.h>
#include <IRLibCombo.h>     // After all protocols, include this


IRsend irsend;
bool repeat = false;
int numberOfBitsMask = 248;
int commandMask = 7;


void setup(){
    Serial.begin(9600);    
}
 
void loop(){
    if(repeat) {
      Serial.println("repeating");
      irsend.send(NEC, 0xFFFFFFFF, 32);
    }
    if (Serial.available()) {      
        byte buffer[5];
        Serial.readBytes(buffer, 5);
        Serial.println(buffer[0]);
        Serial.println(buffer[1]);
        Serial.println(buffer[2]);
        Serial.println(buffer[3]);
        Serial.println(buffer[4]);
        byte commandingByte = buffer[0];

        int numberOfBits = (((int) commandingByte) & numberOfBitsMask) >> 3; // First 5 bits of the commandingByte refer to number of sony-bits to send
        int command = ((int) commandingByte) & commandMask; // remaining 3 bits of commandingByte refer to command (0 send, 1 repeat, 2 stop repeat, 3 sony-signal)

        byte ircode[4];
        for(int i=1; i < 5; i++) {
          ircode[i-1] = buffer[i]; 
        }
        
        if(command == 2) {
          repeat = false;
          Serial.println("Not repeating");
          return;
        } else if (command == 1) {
          repeat = true;
        }
        unsigned long myInt = array_to_int(ircode, 4); // convert the 4 bytes to one nice big value
        if(command == 3) { // Send Sony signal
          irsend.send(SONY, myInt, numberOfBits);
        } else {
        irsend.send(NEC, myInt, 32);
        }
        Serial.println(myInt); // emitting this value!
        char str[32] = "";
        array_to_string(ircode, 4, str);
        Serial.print("Sending");
        Serial.println(str);
    }
}


unsigned long array_to_int(byte array[], unsigned int len)
{
    unsigned long sum = 0;
    Serial.println("= = = CALCULATING = = =");
    
    for (unsigned int i = 0; i < len; i++)
    {
      unsigned long hexValueInInt = array[i];
      unsigned long factor = 1L * (long)power(256, len-1-i);
      unsigned long hexValue32Bit =  hexValueInInt * factor;
      sum = sum + hexValue32Bit;
      Serial.println(sum);
    }
    return sum;
}

long power(int factor, int exponential) {
  if( exponential == 0) return 1;
  long sum = 1;
  for(unsigned int i = 1; i <= exponential; i++) {
    sum = sum * factor;
  }
  return sum;
}


void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
