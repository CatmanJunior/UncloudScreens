import mqtt.*;
MQTTClient client;

void initMQTT() {
  client = new MQTTClient(this);
  client.connect("mqtt://uncloud:uncloud@192.168.15.102", "lijntjes");
  client.subscribe("c2_b1");
  client.subscribe("c2_f1");
  client.subscribe("c2_f2");
  client.subscribe("c2_f3");
}

float fader1 =0;
int fader2 = 0;
int fader3 = 0;

float  xA, yA, xB, yB, x1, y1, x2, y2;

float sx, sy;

float angle;
float speed;
float radi;

float c, s;

void messageReceived(String topic, byte[] payload) {
  println("new message: " + topic + " - " + new String(payload));
  String a = new String(payload);
  if (topic.equals("c2_b1")) {

    if (a.equals("01")) {
      neteja();
      sx = random(5.0);
      sy=random(5.0);
      print("button01");
    }
    if (a.equals("02")) {
      print("button02");
      neteja();
      sx=random(1);
      sy=2.0;
    }
  }
  if (topic.equals("c2_f1")) {
    fader1 = Float.parseFloat(a)/10000;
    println(fader1);
  }
  if (topic.equals("c2_f2")) {
    fader2 = 10-(Integer.parseInt(a)/100);
    if (fader2 < 1) fader2 =1;
  }
  if (topic.equals("c2_f3")) {

    fader3 = Integer.parseInt(a);
    if (fader3 < 1) fader3 =1;
  }
}


void setup() {
 fullScreen(P2D);
  background(255);
  smooth();
  initMQTT();
  sx = random(5.0);
  sy=random(5.0);

  angle=0.0;
  speed=0.01;
  radi=300.0;
  xA = 20;
  yA = height/2;
  xB = width-20;
  yB = height/2;
}

void draw() {
  speed=fader1;
  sy=fader2;
  // dues corbes de lissajus serveixen de punts de control
  c = cos(angle);
  s = sin(angle/sy);
  // c=0;s=0;

  x1 = width/3+c*radi;
  y1 = height/2+s*radi;

  x2 = 2*width/3 + cos(angle/sx)*radi;
  y2 = height/2 + sin(angle)*radi;
  //y2 = y1 + tan(angle*sy)*radi;

  // pintem la corba de bezier
  noFill();
  stroke(0, fader3);
  bezier(xA, yA, x1, y1, x2, y2, xB, yB);

  // fem un pas
  angle+=speed;
}

void neteja() {
  background(255);
}

void keyPressed() {
  switch(key) {
    case('1'):
    neteja();
    sx=5.0;
    sy=random(1);
    break;
    case('2'):
    neteja();
    sx=random(1);
    sy=2.0;
    break;
  default:
    neteja();
    sx = random(5.0);
    sy=random(5.0);
  }
}