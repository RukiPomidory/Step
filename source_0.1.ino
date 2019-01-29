/*

  •__________________________________________•
  |  Картинка с наглядной   |                |
  |  распиновкой двигателей |                |
  |-——————————————|                |
  |                                          |
  |                  ПЕРЕД                   |
  |             |=============|              |
  | A0|--=[ 9]==|             |==[13]=--|A2  |
  |             |             |              |
  |             |   _  _  _   |              |
  | A0|--=[ 8]==|  | || || |  |==[12]=--|A2  |
  |             |  | |[5]| |  |              |
  |             |  | || || |  |              |
  | A1|--=[ 7]==|  `Y´`Y´`Y´  |==[11]=--|A3  |
  |             |   •  •  •   |              |
  |             |             |              |
  | A1|--=[ 6]==|     ЗАД     |==[10]=--|A3  |
  |             |=============|              |
  |                                          |
  |                                          |
  •¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯•
*/

// #define DEBUG

#ifdef  DEBUG
#   define  D_INIT() { Serial.begin(115200); while(!Serial); }   
#   define  D(x)    { Serial.print(x); }
#   define  D_LN(x) { Serial.println(x); } 
#else
#   define  D_INIT()
#   define  D(x)
#   define  D_LN(x)
#endif


#define BODY 5  // Пин двигателя продольного смещения

// герконы ради экономии места соединяются по несколько штук в один пин через разные резисторы
#define LEFT_F A0   // два левых передних геркона
#define LEFT_B A1   // два левых задних
#define RIGHT_F A2  // два правых передних
#define RIGHT_B A3  // два правых задних

#define FORWARD_BUTTON 5    // Кнопки "вперед" и "назад"
#define BACKWARD_BUTTON 4

// Распиновка актуаторов, начиная с задних, заканчивая передними
const int leftLegs = [6, 7, 8, 9];          // Левый ряд
const int rightLegs = [10, 11, 12, 13];     // Правый ряд 

// пограничные значения пина с кнопками
const int limits = [300, 500, 700]; 

////////////////////////////////////////////////////////////////////


void setup() 
{
    D_INIT();

    // Указываем выходные пины
    for(int i = 0; i < 4; i++)
    {
        pinMode(leftLegs[i], OUTPUT);
        pinMode(rightLegs[i], OUTPUT);
    }
    pinMode(BODY, OUTPUT);
}

void loop() 
{
    // Состояние кнопок управления
    bool forwardBtn = digitalRead(FORWARD_BUTTON);
    bool backwardBtn = digitalRead(BACKWARD_BUTTON);
    
    if (forwardBtn)
    {
        
    }
    else
    {
        
    }

    while(forwardBtn || backwardBtn);
}

bool getSwitchState(int pin)
{
    int group;
    int base;
    switch(pin)
    {
        case leftLegs[0]:
        case leftLegs[1]:
            base = 0;
            group = LEFT_F;
            break;

        case leftLegs[2]:
        case leftLegs[3]:
            base = 2;
            group = LEFT_B;
            break;

        case rightLegs[0]:
        case rightLegs[1]:
            base = 0;
            group = RIGHT_F;
            break;

        case rightLegs[2]:
        case rightLegs[3]:
            base = 2;
            group = RIGHT_B;
            break;

        default return false;
    }

    int value = analogRead(group);

    if (value < limits[0])
    {
        return false;
    }
    else if (value < limits[1])
    {
        return pin == base;
    }
    else if (value < limits[2])
    {
        return pin == (base + 1);
    }
    else
    {
        return true;
    }
}




