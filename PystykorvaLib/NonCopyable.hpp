#pragma once

#define NonCopyConstructible(Class) \
	Class(const Class &) = delete;

#define NonMoveConstructible(Class) \
	Class(Class&&) = delete;

#define NonCopyAssignable(Class) \
	Class& operator = (const Class&) = delete;

#define NonMoveAssignable(Class) \
	Class& operator = (Class&&) = delete;

#define NonCopyable(Class) \
	NonCopyConstructible(Class) \
	NonMoveConstructible(Class) \
	NonCopyAssignable(Class) \
	NonMoveAssignable(Class)

#define NonAssignable(Class) \
	NonCopyAssignable(Class) \
	NonMoveAssignable(Class)
