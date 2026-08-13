//
// Sensor class for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#ifndef Fl_Sensor_H
#define Fl_Sensor_H

#include "Fl_Export.H"

/**
  Cross-platform Hardware Sensor wrapper.
  
  This class provides Qt-like capabilities for reading hardware sensors
  such as Accelerometer, Proximity, Compass, Magnetometer, and Gyroscope.
*/
class FL_EXPORT Fl_Sensor {
public:
  enum SensorType {
    Accelerometer,
    Proximity,
    Compass,
    Magnetometer,
    Gyroscope
  };

  struct SensorData {
    double x;
    double y;
    double z;
    double value; // Used for scalar sensors like Proximity
  };

private:
  SensorType type_;
  bool active_;
  void* platform_data_; // Reserved for platform-specific context if needed

public:
  /**
    Initializes a Sensor instance for the specified type.
    \param type The type of sensor (e.g., Accelerometer, Proximity).
  */
  Fl_Sensor(SensorType type);
  
  /**
    Cleans up the sensor instance, stopping it if active.
  */
  ~Fl_Sensor();

  /**
    Returns the type of the sensor.
  */
  SensorType type() const;

  /**
    Starts reading from the sensor.
    \return true if successfully started, false otherwise.
  */
  bool start();
  
  /**
    Stops reading from the sensor.
  */
  void stop();
  
  /**
    Checks if the sensor is currently active and reading data.
    \return true if active, false otherwise.
  */
  bool is_active() const;

  /**
    Reads the latest data from the sensor.
    \return A SensorData struct containing the readings.
  */
  SensorData read_data();
};

#endif
