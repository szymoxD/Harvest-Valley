#include "Keyboard.h"

int Keyboard::Pressed(char zn) const {
	for (int i = 0; i < keys_pressed; i++)
		if (keys[i] == zn)
			return i;
	return -1;
}
bool Keyboard::switched(char zn) const {
	for (int i = 0; i < keys_pressed; i++)
		if (keys[i] == zn)
			return blocked[i];
	return false;
}
void Keyboard::block(char zn) {
	for (int i = 0; i < keys_pressed; i++)
		if (keys[i] == zn)
			blocked[i] = true;
}
void Keyboard::Add(char zn) {
	if (Pressed(zn) == -1) {
		blocked[keys_pressed] = false;
		keys[keys_pressed++] = zn;
	}
}
void Keyboard::Remove(char zn) {
	int i = Pressed(zn);
	if (i != -1) {
		for (int j = i; j < keys_pressed - 1; j++) {
			keys[j] = keys[j + 1];
			blocked[j] = blocked[j + 1];
		}
		keys_pressed--;
	}
}