class MenuState
{
	constructor()
	{
		picker = 0;
		mouseactive = false;
		mousex = Input.GetMouseX();
		mousey = Input.GetMouseY();
	}
	
	picker = 0;
	mouseactive = true;
	mousex = 0;
	mousey = 0;
	
	function Update()
	{
		if(mousex != Input.GetMouseX() || mousey != Input.GetMouseY())
		{
			mousex = Input.GetMouseX();
			mousey = Input.GetMouseY();
			
			if(96 < mousex && mousex < 184)
			{
				if(160 < mousey && mousey < 168)
					picker = 0;
				if(176 < mousey && mousey < 184)
					picker = 1;
				if(192 < mousey && mousey < 200)
					picker = 2;
				if(208 < mousey && mousey < 216)
					picker = 3;
				if(224 < mousey && mousey < 232)
					picker = 4;
			}
			
			mouseactive = true;
		}
		if(Input.isPressed(Input.Up))
		{
			picker -= 1;
			if(picker < 0)
				picker = 4;
			mouseactive = false;
		}
		if(Input.isPressed(Input.Down))
		{
			picker += 1;
			if(picker > 4)
				picker = 0;
			mouseactive = false;
		}
		if(Input.isPressed(Input.A) || Input.isPressed(Input.Start) || Input.isLeftClick())
		{
			switch(picker)
			{
			case 0: PushState(SoundTestState()); break;
			case 1: PushState(InputTestState()); break;
			case 2: PushState(SaveTestState()); break;
			case 3: PushState(OptionsState()); break;
			case 4: Globals.Exit(); break;
			}
		}
	}
	
	function Draw(BackBuffer)
	{
		BackBuffer.Draw("symbols", 66, 80, 160 + (picker * 16), 0);
		PrintString("Sound Test", BackBuffer, 96, 160, 45);
		PrintString("Input Test", BackBuffer, 96, 176, 45);
		PrintString("Save Test", BackBuffer, 96, 192, 45);
		PrintString("Options", BackBuffer, 96, 208, 45);
		PrintString("Exit", BackBuffer, 96, 224, 45);
		
		if(mouseactive)
			BackBuffer.Draw("mouse", 0, mousex, mousey, 1);
	}
}