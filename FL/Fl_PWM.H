//
// PWM class for the Fast Light Tool Kit (FLTK).
//
// Copyright 2026 by Herman Semenoff (GermanAizek)
//
#ifndef Fl_PWM_H
#define Fl_PWM_H

#include "Fl_Export.H"
#include "Fl.H"

/**
  Cross-platform PWM (Pulse Width Modulation) / Analog device wrapper.
  
  This class provides a uniform interface to control PWM channels.
  On Linux, it utilizes the standard sysfs interface (`/sys/class/pwm/`).
  On other platforms (Windows, macOS), the methods act as stubs.
*/
class FL_EXPORT Fl_PWM {
  int chip_;
  int channel_;

  int export_channel();
  int unexport_channel();

public:
  /**
    Initializes a PWM instance for the specified chip and channel.
    The channel is exported if available on the system.
  */
  Fl_PWM(int chip, int channel);
  
  /**
    Cleans up the PWM instance, unexporting the channel.
  */
  ~Fl_PWM();

  /** Returns the hardware chip number this object controls. */
  int chip() const { return chip_; }
  /** Returns the hardware channel number this object controls. */
  int channel() const { return channel_; }

  /**
    Sets the period of the PWM signal in nanoseconds.
    \param p Period in nanoseconds
    \return 0 on success, or -1 on error
  */
  int period(long p);
  
  /**
    Reads the current period of the PWM signal.
    \return Period in nanoseconds, or -1 on error
  */
  long period() const;

  /**
    Sets the duty cycle of the PWM signal in nanoseconds.
    \param d Duty cycle in nanoseconds
    \return 0 on success, or -1 on error
  */
  int duty_cycle(long d);
  
  /**
    Reads the current duty cycle of the PWM signal.
    \return Duty cycle in nanoseconds, or -1 on error
  */
  long duty_cycle() const;
  
  /**
    Sets the duty cycle as a percentage / ratio (0.0 to 1.0)
    This automatically calculates the nanosecond value based on the current period.
    \param percent Duty cycle ratio (0.0 to 1.0)
    \return 0 on success, or -1 on error
  */
  int duty_cycle_percent(double percent);

  /**
    Enables or disables the PWM output.
    \param e 1 to enable, 0 to disable
    \return 0 on success, or -1 on error
  */
  int enable(int e);
  
  /**
    Reads the current enable state.
    \return 1 if enabled, 0 if disabled, or -1 on error
  */
  int enable() const;
};

#endif
