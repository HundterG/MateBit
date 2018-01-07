function HasIndex(table, index)
{
	try
	{
		table[index];
	}
	catch(e)
	{
		return false;
	}
	return true;
}
