#ifndef CLISTATE_INCLUDED
#define CLISTATE_INCLUDED

#include "globals.h"
#include "data_format.h"
#include "cliaction.h"
#include "cliscreen.h"

void state_connect();

void state_loading();

void state_entry();

void state_room();

void state_game();

void state_end();

#endif