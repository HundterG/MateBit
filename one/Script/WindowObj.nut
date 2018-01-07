class WindowObj
{
	wx = 0;
	wy = 0;
	ww = 0;
	wh = 0;
	
	cw = 0;
	ch = 0;
	
	timer = 0;
	
	constructor(x, y, w, h)
	{
		wx = x;
		wy = y;
		ww = w;
		wh = h;
		
		cw = 0;
		ch = 0;
		
		timer = 0;
	}
	
	function Update()
	{
		timer += 1;
		if(timer > 3)
		{
			if(cw < ww)
				cw += 1;
			if(cw > ww)
				cw -= 1;
			if(ch < wh)
				ch += 1;
			if(ch > wh)
				ch -= 1;
			timer = 0;
		}
	}
	
	function isAdjusting()
	{
		return cw != ww || ch != wh;
	}
	
	function Draw(BackBuffer, ...)
	{
		if(cw > 0 && ch > 0)
		{
			// draw the middle
			for(local y = -(ch-1) ; y < ch-1 ; y += 1)
			{
				BackBuffer.Draw("window", 0, wx - (cw * 8), wy + (y * 8), 1, Transform(false,false,false,true), vargv);
				BackBuffer.Draw("window", 0, wx + ((cw-1) * 8), wy + (y * 8), 1, Transform(false,false,true,false), vargv);
			}
			BackBuffer.DrawSolid(wx - ((cw-1) * 8), wy - ((ch-1) * 8), (cw-1) * 16, (ch-1) * 16, 0);
			
			// draw the horazontal bars
			for(local x = -(cw-1) ; x < cw-1 ; x += 1)
			{
				BackBuffer.Draw("window", 0, wx + (x * 8), wy - (ch * 8), 1, vargv);
				BackBuffer.Draw("window", 0, wx + (x * 8), wy + ((ch-1) * 8), 1, Transform(false,true,false,false), vargv);
			}
			
			
			// draw the corners
			BackBuffer.Draw("window", 1, wx - (cw * 8), wy - (ch * 8), 1, vargv);
			BackBuffer.Draw("window", 1, wx + ((cw-1) * 8), wy - (ch * 8), 1, Transform(true,false,false,false), vargv);
			BackBuffer.Draw("window", 1, wx - (cw * 8), wy + ((ch-1) * 8), 1, Transform(false,true,false,false), vargv);
			BackBuffer.Draw("window", 1, wx + ((cw-1) * 8), wy + ((ch-1) * 8), 1, Transform(true,true,false,false), vargv);
		}
	}
}
