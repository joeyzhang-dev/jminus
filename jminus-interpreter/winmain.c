#ifdef _WIN32
#include <windows.h>
// Forward‐declare your normal main
extern int main(int argc, char** argv);
// This is the entry point the MinGW CRT will look for
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmdLine, int show) {
    // Just forward to your real main (which uses __argc/__argv under MinGW)
    return main(__argc, __argv);
}
#endif
