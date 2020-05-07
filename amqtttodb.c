/*
 *
 * amqtttodb.c - take all topics from mosquitto and pass to influx db.
 * install mosquitto-dev & gcc (on deb "apt install libmosquitto-dev gcc")
 * compile with "gcc -s -Wall -o test-influx test-influx.c -lmosquitto"
 * change whatever neded, mosquitto server and influx db/server. 
 * based on test-sub.c, only the callback is changed to fit my needs. test-sub.c not written by me se the below notice: //makodse
 * 
 * test-sub.c
 * Part of the mosquito-test demonstration application
 * Consumes messages from an MQTT topic indefinitely
 * Copyright (c)2016 Kevin Boone. Distributed under the terms of the
 *  GPL v3.0
 *
 *
 */


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mosquitto.h"

// Server connection parameters
#define MQTT_HOSTNAME "192.168.3.220"
#define MQTT_PORT 1883
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
#define MQTT_TOPIC "#"

void my_message_callback(struct mosquitto *mosq, void *obj,
    const struct mosquitto_message *message)
  {
  char wgetcmd[200] = "curl -i -XPOST \"http://192.168.3.220:8086/write?db=db_ahome&precision=s\" --data-binary \'test,sensornamn=";
  strcat(wgetcmd, (char *)message->topic);
  strcat(wgetcmd," value=");
  strcat(wgetcmd, (char *)message->payload);
  strcat(wgetcmd,"\'");
  system(wgetcmd);
  printf("%s\n",wgetcmd);
}

int main (int argc, char **argv)
  {
  struct mosquitto *mosq = NULL;
  mosquitto_lib_init();
mosq = mosquitto_new (NULL, true, NULL);
  if (!mosq)
    {
    fprintf (stderr, "Can't init Mosquitto library\n");
    exit (-1);
    }
  mosquitto_username_pw_set (mosq, MQTT_USERNAME, MQTT_PASSWORD);

  int ret = mosquitto_connect (mosq, MQTT_HOSTNAME, MQTT_PORT, 0);
  if (ret)
    {
    fprintf (stderr, "Can't connect to Mosquitto server\n");
    exit (-1);
    }

  ret = mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
  if (ret)
    {
    fprintf (stderr, "Can't publish to Mosquitto server\n");
    exit (-1);
    }
  mosquitto_message_callback_set (mosq, my_message_callback);
  mosquitto_loop_forever (mosq, -1, 1);
  mosquitto_destroy (mosq);
  mosquitto_lib_cleanup();

  return 0;
  }

