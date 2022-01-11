#include <LinkedList.h>

// Marderalbtraum SW
// Version 1.0
// Copyright 2021, Marc Singer, BBGS21.f

// Depends on LinkedList (https://github.com/ivanseidel/LinkedList), since I am not mad enough to implement that myself

#ifndef _STATE_H // Do not define this header more than once
#define _STATE_H

// Definition of Transition
struct Transition{
  bool (*conditionFunction)(); // Condition to change to this state
  int stateNumber; // Number of this state
};


class State{
  public:
    State();
    ~State();

	void addTransition(bool (*c)(), State* s); // Adds a transition with a state object
    void addTransition(bool (*c)(), int stateNumber); // Adds a transition with the state index
    int evalTransitions(); // Evaluates the Transitions
    int execute();  // Execute the states
    int setTransition(int index, int stateNumber);  // Sets a transition with an index and state number
	
    void (*stateLogic)(); // Pointer to the logic of the state
    LinkedList<struct Transition*> *transitions; // Transitions of the state
	int index; // Index if the state
};

#endif