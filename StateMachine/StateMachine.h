#include <LinkedList.h>
#include "State.h"

// Marderalbtraum SW
// Version 1.0
// Copyright 2021, Marc Singer, BBGS21.f

// Depends on LinkedList (https://github.com/ivanseidel/LinkedList), since I am not mad enough to implement that myself

#ifndef _STATEMACHINE_H // Do not define this header more than once
#define _STATEMACHINE_H

class StateMachine
{
  public:
    
    StateMachine(); // Constructor
    ~StateMachine(); // Static object reference
    void init(); // Initializing the state machine
    void run(); // Run the state machine

    State* addState(void (*functionPointer)()); // Add a state to the machine
    State* transitionTo(State* s); // Transitioning to state (state between states)
    int transitionTo(int i); // Transition function
	
    LinkedList<State*> *stateList; // List of states
	  bool executeOnce = true; // Flag that is set when the state runs the first time after transition
    int currentState = -1;  // Current state
};

#endif