local StateStack = [];
local DropFrame = false;

function Update()
{
	StateStack.top().Update();
}

function Draw(BackBuffer)
{
	BackBuffer.Clear();
	StateStack.top().Draw(BackBuffer);
}

function PushState(newstate)
{
	StateStack.push(newstate);
	DropFrame = true;
}

function PopState()
{
	StateStack.pop();
	DropFrame = true;
}

function SwitchState(newstate)
{
	PopState();
	PushState(newstate);
}

function ResetStack()
{
	StateStack = [];
	DropFrame = true;
}

function tick(BackBuffer)
{
	DropFrame = false;
	if(StateStack.len() <= 0)
		StateStack = [MenuState()];
	Update();
	if(DropFrame == false)
		Draw(BackBuffer);
}
