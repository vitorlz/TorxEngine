#pragma once

#include <cstdint>
#include <bitset>

// -------------------- ECS ------------------------

// An entity is simply a number / index
using Entity = std::uint32_t;
// Used to define the size of arrays
const Entity MAX_ENTITIES = 5000;
// Components also have a unique ID
using ComponentType = std::uint8_t;
// Used to define the size of arrays
const ComponentType MAX_COMPONENTS = 32;

// This signature is simply a sequence of bits that we will use in order to keep track of which components an entity has and
// which components a system needs. We will compare the entity signature with the system signature to see if a given entity
// has all the components a system needs. For example, if MAX_COMPONENTS = 5, an entity could have a signature of 01001, meaning that
// it has the components corresponding to the second bit and the fifth bit. The components hold ID's which represesnt which bit in the signature 
// the correspond to.
using Signature = std::bitset<MAX_COMPONENTS>;

// ------------------- Input -----------------------

enum InputKey {
	NONE,
	W,
	A,
	S,
	D,
	F, 
	Q,
	V,
	K,
	SHIFT_W,
	SHIFT_A,
	SHIFT_S,
	SHIFT_D,
	SPACE,
	TAB,
	LEFT_CONTROL,
	ESC,
};


