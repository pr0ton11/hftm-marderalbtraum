#include "State.h"

// Marderalbtraum SW
// Version 1.0
// Copyright 2021, Marc Singer, BBGS21.f

// Depends on LinkedList (https://github.com/ivanseidel/LinkedList), since I am not mad enough to implement that myself

// Constructor of state
State::State(){
  transitions = new LinkedList<struct Transition*>(); // Transitions are in a linked list for each state
};

State::~State(){}; // Static instance of state

// Adds a Transition based on a boolean condition function
// Accepts a state object to transition to (but still uses its index)
void State::addTransition(bool (*conditionFunction)(), State* s){
  struct Transition* t = new Transition{conditionFunction,s->index}; // Create a new transition with conditionfunction and state index
  transitions->add(t); // Add the new transition to the LinkedList
}

// Adds a Transition based on a boolean condition function
// Accepts a state number to transition to
void State::addTransition(bool (*conditionFunction)(), int stateNumber){
  struct Transition* t = new Transition{conditionFunction,stateNumber}; // Create a new transition with conditionfunction and state index
  transitions->add(t); // Add the new transition to the LinkedList
}

// Evaluates transitions
int State::evalTransitions(){
  if(transitions->size() == 0) return index;  // Return current index if no transition is avaialbe
  bool result = false;  // Holds result (if we should transition or not
  
  for(int i=0; i < transitions->size(); i++){  // Loop all transitions
    result = transitions->get(i)->conditionFunction(); // Call transition functions and get bool result
    if(result == true){
      return transitions->get(i)->stateNumber; // Return the new index after transition
    }
  }
  return index;  // Return current index (no transition has happened)
}

// Runs the state and evaluates transitions
int State::execute(){
  stateLogic(); // Runs the logic of the current state (pointer to function)
  return evalTransitions(); // Evaluates all possible transitions
}

// Sets the index of a transition to a specific state index
int State::setTransition(int index, int state){
	if(transitions->size() == 0) return -1;
	transitions->get(index)->stateNumber = state;
	return state;
}