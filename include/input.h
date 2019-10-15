#pragma once
#include <Windows.h>

enum KeyCode {
	NUM0 = 0x30,
	NUM1 = 0x31,
	NUM2 = 0x32,
	NUM3 = 0x33,
	NUM4 = 0x34,
	NUM5 = 0x35,
	NUM6 = 0x36,
	NUM7 = 0x37,
	NUM8 = 0x38,
	NUM9 = 0x39,
	A = 0x41,
	B = 0x42,
	C = 0x43,
	D = 0x44,
	E = 0x45,
	F = 0x46,
	G = 0x47,
	H = 0x48,
	I = 0x49,
	J = 0x4A,
	K = 0x4B,
	L = 0x4C,
	M = 0x4D,
	N = 0x4E,
	O = 0x4F,
	P = 0x50,
	Q = 0x51,
	R = 0x52,
	S = 0x53,
	T = 0x54,
	U = 0x55,
	V = 0x56,
	W = 0x57,
	X = 0x58,
	Y = 0x59,
	Z = 0x5A,
	UP = 0x26,
	DOWN = 0x28,
	LEFT = 0x25,
	RIGHT = 0x27,
	ESC = 0x1B,
	BACK = 0x08,
	NUMPAD0 = 0x60,
	NUMPAD1 = 0x61,
	NUMPAD2 = 0x62,
	NUMPAD3 = 0x63,
	NUMPAD4 = 0x64,
	NUMPAD5 = 0x65,
	NUMPAD6 = 0x66,
	NUMPAD7 = 0x67,
	NUMPAD8 = 0x68,
	NUMPAD9 = 0x69,
	SPACE = 0x20,
	ENTER = 0x0D,
	LSHIFT = 0xA0,
	RSHIFT = 0xA1,
	LCONTROL = 0xA2,
	RCONTROL = 0xA3,
	TAB = 0x09,
	MOUSELEFT = 0x01,
	MOUSERIGHT = 0x02,
	MOUSEMIDDLE = 0x04,
};

static const KeyCode AllKeys[] = {
	NUM0,
	NUM1,
	NUM2,
	NUM3,
	NUM4,
	NUM5,
	NUM6,
	NUM7,
	NUM8,
	NUM9,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	ESC,
	BACK,
	NUMPAD0,
	NUMPAD1,
	NUMPAD2,
	NUMPAD3,
	NUMPAD4,
	NUMPAD5,
	NUMPAD6,
	NUMPAD7,
	NUMPAD8,
	NUMPAD9,
	SPACE,
	ENTER,
	LSHIFT,
	RSHIFT,
	LCONTROL,
	RCONTROL,
	TAB,
};

static const int KEY_DOWN = 1;
static const int KEY_UP = 0;
static const int KEY_NULL = -1;

class Input
{
	class Key {
	public:
		int value = KEY_NULL;
		bool pressed = false;
	};

	static bool CheckKeyState(const KeyCode key)
	{
		if (GetFocus() == 0) return false;
		if (GetActiveWindow() == 0) return false;

		int val = (int)(GetAsyncKeyState((int)key) & 0x8000);
		SetKey((int)key, val);
		return val != 0;
	}

public:

	static void Update() {
		mouseWheel = 0;
		for (const auto e : AllKeys)
		{
			CheckKeyState(e);
		}
	}

	static void SetKey(const int key, const int value)
	{
		if (keys[key].value != value) keys[key].pressed = true;
		else
		{
			keys[key].pressed = false;
		}
		keys[key].value = value;
	}

	static bool GetKey(const KeyCode key)
	{
		return keys[key].value != 0;
	}

	static bool GetKeyDown(const KeyCode key) {
		return keys[key].value != 0 && keys[(int)key].pressed;
	}

	static bool GetKeyUp(const KeyCode key) {
		return keys[key].value == 0 && keys[(int)key].pressed;
	}

	static float GetMouseWheel()
	{
		return mouseWheel;
	}

	static void SetMouseWheel(const float mouseWheel)
	{
		Input::mouseWheel = mouseWheel;
	}


private:
	static Key keys[163];
	static float mouseWheel;
};