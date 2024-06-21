#include <iostream>
#include <windows.h>

#include "spaceshield.h"

int main()
{
	SetConsoleOutputCP(65001);
	init_shield();

	system("pause");

	close_shield();
	return 0;
}