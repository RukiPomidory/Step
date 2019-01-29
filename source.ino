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

#define DEBUG

#ifdef  DEBUG
#   define  D_INIT() { Serial.begin(115200); while(!Serial); }   
#   define  D(x)    { Serial.print(x); }
#   define  D_LN(x) { Serial.println(x); } 
#else
#   define  D_INIT()
#   define  D(x)
#   define  D_LN(x)
#endif


#define FORWARD 1   
#define BACKWARD 0
#define UP 3
#define DOWN 4

#define BODY 2  // Пин двигателя продольного смещения

// герконы ради экономии места соединяются по несколько штук в один пин через разные резисторы
#define LEFT_F A0   // два левых передних геркона
#define LEFT_B A1   // два левых задних
#define RIGHT_F A2  // два правых передних
#define RIGHT_B A3  // два правых задних

#define FORWARD_BUTTON 5    // Кнопки "вперед" и "назад"
#define BACKWARD_BUTTON 4

#define COMMON 3    // общий пин всех ног

// Распиновка актуаторов, начиная с задних, заканчивая передними
int leftLegs[] = {6, 7, 8, 9};          // Левый ряд
int rightLegs[] = {10, 11, 12, 13};     // Правый ряд 

// пограничные значения пина с кнопками
int limits[] = {300, 500, 700}; 

int legsDuration = 1 * 1000; // Длительность движения ног
int bodyDuration = 2 * 1000; // Длительность движения корпуса

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
    pinMode(COMMON, OUTPUT);
}

void loop() 
{
    // Состояние кнопок управления
    int forwardBtn =  digitalRead(FORWARD_BUTTON);
    int backwardBtn =  digitalRead(BACKWARD_BUTTON);
    
    if (0 == forwardBtn)
    {
        D("FORWARD");
        oneStep(FORWARD);
    }
    else if (0 == backwardBtn)
    {
        D("BACKWARD");
        oneStep(BACKWARD);
    }

    while(forwardBtn || backwardBtn)
    {
        forwardBtn = !(bool) digitalRead(FORWARD_BUTTON);
        backwardBtn = !(bool) digitalRead(BACKWARD_BUTTON);
        delay(5);
    }
}

bool getSwitchState(int pin)
{
    int group;
    int base;
    if (leftLegs[0] == pin ||
        leftLegs[1] == pin)
    {
        base = 0;
            group = LEFT_F;
    }
    else if (leftLegs[2] == pin ||
            leftLegs[3] == pin)
    {
        base = 2;
            group = LEFT_B;
    }
    else if (rightLegs[0] == pin ||
            rightLegs[1] == pin)
    {
        base = 0;
        group = RIGHT_F;
    }
    else if (rightLegs[2] == pin ||
            rightLegs[3] == pin)
    {
        base = 2;
        group = RIGHT_B;
    }
    else
    {
        return false;
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

void oneStep(int direct)
{
    if (FORWARD == direct)
    {
        front(UP);  // Поднимаем перед, перемещаем его вперед и опускаем
        collapse();
        front(DOWN);
        
        back(UP);   // Поднимаем зад, также перемещаем вперед и опускаем
        extend();
        back(DOWN);
    }
    else if (BACKWARD == direct)
    {
        back(UP);   // Аналогично предыдущему, только в другую сторону
        collapse();
        back(DOWN);
        
        front(UP);  
        extend();
        front(DOWN);
    }
    
}

void front(int direct)
{
    int frontLegs[] = {leftLegs[0], leftLegs[2],
                            rightLegs[0], rightLegs[2]};

    move(frontLegs, direct);
}

void back(int direct)
{
    int backLegs[] = {leftLegs[1], leftLegs[3],
                            rightLegs[1], rightLegs[3]};

    move(backLegs, direct);
}

void move(int legs[], int direct)
{
    if (UP == direct)
    {        
        legsUp(legs, legsDuration);
    }
    else if (DOWN == direct)
    {
        legsDown(legs, legsDuration);
    }
}

void extend()
{
    digitalWrite(COMMON, LOW);
    turnAllOff();

    digitalWrite(BODY, HIGH);
    delay(bodyDuration);
    
    digitalWrite(BODY, LOW);
}

void collapse()
{
    
    digitalWrite(COMMON, HIGH);
    turnAllOn();

    digitalWrite(BODY, LOW);
    delay(bodyDuration);
    
    digitalWrite(COMMON, LOW);
    turnAllOff();
}

void legsUp(int legs[], int duration)
{
    digitalWrite(COMMON, HIGH); // Подаем высокий уровень на общий вывод
    turnAllOn();                // Также высокий уровень на всех ногах, чтобы не работали лишние двигатели
    
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(legs[i], LOW); // Создаем разность потенциалов между общим пином и нужными нам выводами
    }

    // крутим двигателями...
    delay(duration);

    // Останавливаем все
    digitalWrite(COMMON, LOW);
    turnAllOff();
}

void legsDown(int legs[], int duration)
{
    digitalWrite(COMMON, LOW); // Подаем низкий уровень на общий вывод
    turnAllOff();              // Также низкий уровень на всех ногах, чтобы не работали лишние двигатели
    
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(legs[i], HIGH); // Создаем разность потенциалов между общим пином и нужными нам выводами
    }

    // крутим двигателями...
    delay(duration);

    // Останавливаем все
    turnAllOff();
}

void legsDownUntilSwitch(int legs[])
{
    digitalWrite(COMMON, LOW);
    turnAllOff();
    
    bool allStop = false;
    while(!allStop)
    {
        allStop = true;
        for (int i = 0; i < 4; i++)
        {
            bool state = getSwitchState(legs[i]);
            
            if (state)
            {
                digitalWrite(legs[i], LOW);
            }
            else
            {
                digitalWrite(legs[i], HIGH);
                allStop = false;
            }
        }
    }
}

void turnAllOn()
{
    for(int i = 0; i < 4; i++)
    {
        digitalWrite(leftLegs[i], HIGH);
        digitalWrite(rightLegs[i], HIGH);
    }
    digitalWrite(BODY, HIGH);
}

void turnAllOff()
{
    for(int i = 0; i < 4; i++)
    {
        digitalWrite(leftLegs[i], LOW);
        digitalWrite(rightLegs[i], LOW);
    }
    digitalWrite(BODY, LOW);
}
