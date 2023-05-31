#pragma once
class Keyboard
{
	char keys[100]{};
	bool blocked[100]{};
	int keys_pressed;
public:
	Keyboard() { keys_pressed = 0; };
	int Pressed(char zn) const;
	bool switched(char zn) const;
	void block(char zn);
	void Add(char zn);
	void Remove(char zn);

};

