#include <IRLib.h>

IRsend irsend;
bool repeat = false;

void setup(){
    Serial.begin(9600);    
}
 
void loop(){
    if(repeat) {
      irsend.send(NEC, 0xFFFFFFFF, 32);
    }
    if (Serial.available()) {
        byte buffer[5];
        Serial.readBytes(buffer, 5);
        byte command = buffer[0];
        
        byte ircode[4];
        for(int i=1; i < 5; i++) {
          ircode[i-1] = buffer[i]; 
        }
        
        if(command == 0) {
          Serial.println("Not repeating");
        } else if (command == 1) {
          repeat = true;
        }
                
        unsigned long myInt = array_to_int(ircode, 4);
        int repeat = 1;
        if(myInt == 0xFFFFFFFF) {
          repeat = 10;
        }
        for (int i = 0; i < repeat; i++) {
          irsend.send(NEC, myInt, 32);
          Serial.println(myInt);
          delay(40);

        }

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
