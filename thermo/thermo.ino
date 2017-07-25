#define refresh 250

//CONSTANTS
int sensor=A0;
int latchPin=1;
int dataPin=2;
int clockPin=3;
int pila=A4;
int pulsador=5;
const byte positivos[4]={10,9,8,7};

//FUNCTIONS
void traducir(float temp, byte* dig);
void visualizar(byte* dig, const byte* numeros);
void printVoltage(int lect_analog);

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(sensor, INPUT);
  pinMode(pila, INPUT);
  pinMode(pulsador, INPUT_PULLUP);
  pinMode(positivos[0], OUTPUT);
  pinMode(positivos[1], OUTPUT);
  pinMode(positivos[2], OUTPUT);
  pinMode(positivos[3], OUTPUT);
	delay(250);
}

void loop()
{
  const byte numeros[11]={3,159,37,13,153,73,65,31,1,9,254};
  byte dig[4];
  float temp, promedio=0.0;
	unsigned long t_inicial, t_actual;
	int buffer[16], indice_buf=1;

  promedio=buffer[0]=analogRead(sensor);
	for(int i=1;i<16;i++) buffer[i]=buffer[0];
	traducir(temp,dig);
	t_inicial=t_actual=millis();

  while(1)
	{
		t_actual=millis();
		visualizar(dig, numeros);
		while(digitalRead(pulsador)==0) // Visualización de voltaje
		{
			printVoltage(analogRead(pila));
		}
		if(t_actual-t_inicial>=refresh)
		{
			promedio=promedio-buffer[indice_buf]/16.0; // Quita la parte proporcional de la temp. antes de coger el valor siguiente y añadirle la dicha parte proporcional de la nueva temperatura
			buffer[indice_buf]=analogRead(sensor); // Leer valor TMP36
			promedio=promedio+buffer[indice_buf]/16.0;
			indice_buf++; if(indice_buf==16) indice_buf=0;
			temp=(5.0*promedio-512.0)/10.24; // Conversión del valor a temperatura
			traducir(temp,dig); // Mete en dig[] los numeros a visualizar
			t_inicial=t_actual;
		}
	}
}

void traducir(float temp, byte* dig) {
	dig[0]=temp/10;
	dig[1]=temp-dig[0]*10;
	dig[2]=temp*10-dig[0]*100-dig[1]*10;
	dig[3]=temp*100-dig[0]*1000-dig[1]*100-dig[2]*10;
}

void visualizar(byte* dig, const byte* numeros) {
	for(int i=0;i<4;i++)
	{
		digitalWrite(latchPin, LOW);
    if(i==1) {shiftOut(dataPin, clockPin, LSBFIRST, numeros[dig[i]]-1);} // +1 para visualizar el punto
		else if(i==0&&numeros[dig[0]]==0) {shiftOut(dataPin, clockPin, LSBFIRST, numeros[10]);} // excepción para mostrar vacío el digito si el primero es 0
		else shiftOut(dataPin, clockPin, LSBFIRST, numeros[dig[i]]);
    digitalWrite(latchPin, HIGH);
    digitalWrite(positivos[i], HIGH);

		delay(2); // Duración calculada experimentalmente
    digitalWrite(positivos[i], LOW);
	}
}

void printVoltage(int lect_analog) {
  // Esta función es la unión de las dos anteriores. Como su uso es muy puntual, las he combinado para ahorrar tiempo en llamadas a las otras dos funciones.
  // Esto viene bien en algunos aspectos aparte de por el del tiempo, por ejemplo que hay definida una V en el vector numeros, o que las condiciones de
  // visualización son diferentes (solo un entero, decimal en el primer dígito, último dígito fijo (la V), etc.)

  float voltage;
	byte numeros[12]={3,159,37,13,153,73,65,31,1,9,254,131};
	byte dig[3];
  // Operación *16.0/1023 para configuración 2k2 y 1k.
  // Voltaje=[(Rmayor+Rmenor)/Rmenor]*5*lect_analog/1023
  // Voltaje=[(10+10)/10]*5*lect_analog/1023
  // =2*5*lect_analog/1023
  // =10*lect_analog/1023
	voltage=lect_analog*10.0/1023;

	dig[0]=voltage;
	dig[1]=voltage*10-dig[0]*10;
	dig[2]=voltage*100-dig[0]*100-dig[1]*10;
  while(digitalRead(pulsador)==0)
  {
  	for(int i=0;i<4;i++)
  	{
  		digitalWrite(latchPin, LOW);
      if(i==0) {shiftOut(dataPin, clockPin, LSBFIRST, numeros[dig[i]]-1);} // +1 para visualizar el punto
  		else if(i==3) {shiftOut(dataPin, clockPin, LSBFIRST, numeros[11]);} // excepción para mostrar vacío el digito si el primero es 0
  		else shiftOut(dataPin, clockPin, LSBFIRST, numeros[dig[i]]);
      digitalWrite(latchPin, HIGH);
      digitalWrite(positivos[i], HIGH);

  		delay(2); // Duración calculada experimentalmente
      digitalWrite(positivos[i], LOW);
  	}
  }
}
