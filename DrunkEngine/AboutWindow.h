#ifndef _ABOUTWINDOW_H_
#define _ABOUTWINDOW_H_

#include "Window.h"

class AboutWindow : public Window
{
public:
	AboutWindow(Application* app);
	virtual ~AboutWindow();

	void Draw() override;

public:

};

#endif