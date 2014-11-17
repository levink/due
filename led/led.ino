unsigned long int last = 0;
const long DELAY = 100;
int led[3] = {2,3,4};
int current = 0;

void setup() {
  pinMode(led[0], OUTPUT);
  pinMode(led[1], OUTPUT);
  pinMode(led[2], OUTPUT);
  last = millis();
}

void loop() {
  unsigned long int cur = millis();
  if (cur - last <= DELAY) return;
  
  for(int i=0; i < 3; i++)
  {
    if (i == current) analogWrite(led[i], 0);
    else analogWrite(led[i], 255);
  }
  current = (current + 1 ) %3;
  last = cur;
}
