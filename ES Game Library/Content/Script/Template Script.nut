script <- null;

function CreateCoroutine()
{
	script = Main();
}

function ResumeCoroutine()
{
	return resume script;
}

function Main()
{
//	yield 1;

	return 0;
}
