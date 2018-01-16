import mqtt.*;

MQTTClient client;


// client.unsubscribe("/example");


void keyPressed() {
  client.publish("/hello", "world");
}















void messageReceived(String topic, byte[] payload) {
  println("new message: " + topic + " - " + new String(payload));
  if (topic.equals("button")) {
    String a = new String(payload);
    if (a.equals("1")) buttonXZ.pressed = !buttonXZ.pressed;
    if (a.equals("2")) buttonYZ.pressed = !buttonYZ.pressed;
    if (a.equals("3")) buttonXW.pressed = !buttonXW.pressed;
    if (a.equals("4")) buttonYW.pressed = !buttonYW.pressed;
    if (a.equals("5")) buttonZW.pressed = !buttonZW.pressed;
    if (a.equals("6")) buttonXY.pressed = !buttonZW.pressed;
    if (a.equals("7")) tesseract = new Tesseract();
  }
}