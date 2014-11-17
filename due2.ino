
int led[3] = {2,3,4,2};
bool isActive = false;
void setup() {
  pinMode(led[0], OUTPUT);
  pinMode(led[1], OUTPUT);
  pinMode(led[2], OUTPUT);
}
void loop() {
  for(int i = 0; i<=3;i++)
  {
    if(isActive == false)
    {
      digitalWrite(led[i], HIGH);
      digitalWrite(led[i+1], HIGH);
    }
    else
    {
      digitalWrite(led[i], LOW);
      digitalWrite(led[i+1], LOW);
    }
    if(i==3)
    {
      i=0;
    }
    delay(1000);
  }
}
