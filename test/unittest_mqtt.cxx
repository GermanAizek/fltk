//
// Fl_MQTT_Client test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//

#include <FL/Fl_MQTT_Client.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Display.H>
#include "unittests.h"

static Fl_MQTT_Client *mqtt = 0;
static Fl_Text_Buffer *textbuf = 0;
static Fl_Text_Display *textdisp = 0;
static Fl_Input *host_input = 0;
static Fl_Input *port_input = 0;

static void mqtt_connected_cb(Fl_MQTT_Client* client, void* data) {
    (void)client; (void)data;
    textbuf->append("Status: Connected to broker!\n");
}

static void mqtt_disconnected_cb(Fl_MQTT_Client* client, void* data) {
    (void)client; (void)data;
    textbuf->append("Status: Disconnected.\n");
}

static void mqtt_message_cb(Fl_MQTT_Client* client, const char* topic, const void* payload, int payload_len, void* data) {
    (void)client; (void)data;
    char buf[1024];
    std::string msg((const char*)payload, payload_len);
    snprintf(buf, sizeof(buf), "Received on '%s': %s\n", topic, msg.c_str());
    textbuf->append(buf);
}

static void connect_cb(Fl_Widget *w, void *data) {
    if (!mqtt) return;
    
    mqtt->set_hostname(host_input->value());
    mqtt->set_port(atoi(port_input->value()));
    
    textbuf->append("Connecting...\n");
    if (mqtt->connect() == 0) {
        textbuf->append("Socket connected, waiting for CONNACK...\n");
    } else {
        textbuf->append("Failed to initiate connection.\n");
    }
}

static void disconnect_cb(Fl_Widget *w, void *data) {
    if (!mqtt) return;
    mqtt->disconnect();
}

static void publish_cb(Fl_Widget *w, void *data) {
    if (!mqtt || mqtt->state() != Fl_MQTT_Client::Connected) {
        textbuf->append("Not connected, cannot publish.\n");
        return;
    }
    
    if (mqtt->publish("fltk/test", "Hello from FLTK MQTT Client!") == 0) {
        textbuf->append("Published message to 'fltk/test'.\n");
    } else {
        textbuf->append("Failed to publish message.\n");
    }
}

static void subscribe_cb(Fl_Widget *w, void *data) {
    if (!mqtt || mqtt->state() != Fl_MQTT_Client::Connected) {
        textbuf->append("Not connected, cannot subscribe.\n");
        return;
    }
    
    if (mqtt->subscribe("fltk/test") == 0) {
        textbuf->append("Subscribed to 'fltk/test'.\n");
    } else {
        textbuf->append("Failed to subscribe.\n");
    }
}

Fl_Widget *create_mqtt_test() {
    Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
    g->begin();
    
    mqtt = new Fl_MQTT_Client();
    mqtt->set_connected_callback(mqtt_connected_cb);
    mqtt->set_disconnected_callback(mqtt_disconnected_cb);
    mqtt->set_message_callback(mqtt_message_cb);
    
    host_input = new Fl_Input(UT_TESTAREA_X + 60, UT_TESTAREA_Y + 10, 150, 25, "Host:");
    host_input->value("test.mosquitto.org");
    
    port_input = new Fl_Input(UT_TESTAREA_X + 260, UT_TESTAREA_Y + 10, 60, 25, "Port:");
    port_input->value("1883");
    
    Fl_Button *btn_conn = new Fl_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 45, 100, 30, "Connect");
    btn_conn->callback(connect_cb);
    
    Fl_Button *btn_disconn = new Fl_Button(UT_TESTAREA_X + 120, UT_TESTAREA_Y + 45, 100, 30, "Disconnect");
    btn_disconn->callback(disconnect_cb);
    
    Fl_Button *btn_sub = new Fl_Button(UT_TESTAREA_X + 230, UT_TESTAREA_Y + 45, 100, 30, "Subscribe");
    btn_sub->callback(subscribe_cb);
    
    Fl_Button *btn_pub = new Fl_Button(UT_TESTAREA_X + 340, UT_TESTAREA_Y + 45, 100, 30, "Publish");
    btn_pub->callback(publish_cb);
    
    textbuf = new Fl_Text_Buffer();
    textdisp = new Fl_Text_Display(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 85, UT_TESTAREA_W - 20, UT_TESTAREA_H - 105);
    textdisp->buffer(textbuf);
    textbuf->append("MQTT Client Test.\n");
    
    g->end();
    return g;
}

UnitTest mqtt_test(UT_TEST_MQTT, "MQTT", create_mqtt_test);
