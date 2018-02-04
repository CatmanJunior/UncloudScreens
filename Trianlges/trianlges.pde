import mqtt.*;
MQTTClient client;

void initMQTT() {
  client = new MQTTClient(this);
  client.connect("mqtt://uncloud:uncloud@192.168.15.102", "processing");
  client.subscribe("c2_b1");
  client.subscribe("c2_f1");
  client.subscribe("c2_f2");
  client.subscribe("c2_f3");
}

float fader1 =0;
int fader2 = 0;
int fader3 = 0;

void messageReceived(String topic, byte[] payload) {
  println("new message: " + topic + " - " + new String(payload));
  String a = new String(payload);
  if (topic.equals("c2_b1")) {
    if (a.equals("01")) print("button01");
    if (a.equals("02")) print("button02");
  }
  if (topic.equals("c2_f1")) {
    fader1 = Float.parseFloat(a)/1000000;
    println(fader1);
  }
  if (topic.equals("c2_f2")) {
    fader2 = 10-(Integer.parseInt(a)/100);
    if (fader2 < 1) fader2 =1;
  }
  if (topic.equals("c2_f3")) {

    fader3 = 10-(Integer.parseInt(a)/100);
    if (fader3 < 1) fader3 =1;
  }
}

float t=0.001;

void setup()
{
  initMQTT();
  fullScreen(P2D);
  strokeWeight(3);
}

void draw()
{
  background(0);
  stroke(255);
  strokeWeight(fader2);
  harom(500, 460, 40, 460, fader3, (sin(fader1*millis()%(2*PI))+1)/2);
}

void harom(float ax, float ay, float bx, float by, int level, float ratio)
{
  if (level!=0) {
    float vx, vy, nx, ny, cx, cy;
    vx=bx-ax;
    vy=by-ay;
    nx=cos(PI/3)*vx-sin(PI/3)*vy; 
    ny=sin(PI/3)*vx+cos(PI/3)*vy; 
    cx=ax+nx;
    cy=ay+ny;
    line(ax, ay, bx, by);
    line(ax, ay, cx, cy);
    line(cx, cy, bx, by);
    harom(ax*ratio+cx*(1-ratio), ay*ratio+cy*(1-ratio), ax*(1-ratio)+bx*ratio, ay*(1-ratio)+by*ratio, level-1, ratio);
  }
}