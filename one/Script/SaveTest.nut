class SaveTestState
{
	constructor()
	{
		number = 0;
	}
	
	number = 0;
	
	function Update()
	{
		if(Input.isPressed(Input.Up))
		{
			local f = SaveFile();
			if(f.Load("TestSave.sav"))
				number = f.ReadInt();
		}
		
		if(Input.isPressed(Input.Down))
		{
			local f = SaveFile();
			f.WriteInt(number);
			f.Save("TestSave.sav");
		}
		
		if(Input.isPressed(Input.Select) || Input.isPressed(Input.A))
			number+=1;
		
		if(Input.isPressed(Input.B))
			PopState();
	}
	
	function Draw(BackBuffer)
	{
		PrintString("Up to load", BackBuffer, 68, 60, 45);
		PrintString("Down to save", BackBuffer, 68, 68, 45);
		PrintString("Current number: " + number.tostring(), BackBuffer, 68, 84, 45);
	}
}