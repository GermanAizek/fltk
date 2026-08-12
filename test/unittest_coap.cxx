//
// Fl_CoAP_Client test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//

#include <FL/Fl_CoAP_Client.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Text_Display.H>
#include "unittests.h"

static Fl_CoAP_Client *coap = 0;
static Fl_Text_Buffer *textbuf = 0;
static Fl_Text_Display *textdisp = 0;
static Fl_Input *host_input = 0;
static Fl_Input *port_input = 0;
static Fl_Input *path_input = 0;
static Fl_Choice *method_choice = 0;

static void coap_response_cb(Fl_CoAP_Client* client, int message_id, int code, const void* payload, int payload_len, void* data) {
    (void)client; (void)data;
    char buf[1024];
    std::string msg((const char*)payload, payload_len);
    snprintf(buf, sizeof(buf), "Response [ID: %d, Code: %d.%02d]: %s\n", message_id, code >> 5, code & 0x1F, msg.c_str());
    textbuf->append(buf);
}

static void connect_cb(Fl_Widget *w, void *data) {
    if (!coap) return;
    
    coap->set_hostname(host_input->value());
    coap->set_port(atoi(port_input->value()));
    
    textbuf->append("Connecting...\n");
    if (coap->connect() == 0) {
        textbuf->append("UDP Socket connected.\n");
    } else {
        textbuf->append("Failed to initiate connection.\n");
    }
}

static void disconnect_cb(Fl_Widget *w, void *data) {
    if (!coap) return;
    coap->disconnect();
    textbuf->append("Disconnected.\n");
}

static void send_cb(Fl_Widget *w, void *data) {
    if (!coap || !coap->is_connected()) {
        textbuf->append("Not connected, cannot send request.\n");
        return;
    }
    
    Fl_CoAP_Client::Method m = (Fl_CoAP_Client::Method)(method_choice->value() + 1);
    int msg_id = coap->request(m, path_input->value(), "Hello from FLTK CoAP Client!");
    
    if (msg_id > 0) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Sent request (ID: %d) to '%s'\n", msg_id, path_input->value());
        textbuf->append(buf);
    } else {
        textbuf->append("Failed to send request.\n");
    }
}

Fl_Widget *create_coap_test() {
    Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
    g->begin();
    
    coap = new Fl_CoAP_Client();
    coap->set_response_callback(coap_response_cb);
    
    host_input = new Fl_Input(UT_TESTAREA_X + 60, UT_TESTAREA_Y + 10, 150, 25, "Host:");
    host_input->value("coap.me");
    
    port_input = new Fl_Input(UT_TESTAREA_X + 260, UT_TESTAREA_Y + 10, 60, 25, "Port:");
    port_input->value("5683");
    
    path_input = new Fl_Input(UT_TESTAREA_X + 60, UT_TESTAREA_Y + 45, 150, 25, "Path:");
    path_input->value("test");
    
    method_choice = new Fl_Choice(UT_TESTAREA_X + 280, UT_TESTAREA_Y + 45, 80, 25, "Method:");
    method_choice->add("GET");
    method_choice->add("POST");
    method_choice->add("PUT");
    method_choice->add("DELETE");
    method_choice->value(0); // GET
    
    Fl_Button *btn_conn = new Fl_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 80, 100, 30, "Connect");
    btn_conn->callback(connect_cb);
    
    Fl_Button *btn_disconn = new Fl_Button(UT_TESTAREA_X + 120, UT_TESTAREA_Y + 80, 100, 30, "Disconnect");
    btn_disconn->callback(disconnect_cb);
    
    Fl_Button *btn_send = new Fl_Button(UT_TESTAREA_X + 230, UT_TESTAREA_Y + 80, 100, 30, "Send Request");
    btn_send->callback(send_cb);
    
    textbuf = new Fl_Text_Buffer();
    textdisp = new Fl_Text_Display(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 120, UT_TESTAREA_W - 20, UT_TESTAREA_H - 140);
    textdisp->buffer(textbuf);
    textbuf->append("CoAP Client Test.\n");
    
    g->end();
    return g;
}

UnitTest coap_test(UT_TEST_COAP, "CoAP", create_coap_test);
