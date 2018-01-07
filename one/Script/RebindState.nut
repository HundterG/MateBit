class RebindState
{
	constructor()
	{
		state = 0;
		timer = 0;
		window = WindowObj(128, 136, 12, 12);
		window.cw = window.ch = 12;
	}
	
	state = 0;
	timer = 0;
	window = null;
	
	function Update()
	{
		switch(state)
		{
		case 0: Input.SetRebind(Input.Up); state = 1; break;
		case 1: if(Input.isPressed(Input.Up)) { Input.SetRebind(Input.Right); state = 2; } break;
		case 2: if(Input.isPressed(Input.Right)) { Input.SetRebind(Input.Down); state = 3; } break;
		case 3: if(Input.isPressed(Input.Down)) { Input.SetRebind(Input.Left); state = 4; } break;
		case 4: if(Input.isPressed(Input.Left)) { Input.SetRebind(Input.A); state = 5; } break;
		case 5: if(Input.isPressed(Input.A)) { Input.SetRebind(Input.B); state = 6; } break;
		case 6: if(Input.isPressed(Input.B)) { Input.SetRebind(Input.Start); state = 7; } break;
		case 7: if(Input.isPressed(Input.Start)) { Input.SetRebind(Input.Select); state = 8; } break;
		case 8: if(Input.isPressed(Input.Select)) state = 9; break;
		case 9: timer += 1; if(timer > 600) state = 10; break;
		case 10:
			if(Input.isPressed(Input.B))
				PopState();
			break;
		}
	}
	
	function Draw(BackBuffer)
	{
		PrintString("Rebind Keyboard", BackBuffer, 70, 20, 45);
		window.Draw(BackBuffer);
		local p = Pallet();
		for(local i=160 ; i<170 ; i+=1)
			p.Set(i, 0);
		
		switch(state)
		{
		case 1: p.Set(164, 224); PrintString("Rebind Up", BackBuffer, 70, 108, 45); break;
		case 2: p.Set(167, 224); PrintString("Rebind Right", BackBuffer, 70, 108, 45); break;
		case 3: p.Set(165, 224); PrintString("Rebind Down", BackBuffer, 70, 108, 45); break;
		case 4: p.Set(166, 224); PrintString("Rebind Left", BackBuffer, 70, 108, 45); break;
		case 5: p.Set(160, 224); PrintString("Rebind A", BackBuffer, 70, 108, 45); break;
		case 6: p.Set(161, 224); PrintString("Rebind B", BackBuffer, 70, 108, 45); break;
		case 7: p.Set(162, 224); PrintString("Rebind Start", BackBuffer, 70, 108, 45); break;
		case 8: p.Set(163, 224); PrintString("Rebind Select", BackBuffer, 70, 108, 45); break;
		case 9: 
			PrintString("Test Configuration", BackBuffer, 54, 108, 45);
			PrintString((10-(timer/60).tointeger()).tostring(), BackBuffer, 54, 116, 45);
		case 10:
			if(state == 10)
				PrintString("Press B to go back", BackBuffer, 54, 108, 45);
			if(Input.isHeld(Input.A))
				p.Set(160, 224);
			if(Input.isHeld(Input.B))
				p.Set(161, 224);
			if(Input.isHeld(Input.Start))
				p.Set(162, 224);
			if(Input.isHeld(Input.Select))
				p.Set(163, 224);
			if(Input.isHeld(Input.Up))
				p.Set(164, 224);
			if(Input.isHeld(Input.Down))
				p.Set(165, 224);
			if(Input.isHeld(Input.Left))
				p.Set(166, 224);
			if(Input.isHeld(Input.Right))
				p.Set(167, 224);
		}
		
		BackBuffer.Draw("controller", 0, 68, 140, 1, p);
	}
}
