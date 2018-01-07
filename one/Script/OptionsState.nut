// fix all sizes to be power of 8
class OptionsState
{
	constructor(opend = false)
	{
		picker = 0;
		mouseactive = false;
		mousex = Input.GetMouseX();
		mousey = Input.GetMouseY();
		window = WindowObj(128, 136, 12, 12);
		if(opend)
		{
			window.cw = 12;
			window.ch = 12;
		}
		moveTimer = 0;
	}
	
	picker = 0;
	mouseactive = true;
	mousex = 0;
	mousey = 0;
	window = null;
	moveTimer = 0;
	
	function Update()
	{
		if(mousex != Input.GetMouseX() || mousey != Input.GetMouseY())
		{
			mousex = Input.GetMouseX();
			mousey = Input.GetMouseY();
			mouseactive = true;
			
			if(68 < mousex && mousex < 176 && 60 < mousey && mousey < 86)
				picker = 0;
			if(68 < mousex && mousex < 176 && 92 < mousey && mousey < 118)
				picker = 1;
			if(68 < mousex && mousex < 176 && 124 < mousey && mousey < 152)
				picker = 2;
			if(68 < mousex && mousex < 176 && 156 < mousey && mousey < 168)
				picker = 3;
		}
		if(Input.isPressed(Input.Up))
		{
			picker -= 1;
			if(picker < 0)
				picker = 3;
			mouseactive = false;
		}
		if(Input.isPressed(Input.Down))
		{
			picker += 1;
			if(picker > 3)
				picker = 0;
			mouseactive = false;
		}

		switch(picker)
		{
		case 0:
			if(Input.isLeftClick())
			{
				if(92 < mousex && mousex < 116)
					Globals.SetFullScreen(false);
				if(156 < mousex && mousex < 180)
					Globals.SetFullScreen(true);
			}
			if(Input.isPressed(Input.Left) || Input.isPressed(Input.Right))
				Globals.SetFullScreen(!Globals.GetFullScreen());
			break;
		case 1:
			if(Input.isLeftClick())
			{
				if(84 < mousex && mousex < 108)
					Globals.SetAspectRatio(Globals.x11);
				if(116 < mousex && mousex < 140)
					Globals.SetAspectRatio(Globals.x34);
				if(162 < mousex && mousex < 186)
					Globals.SetAspectRatio(Globals.xany);
			}
			if(Input.isPressed(Input.Right))
			{
				switch(Globals.GetAspectRatio())
				{
				case Globals.x11: Globals.SetAspectRatio(Globals.x34); break;
				case Globals.x34: Globals.SetAspectRatio(Globals.xany); break;
				case Globals.xany: Globals.SetAspectRatio(Globals.x11); break;
				}
			}
			if(Input.isPressed(Input.Left))
			{
				switch(Globals.GetAspectRatio())
				{
				case Globals.x11: Globals.SetAspectRatio(Globals.xany); break;
				case Globals.x34: Globals.SetAspectRatio(Globals.x11); break;
				case Globals.xany: Globals.SetAspectRatio(Globals.x34); break;
				}
			}
			break;
		case 2:
			if(Input.isLeftHeld())
				Globals.SetVolume(mousex - 82);
			if(Input.isHeld(Input.Right))
			{
				if(moveTimer == 0 || 30 < moveTimer)
					Globals.SetVolume(Globals.GetVolume() + 1);
				moveTimer += 1;
			}
			else if(Input.isHeld(Input.Left))
			{
				if(moveTimer == 0 || 30 < moveTimer)
					Globals.SetVolume(Globals.GetVolume() - 1);
				moveTimer += 1;
			}
			else
				moveTimer = 0;
			break;
		case 3:
			if(Input.isPressed(Input.A) || Input.isPressed(Input.Start) || Input.isLeftClick())
				PushState(RebindState());
			break;
		}
		
		if(Input.isPressed(Input.B))
			PopState();
		
		window.Update();
	}
	
	function Draw(BackBuffer)
	{
		PrintString("Options", BackBuffer, 90, 20, 45);
		window.Draw(BackBuffer);
		if(!window.isAdjusting())
		{
			local p = Pallet();
			
			PrintString("FullScreen", BackBuffer, 68, 60, 45);
			if(Globals.GetFullScreen()) p.Set(255, 73);
			else p.Set(255, 255);
			PrintString("Off", BackBuffer, 92, 76, 45, p);
			if(!Globals.GetFullScreen()) p.Set(255, 73);
			else p.Set(255, 255);
			PrintString("On", BackBuffer, 156, 76, 45, p);
			
			PrintString("Aspect Ratio", BackBuffer, 68, 92, 45);
			if(Globals.GetAspectRatio() == Globals.x11) p.Set(255, 255);
			else p.Set(255, 73);
			PrintString("1x1", BackBuffer, 84, 108, 45, p);
			if(Globals.GetAspectRatio() == Globals.x34) p.Set(255, 255);
			else p.Set(255, 73);
			PrintString("4x3", BackBuffer, 116, 108, 45, p);
			if(Globals.GetAspectRatio() == Globals.xany) p.Set(255, 255);
			else p.Set(255, 73);
			PrintString("ANY", BackBuffer, 162, 108, 45, p);
			
			PrintString("Sound", BackBuffer, 68, 124, 45);
			PrintString("Bar", BackBuffer, 84, 140, 45);
			BackBuffer.Draw("soundbar", 1, 80, 140);
			for(local i=11 ; i<22 ; i+=1) BackBuffer.Draw("soundbar", 0, i*8, 140);
			BackBuffer.Draw("soundbar", 2, 176, 140);
			BackBuffer.Draw("soundbar", 3, 78 + (Globals.GetVolume()).tointeger(), 144, 0);
			BackBuffer.Draw("soundbar", 4, 78 + (Globals.GetVolume()).tointeger(), 136, 0);
			
			PrintString("ReBind Keyboard", BackBuffer, 68, 156, 45);
			
			BackBuffer.Draw("symbols", 66, 52, 60 + (picker * 32), 0);
		}
		
		if(mouseactive)
			BackBuffer.Draw("mouse", 0, mousex, mousey, 1);
	}
}