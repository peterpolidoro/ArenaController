// ----------------------------------------------------------------------------
// ArenaController.hpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#ifndef ARENA_CONTROLLER_HPP
#define ARENA_CONTROLLER_HPP

#include <Arduino.h>

#include "Display.hpp"
#include "Storage.hpp"


struct ArenaController
{
  void setup();

  arena_controller::Display display;
  arena_controller::Storage storage;
};

#endif
