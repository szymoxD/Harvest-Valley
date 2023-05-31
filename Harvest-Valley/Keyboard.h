#pragma once
class Keyboard
{
	char keys[100]{};
	int keys_pressed;
public:
	Keyboard() { keys_pressed = 0; };
	int Pressed(char zn) const;
	void Add(char zn);
	void Remove(char zn);

};

