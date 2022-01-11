#include "StateMachine.h"

// Marderalbtraum SW
// Version 1.0
// Copyright 2021, Marc Singer, BBGS21.f

// Depends on LinkedList (https://github.com/ivanseidel/LinkedList), since I am not mad enough to implement that myself


StateMachine::StateMachine(){
  stateList = new LinkedList<State*>(); // States are in a linked list for each state
};

StateMachine::~StateMachine(){}; // Static instance of machine

// Runs the state machine
// This function has to be run in the main loop
void StateMachine::run(){
  if(stateList->size() == 0) return; // Return if there are no states

  // Run state 0 when StateMachine runs
  if(currentState == -1){
    currentState = 0;
  }
  
  int initialState = currentState; // Set current state
  int next = stateList->get(currentState)->execute(); // Get the next state that has to be run
  if(initialState == currentState){ // Check if we did not transition
    executeOnce = (currentState == next)?false:true; // Set this flag when we did not transition
    currentState = next;  // Set current state to next state
  }
}

// Add a state with a function pointer to the state machine
State* StateMachine::addState(void(*functionPointer)()){
  State* s = new State(); // Create new state
  s->stateLogic = functionPointer; // Set the logic of the state to the function passed in this function
  stateList->add(s); // Add state to the list
  s->index = stateList->size()-1; // Set state index
  return s; // Return the state
}

// Transition to a new state
State* StateMachine::transitionTo(State* s){
  this->currentState = s->index; // Update current state to new state
  this->executeOnce = true; // Set execute once to true
  return s; // Return the state
}

// Transition to a new state with index
int StateMachine::transitionTo(int i){
  if(i < stateList->size()){  // Check if new state index is valid
	this->currentState = i; // Update current state to new state
	this->executeOnce = true; // Set execute once to true
	return i; // Return index
  }
  return currentState; // Stay at the current state
}