#ifndef _MYWIN_
#define _MYWIN_

bool WindowInit(void);
void WindowUpdate(void);
void WindowDraw(void);
void WindowPresentRequest(unsigned char buffer[65536]);
void WindowErrorMsg(char const *msg);
void WindowExit(void);

#endif
