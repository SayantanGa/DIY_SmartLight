#define LDR_PIN A1

const int DEFAULT_BRIGHTNESS = 196;
const int DEFAULT_TEMP_INDEX = 2;
int LDR = 0;

//Function definitions for controlling brightness:
int LinearBrightness(int x){
    return int(255*(1-x/1023));
}

int HigherBrightness(int x){
    return int(255*(sqrt(1-(x*x)/1046529)));
}

int LowerBrightness(int x){
    return int(255*(1-(sqrt(1-(1-x/1023)*(1-x/1023)))));
}

//
int Temperatures[11][3] = {{200,245,254},{240,255,255},{255,255,255},{255,255,200},{248,255,183},{255,248,167},{255,234,144},{255,218,122},{255,182,78},{255,139,39},{255,89,11}};
int (*Brightness_Functions[3])(int) = {LinearBrightness, HigherBrightness, LowerBrightness};

class LED{
    public:
        int Pins[2] = {3, 5};
        int RGB_Pins[3] = {9, 10, 11};
        int brightness = DEFAULT_BRIGHTNESS;
        int brightness_function_index = 0;
        int temp_index = DEFAULT_TEMP_INDEX;
        int r = Temperatures[temp_index][0]*(brightness/255);
        int g = Temperatures[temp_index][1]*(brightness/255);
        int b = Temperatures[temp_index][2]*(brightness/255);
  
        void glow(){
            for (int i = 0; i < 2; i++){
                analogWrite(Pins[i], brightness);
              	delay(50);
              	analogWrite(RGB_Pins[0], r);
        		delay(50);
        		analogWrite(RGB_Pins[1], g);
        		delay(50);
        		analogWrite(RGB_Pins[2], b);
        		delay(50);
            }
        }
  
        void dim(){
            for (int i = 0; i < 2; i++){
                digitalWrite(Pins[i], LOW);
              	delay(50);
            }
            for (int i = 0; i < 3; i++){
                digitalWrite(RGB_Pins[i], LOW);
              	delay(50);
            }
        }
  
        void update(){
            brightness = Brightness_Functions[brightness_function_index](LDR);
            r = Temperatures[temp_index][0]*(brightness/255);
            g = Temperatures[temp_index][1]*(brightness/255);
            b = Temperatures[temp_index][2]*(brightness/255);
            glow();
        }
  
        void Increase_Brightness(){
            brightness_function_index = 1;
            update();
        }
  
        void Decrease_Brightness(){
            brightness_function_index = 2;
            update();
        }
  
  		void Hotter(){
          	if(temp_index < 10){
            	temp_index++;
          	}
          	update();
        }
  
        void Cooler(){
          	if(temp_index > 0){
              	temp_index--;
            }
          	update();
        }
};

char data[4];
const int data_size = 4;
LED Light;


void setup(){
  Serial.begin(9600);
  pinMode(LDR_PIN, INPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  
  Light.brightness = DEFAULT_BRIGHTNESS;
  Light.glow();
}

void loop(){
  LDR = analogRead(LDR_PIN);
  if(Serial.available())
  {
    Serial.readBytes(data, data_size);
    if(data[0]=='1')
    {
      Light.glow();
      if(data[1]=='0')
      {
        Light.glow();
      }
      if(data[1]=='1')
        Light.update();
      if(data[2]=='1')
        Light.Increase_Brightness();
      if(data[2]=='2')
        Light.Decrease_Brightness();
      if(data[3]=='1')
        Light.Hotter();
      if(data[3]=='2')
        Light.Cooler();
    }
    else if(data[0]=='0')
      Light.dim();
  }
}