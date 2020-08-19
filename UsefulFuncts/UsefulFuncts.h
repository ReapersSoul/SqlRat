#pragma once
#include <iostream>
#include <Windows.h>

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <memory.h>

#define CHECK(a) if ((a)!= 0) return -1;

using namespace std;

namespace UFuncts {

	class StdOutRedirect
	{

	public:
		StdOutRedirect(int bufferSize) {
			BufferSize = bufferSize;
			if (_pipe(fdStdOutPipe, bufferSize, O_TEXT) != 0)
			{
				//treat error eventually
			}
			fdStdOut = _dup(_fileno(stdout));
		}

		~StdOutRedirect() {
			_close(fdStdOut);
			_close(fdStdOutPipe[1]);
			_close(fdStdOutPipe[0]);
		}

		int Start() {
			fflush(stdout);
			CHECK(_dup2(fdStdOutPipe[1], _fileno(stdout)));
			ios::sync_with_stdio();
			setvbuf(stdout, NULL, _IONBF, 0); // absolutely needed
			return 0;
		}
		int Stop() {
			CHECK(_dup2(fdStdOut, _fileno(stdout)));
			ios::sync_with_stdio();
			return 0;
		}
		int GetBuffer(string* buffer) {
			shared_ptr<char> tmp(new char[BufferSize]);
			int nOutRead = _read(fdStdOutPipe[0], tmp.get(), BufferSize);
			tmp.get()[nOutRead] = '\0';
			*buffer = string(tmp.get());
			return nOutRead;
		}

	private:
		int BufferSize;
		int fdStdOutPipe[2];
		int fdStdOut;
	};

	struct cout_redirect {
		cout_redirect(std::streambuf* new_buffer)
			: old(std::cout.rdbuf(new_buffer))
		{ }

		~cout_redirect() {
			std::cout.rdbuf(old);
		}

	private:
		std::streambuf* old;
	};

	void ColorConsole(int Fcolor, int Bcolor, bool Fbright, bool Bbright) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if (Fbright && Fcolor < 8 && Bbright && Bcolor < 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor | FOREGROUND_INTENSITY | BACKGROUND_INTENSITY);
		}
		else if (Fbright && Fcolor < 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor | FOREGROUND_INTENSITY);
		}
		else if (Bbright && Bcolor < 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor | BACKGROUND_INTENSITY);
		}
		else if (!Fbright && (Fcolor > 8) && !Bbright && (Bcolor > 8)) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 ^ BACKGROUND_INTENSITY | Fcolor ^ FOREGROUND_INTENSITY);
		}
		else if (!Fbright && Fcolor > 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor ^ FOREGROUND_INTENSITY);
		}
		else if (!Bbright && Bcolor > 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 ^ BACKGROUND_INTENSITY | Fcolor);
		}
		else {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor);
		}
	}

	int printfC(const char* _Format, int Fcolor, int Bcolor, bool Fbright, bool Bbright ...) {
		va_list list;
		va_start(list, Bbright);

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if (Fbright && Fcolor < 8 && Bbright && Bcolor < 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor | FOREGROUND_INTENSITY | BACKGROUND_INTENSITY);
		}
		else if (Fbright && Fcolor < 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor | FOREGROUND_INTENSITY);
		}
		else if (Bbright && Bcolor < 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor | BACKGROUND_INTENSITY);
		}
		else if (!Fbright && (Fcolor > 8) && !Bbright && (Bcolor > 8)) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 ^ BACKGROUND_INTENSITY | Fcolor ^ FOREGROUND_INTENSITY);
		}
		else if (!Fbright && Fcolor > 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor ^ FOREGROUND_INTENSITY);
		}
		else if (!Bbright && Bcolor > 8) {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 ^ BACKGROUND_INTENSITY | Fcolor);
		}
		else {
			SetConsoleTextAttribute(hConsole, Bcolor << 4 | Fcolor);
		}
		int ret = vprintf(_Format, list);
		SetConsoleTextAttribute(hConsole, 15);

		va_end(list);
		return ret;
	}
}