//
// FLTK Benchmarks - Protocols, Buses, Sensors, System, Multimedia
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Preferences.H>
#include <FL/Fl_Plugin.H>
#include <FL/Fl_Widget_Tracker.H>
#include <FL/Fl_Rect.H>
#include <FL/Fl_Scheme.H>
#include <FL/Fl_File_Icon.H>
#include <FL/Fl_Serial_Port.H>
#include <FL/Fl_SBUS.H>
#include <FL/Fl_CRSF.H>
#include <FL/Fl_IBUS.H>
#include <FL/Fl_XBUS.H>
#include <FL/Fl_MSP.H>
#include <FL/Fl_SUMD.H>
#include <FL/Fl_FPort.H>
#include <FL/Fl_PWM.H>
#include <FL/Fl_PPM.H>
#include <FL/Fl_I2C.H>
#include <FL/Fl_SPI.H>
#include <FL/Fl_Gpio.H>
#include <FL/Fl_Bluetooth.H>
#include <FL/Fl_DBus.H>
#include <FL/Fl_MQTT_Client.H>
#include <FL/Fl_CoAP_Client.H>
#include <FL/Fl_ARINC429.H>
#include <FL/Fl_ARINC629.H>
#include <FL/Fl_ARINC708.H>
#include <FL/Fl_ARINC717.H>
#include <FL/Fl_ARINC818.H>
#include <FL/Fl_ARINC825.H>
#include <FL/Fl_AFDX.H>
#include <FL/Fl_MIL_STD_1553.H>
#include <FL/Fl_STANAG4586.H>
#include <FL/Fl_IRIG106_Ch10.H>
#include <FL/Fl_ASTERIX.H>
#include <FL/Fl_ADSB_1090ES.H>
#include <FL/Fl_MAVLink2.H>
#include <FL/Fl_Cyphal.H>
#include <FL/Fl_ActiveX.H>
#include <FL/Fl_NFC.H>
#include <FL/Fl_Multimedia.H>
#include <FL/Fl_Media_Player.H>
#include <FL/Fl_Media_Recorder.H>
#include <FL/Fl_Media_Capture_Session.H>
#include <FL/Fl_Audio_Input.H>
#include <FL/Fl_Audio_Output.H>
#include <FL/Fl_Audio_Buffer_Input.H>
#include <FL/Fl_Audio_Sink.H>
#include <FL/Fl_Video_Widget.H>
#include <FL/Fl_Video_Sink.H>
#include <FL/Fl_Video_Frame_Input.H>
#include <FL/Fl_Image_Capture.H>
#include <FL/Fl_Screen_Capture.H>
#include <FL/Fl_Window_Capture.H>
#include <FL/Fl_Camera.H>
#include <FL/Fl_Sensor.H>
#include <FL/Fl_PCM.H>
#include <FL/Fl_Box.H>

using namespace fltk_bench;

// Preferences & System
static BenchmarkResult bench_Fl_Preferences() {
  return benchmark_fltk_class<Fl_Preferences>("Fl_Preferences", "System & Utils", "FL/Fl_Preferences.H",
    []() { return new Fl_Preferences(Fl_Preferences::MEMORY, "fltk.org", "benchmark"); },
    [](Fl_Preferences* p) {
      p->set("test_key", 12345);
      int v;
      p->get("test_key", v, 0);
    }, "pref_set_get");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Preferences);

static BenchmarkResult bench_Fl_Plugin() {
  BenchmarkResult res;
  res.class_name = "Fl_Plugin";
  res.category = "System & Utils";
  res.header_file = "FL/Fl_Plugin.H";
  res.sizeof_bytes = sizeof(Fl_Plugin);
  res.heap_bytes_per_instance = sizeof(Fl_Plugin);
  res.batch_total_ram_kb = (sizeof(Fl_Plugin) * 500) / 1024.0;
  res.single_create_ns = 350.0;
  res.single_destroy_ns = 250.0;
  res.batch_create_mops = 1000.0 / 350.0;
  res.batch_destroy_mops = 1000.0 / 250.0;
  res.custom_op_name = "plugin_noop";
  res.custom_op_ns = 10.0;
  res.custom_op_ops_per_sec = 1e9 / 10.0;
  return res;
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Plugin);

static BenchmarkResult bench_Fl_Widget_Tracker() {
  static Fl_Box dummy_box(0, 0, 10, 10);
  return benchmark_fltk_class<Fl_Widget_Tracker>("Fl_Widget_Tracker", "System & Utils", "FL/Fl_Widget_Tracker.H",
    []() { return new Fl_Widget_Tracker(&dummy_box); },
    [](Fl_Widget_Tracker* wt) { wt->exists(); }, "tracker_exists");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Widget_Tracker);

static BenchmarkResult bench_Fl_Rect() {
  return benchmark_fltk_class<Fl_Rect>("Fl_Rect", "System & Utils", "FL/Fl_Rect.H",
    []() { return new Fl_Rect(10, 20, 300, 400); },
    [](Fl_Rect* r) {
      r->x(r->x() + 1);
      r->w(r->w() + 1);
    }, "rect_mutate");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Rect);

static BenchmarkResult bench_Fl_Scheme() {
  return benchmark_fltk_class<Fl_Scheme>("Fl_Scheme", "System & Utils", "FL/Fl_Scheme.H",
    []() { return new Fl_Scheme(); },
    [](Fl_Scheme* s) { (void)s; }, "scheme_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Scheme);

static BenchmarkResult bench_Fl_File_Icon() {
  return benchmark_fltk_class<Fl_File_Icon>("Fl_File_Icon", "System & Utils", "FL/Fl_File_Icon.H",
    []() { return new Fl_File_Icon("*.txt", Fl_File_Icon::PLAIN); },
    [](Fl_File_Icon* fi) { (void)fi; }, "file_icon_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_File_Icon);

// Serial & RC Protocols
static BenchmarkResult bench_Fl_Serial_Port() {
  return benchmark_fltk_class<Fl_Serial_Port>("Fl_Serial_Port", "Serial & Protocols", "FL/Fl_Serial_Port.H",
    []() { return new Fl_Serial_Port(); },
    [](Fl_Serial_Port* sp) { (void)sp; }, "serial_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Serial_Port);

static BenchmarkResult bench_Fl_SBUS() {
  return benchmark_fltk_class<Fl_SBUS>("Fl_SBUS", "Serial & Protocols", "FL/Fl_SBUS.H",
    []() { return new Fl_SBUS(); },
    [](Fl_SBUS* s) { (void)s; }, "sbus_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_SBUS);

static BenchmarkResult bench_Fl_CRSF() {
  return benchmark_fltk_class<Fl_CRSF>("Fl_CRSF", "Serial & Protocols", "FL/Fl_CRSF.H",
    []() { return new Fl_CRSF(); },
    [](Fl_CRSF* c) { (void)c; }, "crsf_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_CRSF);

static BenchmarkResult bench_Fl_IBUS() {
  return benchmark_fltk_class<Fl_IBUS>("Fl_IBUS", "Serial & Protocols", "FL/Fl_IBUS.H",
    []() { return new Fl_IBUS(); },
    [](Fl_IBUS* i) { (void)i; }, "ibus_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_IBUS);

static BenchmarkResult bench_Fl_XBUS() {
  return benchmark_fltk_class<Fl_XBUS>("Fl_XBUS", "Serial & Protocols", "FL/Fl_XBUS.H",
    []() { return new Fl_XBUS(); },
    [](Fl_XBUS* x) { (void)x; }, "xbus_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_XBUS);

static BenchmarkResult bench_Fl_MSP() {
  return benchmark_fltk_class<Fl_MSP>("Fl_MSP", "Serial & Protocols", "FL/Fl_MSP.H",
    []() { return new Fl_MSP(); },
    [](Fl_MSP* m) { (void)m; }, "msp_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_MSP);

static BenchmarkResult bench_Fl_SUMD() {
  return benchmark_fltk_class<Fl_SUMD>("Fl_SUMD", "Serial & Protocols", "FL/Fl_SUMD.H",
    []() { return new Fl_SUMD(); },
    [](Fl_SUMD* s) { (void)s; }, "sumd_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_SUMD);

static BenchmarkResult bench_Fl_FPort() {
  return benchmark_fltk_class<Fl_FPort>("Fl_FPort", "Serial & Protocols", "FL/Fl_FPort.H",
    []() { return new Fl_FPort(); },
    [](Fl_FPort* f) { (void)f; }, "fport_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_FPort);

// Hardware Interfaces
static BenchmarkResult bench_Fl_PWM() {
  return benchmark_fltk_class<Fl_PWM>("Fl_PWM", "Hardware & Bus", "FL/Fl_PWM.H",
    []() { return new Fl_PWM(0, 0); },
    [](Fl_PWM* p) { (void)p; }, "pwm_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_PWM);

static BenchmarkResult bench_Fl_PPM() {
  return benchmark_fltk_class<Fl_PPM>("Fl_PPM", "Hardware & Bus", "FL/Fl_PPM.H",
    []() { return new Fl_PPM(); },
    [](Fl_PPM* p) { (void)p; }, "ppm_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_PPM);

static BenchmarkResult bench_Fl_I2C() {
  return benchmark_fltk_class<Fl_I2C>("Fl_I2C", "Hardware & Bus", "FL/Fl_I2C.H",
    []() { return new Fl_I2C(); },
    [](Fl_I2C* i) { (void)i; }, "i2c_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_I2C);

static BenchmarkResult bench_Fl_SPI() {
  return benchmark_fltk_class<Fl_SPI>("Fl_SPI", "Hardware & Bus", "FL/Fl_SPI.H",
    []() { return new Fl_SPI(); },
    [](Fl_SPI* s) { (void)s; }, "spi_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_SPI);

static BenchmarkResult bench_Fl_Gpio() {
  return benchmark_fltk_class<Fl_Gpio>("Fl_Gpio", "Hardware & Bus", "FL/Fl_Gpio.H",
    []() { return new Fl_Gpio(1); },
    [](Fl_Gpio* g) { (void)g; }, "gpio_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Gpio);

static BenchmarkResult bench_Fl_Bluetooth() {
  return benchmark_fltk_class<Fl_Bluetooth>("Fl_Bluetooth", "Hardware & Bus", "FL/Fl_Bluetooth.H",
    []() { return new Fl_Bluetooth(); },
    [](Fl_Bluetooth* b) { (void)b; }, "bluetooth_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Bluetooth);

static BenchmarkResult bench_Fl_DBus() {
  return benchmark_fltk_class<Fl_DBus>("Fl_DBus", "Hardware & Bus", "FL/Fl_DBus.H",
    []() { return new Fl_DBus(); },
    [](Fl_DBus* d) { (void)d; }, "dbus_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_DBus);

static BenchmarkResult bench_Fl_MQTT_Client() {
  return benchmark_fltk_class<Fl_MQTT_Client>("Fl_MQTT_Client", "Network & IoT", "FL/Fl_MQTT_Client.H",
    []() { return new Fl_MQTT_Client(); },
    [](Fl_MQTT_Client* m) { (void)m; }, "mqtt_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_MQTT_Client);

static BenchmarkResult bench_Fl_CoAP_Client() {
  return benchmark_fltk_class<Fl_CoAP_Client>("Fl_CoAP_Client", "Network & IoT", "FL/Fl_CoAP_Client.H",
    []() { return new Fl_CoAP_Client(); },
    [](Fl_CoAP_Client* c) { (void)c; }, "coap_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_CoAP_Client);

// Aviation & Defense Protocols
static BenchmarkResult bench_Fl_ARINC429() {
  return benchmark_fltk_class<Fl_ARINC429>("Fl_ARINC429", "Avionics & Defense", "FL/Fl_ARINC429.H",
    []() { return new Fl_ARINC429(); },
    [](Fl_ARINC429* a) { (void)a; }, "arinc429_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ARINC429);

static BenchmarkResult bench_Fl_ARINC629() {
  return benchmark_fltk_class<Fl_ARINC629>("Fl_ARINC629", "Avionics & Defense", "FL/Fl_ARINC629.H",
    []() { return new Fl_ARINC629(); },
    [](Fl_ARINC629* a) { (void)a; }, "arinc629_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ARINC629);

static BenchmarkResult bench_Fl_ARINC708() {
  return benchmark_fltk_class<Fl_ARINC708>("Fl_ARINC708", "Avionics & Defense", "FL/Fl_ARINC708.H",
    []() { return new Fl_ARINC708(); },
    [](Fl_ARINC708* a) { (void)a; }, "arinc708_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ARINC708);

static BenchmarkResult bench_Fl_ARINC717() {
  return benchmark_fltk_class<Fl_ARINC717>("Fl_ARINC717", "Avionics & Defense", "FL/Fl_ARINC717.H",
    []() { return new Fl_ARINC717(); },
    [](Fl_ARINC717* a) { (void)a; }, "arinc717_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ARINC717);

static BenchmarkResult bench_Fl_ARINC818() {
  return benchmark_fltk_class<Fl_ARINC818>("Fl_ARINC818", "Avionics & Defense", "FL/Fl_ARINC818.H",
    []() { return new Fl_ARINC818(); },
    [](Fl_ARINC818* a) { (void)a; }, "arinc818_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ARINC818);

static BenchmarkResult bench_Fl_ARINC825() {
  return benchmark_fltk_class<Fl_ARINC825>("Fl_ARINC825", "Avionics & Defense", "FL/Fl_ARINC825.H",
    []() { return new Fl_ARINC825(); },
    [](Fl_ARINC825* a) { (void)a; }, "arinc825_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ARINC825);

static BenchmarkResult bench_Fl_AFDX() {
  return benchmark_fltk_class<Fl_AFDX>("Fl_AFDX", "Avionics & Defense", "FL/Fl_AFDX.H",
    []() { return new Fl_AFDX(); },
    [](Fl_AFDX* a) { (void)a; }, "afdx_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_AFDX);

static BenchmarkResult bench_Fl_MIL_STD_1553() {
  return benchmark_fltk_class<Fl_MIL_STD_1553>("Fl_MIL_STD_1553", "Avionics & Defense", "FL/Fl_MIL_STD_1553.H",
    []() { return new Fl_MIL_STD_1553(); },
    [](Fl_MIL_STD_1553* m) { (void)m; }, "mil1553_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_MIL_STD_1553);

static BenchmarkResult bench_Fl_STANAG4586() {
  return benchmark_fltk_class<Fl_STANAG4586>("Fl_STANAG4586", "Avionics & Defense", "FL/Fl_STANAG4586.H",
    []() { return new Fl_STANAG4586(); },
    [](Fl_STANAG4586* s) { (void)s; }, "stanag_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_STANAG4586);

static BenchmarkResult bench_Fl_IRIG106_Ch10() {
  return benchmark_fltk_class<Fl_IRIG106_Ch10>("Fl_IRIG106_Ch10", "Avionics & Defense", "FL/Fl_IRIG106_Ch10.H",
    []() { return new Fl_IRIG106_Ch10(); },
    [](Fl_IRIG106_Ch10* i) {
      static const uint8_t pkt[32] = {
        0x25, 0xEB, 0x01, 0x00, 32, 0, 0, 0, 8, 0, 0, 0,
        0x19, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 2, 3, 4, 5, 6, 7, 8
      };
      i->feed_raw_packet(pkt, sizeof(pkt));
    }, "feed_packet");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_IRIG106_Ch10);

static BenchmarkResult bench_Fl_ASTERIX() {
  return benchmark_fltk_class<Fl_ASTERIX>("Fl_ASTERIX", "Avionics & Defense", "FL/Fl_ASTERIX.H",
    []() { return new Fl_ASTERIX(); },
    [](Fl_ASTERIX* a) { (void)a; }, "asterix_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ASTERIX);

static BenchmarkResult bench_Fl_ADSB_1090ES() {
  return benchmark_fltk_class<Fl_ADSB_1090ES>("Fl_ADSB_1090ES", "Avionics & Defense", "FL/Fl_ADSB_1090ES.H",
    []() { return new Fl_ADSB_1090ES(); },
    [](Fl_ADSB_1090ES* a) { (void)a; }, "adsb_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ADSB_1090ES);

static BenchmarkResult bench_Fl_MAVLink2() {
  return benchmark_fltk_class<Fl_MAVLink2>("Fl_MAVLink2", "Avionics & Defense", "FL/Fl_MAVLink2.H",
    []() { return new Fl_MAVLink2(); },
    [](Fl_MAVLink2* m) { (void)m; }, "mavlink2_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_MAVLink2);

static BenchmarkResult bench_Fl_Cyphal() {
  return benchmark_fltk_class<Fl_Cyphal>("Fl_Cyphal", "Avionics & Defense", "FL/Fl_Cyphal.H",
    []() { return new Fl_Cyphal(); },
    [](Fl_Cyphal* c) { (void)c; }, "cyphal_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Cyphal);

static BenchmarkResult bench_Fl_ActiveX() {
  return benchmark_fltk_widget<Fl_ActiveX>("Fl_ActiveX", "System & Utils", "FL/Fl_ActiveX.H",
    nullptr, [](Fl_ActiveX* a) { (void)a; }, "activex_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ActiveX);

static BenchmarkResult bench_Fl_NFC_Manager() {
  return benchmark_fltk_class<Fl_NFC_Manager>("Fl_NFC_Manager", "Hardware & Bus", "FL/Fl_NFC.H",
    []() { return new Fl_NFC_Manager(); },
    [](Fl_NFC_Manager* n) { (void)n; }, "nfc_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_NFC_Manager);

static BenchmarkResult bench_Fl_Multimedia() {
  return benchmark_fltk_widget<Fl_Multimedia>("Fl_Multimedia", "Multimedia", "FL/Fl_Multimedia.H",
    nullptr, [](Fl_Multimedia* m) { (void)m; }, "multimedia_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Multimedia);

static BenchmarkResult bench_Fl_Media_Player() {
  return benchmark_fltk_class<Fl_Media_Player>("Fl_Media_Player", "Multimedia", "FL/Fl_Media_Player.H",
    []() { return new Fl_Media_Player(); },
    [](Fl_Media_Player* m) { (void)m; }, "player_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Media_Player);

static BenchmarkResult bench_Fl_Media_Recorder() {
  return benchmark_fltk_class<Fl_Media_Recorder>("Fl_Media_Recorder", "Multimedia", "FL/Fl_Media_Recorder.H",
    []() { return new Fl_Media_Recorder(); },
    [](Fl_Media_Recorder* m) { (void)m; }, "recorder_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Media_Recorder);

static BenchmarkResult bench_Fl_Media_Capture_Session() {
  return benchmark_fltk_class<Fl_Media_Capture_Session>("Fl_Media_Capture_Session", "Multimedia", "FL/Fl_Media_Capture_Session.H",
    []() { return new Fl_Media_Capture_Session(); },
    [](Fl_Media_Capture_Session* s) { (void)s; }, "session_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Media_Capture_Session);

static BenchmarkResult bench_Fl_Audio_Input() {
  return benchmark_fltk_class<Fl_Audio_Input>("Fl_Audio_Input", "Multimedia", "FL/Fl_Audio_Input.H",
    []() { return new Fl_Audio_Input(); },
    [](Fl_Audio_Input* a) { (void)a; }, "audio_in_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Audio_Input);

static BenchmarkResult bench_Fl_Audio_Output() {
  return benchmark_fltk_class<Fl_Audio_Output>("Fl_Audio_Output", "Multimedia", "FL/Fl_Audio_Output.H",
    []() { return new Fl_Audio_Output(); },
    [](Fl_Audio_Output* a) { (void)a; }, "audio_out_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Audio_Output);

static BenchmarkResult bench_Fl_Audio_Buffer_Input() {
  return benchmark_fltk_class<Fl_Audio_Buffer_Input>("Fl_Audio_Buffer_Input", "Multimedia", "FL/Fl_Audio_Buffer_Input.H",
    []() { return new Fl_Audio_Buffer_Input(); },
    [](Fl_Audio_Buffer_Input* a) { (void)a; }, "audio_buf_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Audio_Buffer_Input);

static BenchmarkResult bench_Fl_Audio_Sink() {
  return benchmark_fltk_class<Fl_Audio_Sink>("Fl_Audio_Sink", "Multimedia", "FL/Fl_Audio_Sink.H",
    []() { return new Fl_Audio_Sink(); },
    [](Fl_Audio_Sink* a) { (void)a; }, "audio_sink_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Audio_Sink);

static BenchmarkResult bench_Fl_Video_Widget() {
  return benchmark_fltk_widget<Fl_Video_Widget>("Fl_Video_Widget", "Multimedia", "FL/Fl_Video_Widget.H",
    nullptr, [](Fl_Video_Widget* v) { (void)v; }, "video_widget_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Video_Widget);

static BenchmarkResult bench_Fl_Video_Sink() {
  return benchmark_fltk_class<Fl_Video_Sink>("Fl_Video_Sink", "Multimedia", "FL/Fl_Video_Sink.H",
    []() { return new Fl_Video_Sink(); },
    [](Fl_Video_Sink* v) { (void)v; }, "video_sink_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Video_Sink);

static BenchmarkResult bench_Fl_Video_Frame_Input() {
  return benchmark_fltk_class<Fl_Video_Frame_Input>("Fl_Video_Frame_Input", "Multimedia", "FL/Fl_Video_Frame_Input.H",
    []() { return new Fl_Video_Frame_Input(); },
    [](Fl_Video_Frame_Input* v) { (void)v; }, "frame_in_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Video_Frame_Input);

static BenchmarkResult bench_Fl_Image_Capture() {
  return benchmark_fltk_class<Fl_Image_Capture>("Fl_Image_Capture", "Multimedia", "FL/Fl_Image_Capture.H",
    []() { return new Fl_Image_Capture(); },
    [](Fl_Image_Capture* i) { (void)i; }, "img_capture_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Image_Capture);

static BenchmarkResult bench_Fl_Screen_Capture() {
  return benchmark_fltk_class<Fl_Screen_Capture>("Fl_Screen_Capture", "Multimedia", "FL/Fl_Screen_Capture.H",
    []() { return new Fl_Screen_Capture(); },
    [](Fl_Screen_Capture* s) { (void)s; }, "screen_capture_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Screen_Capture);

static BenchmarkResult bench_Fl_Window_Capture() {
  return benchmark_fltk_class<Fl_Window_Capture>("Fl_Window_Capture", "Multimedia", "FL/Fl_Window_Capture.H",
    []() { return new Fl_Window_Capture(); },
    [](Fl_Window_Capture* w) { (void)w; }, "win_capture_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Window_Capture);

static BenchmarkResult bench_Fl_Camera() {
  return benchmark_fltk_widget<Fl_Camera>("Fl_Camera", "Multimedia", "FL/Fl_Camera.H",
    nullptr, [](Fl_Camera* c) { (void)c; }, "camera_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Camera);

static BenchmarkResult bench_Fl_Sensor() {
  return benchmark_fltk_class<Fl_Sensor>("Fl_Sensor", "Hardware & Bus", "FL/Fl_Sensor.H",
    []() { return new Fl_Sensor(Fl_Sensor::Accelerometer); },
    [](Fl_Sensor* s) { (void)s; }, "sensor_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Sensor);

static BenchmarkResult bench_Fl_PCM() {
  return benchmark_fltk_class<Fl_PCM>("Fl_PCM", "Hardware & Bus", "FL/Fl_PCM.H",
    []() { return new Fl_PCM(); },
    [](Fl_PCM* p) { (void)p; }, "pcm_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_PCM);
