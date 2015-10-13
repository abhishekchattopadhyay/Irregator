/*
 SMS receiver

 This sketch, for the Arduino GSM shield, waits for a SMS message
 and displays it through the Serial port.

 Circuit:
 * GSM shield attached to and Arduino
 * SIM card that can receive SMS messages

 created 25 Feb 2012
 by Javier Zorzano / TD

 This example is in the public domain.

 http://www.arduino.cc/en/Tutorial/GSMExamplesReceiveSMS

*/

// include the GSM library
#include <GSM.h>

// Start port of the Shiled
#define shieldStartPin 9

// PIN Number for the SIM
#define PINNUMBER ""

// Define the relayPin
#define relayPin 13

// Define states
#define ON 1
#define OFF 0

byte state = OFF;

// Acceptable Responses
String response[3] = { "ON", "OFF", "REGISTER"};

// initialize the library instances
GSM gsmAccess;
GSM_SMS sms;

// Array to hold the number a SMS is retreived from
char senderNumber[20];

// String to store authenticated user number
String userNumber="";

// String to read the sms
String gsm_msg = "";

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  Serial.println("SMS Messages Receiver");
  
  // Start the shiled
  digitalWrite(shieldStartPin,HIGH);
  pinMode(shieldStartPin,OUTPUT);

  // wait for a min of 1100 mSec
  delay(1500);
  digitalWrite(shieldStartPin,LOW);

  // connection state
  boolean notConnected = true;

  // Start GSM connection
  while (notConnected)
  {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");
  Serial.println("Waiting for messages");

  // Setup the relaypin
  digitalWrite(relayPin,OFF);
  pinMode(relayPin,OUTPUT);
}

void loop()
{
  char c;
  int it = 0;

  // If there are any SMSs available()
  if (sms.available())
  {
    Serial.print("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#')
    {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while (c = sms.read()){
      Serial.print(c);
      // construct the SMS string
      gsm_msg += c;
    }

    // Display the SMS
    Serial.print("\n The Final String MSg: ");
    Serial.println(gsm_msg);

    // Compare the message with formated texts
    for(it = 0; it < 3; ++it)
    {
      if(response[it].equalsIgnoreCase(gsm_msg))
      {
        Serial.print("Response matched :");
        Serial.println(it);
        break;
      }
    }
    String local(senderNumber);
    switch (it)
    {
      case 0:
      // Check the validity of the sender
      if(local.equalsIgnoreCase(userNumber))
      {
        if(state == ON)
        {
          Serial.println("Already On");
          sendSMS(userNumber, "Already On");
        }
        else 
        {
          state = !state;
          digitalWrite(relayPin,state);
          sendSMS(userNumber, "Equipment Started");
        }
      }
      else
      {
        Serial.println("UnAuthorized uesr");
      }
      break;
      
      case 1:
      if(local.equalsIgnoreCase(userNumber))
      {
         if(state == OFF)
        {
          Serial.println("Already Off");
          sendSMS(userNumber, "Already OFF");
        }
        else 
        {
          state = !state;
          digitalWrite(relayPin, state);
          sendSMS(userNumber, "Equipment Stopped");
        }
      }
      else
      {
        Serial.println("Unauthorised user");
      }
      break;
      
      case 2:
      userNumber = local;
      memset(senderNumber,0,20);
      Serial.println("New User Registration Request");
      sendSMS(userNumber, "Registered: OK");
      break;
      
     default:
     Serial.println("Default case-> must never have reached here");
     break;
    }

    Serial.println("\nEND OF MESSAGE");
    gsm_msg="";

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);
}

void sendSMS(String remoteNum, String textMsg)
{
   // send the message
   char number[20] = "";
   remoteNum.toCharArray(number,20);
   char msg[20]="";
   textMsg.toCharArray(msg,20);
   sms.beginSMS(number);
   sms.print(msg);
   sms.endSMS();
}


