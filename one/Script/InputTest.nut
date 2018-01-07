class InputTestState
{
	constructor()
	{
		x = 16;
		y = 16;
		w = 32;
		h = 32;
	}
	
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	
	function Update()
	{
		if(Input.isHeld(Input.Up))
			y-=1;
		if(Input.isHeld(Input.Down))
			y+=1;
		if(Input.isHeld(Input.Left))
			x-=1;
		if(Input.isHeld(Input.Right))
			x+=1;
		
		if(Input.isPressed(Input.B))
			PopState();
	}
	
	function Draw(BackBuffer)
	{
		BackBuffer.DrawSolid(x, y, w, h, 255);
	}
}