function PrintString(str, BackBuffer, x, y, xwidth, ...)
{
	local curx = 0;
	local wordarray = split(str, " ");
	local charindex = 0;
	foreach(word in wordarray)
	{
		if(word.len() + curx > xwidth)
		{
			// go to new line
			curx = 0;
			y += 8;
		}
		
		// print word
		foreach(charecter in word)
		{
			if('A' <= charecter && charecter <= 'Z')
				charindex = charecter - 'A';
			else if('a' <= charecter && charecter <= 'z')
				charindex = charecter - 'a' + 26;
			else if('0' <= charecter && charecter <= '9')
				charindex = charecter - '0' + 52;
			else if(charecter == ',')
				charindex = 65;
			else if(charecter == '.')
				charindex = 62;
			else if(charecter == '?')
				charindex = 64;
			else if(charecter == '!')
				charindex = 63;
			else if(charecter == '_')
				charindex = 67;
			else if(charecter == '-')
				charindex = 68;
			else if(charecter == '\n')
			{
				curx += 1000;
				break;
			}
			else if(charecter == '/' || charecter == '\\')
				charindex = 70;
			else
				continue;
			
			if(vargv.len() > 0)
				BackBuffer.Draw("symbols", charindex, x + (curx * 8), y, 0, vargv);
			else
				BackBuffer.Draw("symbols", charindex, x + (curx * 8), y, 0);
			curx += 1;
		}
		curx += 1;
	}
}
