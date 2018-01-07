class SoundTestState
{
	constructor()
	{
		Audio.PlayBGM("mary", false);
	}
	
	function Update()
	{
		if(Input.isPressed(Input.Select) || Input.isPressed(Input.A))
			Audio.PlaySFX("jingle", true);
		
		if(Input.isPressed(Input.B))
			PopState();
	}
	
	function Draw(BackBuffer)
	{
	}
}